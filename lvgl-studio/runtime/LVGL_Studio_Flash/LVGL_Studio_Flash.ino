// ============================================================
// ESP32-S3 LVGL Studio Flash — minimal dashboard runtime
//
// Only renders the Editor-exported `lvgl_studio_flash` screen
// (gauges + fuel bar + warning strip). Live data arrives via
// ESP-NOW from the C3 sensor hub and is pushed into the bound
// subjects (flash_rpm, flash_mph, flash_fuel, flash_fuel_bar).
// ============================================================

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <esp_timer.h>

#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lvgl.h"
#include "lvgl_editor.h"
#include "lvgl_editor_gen.h"

#include "i2c.h"
#include "io_extension.h"
#include "rgb_lcd_port.h"
#include "sensor_packet.h"

// ---------- Display ----------
// Full-bleed 1024x600 XML: offset stays 0; tune hsync/vsync porches in rgb_lcd_port.cpp only.
// Use the panel's PSRAM framebuffers + vsync (Waveshare lvgl_port pattern) — partial flushes
// without vsync tear and look corrupted on this RGB bus.
static esp_lcd_panel_handle_t s_panel = nullptr;
static lv_display_t          *s_disp  = nullptr;

static void disp_flush_wait_cb(lv_display_t *disp)
{
    if (!lv_display_flush_is_last(disp)) return;
    ulTaskNotifyValueClear(NULL, ULONG_MAX);
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
}

static void disp_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    esp_lcd_panel_draw_bitmap(s_panel,
                              area->x1, area->y1,
                              area->x2 + 1, area->y2 + 1,
                              px_map);
    lv_display_flush_ready(disp);
}

// LVGL needs a periodic tick.
static void lv_tick_task(void *)
{
    lv_tick_inc(2);
}

// ---------- ESP-NOW receive ----------
static volatile bool s_packetPending = false;
static SensorPacket  s_packet        = {};
static uint32_t      s_rxCount       = 0;
static uint32_t      s_lastRxAt      = 0;

static void onEspNowReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len)
{
    (void)info;
    if (len != sizeof(SensorPacket)) return;
    const SensorPacket *pkt = reinterpret_cast<const SensorPacket *>(data);
    if (pkt->magic != PACKET_MAGIC || pkt->version != PACKET_VERSION) return;
    memcpy((void *)&s_packet, data, sizeof(SensorPacket));
    s_packetPending = true;
    s_rxCount++;
    s_lastRxAt = millis();
}

static bool initEspNow()
{
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N);
    if (esp_now_init() != ESP_OK) return false;
    esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
    esp_now_register_recv_cb(onEspNowReceive);
    return true;
}

// ---------- Boot Sweep Animation ----------
enum BootState {
    BOOT_SWEEP_UP,
    BOOT_SWEEP_DOWN,
    BOOT_ACTIVE
};

static BootState s_bootState = BOOT_SWEEP_UP;
static uint32_t  s_bootStartMs = 0;

// ---------- Helper functions to find arc/label widgets ----------
static lv_obj_t * dashboard_container()
{
    if (!lvgl_studio_flash) return nullptr;
    return lv_obj_get_child(lvgl_studio_flash, 0);
}

// Gen-output child order (lvgl_studio_flash_gen.c):
//   0=turnL  1=turnR
//   2=RPM arc  3=RPM number  4="RPM" label
//   5=center chip  6=drive_mode  7=odo
//   8=MPH arc  9=MPH number  10="MPH" label
//   11=FUEL label  12=fuel%  13=fuel bar
//   14=warning strip
//   15=SYSTEM CHECK  16=SEARCHING
static lv_obj_t * get_system_check_label()
{
    lv_obj_t * container = dashboard_container();
    if (!container) return nullptr;
    return lv_obj_get_child(container, 15);
}

static lv_obj_t * get_transport_label()
{
    lv_obj_t * container = dashboard_container();
    if (!container) return nullptr;
    return lv_obj_get_child(container, 16);
}

static lv_obj_t * get_rpm_arc()
{
    lv_obj_t * container = dashboard_container();
    if (!container) return nullptr;
    return lv_obj_get_child(container, 2);
}

static lv_obj_t * get_mph_arc()
{
    lv_obj_t * container = dashboard_container();
    if (!container) return nullptr;
    return lv_obj_get_child(container, 8);
}

static void tune_arc_widget(lv_obj_t * arc)
{
    if (!arc) return;
    lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);
    lv_arc_set_mode(arc, LV_ARC_MODE_NORMAL);
    lv_obj_set_style_arc_rounded(arc, true, LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(arc, true, LV_PART_INDICATOR);
}

/** 400×400 arc bbox inside ~32 px safe-area inset (matches lvgl_studio_flash.xml). */
static void center_gauge_arcs()
{
    static constexpr int32_t kArcSize = 400;
    static constexpr int32_t kArcY    = 100;
    static constexpr int32_t kArcR    = kArcSize / 2;
    static constexpr int32_t kLeftCx  = 244;
    static constexpr int32_t kRightCx = 780;

    lv_obj_t * rpm = get_rpm_arc();
    if (rpm) {
        lv_obj_set_pos(rpm, kLeftCx - kArcR, kArcY);
        lv_obj_set_size(rpm, kArcSize, kArcSize);
    }
    lv_obj_t * mph = get_mph_arc();
    if (mph) {
        lv_obj_set_pos(mph, kRightCx - kArcR, kArcY);
        lv_obj_set_size(mph, kArcSize, kArcSize);
    }
}

static void fix_top_left_label(lv_obj_t * label, int32_t x, int32_t y, int32_t w)
{
    if (!label) return;
    lv_obj_set_align(label, LV_ALIGN_TOP_LEFT);
    lv_obj_set_pos(label, x, y);
    if (w > 0) {
        lv_obj_set_width(label, w);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    }
}

static void finalize_dashboard_layout()
{
    lv_obj_t * container = dashboard_container();
    if (!container) return;

    // Paint screen black behind the XML container.
    if (lvgl_studio_flash) {
        lv_obj_set_style_bg_color(lvgl_studio_flash, lv_color_hex(0x000000), 0);
        lv_obj_set_style_bg_opa(lvgl_studio_flash, LV_OPA_COVER, 0);
    }

    // Trust XML positions. Container fills full panel, no shift.
    lv_obj_set_align(container, LV_ALIGN_TOP_LEFT);
    lv_obj_set_pos(container, 0, 0);
    lv_obj_set_size(container, EXAMPLE_LCD_H_RES, EXAMPLE_LCD_V_RES);
    lv_obj_clear_flag(container, LV_OBJ_FLAG_SCROLLABLE);

    if (lvgl_studio_flash) {
        lv_obj_set_size(lvgl_studio_flash, EXAMPLE_LCD_H_RES, EXAMPLE_LCD_V_RES);
    }

    // Trust gen .c XML positions. Only tune arc styling (rounded ends, no click).
    tune_arc_widget(get_rpm_arc());
    tune_arc_widget(get_mph_arc());
}

// ---------- Apply packet → LVGL subjects ----------
static void applyPacket(const SensorPacket &p)
{
    char buf[16];

    snprintf(buf, sizeof(buf), "%u", p.rpm);
    lv_subject_copy_string(&flash_rpm, buf);

    snprintf(buf, sizeof(buf), "%u", p.mph);
    lv_subject_copy_string(&flash_mph, buf);

    snprintf(buf, sizeof(buf), "%u%%", p.fuelPct);
    lv_subject_copy_string(&flash_fuel, buf);

    int32_t bar = p.fuelPct;
    if (bar < 0) bar = 0;
    if (bar > 100) bar = 100;
    lv_subject_set_int(&flash_fuel_bar, bar);

    // Update gauge arc values (scaled to 0-100%) so indicators move!
    int32_t rpm_bar = (static_cast<int32_t>(p.rpm) * 100) / 8000;
    if (rpm_bar < 0) rpm_bar = 0;
    if (rpm_bar > 100) rpm_bar = 100;
    lv_subject_set_int(&sp_rpm_bar, rpm_bar);

    // Set indicator opacity to transparent if value is 0 to avoid the "messed up pixels" / dot at 0
    lv_obj_t * rpm_arc = get_rpm_arc();
    if (rpm_arc) {
        if (rpm_bar == 0) {
            lv_obj_set_style_opa(rpm_arc, LV_OPA_TRANSP, LV_PART_INDICATOR);
        } else {
            lv_obj_set_style_opa(rpm_arc, LV_OPA_COVER, LV_PART_INDICATOR);
        }
    }

    int32_t speed_bar = (static_cast<int32_t>(p.mph) * 100) / 120;
    if (speed_bar < 0) speed_bar = 0;
    if (speed_bar > 100) speed_bar = 100;
    lv_subject_set_int(&sp_speed_bar, speed_bar);

    lv_obj_t * mph_arc = get_mph_arc();
    if (mph_arc) {
        if (speed_bar == 0) {
            lv_obj_set_style_opa(mph_arc, LV_OPA_TRANSP, LV_PART_INDICATOR);
        } else {
            lv_obj_set_style_opa(mph_arc, LV_OPA_COVER, LV_PART_INDICATOR);
        }
    }
}

// ---------- Setup ----------
void setup()
{
    Serial.begin(115200);
    // 3-second USB CDC settle window. Lets the host enumerate the native USB CDC
    // before any heavy panel/LVGL init. Esptool's auto-reset (DTR/RTS sequence)
    // needs the port present, so a stable USB during boot = no manual BOOT+RESET.
    for (int i = 0; i < 6; ++i) {
        Serial.printf("[boot %d/6] waiting for host USB enumeration...\n", i + 1);
        delay(500);
    }
    Serial.println("\n=== S3 LVGL Studio Flash (minimal dashboard) ===");

    Serial.println("Initializing Waveshare I2C...");
    DEV_I2C_Init();
    delay(100);

    Serial.println("Initializing Waveshare IO expander...");
    IO_EXTENSION_Init();
    delay(100);

    Serial.println("Initializing RGB LCD panel...");
    s_panel = waveshare_esp32_s3_rgb_lcd_init();
    if (!s_panel) {
        Serial.println("Panel init failed");
        return;
    }
    delay(100);

    rgb_lcd_bind_vsync_task(xTaskGetCurrentTaskHandle());

    Serial.println("Turning on LCD backlight...");
    wavesahre_rgb_lcd_bl_on();
    delay(300);

    void *fb1 = nullptr;
    void *fb2 = nullptr;
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_get_frame_buffer(s_panel, 2, &fb1, &fb2));

    const uint32_t fb_bytes = EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES * sizeof(lv_color_t);
    Serial.printf("LVGL full-frame buffers: %u bytes x2 (PSRAM)\n", (unsigned)fb_bytes);

    // LVGL core
    lv_init();

    s_disp = lv_display_create(EXAMPLE_LCD_H_RES, EXAMPLE_LCD_V_RES);
    lv_display_set_color_format(s_disp, LV_COLOR_FORMAT_RGB565);
    lv_display_set_flush_cb(s_disp, disp_flush_cb);
    lv_display_set_flush_wait_cb(s_disp, disp_flush_wait_cb);
    lv_display_set_buffers(s_disp, fb1, fb2, fb_bytes, LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_offset(s_disp, 0, 0);

    // LVGL tick from esp_timer (2 ms cadence — matches LV_DEF_REFR_PERIOD).
    const esp_timer_create_args_t tick_args = {
        .callback        = lv_tick_task,
        .arg             = nullptr,
        .dispatch_method = ESP_TIMER_TASK,
        .name            = "lv_tick",
        .skip_unhandled_events = true,
    };
    esp_timer_handle_t tick_timer = nullptr;
    esp_timer_create(&tick_args, &tick_timer);
    esp_timer_start_periodic(tick_timer, 2 * 1000);   // 2 ms

    // Build the Editor-generated UI (creates `lvgl_studio_flash` + binds subjects).
    lvgl_editor_init("A:/");
    if (lvgl_studio_flash) {
        lv_scr_load(lvgl_studio_flash);
        Serial.println("UI screen loaded");

        // Absolutely disable scrolling and default borders/padding on screen
        lv_obj_clear_flag(lvgl_studio_flash, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_pad_all(lvgl_studio_flash, 0, 0);
        lv_obj_set_style_border_width(lvgl_studio_flash, 0, 0);

        finalize_dashboard_layout();
    } else {
        Serial.println("WARN: lvgl_studio_flash screen null");
    }

    // Initialize boot animation timing
    s_bootStartMs = millis();
    s_bootState = BOOT_SWEEP_UP;

    // Initial placeholders so the screen isn't blank before the first packet.
    lv_subject_copy_string(&flash_rpm, "0");
    lv_subject_copy_string(&flash_mph, "0");
    lv_subject_copy_string(&flash_fuel, "0%");
    lv_subject_set_int(&flash_fuel_bar, 0);
    lv_subject_set_int(&sp_rpm_bar, 0);
    lv_subject_set_int(&sp_speed_bar, 0);
    lv_subject_copy_string(&sp_gear, "P");
    lv_subject_copy_string(&sp_drive_mode, "READY");

    // Initial opacity of indicators is transparent to avoid any 0-value dot showing
    lv_obj_t * rpm_arc = get_rpm_arc();
    if (rpm_arc) lv_obj_set_style_opa(rpm_arc, LV_OPA_TRANSP, LV_PART_INDICATOR);
    lv_obj_t * mph_arc = get_mph_arc();
    if (mph_arc) lv_obj_set_style_opa(mph_arc, LV_OPA_TRANSP, LV_PART_INDICATOR);

    // ESP-NOW receiver
    if (!initEspNow()) {
        Serial.println("ESP-NOW init failed");
    } else {
        Serial.printf("ESP-NOW ready ch=%u MAC=%s\n", ESPNOW_CHANNEL, WiFi.macAddress().c_str());
    }

    if (s_disp && lvgl_studio_flash) {
        lv_obj_invalidate(lvgl_studio_flash);
        lv_refr_now(s_disp);
    }
}

// ---------- Loop ----------
void loop()
{
    const uint32_t now = millis();

    // Boot anim at ~25 Hz — full-frame RGB + vsync can't keep up at 200+ Hz loop rate.
    static uint32_t s_lastBootUiMs = 0;
    if (s_bootState != BOOT_ACTIVE && (now - s_lastBootUiMs) < 50) {
        lv_timer_handler();
        delay(5);
        return;
    }
    if (s_bootState != BOOT_ACTIVE) {
        s_lastBootUiMs = now;
    }

    // Boot sweep animation state machine
    if (s_bootState == BOOT_SWEEP_UP) {
        uint32_t elapsed = now - s_bootStartMs;
        if (elapsed >= 1500) {
            s_bootState = BOOT_SWEEP_DOWN;
            s_bootStartMs = now;
            elapsed = 0;
        }
        float t = elapsed / 1500.0f;
        if (t > 1.0f) t = 1.0f;
        
        // Easing: cubic out for premium sweep feel
        float ease = 1.0f - powf(1.0f - t, 3.0f);
        
        uint32_t rpm = (uint32_t)(8000.0f * ease);
        uint32_t mph = (uint32_t)(120.0f * ease);
        uint32_t fuel = (uint32_t)(100.0f * ease);
        
        char buf[16];
        snprintf(buf, sizeof(buf), "%lu", (unsigned long)rpm);
        lv_subject_copy_string(&flash_rpm, buf);
        
        snprintf(buf, sizeof(buf), "%lu", (unsigned long)mph);
        lv_subject_copy_string(&flash_mph, buf);
        
        snprintf(buf, sizeof(buf), "%lu%%", (unsigned long)fuel);
        lv_subject_copy_string(&flash_fuel, buf);
        
        lv_subject_set_int(&flash_fuel_bar, fuel);
        lv_subject_set_int(&sp_rpm_bar, (int32_t)(100.0f * ease));
        lv_subject_set_int(&sp_speed_bar, (int32_t)(100.0f * ease));
        
        // Set indicator opacity to transparent if 0 to avoid artifacts
        lv_obj_t * rpm_arc = get_rpm_arc();
        if (rpm_arc) lv_obj_set_style_opa(rpm_arc, (rpm == 0) ? LV_OPA_TRANSP : LV_OPA_COVER, LV_PART_INDICATOR);
        lv_obj_t * mph_arc = get_mph_arc();
        if (mph_arc) lv_obj_set_style_opa(mph_arc, (mph == 0) ? LV_OPA_TRANSP : LV_OPA_COVER, LV_PART_INDICATOR);
        
        lv_subject_copy_string(&sp_gear, "8");
        lv_subject_copy_string(&sp_drive_mode, "SYSTEM CHECK");
        
        lv_obj_t * sys_chk = get_system_check_label();
        if (sys_chk) lv_label_set_text(sys_chk, "RUNNING...");
        lv_obj_t * transport = get_transport_label();
        if (transport) lv_label_set_text(transport, "BOOT");
        
    } else if (s_bootState == BOOT_SWEEP_DOWN) {
        uint32_t elapsed = now - s_bootStartMs;
        if (elapsed >= 1000) {
            s_bootState = BOOT_ACTIVE;
            
            // Clean up state back to 0 before active telemetry arrives
            lv_subject_copy_string(&flash_rpm, "0");
            lv_subject_copy_string(&flash_mph, "0");
            lv_subject_copy_string(&flash_fuel, "0%");
            lv_subject_set_int(&flash_fuel_bar, 0);
            lv_subject_set_int(&sp_rpm_bar, 0);
            lv_subject_set_int(&sp_speed_bar, 0);
            
            lv_obj_t * rpm_arc = get_rpm_arc();
            if (rpm_arc) lv_obj_set_style_opa(rpm_arc, LV_OPA_TRANSP, LV_PART_INDICATOR);
            lv_obj_t * mph_arc = get_mph_arc();
            if (mph_arc) lv_obj_set_style_opa(mph_arc, LV_OPA_TRANSP, LV_PART_INDICATOR);
            
            lv_subject_copy_string(&sp_gear, "P");
            lv_subject_copy_string(&sp_drive_mode, "READY");
            
            lv_obj_t * sys_chk = get_system_check_label();
            if (sys_chk) lv_label_set_text(sys_chk, "PASS");
            lv_obj_t * transport = get_transport_label();
            if (transport) lv_label_set_text(transport, "READY");
        } else {
            float t = elapsed / 1000.0f;
            if (t > 1.0f) t = 1.0f;
            
            // Easing: cubic in-out for premium deceleration sweep feel
            float ease = t < 0.5f ? 4.0f * t * t * t : 1.0f - powf(-2.0f * t + 2.0f, 3.0f) / 2.0f;
            float val = 1.0f - ease;
            
            uint32_t rpm = (uint32_t)(8000.0f * val);
            uint32_t mph = (uint32_t)(120.0f * val);
            uint32_t fuel = (uint32_t)(100.0f * val);
            
            char buf[16];
            snprintf(buf, sizeof(buf), "%lu", (unsigned long)rpm);
            lv_subject_copy_string(&flash_rpm, buf);
            
            snprintf(buf, sizeof(buf), "%lu", (unsigned long)mph);
            lv_subject_copy_string(&flash_mph, buf);
            
            snprintf(buf, sizeof(buf), "%lu%%", (unsigned long)fuel);
            lv_subject_copy_string(&flash_fuel, buf);
            
            lv_subject_set_int(&flash_fuel_bar, fuel);
            lv_subject_set_int(&sp_rpm_bar, (int32_t)(100.0f * val));
            lv_subject_set_int(&sp_speed_bar, (int32_t)(100.0f * val));
            
            lv_obj_t * rpm_arc = get_rpm_arc();
            if (rpm_arc) lv_obj_set_style_opa(rpm_arc, (rpm == 0) ? LV_OPA_TRANSP : LV_OPA_COVER, LV_PART_INDICATOR);
            lv_obj_t * mph_arc = get_mph_arc();
            if (mph_arc) lv_obj_set_style_opa(mph_arc, (mph == 0) ? LV_OPA_TRANSP : LV_OPA_COVER, LV_PART_INDICATOR);
            
            lv_subject_copy_string(&sp_gear, "P");
            lv_subject_copy_string(&sp_drive_mode, "READY");
            
            lv_obj_t * sys_chk = get_system_check_label();
            if (sys_chk) lv_label_set_text(sys_chk, "RUNNING...");
            lv_obj_t * transport = get_transport_label();
            if (transport) lv_label_set_text(transport, "BOOT");
        }
    } else {
        // Active mode: drain ESP-NOW packets into subjects
        if (s_packetPending) {
            SensorPacket snap;
            noInterrupts();
            snap = s_packet;
            s_packetPending = false;
            interrupts();
            applyPacket(snap);
        }

        // Active mode connection check
        lv_obj_t * sys_chk = get_system_check_label();
        lv_obj_t * transport = get_transport_label();
        const bool linked = (s_lastRxAt != 0) && ((millis() - s_lastRxAt) <= 2000);
        if (sys_chk) {
            lv_label_set_text(sys_chk, linked ? "ALL SYSTEMS CLEAR" : "SYSTEM CHECK");
        }
        if (transport) {
            lv_label_set_text(transport, linked ? "LIVE" : "SEARCHING");
        }
    }

    // Cap steady-state refresh (~30 Hz) — full-frame RGB can't sustain tight loop + vsync.
    static uint32_t s_lastUiMs = 0;
    if (s_bootState == BOOT_ACTIVE && (now - s_lastUiMs) < 33) {
        delay(5);
        return;
    }
    s_lastUiMs = now;

    const uint32_t next_ms = lv_timer_handler();
    delay(next_ms > 10 ? 5 : 2);

    // Periodic link health log.
    static uint32_t lastLog = 0;
    if (now - lastLog > 1000) {
        lastLog = now;
        const uint32_t age = s_lastRxAt ? (now - s_lastRxAt) : 0xFFFFFFFFu;
        Serial.printf("rx=%lu age=%lums rpm=%u mph=%u fuel=%u\n",
                      (unsigned long)s_rxCount, (unsigned long)age,
                      s_packet.rpm, s_packet.mph, s_packet.fuelPct);
    }
}
