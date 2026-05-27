#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <esp_private/wifi.h>
#include <U8g2lib.h>
#include <DHT.h>
#include <Adafruit_NeoPixel.h>

#include "c3_pins.h"
#include "sensor_packet.h"
#include "control_packet.h"
#include <U8g2lib.h>
#include <Wire.h>

// ---------- Pins ----------
// US_TRIG_PIN = GPIO 1, US_ECHO_PIN = GPIO 0 (see c3_pins.h)
// Echo is 5V-level — use the 1k/2k divider noted in c3_pins.h.
constexpr uint8_t DHT_PIN = 3;   // DHT22/DHT11 DATA (10k pull-up to 3.3V)

// WS2812 vehicle strips: front GPIO9, rear GPIO10 (see c3_pins.h). Level-shifter recommended for 5V strips.
constexpr uint8_t  STRIP_BRIGHTNESS = 180;    // 0..255
constexpr uint32_t PIXEL_INTERVAL_MS = 12;    // faster strip refresh, calmer visible motion

#define DHTTYPE DHT11            // blue module; change to DHT22 for white AM2302

// Ultrasonic disable — pulseIn() blocks up to ECHO_TIMEOUT_US and starves ESP-NOW TX rate.
#ifndef US_ENABLED
#define US_ENABLED 0
#endif

// ---------- Timing ----------
constexpr uint32_t ECHO_TIMEOUT_US = 6000;   // ~1 m max range (limit pulseIn() worst-case block to 6 ms)
constexpr uint32_t US_INTERVAL_MS  = 20;     // faster ultrasonic refresh
constexpr uint32_t DHT_INTERVAL_MS = 2000;   // DHT22 min read period
constexpr uint32_t DISPLAY_MS      = 100;
constexpr uint32_t TX_INTERVAL_MS  = 50;     // 20 Hz unicast — leaves headroom for MAC retries
constexpr uint32_t DIST_STALE_MS   = 1200;

// Sample mode belongs on the C3 so the S3 still proves it is displaying packets
// coming from the sender, not locally-generated dashboard animation.
#define C3_SAMPLE_MODE 0

uint8_t lightFlags = 0;
bool demoSequenceEnabled = true;

// Unicast to S3 (10:51:db:74:f4:b8) — MAC-layer ACK + auto-retry, dramatic reliability boost vs broadcast.
static uint8_t S3_PEER_MAC[6] = {0x10, 0x51, 0xDB, 0x74, 0xF4, 0xB8};
#define BROADCAST_MAC S3_PEER_MAC
uint32_t txSeq      = 0;
uint32_t lastTxAt   = 0;
bool     espNowReady = false;
volatile bool     espNowSendReady = true;   // gate: true = safe to queue next send
volatile uint32_t espNowTxOkCount = 0;
volatile uint32_t espNowTxFailCount = 0;
volatile uint32_t espNowQueueFailCount = 0;

// ---------- ESP-NOW (S3 → C3 light commands) ----------
void onEspNowRecvC3(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
    (void)info;
    if (len != sizeof(ControlPacket)) return;
    const ControlPacket *pkt = reinterpret_cast<const ControlPacket *>(data);
    if (pkt->magic != CONTROL_MAGIC || pkt->version != CONTROL_VERSION) return;
    demoSequenceEnabled = false;
    lightFlags = pkt->lights;
}

void onEspNowSentC3(const esp_now_send_info_t *info, esp_now_send_status_t status) {
    (void)info;
    if (status == ESP_NOW_SEND_SUCCESS) {
        espNowTxOkCount++;
    } else {
        espNowTxFailCount++;
    }
    espNowSendReady = true;   // unlock next send
}

// ---------- Peripherals ----------
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
HardwareSerial bridgeAltUart(0);
DHT dht(DHT_PIN, DHTTYPE);
Adafruit_NeoPixel frontStrip(C3_FRONT_LED_COUNT, C3_FRONT_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rearStrip(C3_REAR_LED_COUNT, C3_REAR_LED_PIN, NEO_GRB + NEO_KHZ800);

// ---------- Smoothing ----------
// EMA with outlier gate: jumps larger than this are treated as a fresh target
// and snap immediately instead of crawling.
constexpr float DIST_EMA_ALPHA   = 0.06f;  // more smoothing, slower displayed distance motion
constexpr float DIST_SNAP_DELTA  = 18.0f;  // tolerate larger jumps before snapping

// ---------- State ----------
float distanceCmRaw = NAN;  // last raw measurement
float distanceCm    = NAN;  // smoothed, shown on screen
float tempC         = NAN;
float humidity      = NAN;
bool distanceValid  = false;
uint32_t lastDistanceEchoAt = 0;

// ---------- Tach ----------
constexpr uint32_t TACH_MIN_PERIOD_US    = 3500;    // reject impossible spikes/noise
constexpr uint32_t TACH_SIGNAL_TIMEOUT_US = 700000;  // no pulse after this means no live signal
constexpr uint8_t  TACH_MIN_LOCK_PULSES  = 3;       // ignore one-off random edges
constexpr uint32_t TACH_SHORT_PERIOD_PCT = 40;      // reject sudden half-period false edges (must be <50 to avoid latch-up)
constexpr float    TACH_EMA_ALPHA_UP     = 0.30f;
constexpr float    TACH_EMA_ALPHA_DOWN   = 0.18f;
constexpr float    TACH_SLEW_UP_RPM_PER_SEC   = 7000.0f;
constexpr float    TACH_SLEW_DOWN_RPM_PER_SEC = 9000.0f;
constexpr uint32_t TACH_DIAG_MS          = 250;
constexpr float    TACH_MAX_RPM          = 12000.0f;

volatile uint32_t tachLastPulseUs = 0;
volatile uint32_t tachPeriodUs    = 0;
volatile uint32_t tachPulseCount  = 0;
volatile uint32_t tachRejectCount = 0;
uint16_t tachRawRpm = 0;
float tachFilteredRpm = 0.0f;
uint16_t currentRpm = 0;
uint32_t lastTachDiagAt = 0;
uint32_t lastTachUpdateAt = 0;
bool tachLocked = false;
uint16_t currentMph = 0;

static float slewLimit(float current, float target, float maxStep) {
    if (target > current + maxStep) return current + maxStep;
    if (target < current - maxStep) return current - maxStep;
    return target;
}

void IRAM_ATTR tachIsr() {
    const uint32_t now = micros();
    const uint32_t dt  = now - tachLastPulseUs;
    if (dt < TACH_MIN_PERIOD_US) {
        tachRejectCount++;
        return;
    }
    if (tachPeriodUs != 0 && dt < ((tachPeriodUs * TACH_SHORT_PERIOD_PCT) / 100UL)) {
        tachRejectCount++;
        return;
    }
    tachPeriodUs    = dt;
    tachLastPulseUs = now;
    tachPulseCount++;
}

static void updateTachRpm(uint32_t nowMs) {
    const uint32_t dtMs = lastTachUpdateAt == 0 ? TX_INTERVAL_MS : (nowMs - lastTachUpdateAt);
    lastTachUpdateAt = nowMs;

    uint32_t periodUs;
    uint32_t lastUs;
    uint32_t pulseCount;
    noInterrupts();
    periodUs = tachPeriodUs;
    lastUs   = tachLastPulseUs;
    pulseCount = tachPulseCount;
    interrupts();

    const uint32_t ageUs = lastUs == 0 ? 0xFFFFFFFFUL : (micros() - lastUs);
    const bool liveSignal = periodUs != 0 &&
                            pulseCount >= TACH_MIN_LOCK_PULSES &&
                            ageUs <= TACH_SIGNAL_TIMEOUT_US;

    tachLocked = liveSignal;

    if (liveSignal) {
        const float freqHz = 1000000.0f / static_cast<float>(periodUs);
        float targetRpm = (freqHz * 60.0f) / TACH_PULSES_PER_REV;
        if (targetRpm < 0.0f) targetRpm = 0.0f;
        if (targetRpm > TACH_MAX_RPM) targetRpm = TACH_MAX_RPM;

        tachRawRpm = static_cast<uint16_t>(targetRpm + 0.5f);
        if (tachFilteredRpm < 50.0f) {
            tachFilteredRpm = targetRpm;
        } else {
            const float alpha = targetRpm >= tachFilteredRpm ? TACH_EMA_ALPHA_UP : TACH_EMA_ALPHA_DOWN;
            const float blendedRpm = tachFilteredRpm + alpha * (targetRpm - tachFilteredRpm);
            const float slewPerSec = targetRpm >= tachFilteredRpm ? TACH_SLEW_UP_RPM_PER_SEC : TACH_SLEW_DOWN_RPM_PER_SEC;
            tachFilteredRpm = slewLimit(tachFilteredRpm, blendedRpm, slewPerSec * (static_cast<float>(dtMs) / 1000.0f));
        }
    } else {
        tachRawRpm = 0;
        if (lastUs == 0 || ageUs > TACH_SIGNAL_TIMEOUT_US) {
            tachFilteredRpm = 0.0f;
        } else {
            const float blendedRpm = tachFilteredRpm + TACH_EMA_ALPHA_DOWN * (0.0f - tachFilteredRpm);
            tachFilteredRpm = slewLimit(tachFilteredRpm, blendedRpm, TACH_SLEW_DOWN_RPM_PER_SEC * (static_cast<float>(dtMs) / 1000.0f));
        }
    }

    currentRpm = static_cast<uint16_t>(tachFilteredRpm + 0.5f);
    if (currentRpm < 20 && !liveSignal) currentRpm = 0;
}

// ---------- VSS (vehicle speed) ----------
constexpr uint32_t VSS_MIN_PERIOD_US     = 500;
constexpr uint32_t VSS_SIGNAL_TIMEOUT_US = 1500000;
constexpr float    VSS_EMA_ALPHA         = 0.28f;
constexpr float    VSS_SLEW_MPH_PER_SEC  = 90.0f;

volatile uint32_t vssLastPulseUs = 0;
volatile uint32_t vssPeriodUs    = 0;
volatile uint32_t vssPulseCount  = 0;
volatile uint32_t vssRejectCount = 0;
float vssFilteredMph = 0.0f;
uint16_t vssRawMph = 0;
uint32_t lastVssUpdateAt = 0;
bool vssActive = false;

void IRAM_ATTR vssIsr() {
    const uint32_t now = micros();
    const uint32_t dt  = now - vssLastPulseUs;
    if (dt < VSS_MIN_PERIOD_US) {
        vssRejectCount++;
        return;
    }
    vssPeriodUs    = dt;
    vssLastPulseUs = now;
    vssPulseCount++;
}

static void updateVssMph(uint32_t nowMs) {
    const uint32_t dtMs = lastVssUpdateAt == 0 ? TX_INTERVAL_MS : (nowMs - lastVssUpdateAt);
    lastVssUpdateAt = nowMs;

    uint32_t periodUs;
    uint32_t lastUs;
    noInterrupts();
    periodUs = vssPeriodUs;
    lastUs   = vssLastPulseUs;
    interrupts();

    const uint32_t ageUs = lastUs == 0 ? 0xFFFFFFFFUL : (micros() - lastUs);
    vssActive = periodUs != 0 && ageUs <= VSS_SIGNAL_TIMEOUT_US;

    float targetMph = 0.0f;
    if (vssActive) {
        const float freqHz = 1000000.0f / static_cast<float>(periodUs);
        targetMph = (freqHz * 3600.0f) / VSS_PULSES_PER_MILE;
        if (targetMph < 0.0f)   targetMph = 0.0f;
        if (targetMph > 200.0f) targetMph = 200.0f;
    }

    vssRawMph = static_cast<uint16_t>(targetMph + 0.5f);
    const float blendedMph = vssFilteredMph + VSS_EMA_ALPHA * (targetMph - vssFilteredMph);
    vssFilteredMph = slewLimit(vssFilteredMph, blendedMph, VSS_SLEW_MPH_PER_SEC * (static_cast<float>(dtMs) / 1000.0f));
    if (vssFilteredMph < 0.4f && !vssActive) vssFilteredMph = 0.0f;
    currentMph = static_cast<uint16_t>(vssFilteredMph + 0.5f);
}

// ---------- Fuel ----------
// ADC reading averaged over N samples. Map sender ohms -> percent linearly between
// FUEL_R_FULL (100%) and FUEL_R_EMPTY (0%).
struct FuelReading {
    uint16_t adc = 0;
    float volts = 0.0f;
    float ohms = NAN;
    uint8_t pct = 0;
    bool connected = false;
};

FuelReading currentFuel;

static FuelReading readFuel() {
    constexpr int N = 16;
    constexpr uint16_t ADC_OPEN_HIGH = 4070;
    constexpr uint16_t ADC_SHORT_LOW = 2;

    if (!FUEL_SENDER_ENABLED) {
        return FuelReading{};
    }

    uint32_t accum = 0;
    for (int i = 0; i < N; i++) {
        accum += analogRead(FUEL_PIN);
    }
    FuelReading reading;
    const float adc = static_cast<float>(accum) / N;
    reading.adc = static_cast<uint16_t>(adc + 0.5f);

    if (reading.adc >= ADC_OPEN_HIGH || reading.adc <= ADC_SHORT_LOW) {
        reading.connected = false;
        return reading;
    }

    // ESP32-C3 default ADC: 12-bit, 0..4095, 0..3.3V approx
    const float v = (adc / 4095.0f) * 3.3f;
    reading.volts = v;
    if (v <= 0.001f || v >= 3.299f) return reading;

    // V = Vcc * R_sender / (R_sender + R_pullup)  =>  R_sender = R_pullup * V / (Vcc - V)
    const float r = FUEL_PULLUP_OHMS * v / (3.3f - v);
    reading.ohms = r;

    // Linear inverse: full at FUEL_R_FULL (low ohms), empty at FUEL_R_EMPTY
    float pct = 100.0f * (FUEL_R_EMPTY - r) / (FUEL_R_EMPTY - FUEL_R_FULL);
    if (pct < 0.0f)   pct = 0.0f;
    if (pct > 100.0f) pct = 100.0f;
    reading.pct = static_cast<uint8_t>(pct + 0.5f);
    reading.connected = true;
    return reading;
}

uint8_t  currentFuelPct = 0;
float fuelFilteredPct = NAN;

uint32_t lastUsAt      = 0;
uint32_t lastDhtAt     = 0;
uint32_t lastDisplayAt = 0;
uint32_t lastPixelAt   = 0;

constexpr uint32_t DEMO_SEQUENCE_MS = 12000;
constexpr uint32_t BLINK_CYCLE_MS   = 1050;
constexpr uint32_t BLINK_ON_MS      = 640;

static int activeSweepCount(uint32_t now, uint16_t ledsPerSide) {
    const uint32_t cyclePos = now % BLINK_CYCLE_MS;
    if (cyclePos >= BLINK_ON_MS) return 0;
    const float progress = static_cast<float>(cyclePos) / static_cast<float>(BLINK_ON_MS);
    return 1 + static_cast<int>(progress * static_cast<float>(ledsPerSide - 1));
}

static void fillRange(Adafruit_NeoPixel& strip, int start, int endInclusive, uint32_t color) {
    if (start > endInclusive) return;
    for (int i = start; i <= endInclusive; ++i) {
        strip.setPixelColor(i, color);
    }
}

static void applySweepingTurn(Adafruit_NeoPixel& strip, bool leftSide, uint32_t color, int count) {
    // Each 18-pixel strip is treated as the left and right side of the car.
    const int half = strip.numPixels() / 2;
    if (count <= 0 || half <= 0) return;
    const int lit = count > half ? half : count;

    if (leftSide) {
        fillRange(strip, 0, lit - 1, color);
    } else {
        fillRange(strip, strip.numPixels() - lit, strip.numPixels() - 1, color);
    }
}

static void updateDemoSequence(uint32_t now) {
    if (!demoSequenceEnabled) return;

    const uint32_t phase = now % DEMO_SEQUENCE_MS;
    uint8_t demoFlags = 0;

    if (phase < 1200) {
        demoFlags = LIGHT_LEFT;
    } else if (phase < 2400) {
        demoFlags = LIGHT_RIGHT;
    } else if (phase < 3400) {
        demoFlags = LIGHT_BRAKE;
    } else if (phase < 4600) {
        demoFlags = LIGHT_HAZARD;
    } else if (phase < 5600) {
        demoFlags = LIGHT_HEAD;
    } else if (phase < 6800) {
        demoFlags = LIGHT_HEAD | LIGHT_LEFT;
    } else {
        demoFlags = LIGHT_HEAD | LIGHT_RIGHT | LIGHT_BRAKE;
    }

    lightFlags = demoFlags;
}

static void updateSampleSensorData(uint32_t now) {
#if C3_SAMPLE_MODE
    const float t = now / 1000.0f;

    currentRpm = static_cast<uint16_t>(3600.0f + sinf(t * 1.35f) * 2300.0f);
    tachRawRpm = currentRpm;
    tachFilteredRpm = currentRpm;
    tachLocked = true;

    currentMph = static_cast<uint16_t>(58.0f + cosf(t * 0.65f) * 24.0f);
    vssRawMph = currentMph;
    vssFilteredMph = currentMph;
    vssActive = true;

    currentFuelPct = static_cast<uint8_t>(68.0f + sinf(t * 0.18f) * 18.0f);
    fuelFilteredPct = currentFuelPct;
    currentFuel.connected = true;
    currentFuel.pct = currentFuelPct;
    currentFuel.adc = 1600 + static_cast<uint16_t>((100 - currentFuelPct) * 12);
    currentFuel.ohms = FUEL_R_EMPTY - ((FUEL_R_EMPTY - FUEL_R_FULL) * (currentFuelPct / 100.0f));

    tempC = 22.0f + sinf(t * 0.12f) * 3.0f;
    humidity = 45.0f + cosf(t * 0.10f) * 8.0f;
#else
    (void)now;
#endif
}

// ---------- Exterior lights (front / rear strips on this board) ----------
void renderExteriorLights() {
    const uint8_t flags = lightFlags;
    const uint32_t now = millis();
    const int frontHalf = frontStrip.numPixels() / 2;
    const int rearHalf = rearStrip.numPixels() / 2;
    const int sweepFront = activeSweepCount(now, frontHalf > 0 ? frontHalf : 1);
    const int sweepRear = activeSweepCount(now, rearHalf > 0 ? rearHalf : 1);
    const uint32_t frontHead = frontStrip.Color(220, 235, 255);
    const uint32_t rearTail = rearStrip.Color(38, 0, 0);
    const uint32_t rearBrake = rearStrip.Color(255, 0, 0);
    const uint32_t amber = frontStrip.Color(255, 140, 0);

    frontStrip.clear();

    if (flags & LIGHT_HEAD) {
        for (int i = 0; i < C3_FRONT_LED_COUNT; i++) {
            frontStrip.setPixelColor(i, frontHead);
        }
    }

    if ((flags & LIGHT_LEFT) || (flags & LIGHT_HAZARD)) {
        applySweepingTurn(frontStrip, true, amber, sweepFront);
    }

    if ((flags & LIGHT_RIGHT) || (flags & LIGHT_HAZARD)) {
        applySweepingTurn(frontStrip, false, amber, sweepFront);
    }

    frontStrip.setBrightness(STRIP_BRIGHTNESS);
    frontStrip.show();

    rearStrip.clear();

    for (int i = 0; i < C3_REAR_LED_COUNT; i++) {
        rearStrip.setPixelColor(i, rearTail);
    }

    if (flags & LIGHT_BRAKE) {
        for (int i = 0; i < C3_REAR_LED_COUNT; i++) {
            rearStrip.setPixelColor(i, rearBrake);
        }
    }

    if ((flags & LIGHT_LEFT) || (flags & LIGHT_HAZARD)) {
        applySweepingTurn(rearStrip, true, rearStrip.Color(255, 120, 0), sweepRear);
    }

    if ((flags & LIGHT_RIGHT) || (flags & LIGHT_HAZARD)) {
        applySweepingTurn(rearStrip, false, rearStrip.Color(255, 120, 0), sweepRear);
    }

    rearStrip.setBrightness(STRIP_BRIGHTNESS);
    rearStrip.show();
}

bool initWifiAndEspNow() {
    // Pure STA, fixed channel. LR protocol for max ESP-NOW reliability.
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    delay(50);

    // Standard 11b/g/n. Drop LR — unicast retries cover reliability already.
    esp_wifi_set_protocol(WIFI_IF_STA,
        WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N);

    if (esp_now_init() != ESP_OK) return false;

    esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);

    esp_now_peer_info_t peer = {};
    memcpy(peer.peer_addr, BROADCAST_MAC, 6);
    peer.channel = ESPNOW_CHANNEL;
    peer.encrypt = false;
    peer.ifidx   = WIFI_IF_STA;
    if (!esp_now_is_peer_exist(BROADCAST_MAC)) {
        if (esp_now_add_peer(&peer) != ESP_OK) return false;
    }
    esp_now_register_recv_cb(onEspNowRecvC3);
    esp_now_register_send_cb(onEspNowSentC3);
    return true;
}

void sendSensorPacket() {
    // Don't queue if the previous send hasn't completed yet.
    // This is the key fix: blindly calling esp_now_send() every 50ms
    // overflows the internal queue when the WiFi task is slow, causing
    // ESP_ERR_ESPNOW_NO_MEM and eventually 100% queueFail.
    if (!espNowSendReady) return;

    SensorPacket pkt = {};
    pkt.magic         = PACKET_MAGIC;
    pkt.version       = PACKET_VERSION;
    pkt.lights        = lightFlags;
    pkt.rpm           = currentRpm;
    pkt.mph           = currentMph;
    pkt.fuelPct       = currentFuelPct;
    pkt.statusFlags   = 0;
    if (currentFuel.connected) pkt.statusFlags |= SENSOR_FUEL_CONNECTED;
    if (tachLocked)            pkt.statusFlags |= SENSOR_TACH_LOCKED;
    if (vssActive)             pkt.statusFlags |= SENSOR_VSS_ACTIVE;
    if (distanceValid)         pkt.statusFlags |= SENSOR_DISTANCE_VALID;
    if (!isnan(tempC) || !isnan(humidity)) pkt.statusFlags |= SENSOR_DHT_VALID;
    pkt.seq           = ++txSeq;
    pkt.millisSent    = millis();
    pkt.distanceCm    = distanceCm;
    pkt.distanceCmRaw = distanceCmRaw;
    pkt.tempC         = tempC;
    pkt.humidity      = humidity;

    // Send over both C3 UART candidates. This keeps the S3 wired backup alive
    // even if ESP-NOW is retrying, and covers both GPIO1 and the board TX pad.
    Serial1.write(reinterpret_cast<uint8_t*>(&pkt), sizeof(pkt));
    bridgeAltUart.write(reinterpret_cast<uint8_t*>(&pkt), sizeof(pkt));

    // Don't let a stuck/slow WiFi send path stop the hardwired UART backup.
    if (!espNowSendReady) return;
    espNowSendReady = false;   // lock until callback fires
    if (esp_now_send(BROADCAST_MAC, reinterpret_cast<uint8_t*>(&pkt), sizeof(pkt)) != ESP_OK) {
        espNowQueueFailCount++;
        espNowSendReady = true;  // unlock if send failed synchronously
    }
}

float measureDistanceCm() {
    digitalWrite(US_TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(US_TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(US_TRIG_PIN, LOW);

    const unsigned long us = pulseIn(US_ECHO_PIN, HIGH, ECHO_TIMEOUT_US);
    if (us == 0) return NAN;
    return (us * 0.0343f) * 0.5f;
}

void drawDisplay() {
    u8g2.clearBuffer();
    u8g2.setDrawColor(1);

    const int x = OLED_X_OFFSET;
    const int y = OLED_Y_OFFSET;
    char line[24];

    // This C3 OLED is a 72x40 visible window inside a 128x64 buffer.
    // Keep all live text inside that window so it is not clipped.
    u8g2.setFont(u8g2_font_6x10_tf);
    snprintf(line, sizeof(line), "RPM %u", currentRpm);
    u8g2.drawStr(x, y + 9, line);

    snprintf(line, sizeof(line), "MPH %u", currentMph);
    u8g2.drawStr(x, y + 22, line);

    snprintf(line, sizeof(line), "FUEL %u%%", currentFuelPct);
    u8g2.drawStr(x, y + 32, line);

    u8g2.drawStr(x, y + 40, "TX W+U");

    u8g2.sendBuffer();
}

// Web UI removed — pure ESP-NOW only for max realtime.

void setup() {
    Serial.begin(115200);
    delay(200);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);  // active-LOW: off at boot

    // UART bridge to S3
    Serial1.begin(UART_BRIDGE_BAUD, SERIAL_8N1, UART_BRIDGE_RX_PIN, UART_BRIDGE_TX_PIN);
    bridgeAltUart.begin(UART_BRIDGE_BAUD, SERIAL_8N1, UART_BRIDGE_ALT_RX_PIN, UART_BRIDGE_ALT_TX_PIN);

#if US_ENABLED
    pinMode(US_TRIG_PIN, OUTPUT);
    pinMode(US_ECHO_PIN, INPUT);
    digitalWrite(US_TRIG_PIN, LOW);
#endif

    pinMode(TACH_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(TACH_PIN), tachIsr, FALLING);

    pinMode(VSS_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(VSS_PIN), vssIsr, FALLING);

    pinMode(FUEL_PIN, INPUT);
    analogReadResolution(12);

    Wire.begin(OLED_SDA, OLED_SCL);
    u8g2.setI2CAddress(OLED_ADDR * 2);
    u8g2.begin();
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_profont12_tf);
    u8g2.drawStr(OLED_X_OFFSET, OLED_Y_OFFSET + 10, "BOOTING...");
    u8g2.sendBuffer();

    Serial.println("C3 Sensor Hub Initialized.");
    dht.begin();

    frontStrip.begin();
    frontStrip.clear();
    frontStrip.show();
    rearStrip.begin();
    rearStrip.clear();
    rearStrip.show();

    espNowReady = initWifiAndEspNow();
    Serial.printf("ESP-NOW: %s  channel=%u  MAC=%s  TX=%lums/%uHz\n",
                  espNowReady ? "ready" : "FAILED",
                  ESPNOW_CHANNEL,
                  WiFi.macAddress().c_str(),
                  static_cast<unsigned long>(TX_INTERVAL_MS),
                  1000u / TX_INTERVAL_MS);

    Serial.println();
    Serial.println("ESP32-C3 sensor hub + exterior WS2812");
    Serial.printf("US  TRIG=GPIO%u ECHO=GPIO%u\n", US_TRIG_PIN, US_ECHO_PIN);
    Serial.printf("DHT DATA=GPIO%u type=%s\n", DHT_PIN,
                  DHTTYPE == DHT22 ? "DHT22" : "DHT11");
    Serial.printf("EXT WIFI module UART: module TX -> GPIO%u, module RX -> GPIO%u\n",
                  EXT_WIFI_MODULE_RX_PIN,
                  EXT_WIFI_MODULE_TX_PIN);
    Serial.printf("UART bridge TX primary GPIO%u + fallback GPIO%u -> S3 UART2 RX\n",
                  UART_BRIDGE_TX_PIN,
                  UART_BRIDGE_ALT_TX_PIN);
    Serial.printf("LED front GPIO%u x%u | rear GPIO%u x%u\n",
                  C3_FRONT_LED_PIN, C3_FRONT_LED_COUNT,
                  C3_REAR_LED_PIN, C3_REAR_LED_COUNT);
    Serial.printf("TACH GPIO%u ppr=%.2f minPeriod=%lu us timeout=%lu us\n",
                  TACH_PIN,
                  static_cast<double>(TACH_PULSES_PER_REV),
                  static_cast<unsigned long>(TACH_MIN_PERIOD_US),
                  static_cast<unsigned long>(TACH_SIGNAL_TIMEOUT_US));
    Serial.printf("FUEL GPIO%u pullup=%.1f ohm full=%.1f empty=%.1f\n",
                  FUEL_PIN,
                  static_cast<double>(FUEL_PULLUP_OHMS),
                  static_cast<double>(FUEL_R_FULL),
                  static_cast<double>(FUEL_R_EMPTY));
    if (!FUEL_SENDER_ENABLED) {
        Serial.println("FUEL sender disabled in firmware until the divider is wired.");
    }
}

void loop() {
    const uint32_t now = millis();

    updateDemoSequence(now);

    // Ultrasonic disabled. No pulseIn() calls = no 25 ms blocking stall = loop free
    // to run TX at full 100 Hz. Re-enable by flipping US_ENABLED to 1.
#if US_ENABLED
    const uint32_t usInterval = (lastDistanceEchoAt && (now - lastDistanceEchoAt) <= DIST_STALE_MS)
        ? US_INTERVAL_MS
        : 500;
    if (now - lastUsAt >= usInterval) {
        lastUsAt = now;
        const float raw = measureDistanceCm();
        distanceCmRaw = raw;
        if (!isnan(raw)) {
            lastDistanceEchoAt = now;
            distanceValid = true;
            if (isnan(distanceCm) || fabsf(raw - distanceCm) > DIST_SNAP_DELTA) {
                distanceCm = raw;
            } else {
                distanceCm = distanceCm + DIST_EMA_ALPHA * (raw - distanceCm);
            }
        } else if (lastDistanceEchoAt == 0 || now - lastDistanceEchoAt > DIST_STALE_MS) {
            distanceValid = false;
            distanceCm = NAN;
        }
    }
#else
    distanceCm = NAN;
    distanceCmRaw = NAN;
    distanceValid = false;
#endif

    if (now - lastDhtAt >= DHT_INTERVAL_MS) {
        lastDhtAt = now;
        const float t = dht.readTemperature();
        const float h = dht.readHumidity();
        if (!isnan(t)) tempC = isnan(tempC) ? t : tempC + 0.25f * (t - tempC);
        if (!isnan(h)) humidity = isnan(humidity) ? h : humidity + 0.25f * (h - humidity);

        Serial.printf("dist=%s cm | T=%s C | RH=%s %%\n",
            isnan(distanceCm) ? "---" : String(distanceCm, 1).c_str(),
            isnan(tempC)      ? "---" : String(tempC, 1).c_str(),
            isnan(humidity)   ? "---" : String(humidity, 1).c_str());
    }

    updateTachRpm(now);
    updateVssMph(now);
    currentFuel    = readFuel();
    if (currentFuel.connected) {
        fuelFilteredPct = isnan(fuelFilteredPct)
            ? currentFuel.pct
            : fuelFilteredPct + 0.10f * (static_cast<float>(currentFuel.pct) - fuelFilteredPct);
        currentFuelPct = static_cast<uint8_t>(fuelFilteredPct + 0.5f);
    } else {
        fuelFilteredPct = NAN;
        currentFuelPct = 0;
    }
    updateSampleSensorData(now);

    if (now - lastDisplayAt >= DISPLAY_MS) {
        lastDisplayAt = now;
        drawDisplay();
    }

    if (now - lastTachDiagAt >= TACH_DIAG_MS) {
        lastTachDiagAt = now;
        uint32_t periodUs;
        uint32_t lastUs;
        uint32_t pulseCount;
        uint32_t rejectCount;
        noInterrupts();
        periodUs = tachPeriodUs;
        lastUs = tachLastPulseUs;
        pulseCount = tachPulseCount;
        rejectCount = tachRejectCount;
        interrupts();
        const uint32_t ageMs = lastUs == 0 ? 0xFFFFFFFFUL : ((micros() - lastUs) / 1000UL);
        Serial.printf("tach pin=GPIO%u level=%u raw=%u rpm=%u period=%luus age=%lums pulses=%lu reject=%lu | fuel %s adc=%u pct=%u ohm=",
                      TACH_PIN,
                      digitalRead(TACH_PIN),
                      tachRawRpm,
                      currentRpm,
                      static_cast<unsigned long>(periodUs),
                      static_cast<unsigned long>(ageMs),
                      static_cast<unsigned long>(pulseCount),
                      static_cast<unsigned long>(rejectCount),
                      currentFuel.connected ? "ok" : "open",
                      currentFuel.adc,
                      currentFuelPct);
        if (currentFuel.connected && !isnan(currentFuel.ohms)) {
            Serial.printf("%.1f\n", static_cast<double>(currentFuel.ohms));
        } else {
            Serial.println("---");
        }
    }

    if (espNowReady && now - lastTxAt >= TX_INTERVAL_MS) {
        lastTxAt = now;
        sendSensorPacket();
    }

    // Periodic ESP-NOW TX health log (every 1s)
    static uint32_t lastNetLogAt = 0;
    if (now - lastNetLogAt > 1000) {
        lastNetLogAt = now;
        Serial.printf("ESPNOW seq=%lu okCb=%lu failCb=%lu queueFail=%lu  ch=%d\n",
                      static_cast<unsigned long>(txSeq),
                      static_cast<unsigned long>(espNowTxOkCount),
                      static_cast<unsigned long>(espNowTxFailCount),
                      static_cast<unsigned long>(espNowQueueFailCount),
                      WiFi.channel());
    }

    if (now - lastPixelAt >= PIXEL_INTERVAL_MS) {
        lastPixelAt = now;
        renderExteriorLights();
    }
}
