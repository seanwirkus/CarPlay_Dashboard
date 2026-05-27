/*
 * LVGL Studio Flash Runtime
 * This sketch is auto-fed by lvgl-studio/bridge_server.py
 *
 * Also receives ESP-NOW sensor packets from the ESP32-C3 sender and renders
 * a small live-readout overlay on top of whatever dashboard is loaded.
 */

#include <cstring>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

#include "lvgl_port.h"
#include "i2c.h"
#include "io_extension.h"
#include "exterior_leds.h"
#include "generated_ui.h"

// ---------- ESP-NOW link (must match the C3 sender) ----------
// SensorPacket.lights (bit flags): HEAD=0x01 LEFT=0x02 RIGHT=0x04 BRAKE=0x08 HAZARD=0x10
// Drives WS2812: GPIO9 front (turns + headlights), GPIO10 rear (turns + brake). See exterior_leds.cpp.
static constexpr uint8_t  ESPNOW_CHANNEL = 1;
static constexpr uint32_t PACKET_MAGIC   = 0x45535052;

struct __attribute__((packed)) SensorPacket {
    uint32_t magic;
    uint8_t  version;
    uint8_t  lights;   // LIGHT_* flags from sender (GPIO 9/10 strips)
    uint8_t  _pad[2];
    uint32_t seq;
    uint32_t millisSent;
    float    distanceCm;
    float    distanceCmRaw;
    float    tempC;
    float    humidity;
};

// Updated from the ESP-NOW receive callback (WiFi task context).
// Loop reads these under the LVGL mutex.
static volatile float    g_distanceCm = NAN;
static volatile float    g_tempC      = NAN;
static volatile float    g_humidity   = NAN;
static volatile uint32_t g_rxCount    = 0;
static volatile uint32_t g_lastRxMs   = 0;
static volatile uint8_t  g_lightFlags = 0;

// LVGL overlay objects
static lv_obj_t* ovl_panel = nullptr;
static lv_obj_t* ovl_title = nullptr;
static lv_obj_t* ovl_dist  = nullptr;
static lv_obj_t* ovl_temp  = nullptr;
static lv_obj_t* ovl_hum   = nullptr;
static lv_obj_t* ovl_link  = nullptr;

static void on_espnow_recv(const esp_now_recv_info_t* info,
                           const uint8_t* data, int len) {
    if (len != (int)sizeof(SensorPacket)) return;
    SensorPacket pkt;
    memcpy(&pkt, data, sizeof(pkt));
    if (pkt.magic != PACKET_MAGIC) return;

    g_distanceCm = pkt.distanceCm;
    g_tempC      = pkt.tempC;
    g_humidity   = pkt.humidity;
    g_lightFlags = pkt.lights;
    g_rxCount    = g_rxCount + 1;
    g_lastRxMs   = millis();
}

static lv_obj_t* overlay_make_line(lv_obj_t* panel, int y) {
    lv_obj_t* l = lv_label_create(panel);
    lv_obj_set_style_text_color(l, lv_color_hex(0xE6E8EE), LV_PART_MAIN);
    lv_obj_set_style_text_font(l, &lv_font_montserrat_22, LV_PART_MAIN);
    lv_obj_align(l, LV_ALIGN_TOP_LEFT, 0, y);
    return l;
}

static void espnow_begin() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(false, true);
    esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW init failed");
        return;
    }
    esp_now_register_recv_cb(on_espnow_recv);
    Serial.printf("ESP-NOW ready  channel=%u  MAC=%s\n",
                  ESPNOW_CHANNEL, WiFi.macAddress().c_str());
}

static void overlay_build(lv_obj_t* scr) {
    ovl_panel = lv_obj_create(scr);
    lv_obj_set_size(ovl_panel, 260, 140);
    lv_obj_align(ovl_panel, LV_ALIGN_TOP_RIGHT, -16, 16);
    lv_obj_set_style_bg_color(ovl_panel, lv_color_hex(0x0B0D12), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ovl_panel, LV_OPA_80, LV_PART_MAIN);
    lv_obj_set_style_border_color(ovl_panel, lv_color_hex(0x2A3045), LV_PART_MAIN);
    lv_obj_set_style_border_width(ovl_panel, 1, LV_PART_MAIN);
    lv_obj_set_style_radius(ovl_panel, 12, LV_PART_MAIN);
    lv_obj_set_style_pad_all(ovl_panel, 10, LV_PART_MAIN);
    lv_obj_clear_flag(ovl_panel, LV_OBJ_FLAG_SCROLLABLE);

    ovl_title = lv_label_create(ovl_panel);
    lv_label_set_text(ovl_title, "SENSORS");
    lv_obj_set_style_text_color(ovl_title, lv_color_hex(0x8B93A7), LV_PART_MAIN);
    lv_obj_set_style_text_font(ovl_title, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_align(ovl_title, LV_ALIGN_TOP_LEFT, 0, 0);

    ovl_dist = overlay_make_line(ovl_panel, 22);
    ovl_temp = overlay_make_line(ovl_panel, 54);
    ovl_hum  = overlay_make_line(ovl_panel, 86);

    ovl_link = lv_label_create(ovl_panel);
    lv_obj_set_style_text_color(ovl_link, lv_color_hex(0x667085), LV_PART_MAIN);
    lv_obj_set_style_text_font(ovl_link, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_align(ovl_link, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    lv_label_set_text(ovl_dist, "D  —");
    lv_label_set_text(ovl_temp, "T  —");
    lv_label_set_text(ovl_hum,  "H  —");
    lv_label_set_text(ovl_link, "no link");
}

static void overlay_tick() {
    if (!ovl_dist) return;
    char buf[32];

    const float d = g_distanceCm;
    if (isnan(d)) lv_label_set_text(ovl_dist, "D  ---");
    else { snprintf(buf, sizeof(buf), "D  %.1f cm", d); lv_label_set_text(ovl_dist, buf); }

    const float t = g_tempC;
    if (isnan(t)) lv_label_set_text(ovl_temp, "T  ---");
    else { snprintf(buf, sizeof(buf), "T  %.1f °C", t); lv_label_set_text(ovl_temp, buf); }

    const float h = g_humidity;
    if (isnan(h)) lv_label_set_text(ovl_hum, "H  ---");
    else { snprintf(buf, sizeof(buf), "H  %.1f %%", h); lv_label_set_text(ovl_hum, buf); }

    const uint32_t age = millis() - g_lastRxMs;
    if (g_lastRxMs == 0 || age > 1500) {
        lv_label_set_text(ovl_link, "no link");
        lv_obj_set_style_text_color(ovl_link, lv_color_hex(0xFF5560), LV_PART_MAIN);
    } else {
        snprintf(buf, sizeof(buf), "rx %lu   %lu ms", (unsigned long)g_rxCount, (unsigned long)age);
        lv_label_set_text(ovl_link, buf);
        lv_obj_set_style_text_color(ovl_link, lv_color_hex(0x667085), LV_PART_MAIN);
    }
}

void setup() {
    static esp_lcd_panel_handle_t panel_handle = NULL;
    static esp_lcd_touch_handle_t tp_handle = NULL;

    Serial.begin(115200);
    delay(300);
    Serial.println("LVGL Studio runtime boot");

    DEV_I2C_Init();
    IO_EXTENSION_Init();

    panel_handle = waveshare_esp32_s3_rgb_lcd_init();
    if (panel_handle == NULL) {
        Serial.println("LCD init failed");
        return;
    }

    wavesahre_rgb_lcd_bl_on();
    tp_handle = NULL;  // non-touch unit

    esp_err_t lvret = lvgl_port_init(panel_handle, tp_handle);
    if (lvret != ESP_OK) {
        Serial.printf("lvgl_port_init failed: %d\n", (int)lvret);
        return;
    }

    if (lvgl_port_lock(-1)) {
        build_dashboard(lv_scr_act());
        overlay_build(lv_scr_act());
        lvgl_port_unlock();
    }

    espnow_begin();

    exterior_leds_init();
    Serial.println("Exterior WS2812: front GPIO9, rear GPIO10 (Serial test: L:1f = all on)");

    Serial.println("LVGL Studio dashboard + sensor overlay ready");
}

void loop() {
    exterior_leds_poll_serial();
    exterior_leds_set_flags(g_lightFlags);
    exterior_leds_tick(millis());

    static uint32_t last_demo_ms = 0;
    uint32_t now = millis();
    if (now - last_demo_ms >= 50) {
        last_demo_ms = now;
        if (lvgl_port_lock(2)) {
            studio_demo_tick();
            overlay_tick();
            lvgl_port_unlock();
        }
    }
    delay(5);
}
