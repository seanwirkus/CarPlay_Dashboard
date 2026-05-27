// ============================================================
// ESP32-S3 Dashboard Receiver
// Receives SensorPacket from C3 via ESP-NOW, serves the React
// dashboard over Wi-Fi, and pushes live data via SSE.
// Exterior WS2812 strips live on the C3; light commands from the
// web UI are forwarded to the C3 via ESP-NOW (ControlPacket).
// ============================================================

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

#include "sensor_packet.h"
#include "control_packet.h"
#include "waveshare_7b_lcd.h"

// ---------- Access Point ----------
constexpr const char* AP_SSID = "ESP32S3-Dashboard";
constexpr const char* AP_PASS = "dashboard1";   // 8+ chars required

// ---------- Timing ----------
constexpr uint32_t SSE_BROADCAST_MS  = 100;  // 10 Hz SSE push
constexpr uint32_t PACKET_STALE_MS   = 3000;
constexpr uint32_t RX_LOG_MS         = 500;

WebServer webServer(80);

// ---------- Dashboard UI mirror (web / native LVGL consumers) ----------
struct DashboardUiState {
    float    speedMph  = 0.0f;
    float    rpm       = 0.8f;   // ×1000 for display (matches React)
    char     gear      = 'P';    // P R N D or 1–3
    uint8_t  fuel      = 72;
    uint16_t tempF     = 195;
    bool     ecoMode   = true;
    uint32_t odometer  = 14175;
};

static uint8_t BROADCAST_MAC[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// ---------- State ----------
SensorPacket makeEmptyPacket() {
    SensorPacket packet = {};
    packet.magic = PACKET_MAGIC;
    packet.version = PACKET_VERSION;
    packet.distanceCm = NAN;
    packet.distanceCmRaw = NAN;
    packet.tempC = NAN;
    packet.humidity = NAN;
    return packet;
}

struct BridgeSnapshot {
    SensorPacket packet;
    bool packetReceived;
    uint32_t lastPacketAt;
    uint32_t rxCount;
    float rxRateHz;
    uint32_t lostPacketCount;
    uint32_t duplicatePacketCount;
    uint32_t outOfOrderPacketCount;
    uint32_t lastSeqGap;
};

SensorPacket lastPacket = makeEmptyPacket();
bool packetReceived = false;
uint32_t lastPacketAt = 0;
uint32_t rxCount = 0;
float rxRateHz = 0.0f;
uint32_t lostPacketCount = 0;
uint32_t duplicatePacketCount = 0;
uint32_t outOfOrderPacketCount = 0;
uint32_t lastSeq = 0;
uint32_t lastSeqGap = 0;
bool haveLastSeq = false;
portMUX_TYPE bridgeStateMux = portMUX_INITIALIZER_UNLOCKED;

DashboardUiState dashboardUi;

uint32_t lastSseBroadcast = 0;
uint32_t lastRxLogAt = 0;
uint32_t lastRxRateAt = 0;
uint32_t lastRxRateCount = 0;

BridgeSnapshot readBridgeSnapshot(bool clearPacketReceived = false) {
    BridgeSnapshot snapshot = {};

    portENTER_CRITICAL(&bridgeStateMux);
    snapshot.packet = lastPacket;
    snapshot.packetReceived = packetReceived;
    snapshot.lastPacketAt = lastPacketAt;
    snapshot.rxCount = rxCount;
    snapshot.rxRateHz = rxRateHz;
    snapshot.lostPacketCount = lostPacketCount;
    snapshot.duplicatePacketCount = duplicatePacketCount;
    snapshot.outOfOrderPacketCount = outOfOrderPacketCount;
    snapshot.lastSeqGap = lastSeqGap;
    if (clearPacketReceived) packetReceived = false;
    portEXIT_CRITICAL(&bridgeStateMux);

    return snapshot;
}

DashboardUiState readDashboardUiSnapshot() {
    DashboardUiState snap;
    portENTER_CRITICAL(&bridgeStateMux);
    snap = dashboardUi;
    portEXIT_CRITICAL(&bridgeStateMux);
    return snap;
}

void sendControlLights(uint8_t lights) {
    ControlPacket pkt = {};
    pkt.magic   = CONTROL_MAGIC;
    pkt.version = CONTROL_VERSION;
    pkt.lights  = lights;
    esp_now_send(BROADCAST_MAC, reinterpret_cast<uint8_t*>(&pkt), sizeof(pkt));
}

static bool readJsonBody(String& out, size_t maxLen = 1536) {
    out = "";
    if (!webServer.hasHeader("Content-Length")) return false;
    const size_t len = (size_t)webServer.header("Content-Length").toInt();
    if (len == 0 || len > maxLen) return false;
    out.reserve(len);
    WiFiClient& client = webServer.client();
    const uint32_t deadline = millis() + 800;
    while (out.length() < len && client.connected() && millis() < deadline) {
        const int avail = client.available();
        if (avail > 0) {
            const size_t need = len - out.length();
            const int take = avail < (int)need ? avail : (int)need;
            for (int i = 0; i < take; ++i) {
                out += (char)client.read();
            }
        } else {
            delay(1);
        }
    }
    return out.length() == len;
}

// ---------- ESP-NOW receive callback ----------
void onEspNowReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
    (void)info;
    if (len != sizeof(SensorPacket)) return;
    
    const SensorPacket* pkt = reinterpret_cast<const SensorPacket*>(data);
    if (pkt->magic != PACKET_MAGIC) return;
    if (pkt->version != PACKET_VERSION) return;

    const uint32_t receivedAt = millis();
    portENTER_CRITICAL(&bridgeStateMux);
    lastSeqGap = 0;
    if (haveLastSeq) {
        const uint32_t seqDiff = pkt->seq - lastSeq;
        if (seqDiff == 0) {
            duplicatePacketCount++;
        } else if (seqDiff < 0x80000000UL) {
            if (seqDiff > 1) {
                lastSeqGap = seqDiff - 1;
                lostPacketCount += lastSeqGap;
            }
            lastSeq = pkt->seq;
        } else {
            outOfOrderPacketCount++;
        }
    } else {
        lastSeq = pkt->seq;
        haveLastSeq = true;
    }
    memcpy(&lastPacket, data, sizeof(SensorPacket));
    packetReceived = true;
    lastPacketAt = receivedAt;
    ++rxCount;
    portEXIT_CRITICAL(&bridgeStateMux);
}

// ---------- Status JSON ----------
String buildStatusJson() {
    const BridgeSnapshot snapshot = readBridgeSnapshot();
    const DashboardUiState ui = readDashboardUiSnapshot();
    const uint32_t now = millis();
    const bool connected = snapshot.rxCount > 0 && (now - snapshot.lastPacketAt < PACKET_STALE_MS);
    const uint32_t totalExpected = snapshot.rxCount + snapshot.lostPacketCount;
    const float packetLossPct = totalExpected == 0
        ? 0.0f
        : (100.0f * static_cast<float>(snapshot.lostPacketCount) / static_cast<float>(totalExpected));
    const bool distanceValid = connected && (snapshot.packet.statusFlags & SENSOR_DISTANCE_VALID);

    JsonDocument doc;
    doc["connected"]   = connected;
    doc["rxCount"]     = snapshot.rxCount;
    doc["rxRateHz"]    = snapshot.rxRateHz;
    doc["seq"]         = snapshot.packet.seq;
    doc["lostPackets"] = snapshot.lostPacketCount;
    doc["duplicatePackets"] = snapshot.duplicatePacketCount;
    doc["outOfOrderPackets"] = snapshot.outOfOrderPacketCount;
    doc["lastSeqGap"]  = snapshot.lastSeqGap;
    doc["packetLossPct"] = packetLossPct;
    doc["statusFlags"] = snapshot.packet.statusFlags;
    doc["lights"]      = snapshot.packet.lights;
    if (!distanceValid || isnan(snapshot.packet.distanceCm)) doc["distanceCm"] = nullptr;
    else doc["distanceCm"] = snapshot.packet.distanceCm;
    if (!distanceValid || isnan(snapshot.packet.distanceCmRaw)) doc["distanceCmRaw"] = nullptr;
    else doc["distanceCmRaw"] = snapshot.packet.distanceCmRaw;
    if (isnan(snapshot.packet.tempC)) doc["tempC"] = nullptr;
    else doc["tempC"] = snapshot.packet.tempC;
    if (isnan(snapshot.packet.humidity)) doc["humidity"] = nullptr;
    else doc["humidity"] = snapshot.packet.humidity;
    if (snapshot.rxCount == 0) doc["packetAgeMs"] = nullptr;
    else doc["packetAgeMs"] = (now - snapshot.lastPacketAt);
    doc["uptimeMs"]    = now;
    doc["ip"]          = WiFi.softAPIP().toString();

    doc["speed"]       = connected ? snapshot.packet.mph : ui.speedMph;
    doc["rpm"]         = connected ? (snapshot.packet.rpm / 1000.0f) : ui.rpm;
    doc["gear"]        = String(ui.gear);
    doc["fuel"]        = connected && (snapshot.packet.statusFlags & SENSOR_FUEL_CONNECTED) ? snapshot.packet.fuelPct : ui.fuel;
    doc["fuelConnected"] = connected && (snapshot.packet.statusFlags & SENSOR_FUEL_CONNECTED);
    doc["tachLocked"]  = connected && (snapshot.packet.statusFlags & SENSOR_TACH_LOCKED);
    doc["vssActive"]   = connected && (snapshot.packet.statusFlags & SENSOR_VSS_ACTIVE);
    doc["distanceValid"] = distanceValid;
    doc["temperature"] = ui.tempF;
    doc["ecoMode"]     = ui.ecoMode;
    doc["odometer"]    = ui.odometer;

    String j;
    serializeJson(doc, j);
    return j;
}

void handlePostUi() {
    String body;
    if (!readJsonBody(body)) {
        webServer.send(400, "application/json", "{\"error\":\"bad body\"}");
        return;
    }

    JsonDocument inDoc;
    if (deserializeJson(inDoc, body)) {
        webServer.send(400, "application/json", "{\"error\":\"invalid json\"}");
        return;
    }

    portENTER_CRITICAL(&bridgeStateMux);
    if (!inDoc["speed"].isNull()) {
        dashboardUi.speedMph = inDoc["speed"].as<float>();
        if (dashboardUi.speedMph < 0) dashboardUi.speedMph = 0;
        if (dashboardUi.speedMph > 320) dashboardUi.speedMph = 320;
    }
    if (!inDoc["rpm"].isNull()) {
        dashboardUi.rpm = inDoc["rpm"].as<float>();
        if (dashboardUi.rpm < 0) dashboardUi.rpm = 0;
        if (dashboardUi.rpm > 8) dashboardUi.rpm = 8;
    }
    if (!inDoc["gear"].isNull()) {
        const char *g = inDoc["gear"].as<const char *>();
        if (g && g[0] && !g[1]) dashboardUi.gear = g[0];
    }
    if (!inDoc["fuel"].isNull()) {
        int f = inDoc["fuel"].as<int>();
        if (f < 0) f = 0;
        if (f > 100) f = 100;
        dashboardUi.fuel = (uint8_t)f;
    }
    if (!inDoc["temperature"].isNull()) {
        int t = inDoc["temperature"].as<int>();
        if (t < 0) t = 0;
        if (t > 400) t = 400;
        dashboardUi.tempF = (uint16_t)t;
    }
    if (!inDoc["ecoMode"].isNull()) {
        dashboardUi.ecoMode = inDoc["ecoMode"].as<bool>();
    }
    if (!inDoc["odometer"].isNull()) {
        dashboardUi.odometer = inDoc["odometer"].as<uint32_t>();
    }
    portEXIT_CRITICAL(&bridgeStateMux);

    webServer.send(200, "application/json", buildStatusJson());
}

void handlePostLights() {
    uint8_t flags = 0;
    bool ok = false;

    if (webServer.hasArg("flags")) {
        int v = webServer.arg("flags").toInt();
        if (v < 0) v = 0;
        if (v > 255) v = 255;
        flags = (uint8_t)v;
        ok = true;
    } else {
        String body;
        if (readJsonBody(body)) {
            JsonDocument d;
            if (!deserializeJson(d, body) && !d["lights"].isNull()) {
                int v = d["lights"].as<int>();
                if (v < 0) v = 0;
                if (v > 255) v = 255;
                flags = (uint8_t)v;
                ok = true;
            }
        }
    }

    if (!ok) {
        webServer.send(400, "application/json", "{\"error\":\"missing lights\"}");
        return;
    }

    sendControlLights(flags);
    webServer.send(200, "application/json", buildStatusJson());
}

// ---------- Web Server Handlers ----------
// Serve the built React dashboard from SPIFFS
void handleRoot() {
    if (SPIFFS.exists("/index.html")) {
        File f = SPIFFS.open("/index.html", "r");
        webServer.streamFile(f, "text/html");
        f.close();
    } else {
        // Fallback: simple redirect/status page
        String html = "<!doctype html><html><head><meta charset='utf-8'/>"
            "<title>ESP32-S3 Dashboard</title>"
            "<style>body{background:#0a0a0f;color:#e0e0e0;font-family:system-ui;display:flex;"
            "justify-content:center;align-items:center;height:100vh;margin:0}"
            ".card{text-align:center;padding:40px;border:1px solid #333;border-radius:16px;"
            "background:#111;max-width:500px}"
            "h1{color:#3b82f6}pre{text-align:left;font-size:12px;color:#888}"
            "</style></head><body><div class='card'>"
            "<h1>ESP32-S3 Dashboard</h1>"
            "<p>Dashboard files not found in SPIFFS.</p>"
            "<p>Build the React app and upload the filesystem:</p>"
            "<pre>cd dashboard && npm run build\n"
            "pio run -e s3-dashboard -t uploadfs</pre>"
            "<p>Live sensor data:</p>"
            "<pre id='data'>Loading...</pre>"
            "<script>"
            "setInterval(async()=>{"
            "try{const r=await fetch('/api/status');const j=await r.json();"
            "document.getElementById('data').textContent=JSON.stringify(j,null,2)}"
            "catch(e){}},500)"
            "</script>"
            "</div></body></html>";
        webServer.send(200, "text/html", html);
    }
}

void handleStatus() {
    webServer.send(200, "application/json", buildStatusJson());
}

// Serve static files from SPIFFS (CSS, JS, assets)
void handleNotFound() {
    String path = webServer.uri();
    
    if (SPIFFS.exists(path)) {
        String contentType = "text/plain";
        if (path.endsWith(".html")) contentType = "text/html";
        else if (path.endsWith(".css")) contentType = "text/css";
        else if (path.endsWith(".js")) contentType = "application/javascript";
        else if (path.endsWith(".json")) contentType = "application/json";
        else if (path.endsWith(".png")) contentType = "image/png";
        else if (path.endsWith(".svg")) contentType = "image/svg+xml";
        else if (path.endsWith(".ico")) contentType = "image/x-icon";
        else if (path.endsWith(".woff")) contentType = "font/woff";
        else if (path.endsWith(".woff2")) contentType = "font/woff2";
        
        File f = SPIFFS.open(path, "r");
        webServer.streamFile(f, contentType);
        f.close();
        return;
    }
    
    // SPA fallback: serve index.html for client-side routing
    if (SPIFFS.exists("/index.html") && !path.startsWith("/api/")) {
        File f = SPIFFS.open("/index.html", "r");
        webServer.streamFile(f, "text/html");
        f.close();
        return;
    }
    
    webServer.send(404, "text/plain", "Not Found");
}

// ---------- SSE (Server-Sent Events) ----------
WiFiClient sseClient;

void handleEvents() {
    if (sseClient) sseClient.stop();
    sseClient = webServer.client();
    sseClient.setNoDelay(true);
    sseClient.println("HTTP/1.1 200 OK");
    sseClient.println("Content-Type: text/event-stream");
    sseClient.println("Cache-Control: no-cache");
    sseClient.println("Connection: keep-alive");
    sseClient.println("Access-Control-Allow-Origin: *");
    sseClient.println("X-Accel-Buffering: no");
    sseClient.println();
    sseClient.println("retry: 1000");
    sseClient.println();
}

void sendSSEData() {
    if (!sseClient || !sseClient.connected()) return;
    
    String data = "data: " + buildStatusJson() + "\n\n";
    if (!sseClient.print(data)) {
        sseClient.stop();
    }
}

void configureWebServer() {
    webServer.on("/", HTTP_GET, handleRoot);
    webServer.on("/api/status", HTTP_GET, handleStatus);
    webServer.on("/api/ui", HTTP_POST, handlePostUi);
    webServer.on("/api/lights", HTTP_POST, handlePostLights);
    webServer.on("/events", HTTP_GET, handleEvents);
    webServer.onNotFound(handleNotFound);
    webServer.begin();
}

// ---------- Init ----------
bool initWifiAndEspNow() {
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(AP_SSID, AP_PASS, ESPNOW_CHANNEL);
    WiFi.setSleep(false);
    delay(100);
    esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);

    if (esp_now_init() != ESP_OK) return false;

    esp_now_peer_info_t txPeer = {};
    memcpy(txPeer.peer_addr, BROADCAST_MAC, 6);
    txPeer.channel = ESPNOW_CHANNEL;
    txPeer.encrypt = false;
    txPeer.ifidx   = WIFI_IF_AP;
    if (!esp_now_is_peer_exist(BROADCAST_MAC)) {
        if (esp_now_add_peer(&txPeer) != ESP_OK) return false;
    }

    esp_now_register_recv_cb(onEspNowReceive);
    return true;
}

void setup() {
    Serial.begin(115200);
    delay(200);

    if (!waveshare_7b_display_begin()) {
        Serial.println("Waveshare 7B RGB LCD init failed — check board / wiring.");
    }

    // Initialize SPIFFS for serving the React dashboard
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS mount failed!");
    } else {
        Serial.println("SPIFFS mounted OK");
        // List files in SPIFFS
        File root = SPIFFS.open("/");
        File file = root.openNextFile();
        while (file) {
            Serial.printf("  SPIFFS: %s (%d bytes)\n", file.name(), file.size());
            file = root.openNextFile();
        }
    }

    // Init WiFi + ESP-NOW receiver
    bool ok = initWifiAndEspNow();
    configureWebServer();

    Serial.printf("\n=== ESP32-S3 Dashboard Receiver ===\n");
    Serial.printf("AP: %s / %s  channel=%u\n", AP_SSID, AP_PASS, ESPNOW_CHANNEL);
    Serial.printf("IP: %s\n", WiFi.softAPIP().toString().c_str());
    Serial.printf("ESP-NOW: %s  MAC=%s\n",
                  ok ? "ready" : "FAILED",
                  WiFi.macAddress().c_str());
    Serial.printf("Open: http://%s/\n", WiFi.softAPIP().toString().c_str());
    Serial.println("Note: WS2812 vehicle strips are on the ESP32-C3 hub, not this board.");

    waveshare_7b_show_splash(AP_SSID, AP_PASS, WiFi.softAPIP().toString().c_str());
}

void loop() {
    const uint32_t now = millis();

    // Serve web clients
    webServer.handleClient();

    // Push SSE data to connected dashboard
    if (now - lastSseBroadcast >= SSE_BROADCAST_MS) {
        lastSseBroadcast = now;
        sendSSEData();
    }

    const BridgeSnapshot snapshot = readBridgeSnapshot(true);

    if (lastRxRateAt == 0) {
        lastRxRateAt = now;
        lastRxRateCount = snapshot.rxCount;
    } else if (now - lastRxRateAt >= 1000) {
        const uint32_t elapsedMs = now - lastRxRateAt;
        const uint32_t delta = snapshot.rxCount - lastRxRateCount;
        const float hz = (static_cast<float>(delta) * 1000.0f) / static_cast<float>(elapsedMs);
        portENTER_CRITICAL(&bridgeStateMux);
        rxRateHz = hz;
        portEXIT_CRITICAL(&bridgeStateMux);
        lastRxRateAt = now;
        lastRxRateCount = snapshot.rxCount;
    }

    // Log received packets
    if (snapshot.packetReceived && now - lastRxLogAt >= RX_LOG_MS) {
        lastRxLogAt = now;
        Serial.printf("RX #%u | rpm=%u mph=%u fuel=%u%s | dist=%.1f cm | T=%.1f C | RH=%.1f%% | lights=0x%02X\n",
            snapshot.packet.seq,
            snapshot.packet.rpm,
            snapshot.packet.mph,
            snapshot.packet.fuelPct,
            (snapshot.packet.statusFlags & SENSOR_FUEL_CONNECTED) ? "%" : "% open",
            snapshot.packet.distanceCm,
            snapshot.packet.tempC,
            snapshot.packet.humidity,
            snapshot.packet.lights);
    }
}
