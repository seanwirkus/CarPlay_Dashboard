// ESP32-S3 RetroView day-mode dashboard
// Framebuffer rendering via gui_paint primitives. ESP-NOW RX from C3 sensor hub.

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <math.h>

#include "rgb_lcd_port.h"
#include "i2c.h"
#include "io_extension.h"
#include "gui_paint.h"
#include "fonts.h"
#include "navigation_dashboard.h"
#include "radar_widget.h"

#include "sensor_packet.h"

static constexpr int LCD_W = EXAMPLE_LCD_H_RES;
static constexpr int LCD_H = EXAMPLE_LCD_V_RES;

// Backbuffer rendered with gui_paint; copied to LCD via wavesahre_rgb_lcd_display.
static uint8_t *backbuffer = nullptr;

// Telemetry from ESP-NOW
static volatile float    g_distanceCm = NAN;
static volatile float    g_tempC      = NAN;
static volatile float    g_humidity   = NAN;
static volatile uint16_t g_rpm        = 0;
static volatile uint8_t  g_lightFlags = 0;
static volatile uint32_t g_rxCount    = 0;
static volatile uint32_t g_lastRxMs   = 0;

static void on_espnow_recv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
    (void)info;
    if (len != static_cast<int>(sizeof(SensorPacket))) return;
    SensorPacket pkt;
    memcpy(&pkt, data, sizeof(pkt));
    if (pkt.magic != PACKET_MAGIC) return;
    g_distanceCm = pkt.distanceCm;
    g_tempC      = pkt.tempC;
    g_humidity   = pkt.humidity;
    g_rpm        = pkt.rpm;
    g_lightFlags = pkt.lights;
    g_rxCount    = g_rxCount + 1;
    g_lastRxMs   = millis();
}

static void espnow_begin() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(false, true);
    WiFi.setSleep(false);
    esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW init failed");
        return;
    }
    esp_now_register_recv_cb(on_espnow_recv);
    Serial.printf("ESP-NOW ready  ch=%u  MAC=%s\n", ESPNOW_CHANNEL, WiFi.macAddress().c_str());
}

// Top status bar — clock, link, RX counter
static void draw_status_bar(uint32_t nowMs) {
    Paint_DrawRectangle(0, 0, LCD_W, 56, NAV_MAP_SKY, DOT_PIXEL_1X1, DRAW_FILL_FULL);

    // Clock placeholder (uptime mm:ss)
    char clk[16];
    int s = nowMs / 1000;
    snprintf(clk, sizeof(clk), "%02d:%02d", (s / 60) % 60, s % 60);
    Paint_DrawString_EN(24, 18, clk, &Font20, NAV_TEXT_WHITE, NAV_MAP_SKY);

    // Brand
    Paint_DrawString_EN(140, 22, "RetroView", &Font16, NAV_TEXT_GRAY, NAV_MAP_SKY);

    // Link status
    const uint32_t age = (g_lastRxMs == 0) ? UINT32_MAX : (nowMs - g_lastRxMs);
    const bool linked = age < 3000;
    const char *linkTxt = linked ? "LINK OK" : "NO LINK";
    const uint16_t linkColor = linked ? NAV_TEXT_GREEN : NAV_RADAR_NEAR;
    Paint_DrawString_EN(LCD_W - 220, 22, linkTxt, &Font16, linkColor, NAV_MAP_SKY);

    char rxStr[24];
    snprintf(rxStr, sizeof(rxStr), "RX %lu", (unsigned long)g_rxCount);
    Paint_DrawString_EN(LCD_W - 110, 22, rxStr, &Font16, NAV_TEXT_GRAY, NAV_MAP_SKY);
}

// Bottom card: Distance (left), RPM (right), Compass (center)
static void draw_bottom_card(uint32_t nowMs) {
    const int cardY = LCD_H - NAV_CARD_HEIGHT;

    // Card surface (white with subtle top border)
    Paint_DrawRectangle(0, cardY, LCD_W, LCD_H, NAV_BACKGROUND_COLOR, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawLine(0, cardY, LCD_W, cardY, NAV_MAP_ROAD_MAIN, DOT_PIXEL_2X2, LINE_STYLE_SOLID);

    // Left: Distance label + value
    Paint_DrawString_EN(32, cardY + 18, "DISTANCE", &Font16, NAV_TEXT_GRAY, NAV_BACKGROUND_COLOR);
    char dbuf[16];
    if (!isnan(g_distanceCm) && g_distanceCm > 0) {
        snprintf(dbuf, sizeof(dbuf), "%.0f", g_distanceCm);
    } else {
        snprintf(dbuf, sizeof(dbuf), "--");
    }
    Paint_DrawString_EN(32, cardY + 50, dbuf, &Font48, NAV_ACCENT_COLOR, NAV_BACKGROUND_COLOR);
    Paint_DrawString_EN(32, cardY + 130, "cm", &Font20, NAV_TEXT_GRAY, NAV_BACKGROUND_COLOR);

    // Right: RPM label + value
    char rbuf[16];
    snprintf(rbuf, sizeof(rbuf), "%u", (unsigned)g_rpm);
    Paint_DrawString_EN(LCD_W - 240, cardY + 18, "ENGINE RPM", &Font16, NAV_TEXT_GRAY, NAV_BACKGROUND_COLOR);
    Paint_DrawString_EN(LCD_W - 240, cardY + 50, rbuf, &Font48, NAV_ACCENT_COLOR, NAV_BACKGROUND_COLOR);
    Paint_DrawString_EN(LCD_W - 240, cardY + 130, "rpm", &Font20, NAV_TEXT_GRAY, NAV_BACKGROUND_COLOR);

    // Center: compass placeholder (cardinal ring)
    const int cx = LCD_W / 2;
    const int cy = cardY + NAV_CARD_HEIGHT / 2;
    const int cr = 56;
    Paint_DrawCircle(cx, cy, cr, NAV_ACCENT_COLOR, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    Paint_DrawString_EN(cx - 6, cy - cr - 18, "N", &Font16, NAV_ACCENT_COLOR, NAV_BACKGROUND_COLOR);
    Paint_DrawString_EN(cx + cr + 8, cy - 8, "E", &Font16, NAV_TEXT_GRAY, NAV_BACKGROUND_COLOR);
    Paint_DrawString_EN(cx - 6, cy + cr + 4, "S", &Font16, NAV_TEXT_GRAY, NAV_BACKGROUND_COLOR);
    Paint_DrawString_EN(cx - cr - 18, cy - 8, "W", &Font16, NAV_TEXT_GRAY, NAV_BACKGROUND_COLOR);
    // Needle pointing north for now
    Paint_DrawLine(cx, cy, cx, cy - cr + 8, NAV_RADAR_NEAR, DOT_PIXEL_3X3, LINE_STYLE_SOLID);
    Paint_DrawCircle(cx, cy, 5, NAV_TEXT_WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
}

static void render_frame(uint32_t nowMs) {
    // Clear to day background
    Paint_Clear(NAV_BACKGROUND_COLOR);

    draw_status_bar(nowMs);

    // Radar widget — upper half between status bar and bottom card
    const int radarTop = 56;
    const int radarBottom = LCD_H - NAV_CARD_HEIGHT;
    const int cx = LCD_W / 2;
    const int cy = radarBottom - 40;  // anchored near card so arcs open upward
    const int maxR = (radarBottom - radarTop) - 70;
    radar_draw(cx, cy, maxR, g_distanceCm);

    draw_bottom_card(nowMs);
}

void setup() {
    Serial.begin(115200);
    delay(1500);
    Serial.println("RetroView day-mode boot");

    DEV_I2C_Init();           delay(50);
    IO_EXTENSION_Init();      delay(50);
    waveshare_esp32_s3_rgb_lcd_init(); delay(80);
    wavesahre_rgb_lcd_bl_on(); delay(200);

    // Allocate backbuffer in PSRAM (1024*600*2 = 1.2MB)
    backbuffer = (uint8_t *)heap_caps_malloc(LCD_W * LCD_H * 2, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!backbuffer) {
        Serial.println("FATAL: backbuffer alloc failed");
        while (1) delay(1000);
    }
    Paint_NewImage(backbuffer, LCD_W, LCD_H, ROTATE_0, NAV_BACKGROUND_COLOR);
    Paint_SetScale(65);  // RGB565 mode — default is 1bpp B/W which produces color garbage on RGB panel
    Paint_SelectImage(backbuffer);
    Paint_Clear(NAV_BACKGROUND_COLOR);
    wavesahre_rgb_lcd_display(backbuffer);

    espnow_begin();
}

void loop() {
    static uint32_t lastDraw = 0;
    const uint32_t now = millis();
    if (now - lastDraw < 33) {  // ~30 fps cap
        delay(2);
        return;
    }
    lastDraw = now;

    render_frame(now);
    wavesahre_rgb_lcd_display(backbuffer);
}
