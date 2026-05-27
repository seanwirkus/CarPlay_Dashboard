/*
 * LVGL Studio Flash Runtime
 *
 * Native dashboard for the Waveshare ESP32-S3-7B. This runtime consumes the
 * live ESP-NOW bridge packets from the ESP32-C3 and renders a simplified
 * operator view with explicit signal indicators and a proximity obstacle field.
 */

#include <cmath>
#include <cstdio>
#include <cstring>

#include <WiFi.h>
#include <esp_now.h>
#include <esp_private/wifi.h>
#include <esp_timer.h>
#include <esp_wifi.h>

#include "lvgl_port.h"
#include "i2c.h"
#include "io_extension.h"
#include "particle_tunnel.h"
#include "s3_pins.h"

static constexpr bool kEnableParticleTunnel = false;

static constexpr uint8_t  ESPNOW_CHANNEL = 11;
static constexpr uint32_t PACKET_MAGIC   = 0x45535052;

// Set to 1 to simulate sample data and force UI active even without sensor lock
#define DEMO_MODE 0

struct __attribute__((packed)) SensorPacket {
    uint32_t magic;
    uint8_t  version;
    uint8_t  lights;
    uint16_t rpm;
    uint32_t seq;
    uint32_t millisSent;
    float    distanceCm;
    float    distanceCmRaw;
    float    tempC;
    float    humidity;
    uint16_t mph;
    uint8_t  fuelPct;
    uint8_t  statusFlags;     // SENSOR_FUEL_CONNECTED / TACH_LOCKED / VSS_ACTIVE / DISTANCE_VALID / DHT_VALID
};

// Sensor status bit flags (must match include/sensor_packet.h SensorStatusBits)
static constexpr uint8_t SENSOR_FUEL_CONNECTED = 1 << 0;
static constexpr uint8_t SENSOR_TACH_LOCKED    = 1 << 1;
static constexpr uint8_t SENSOR_VSS_ACTIVE     = 1 << 2;
static constexpr uint8_t SENSOR_DISTANCE_VALID = 1 << 3;
static constexpr uint8_t SENSOR_DHT_VALID      = 1 << 4;

static volatile uint16_t g_rpm         = 0;
static volatile uint16_t g_mph         = 0;
static volatile uint8_t  g_fuelPct     = 0;
static volatile uint8_t  g_statusFlags = 0;

static volatile float    g_distanceCm = NAN;
static volatile float    g_tempC      = NAN;
static volatile float    g_humidity   = NAN;
static volatile uint8_t  g_lightFlags = 0;
static volatile uint32_t g_rxCount    = 0;
static volatile uint32_t g_lastRxMs   = 0;
static volatile uint32_t g_lastSeq    = 0;
static volatile uint32_t g_dropCount  = 0;
static volatile uint32_t g_wifiRxCount = 0;
static volatile uint32_t g_uartRxCount = 0;
static volatile uint32_t g_lastWifiRxMs = 0;
static volatile uint32_t g_lastUartRxMs = 0;
static volatile uint8_t  g_lastRxSource = 0; // 1 = ESP-NOW WiFi, 2 = UART bridge
static volatile uint32_t g_uart2ByteCount = 0;
static volatile uint32_t g_altUartByteCount = 0;
static volatile uint32_t g_uartBadFrameCount = 0;

static bool  g_haveFilter   = false;
static float g_filtDistCm   = NAN;
static float g_filtTempC    = NAN;
static float g_filtHum      = NAN;
static bool  g_haveUiFilter = false;
static float g_uiDistCm     = NAN;
static float g_uiTempC      = NAN;
static float g_uiHum        = NAN;

static constexpr uint8_t LIGHT_HEAD   = 1 << 0;
static constexpr uint8_t LIGHT_LEFT   = 1 << 1;
static constexpr uint8_t LIGHT_RIGHT  = 1 << 2;
static constexpr uint8_t LIGHT_BRAKE  = 1 << 3;
static constexpr uint8_t LIGHT_HAZARD = 1 << 4;

static lv_obj_t* g_dashRoot         = nullptr;
static lv_obj_t* g_topBar           = nullptr;
static lv_obj_t* g_driveCard        = nullptr;
static lv_obj_t* g_obstacleCard     = nullptr;
static lv_obj_t* g_leftSignalChip   = nullptr;
static lv_obj_t* g_headChip         = nullptr;
static lv_obj_t* g_brakeChip        = nullptr;
static lv_obj_t* g_rightSignalChip  = nullptr;
static lv_obj_t* g_linkLbl          = nullptr;
static lv_obj_t* g_speedLbl         = nullptr;
static lv_obj_t* g_gearLbl          = nullptr;
static lv_obj_t* g_distanceLbl      = nullptr;
static lv_obj_t* g_statusPill       = nullptr;
static lv_obj_t* g_statusPillLbl    = nullptr;
static lv_obj_t* g_tempLbl          = nullptr;
static lv_obj_t* g_humLbl           = nullptr;
static lv_obj_t* g_rxLbl            = nullptr;
static lv_obj_t* g_seqLbl           = nullptr;
static lv_obj_t* g_transportLbl     = nullptr;
static lv_obj_t* g_warningStrip     = nullptr;
static lv_obj_t* g_warningLamp[8]   = {};
static lv_obj_t* g_warningIconLbl[8] = {};
static lv_obj_t* g_warningLampLbl[8] = {};
static lv_obj_t* g_rangeTrack       = nullptr;
static lv_obj_t* g_rangeFill        = nullptr;
static lv_obj_t* g_obstacleField    = nullptr;
static lv_obj_t* g_scanLine         = nullptr;
static lv_obj_t* g_obstacleBadge    = nullptr;
static lv_obj_t* g_obstacleBadgeLbl = nullptr;
static lv_obj_t* g_vehicleMarker    = nullptr;
static lv_obj_t* g_rpmArc           = nullptr;
static lv_obj_t* g_mphArc           = nullptr;
static lv_obj_t* g_fuelBar          = nullptr;
static lv_obj_t* g_fuelLbl          = nullptr;
static lv_obj_t* g_leftTurnLbl      = nullptr;
static lv_obj_t* g_rightTurnLbl     = nullptr;
static uint32_t  g_dashboardBootMs  = 0;
// Smoothed display values
static float g_smoothRpm  = 0.0f;
static float g_smoothMph  = 0.0f;
static float g_smoothFuel = 0.0f;

LV_FONT_DECLARE(FA_Icons_20);

#define FA_GAS_PUMP     "\xEF\x94\xAF"
#define FA_THERMOMETER  "\xEF\x8B\x89"
#define FA_TACHOMETER   "\xEF\x98\xA9"
#define FA_CAR          "\xEF\x86\xB9"
#define FA_OIL_CAN      "\xEF\x98\x93"
#define FA_ROAD         "\xEF\x80\x98"
#define FA_BOLT         "\xEF\x83\xA7"
#define FA_WRENCH       "\xEF\x82\xAD"
#define FA_COG          "\xEF\x80\x93"
#define FA_FIRE         "\xEF\x81\xAD"
#define FA_CAR_SIDE     "\xEF\x83\x90"

static inline float clampf(float value, float minValue, float maxValue) {
    if (value < minValue) return minValue;
    if (value > maxValue) return maxValue;
    return value;
}

static inline float smooth_step(float current, float target, float alpha) {
    return current + (target - current) * alpha;
}

static inline float ease_in_out(float t) {
    t = clampf(t, 0.0f, 1.0f);
    return (t < 0.5f) ? (2.0f * t * t) : (1.0f - ((-2.0f * t + 2.0f) * (-2.0f * t + 2.0f)) * 0.5f);
}

static inline float mixf(float a, float b, float t) {
    return a + (b - a) * clampf(t, 0.0f, 1.0f);
}

static inline uint8_t lerp_u8(uint8_t a, uint8_t b, float t) {
    return static_cast<uint8_t>(roundf(static_cast<float>(a) + (static_cast<float>(b) - static_cast<float>(a)) * t));
}

static inline lv_color_t mix_color(lv_color_t a, lv_color_t b, float t) {
    return lv_color_make(
        lerp_u8(a.ch.red, b.ch.red, t),
        lerp_u8(a.ch.green, b.ch.green, t),
        lerp_u8(a.ch.blue, b.ch.blue, t));
}

static float distance_closeness(float distance_cm) {
    if (std::isnan(distance_cm)) return 0.0f;
    const float near_cm = 14.0f;
    const float far_cm = 140.0f;
    if (distance_cm <= near_cm) return 1.0f;
    if (distance_cm >= far_cm) return 0.0f;
    return 1.0f - ((distance_cm - near_cm) / (far_cm - near_cm));
}

static lv_obj_t* make_panel(lv_obj_t* parent, lv_coord_t x, lv_coord_t y, lv_coord_t w, lv_coord_t h,
                            lv_color_t bg, lv_color_t border, lv_coord_t radius = 24) {
    lv_obj_t* panel = lv_obj_create(parent);
    lv_obj_set_size(panel, w, h);
    lv_obj_set_pos(panel, x, y);
    lv_obj_set_style_bg_color(panel, bg, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(panel, border, LV_PART_MAIN);
    lv_obj_set_style_border_width(panel, 1, LV_PART_MAIN);
    lv_obj_set_style_radius(panel, radius, LV_PART_MAIN);
    lv_obj_set_style_pad_all(panel, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(panel, border, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(panel, 22, LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(panel, LV_OPA_20, LV_PART_MAIN);
    lv_obj_set_style_shadow_spread(panel, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_ofs_x(panel, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_ofs_y(panel, 0, LV_PART_MAIN);
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);
    return panel;
}

static lv_obj_t* make_chip(lv_obj_t* parent, lv_coord_t x, lv_coord_t y, lv_coord_t w, const char* text,
                           lv_obj_t** label_out = nullptr) {
    lv_obj_t* chip = make_panel(parent, x, y, w, 42, lv_color_hex(0x111926), lv_color_hex(0x28354A), 21);
    lv_obj_t* label = lv_label_create(chip);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_hex(0x8EA2BF), LV_PART_MAIN);
    lv_obj_center(label);
    if (label_out) *label_out = label;
    return chip;
}

static void set_signal_chip(lv_obj_t* chip, bool active, lv_color_t active_bg, lv_color_t active_border) {
    if (!chip) return;
    lv_obj_set_style_bg_color(chip, active ? active_bg : lv_color_hex(0x111926), LV_PART_MAIN);
    lv_obj_set_style_border_color(chip, active ? active_border : lv_color_hex(0x28354A), LV_PART_MAIN);
    lv_obj_set_style_shadow_color(chip, active ? active_border : lv_color_hex(0x28354A), LV_PART_MAIN);
    lv_obj_set_style_shadow_width(chip, active ? 18 : 10, LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(chip, active ? LV_OPA_40 : LV_OPA_10, LV_PART_MAIN);
}

static void set_badge(lv_obj_t* badge, lv_obj_t* label, const char* text, lv_color_t bg, lv_color_t fg) {
    if (!badge || !label) return;
    lv_label_set_text(label, text);
    lv_obj_set_style_bg_color(badge, bg, LV_PART_MAIN);
    lv_obj_set_style_border_color(badge, fg, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, fg, LV_PART_MAIN);
}

static lv_obj_t* make_warning_lamp(lv_obj_t* parent,
                                   lv_coord_t x,
                                   const char* icon,
                                   const char* text,
                                   lv_obj_t** icon_out,
                                   lv_obj_t** label_out) {
    lv_obj_t* lamp = lv_obj_create(parent);
    lv_obj_set_size(lamp, 84, 28);
    lv_obj_set_pos(lamp, x, 5);
    lv_obj_set_style_radius(lamp, 6, LV_PART_MAIN);
    lv_obj_set_style_bg_color(lamp, lv_color_hex(0x0C121B), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(lamp, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(lamp, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(lamp, lv_color_hex(0x253247), LV_PART_MAIN);
    lv_obj_set_style_pad_all(lamp, 0, LV_PART_MAIN);
    lv_obj_clear_flag(lamp, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* iconLabel = lv_label_create(lamp);
    lv_label_set_text(iconLabel, icon);
    lv_obj_set_style_text_font(iconLabel, &FA_Icons_20, LV_PART_MAIN);
    lv_obj_set_style_text_color(iconLabel, lv_color_hex(0x4F6077), LV_PART_MAIN);
    lv_obj_set_pos(iconLabel, 8, 4);

    lv_obj_t* label = lv_label_create(lamp);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_hex(0x4F6077), LV_PART_MAIN);
    lv_obj_set_pos(label, 33, 7);
    if (icon_out) *icon_out = iconLabel;
    if (label_out) *label_out = label;
    return lamp;
}

static void set_warning_lamp(uint8_t index, bool active, lv_color_t active_bg, lv_color_t active_fg) {
    if (index >= 8 || !g_warningLamp[index] || !g_warningLampLbl[index]) return;
    lv_obj_set_style_bg_color(g_warningLamp[index], active ? active_bg : lv_color_hex(0x0C121B), LV_PART_MAIN);
    lv_obj_set_style_border_color(g_warningLamp[index], active ? active_fg : lv_color_hex(0x253247), LV_PART_MAIN);
    lv_obj_set_style_shadow_color(g_warningLamp[index], active ? active_fg : lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_shadow_width(g_warningLamp[index], active ? 12 : 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(g_warningLamp[index], active ? LV_OPA_30 : LV_OPA_TRANSP, LV_PART_MAIN);
    if (g_warningIconLbl[index]) {
        lv_obj_set_style_text_color(g_warningIconLbl[index], active ? active_fg : lv_color_hex(0x4F6077), LV_PART_MAIN);
    }
    lv_obj_set_style_text_color(g_warningLampLbl[index], active ? active_fg : lv_color_hex(0x4F6077), LV_PART_MAIN);
}

static void apply_distance_theme(float distance_cm) {
    const float near_t = distance_closeness(distance_cm);
    const lv_color_t far_bg = lv_color_hex(0x08111A);
    const lv_color_t near_bg = lv_color_hex(0x190D10);
    const lv_color_t far_border = lv_color_hex(0x22344A);
    const lv_color_t near_border = lv_color_hex(0x5A2B32);
    const lv_color_t panel_bg = mix_color(lv_color_hex(0x0D1622), lv_color_hex(0x181114), near_t);
    const lv_color_t field_bg = mix_color(lv_color_hex(0x08111A), lv_color_hex(0x160C10), near_t);
    const lv_color_t vehicle_bg = mix_color(lv_color_hex(0x16E4F2), lv_color_hex(0xFF9A6B), near_t);

    if (g_dashRoot) {
        lv_obj_set_style_bg_color(g_dashRoot, mix_color(far_bg, near_bg, near_t), LV_PART_MAIN);
    }
    if (g_topBar) {
        lv_obj_set_style_bg_color(g_topBar, mix_color(lv_color_hex(0x0C141F), lv_color_hex(0x161014), near_t), LV_PART_MAIN);
        lv_obj_set_style_border_color(g_topBar, mix_color(far_border, near_border, near_t), LV_PART_MAIN);
    }
    if (g_driveCard) {
        lv_obj_set_style_bg_color(g_driveCard, panel_bg, LV_PART_MAIN);
        lv_obj_set_style_border_color(g_driveCard, mix_color(far_border, near_border, near_t), LV_PART_MAIN);
    }
    if (g_obstacleCard) {
        lv_obj_set_style_bg_color(g_obstacleCard, panel_bg, LV_PART_MAIN);
        lv_obj_set_style_border_color(g_obstacleCard, mix_color(far_border, near_border, near_t), LV_PART_MAIN);
    }
    if (g_obstacleField) {
        lv_obj_set_style_bg_color(g_obstacleField, field_bg, LV_PART_MAIN);
        lv_obj_set_style_border_color(g_obstacleField, mix_color(far_border, near_border, near_t), LV_PART_MAIN);
    }
    if (g_vehicleMarker) {
        lv_obj_set_style_bg_color(g_vehicleMarker, vehicle_bg, LV_PART_MAIN);
        lv_obj_set_style_border_color(g_vehicleMarker, mix_color(lv_color_hex(0x8CF2FF), lv_color_hex(0xFFD0B3), near_t), LV_PART_MAIN);
        lv_obj_set_style_shadow_color(g_vehicleMarker, vehicle_bg, LV_PART_MAIN);
    }
}

static void custom_dashboard_build(lv_obj_t* scr) {
    // Native cluster: large dual gauges, live transport status, warning strip.
    g_dashRoot = lv_obj_create(scr);
    lv_obj_set_size(g_dashRoot, 1024, 600);
    lv_obj_set_pos(g_dashRoot, 0, 0);
    lv_obj_set_style_bg_color(g_dashRoot, lv_color_hex(0x02050A), LV_PART_MAIN);
    lv_obj_set_style_bg_grad_color(g_dashRoot, lv_color_hex(0x172F1C), LV_PART_MAIN);
    lv_obj_set_style_bg_grad_dir(g_dashRoot, LV_GRAD_DIR_VER, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(g_dashRoot, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(g_dashRoot, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(g_dashRoot, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(g_dashRoot, 0, LV_PART_MAIN);
    lv_obj_clear_flag(g_dashRoot, LV_OBJ_FLAG_SCROLLABLE);

    const int CX_L = 256;
    const int CX_R = 768;
    const int CY   = 300;
    const int ARC_R = 230;

    // Turn Indicators (Top Center)
    g_leftTurnLbl = lv_label_create(g_dashRoot);
    lv_label_set_text(g_leftTurnLbl, LV_SYMBOL_LEFT);
    lv_obj_set_style_text_font(g_leftTurnLbl, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_style_text_color(g_leftTurnLbl, lv_color_hex(0x1A2230), LV_PART_MAIN); // Dim by default
    lv_obj_set_pos(g_leftTurnLbl, 512 - 112, 48);

    g_rightTurnLbl = lv_label_create(g_dashRoot);
    lv_label_set_text(g_rightTurnLbl, LV_SYMBOL_RIGHT);
    lv_obj_set_style_text_font(g_rightTurnLbl, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_style_text_color(g_rightTurnLbl, lv_color_hex(0x1A2230), LV_PART_MAIN);
    lv_obj_set_pos(g_rightTurnLbl, 512 + 112 - 30, 48);

    // ===== LEFT: RPM arc =====
    g_rpmArc = lv_arc_create(g_dashRoot);
    lv_obj_set_size(g_rpmArc, ARC_R * 2, ARC_R * 2);
    lv_obj_set_pos(g_rpmArc, CX_L - ARC_R, CY - ARC_R);
    lv_arc_set_rotation(g_rpmArc, 135);
    lv_arc_set_bg_angles(g_rpmArc, 0, 270);
    lv_arc_set_range(g_rpmArc, 0, 7000);
    lv_arc_set_value(g_rpmArc, 0);
    lv_obj_remove_style(g_rpmArc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(g_rpmArc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_width(g_rpmArc, 28, LV_PART_MAIN);
    lv_obj_set_style_arc_color(g_rpmArc, lv_color_hex(0x1D2B3C), LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(g_rpmArc, true, LV_PART_MAIN);
    lv_obj_set_style_arc_width(g_rpmArc, 28, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(g_rpmArc, lv_color_hex(0x47C0FF), LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(g_rpmArc, true, LV_PART_INDICATOR);
    lv_obj_set_style_shadow_color(g_rpmArc, lv_color_hex(0x47C0FF), LV_PART_INDICATOR);
    lv_obj_set_style_shadow_width(g_rpmArc, 18, LV_PART_INDICATOR);
    lv_obj_set_style_shadow_opa(g_rpmArc, LV_OPA_30, LV_PART_INDICATOR);

    // Big RPM number — child of arc, auto-centered
    g_speedLbl = lv_label_create(g_rpmArc);
    lv_label_set_text(g_speedLbl, "0");
    lv_obj_set_style_text_font(g_speedLbl, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_style_text_color(g_speedLbl, lv_color_hex(0xF7FBFF), LV_PART_MAIN);
    lv_obj_set_style_text_align(g_speedLbl, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(g_speedLbl, LV_ALIGN_CENTER, 0, -10);

    lv_obj_t* rpmCap = lv_label_create(g_rpmArc);
    lv_label_set_text(rpmCap, "RPM");
    lv_obj_set_style_text_font(rpmCap, &lv_font_montserrat_22, LV_PART_MAIN);
    lv_obj_set_style_text_color(rpmCap, lv_color_hex(0x6F86A4), LV_PART_MAIN);
    lv_obj_align(rpmCap, LV_ALIGN_CENTER, 0, 50);

    // ===== RIGHT: MPH arc =====
    g_mphArc = lv_arc_create(g_dashRoot);
    lv_obj_set_size(g_mphArc, ARC_R * 2, ARC_R * 2);
    lv_obj_set_pos(g_mphArc, CX_R - ARC_R, CY - ARC_R);
    lv_arc_set_rotation(g_mphArc, 135);
    lv_arc_set_bg_angles(g_mphArc, 0, 270);
    lv_arc_set_range(g_mphArc, 0, 160);
    lv_arc_set_value(g_mphArc, 0);
    lv_obj_remove_style(g_mphArc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(g_mphArc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_width(g_mphArc, 28, LV_PART_MAIN);
    lv_obj_set_style_arc_color(g_mphArc, lv_color_hex(0x1D2B3C), LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(g_mphArc, true, LV_PART_MAIN);
    lv_obj_set_style_arc_width(g_mphArc, 28, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(g_mphArc, lv_color_hex(0x68E2A0), LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(g_mphArc, true, LV_PART_INDICATOR);
    lv_obj_set_style_shadow_color(g_mphArc, lv_color_hex(0x68E2A0), LV_PART_INDICATOR);
    lv_obj_set_style_shadow_width(g_mphArc, 18, LV_PART_INDICATOR);
    lv_obj_set_style_shadow_opa(g_mphArc, LV_OPA_30, LV_PART_INDICATOR);

    g_distanceLbl = lv_label_create(g_mphArc);
    lv_label_set_text(g_distanceLbl, "0");
    lv_obj_set_style_text_font(g_distanceLbl, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_style_text_color(g_distanceLbl, lv_color_hex(0xF7FBFF), LV_PART_MAIN);
    lv_obj_set_style_text_align(g_distanceLbl, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(g_distanceLbl, LV_ALIGN_CENTER, 0, -10);

    lv_obj_t* mphCap = lv_label_create(g_mphArc);
    lv_label_set_text(mphCap, "MPH");
    lv_obj_set_style_text_font(mphCap, &lv_font_montserrat_22, LV_PART_MAIN);
    lv_obj_set_style_text_color(mphCap, lv_color_hex(0x6F86A4), LV_PART_MAIN);
    lv_obj_align(mphCap, LV_ALIGN_CENTER, 0, 50);

    // ===== Bottom-center FUEL bar =====
    const int FUEL_W = 420;
    const int FUEL_H = 26;
    const int FUEL_X = (1024 - FUEL_W) / 2;
    const int FUEL_Y = 548;

    lv_obj_t* fuelCap = lv_label_create(g_dashRoot);
    lv_label_set_text(fuelCap, LV_SYMBOL_CHARGE " FUEL");
    lv_obj_set_style_text_font(fuelCap, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_text_color(fuelCap, lv_color_hex(0x6F86A4), LV_PART_MAIN);
    lv_obj_set_pos(fuelCap, FUEL_X, FUEL_Y - 24);

    g_fuelLbl = lv_label_create(g_dashRoot);
    lv_label_set_text(g_fuelLbl, "-- %");
    lv_obj_set_style_text_font(g_fuelLbl, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_text_color(g_fuelLbl, lv_color_hex(0xEAF3FF), LV_PART_MAIN);
    lv_obj_set_pos(g_fuelLbl, FUEL_X + FUEL_W - 50, FUEL_Y - 24);

    g_fuelBar = lv_bar_create(g_dashRoot);
    lv_obj_set_size(g_fuelBar, FUEL_W, FUEL_H);
    lv_obj_set_pos(g_fuelBar, FUEL_X, FUEL_Y);
    lv_bar_set_range(g_fuelBar, 0, 100);
    lv_bar_set_value(g_fuelBar, 0, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(g_fuelBar, lv_color_hex(0x1A2230), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(g_fuelBar, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(g_fuelBar, FUEL_H / 2, LV_PART_MAIN);
    lv_obj_set_style_bg_color(g_fuelBar, lv_color_hex(0x68E2A0), LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_color(g_fuelBar, lv_color_hex(0x47C0FF), LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_dir(g_fuelBar, LV_GRAD_DIR_HOR, LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(g_fuelBar, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_radius(g_fuelBar, FUEL_H / 2, LV_PART_INDICATOR);

    g_warningStrip = lv_obj_create(g_dashRoot);
    lv_obj_set_size(g_warningStrip, 742, 38);
    lv_obj_set_pos(g_warningStrip, 141, 6);
    lv_obj_set_style_radius(g_warningStrip, 8, LV_PART_MAIN);
    lv_obj_set_style_bg_color(g_warningStrip, lv_color_hex(0x070A13), LV_PART_MAIN);
    lv_obj_set_style_bg_grad_color(g_warningStrip, lv_color_hex(0x1F1410), LV_PART_MAIN);
    lv_obj_set_style_bg_grad_dir(g_warningStrip, LV_GRAD_DIR_HOR, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(g_warningStrip, LV_OPA_80, LV_PART_MAIN);
    lv_obj_set_style_border_width(g_warningStrip, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(g_warningStrip, lv_color_hex(0x243246), LV_PART_MAIN);
    lv_obj_set_style_pad_all(g_warningStrip, 0, LV_PART_MAIN);
    lv_obj_clear_flag(g_warningStrip, LV_OBJ_FLAG_SCROLLABLE);

    static const char* kWarningIcons[8] = {
        FA_TACHOMETER,
        FA_OIL_CAN,
        FA_BOLT,
        FA_CAR,
        FA_WRENCH,
        FA_FIRE,
        FA_THERMOMETER,
        FA_CAR_SIDE
    };
    static const char* kWarningLabels[8] = {"CEL", "OIL", "BAT", "ABS", "AIR", "BRK", "TMP", "AWD"};
    for (int i = 0; i < 8; ++i) {
        g_warningLamp[i] = make_warning_lamp(g_warningStrip,
                                             10 + (i * 91),
                                             kWarningIcons[i],
                                             kWarningLabels[i],
                                             &g_warningIconLbl[i],
                                             &g_warningLampLbl[i]);
    }

    // Small clear/fault status plus transport detail.
    g_linkLbl = lv_label_create(g_dashRoot);
    lv_label_set_text(g_linkLbl, "SYSTEM CHECK");
    lv_obj_set_style_text_font(g_linkLbl, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_text_color(g_linkLbl, lv_color_hex(0x3D5C78), LV_PART_MAIN);
    lv_obj_set_pos(g_linkLbl, 732, 548);
    lv_obj_set_width(g_linkLbl, 268);
    lv_obj_set_style_text_align(g_linkLbl, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN);

    g_transportLbl = lv_label_create(g_dashRoot);
    lv_label_set_text(g_transportLbl, "SEARCHING");
    lv_obj_set_style_text_font(g_transportLbl, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(g_transportLbl, lv_color_hex(0x6F86A4), LV_PART_MAIN);
    lv_obj_set_pos(g_transportLbl, 732, 570);
    lv_obj_set_width(g_transportLbl, 268);
    lv_obj_set_style_text_align(g_transportLbl, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN);

    g_dashboardBootMs = millis();
}

static void custom_dashboard_build_legacy_unused(lv_obj_t* scr) {
    // (legacy obstacle/scan/etc UI kept here for reference, unreachable)
    (void)scr;
    g_dashRoot = lv_obj_create(scr);
    lv_obj_set_size(g_dashRoot, 1024, 600);
    lv_obj_set_pos(g_dashRoot, 0, 0);
    lv_obj_set_style_bg_color(g_dashRoot, lv_color_hex(0x08111A), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(g_dashRoot, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(g_dashRoot, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(g_dashRoot, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(g_dashRoot, 0, LV_PART_MAIN);
    lv_obj_clear_flag(g_dashRoot, LV_OBJ_FLAG_SCROLLABLE);

    g_topBar = make_panel(g_dashRoot, 24, 18, 976, 58, lv_color_hex(0x0E1722), lv_color_hex(0x22344A), 20);

    lv_obj_t* title = lv_label_create(g_topBar);
    lv_label_set_text(title, "CARPLAY CLUSTER");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_text_color(title, lv_color_hex(0xEAF3FF), LV_PART_MAIN);
    lv_obj_set_pos(title, 18, 10);

    lv_obj_t* subtitle = lv_label_create(g_topBar);
    lv_label_set_text(subtitle, "native pilot hud");
    lv_obj_set_style_text_font(subtitle, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(subtitle, lv_color_hex(0x6F86A4), LV_PART_MAIN);
    lv_obj_set_pos(subtitle, 18, 30);

    g_leftSignalChip = make_chip(g_topBar, 492, 8, 74, LV_SYMBOL_LEFT " L");
    g_headChip = make_chip(g_topBar, 576, 8, 74, LV_SYMBOL_EYE_OPEN " H");
    g_brakeChip = make_chip(g_topBar, 660, 8, 82, LV_SYMBOL_WARNING " B");
    g_rightSignalChip = make_chip(g_topBar, 752, 8, 82, LV_SYMBOL_RIGHT " R");

    g_linkLbl = lv_label_create(g_topBar);
    lv_label_set_text(g_linkLbl, LV_SYMBOL_WIFI " SEARCH");
    lv_obj_set_style_text_font(g_linkLbl, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_text_color(g_linkLbl, lv_color_hex(0x90A7C4), LV_PART_MAIN);
    lv_obj_set_pos(g_linkLbl, 850, 20);

    g_driveCard = make_panel(g_dashRoot, 24, 96, 236, 480, lv_color_hex(0x0D1622), lv_color_hex(0x22344A), 30);
    g_obstacleCard = make_panel(g_dashRoot, 280, 96, 720, 480, lv_color_hex(0x0D1622), lv_color_hex(0x22344A), 30);

    lv_obj_t* driveEyebrow = lv_label_create(g_driveCard);
    lv_label_set_text(driveEyebrow, "PILOT");
    lv_obj_set_style_text_font(driveEyebrow, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(driveEyebrow, lv_color_hex(0x6F86A4), LV_PART_MAIN);
    lv_obj_set_pos(driveEyebrow, 22, 22);

    lv_obj_t* speedCaption = lv_label_create(g_driveCard);
    lv_label_set_text(speedCaption, "Velocity");
    lv_obj_set_style_text_font(speedCaption, &lv_font_montserrat_22, LV_PART_MAIN);
    lv_obj_set_style_text_color(speedCaption, lv_color_hex(0xEAF3FF), LV_PART_MAIN);
    lv_obj_set_pos(speedCaption, 22, 44);

    g_speedLbl = lv_label_create(g_driveCard);
    lv_label_set_text(g_speedLbl, "--");
    lv_obj_set_style_text_font(g_speedLbl, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_style_text_color(g_speedLbl, lv_color_hex(0xF7FBFF), LV_PART_MAIN);
    lv_obj_set_pos(g_speedLbl, 22, 100);

    lv_obj_t* speedUnit = lv_label_create(g_driveCard);
    lv_label_set_text(speedUnit, "RPM");
    lv_obj_set_style_text_font(speedUnit, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_text_color(speedUnit, lv_color_hex(0x7CA2C8), LV_PART_MAIN);
    lv_obj_set_pos(speedUnit, 30, 168);

    lv_obj_t* gearBadge = make_panel(g_driveCard, 124, 86, 88, 88, lv_color_hex(0x101D2B), lv_color_hex(0x334A67), 24);
    g_gearLbl = lv_label_create(gearBadge);
    lv_label_set_text(g_gearLbl, "D");
    lv_obj_set_style_text_font(g_gearLbl, &lv_font_montserrat_40, LV_PART_MAIN);
    lv_obj_set_style_text_color(g_gearLbl, lv_color_hex(0xEAF3FF), LV_PART_MAIN);
    lv_obj_center(g_gearLbl);

    lv_obj_t* distanceCaption = lv_label_create(g_driveCard);
    lv_label_set_text(distanceCaption, "Range gate");
    lv_obj_set_style_text_font(distanceCaption, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_text_color(distanceCaption, lv_color_hex(0x6F86A4), LV_PART_MAIN);
    lv_obj_set_pos(distanceCaption, 22, 224);

    g_distanceLbl = lv_label_create(g_driveCard);
    lv_label_set_text(g_distanceLbl, "-- in");
    lv_obj_set_style_text_font(g_distanceLbl, &lv_font_montserrat_40, LV_PART_MAIN);
    lv_obj_set_style_text_color(g_distanceLbl, lv_color_hex(0xFFDDD8), LV_PART_MAIN);
    lv_obj_set_pos(g_distanceLbl, 22, 246);

    g_statusPill = make_panel(g_driveCard, 22, 310, 110, 38, lv_color_hex(0x14202C), lv_color_hex(0x2B4258), 19);
    g_statusPillLbl = lv_label_create(g_statusPill);
    lv_label_set_text(g_statusPillLbl, "SEARCH");
    lv_obj_set_style_text_font(g_statusPillLbl, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(g_statusPillLbl, lv_color_hex(0x9CB4D0), LV_PART_MAIN);
    lv_obj_center(g_statusPillLbl);

    g_rangeTrack = make_panel(g_driveCard, 22, 366, 192, 16, lv_color_hex(0x13202F), lv_color_hex(0x13202F), 8);
    g_rangeFill = lv_obj_create(g_rangeTrack);
    lv_obj_set_size(g_rangeFill, 28, 16);
    lv_obj_set_pos(g_rangeFill, 0, 0);
    lv_obj_set_style_bg_color(g_rangeFill, lv_color_hex(0x47C0FF), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(g_rangeFill, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(g_rangeFill, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(g_rangeFill, 8, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(g_rangeFill, lv_color_hex(0x47C0FF), LV_PART_MAIN);
    lv_obj_set_style_shadow_width(g_rangeFill, 16, LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(g_rangeFill, LV_OPA_30, LV_PART_MAIN);
    lv_obj_clear_flag(g_rangeFill, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* rangeMin = lv_label_create(g_driveCard);
    lv_label_set_text(rangeMin, "CLEAR");
    lv_obj_set_style_text_font(rangeMin, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(rangeMin, lv_color_hex(0x617790), LV_PART_MAIN);
    lv_obj_set_pos(rangeMin, 22, 388);

    lv_obj_t* rangeMax = lv_label_create(g_driveCard);
    lv_label_set_text(rangeMax, "NEAR");
    lv_obj_set_style_text_font(rangeMax, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(rangeMax, lv_color_hex(0x617790), LV_PART_MAIN);
    lv_obj_set_pos(rangeMax, 162, 388);

    lv_obj_t* obstacleEyebrow = lv_label_create(g_obstacleCard);
    lv_label_set_text(obstacleEyebrow, "FORWARD SCAN");
    lv_obj_set_style_text_font(obstacleEyebrow, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(obstacleEyebrow, lv_color_hex(0x6F86A4), LV_PART_MAIN);
    lv_obj_set_pos(obstacleEyebrow, 24, 22);

    lv_obj_t* obstacleTitle = lv_label_create(g_obstacleCard);
    lv_label_set_text(obstacleTitle, "Range scan");
    lv_obj_set_style_text_font(obstacleTitle, &lv_font_montserrat_22, LV_PART_MAIN);
    lv_obj_set_style_text_color(obstacleTitle, lv_color_hex(0xEAF3FF), LV_PART_MAIN);
    lv_obj_set_pos(obstacleTitle, 24, 44);

    g_obstacleBadge = make_panel(g_obstacleCard, 548, 28, 146, 38, lv_color_hex(0x14202C), lv_color_hex(0x2B4258), 19);
    g_obstacleBadgeLbl = lv_label_create(g_obstacleBadge);
    lv_label_set_text(g_obstacleBadgeLbl, "NO TARGET");
    lv_obj_set_style_text_font(g_obstacleBadgeLbl, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(g_obstacleBadgeLbl, lv_color_hex(0x9CB4D0), LV_PART_MAIN);
    lv_obj_center(g_obstacleBadgeLbl);

    g_obstacleField = make_panel(g_obstacleCard, 24, 88, 672, 274, lv_color_hex(0x09111A), lv_color_hex(0x1F3247), 24);

    for (int i = 0; i < 5; ++i) {
        lv_obj_t* lane = lv_obj_create(g_obstacleField);
        lv_obj_set_size(lane, 2, 224);
        lv_obj_set_pos(lane, 108 + (i * 110), 24);
        lv_obj_set_style_bg_color(lane, lv_color_hex(0x112333), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(lane, LV_OPA_50, LV_PART_MAIN);
        lv_obj_set_style_border_width(lane, 0, LV_PART_MAIN);
        lv_obj_set_style_radius(lane, 1, LV_PART_MAIN);
        lv_obj_clear_flag(lane, LV_OBJ_FLAG_SCROLLABLE);
    }

    lv_obj_t* horizon = lv_obj_create(g_obstacleField);
    lv_obj_set_size(horizon, 620, 2);
    lv_obj_set_pos(horizon, 24, 64);
    lv_obj_set_style_bg_color(horizon, lv_color_hex(0x112333), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(horizon, LV_OPA_50, LV_PART_MAIN);
    lv_obj_set_style_border_width(horizon, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(horizon, 1, LV_PART_MAIN);
    lv_obj_clear_flag(horizon, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* lowerHorizon = lv_obj_create(g_obstacleField);
    lv_obj_set_size(lowerHorizon, 620, 2);
    lv_obj_set_pos(lowerHorizon, 24, 194);
    lv_obj_set_style_bg_color(lowerHorizon, lv_color_hex(0x102132), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(lowerHorizon, LV_OPA_40, LV_PART_MAIN);
    lv_obj_set_style_border_width(lowerHorizon, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(lowerHorizon, 1, LV_PART_MAIN);
    lv_obj_clear_flag(lowerHorizon, LV_OBJ_FLAG_SCROLLABLE);

    g_scanLine = lv_obj_create(g_obstacleField);
    lv_obj_set_size(g_scanLine, 620, 2);
    lv_obj_set_pos(g_scanLine, 24, 116);
    lv_obj_set_style_bg_color(g_scanLine, lv_color_hex(0x58D5FF), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(g_scanLine, LV_OPA_60, LV_PART_MAIN);
    lv_obj_set_style_border_width(g_scanLine, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(g_scanLine, 1, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(g_scanLine, lv_color_hex(0x58D5FF), LV_PART_MAIN);
    lv_obj_set_style_shadow_width(g_scanLine, 18, LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(g_scanLine, LV_OPA_30, LV_PART_MAIN);
    lv_obj_clear_flag(g_scanLine, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* farLbl = lv_label_create(g_obstacleField);
    lv_label_set_text(farLbl, "FAR");
    lv_obj_set_style_text_font(farLbl, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(farLbl, lv_color_hex(0x6F86A4), LV_PART_MAIN);
    lv_obj_set_pos(farLbl, 610, 18);

    lv_obj_t* nearLbl = lv_label_create(g_obstacleField);
    lv_label_set_text(nearLbl, "BUMPER");
    lv_obj_set_style_text_font(nearLbl, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(nearLbl, lv_color_hex(0x6F86A4), LV_PART_MAIN);
    lv_obj_set_pos(nearLbl, 586, 236);

    g_vehicleMarker = make_panel(g_obstacleField, 304, 220, 64, 34, lv_color_hex(0x16E4F2), lv_color_hex(0x71F2FF), 17);
    lv_obj_t* vehicleLbl = lv_label_create(g_vehicleMarker);
    lv_label_set_text(vehicleLbl, "YOU");
    lv_obj_set_style_text_font(vehicleLbl, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(vehicleLbl, lv_color_hex(0x051116), LV_PART_MAIN);
    lv_obj_center(vehicleLbl);

    lv_obj_t* tempPanel = make_panel(g_obstacleCard, 24, 418, 152, 42, lv_color_hex(0x101A26), lv_color_hex(0x22344A), 16);
    g_tempLbl = lv_label_create(tempPanel);
    lv_label_set_text(g_tempLbl, "TEMP -- F");
    lv_obj_set_style_text_font(g_tempLbl, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_text_color(g_tempLbl, lv_color_hex(0xEAF3FF), LV_PART_MAIN);
    lv_obj_set_pos(g_tempLbl, 14, 12);

    lv_obj_t* humPanel = make_panel(g_obstacleCard, 188, 418, 152, 42, lv_color_hex(0x101A26), lv_color_hex(0x22344A), 16);
    g_humLbl = lv_label_create(humPanel);
    lv_label_set_text(g_humLbl, "HUM --%");
    lv_obj_set_style_text_font(g_humLbl, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_text_color(g_humLbl, lv_color_hex(0xEAF3FF), LV_PART_MAIN);
    lv_obj_set_pos(g_humLbl, 14, 12);

    lv_obj_t* rxPanel = make_panel(g_obstacleCard, 352, 418, 152, 42, lv_color_hex(0x101A26), lv_color_hex(0x22344A), 16);
    g_rxLbl = lv_label_create(rxPanel);
    lv_label_set_text(g_rxLbl, "RX 0");
    lv_obj_set_style_text_font(g_rxLbl, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_text_color(g_rxLbl, lv_color_hex(0xEAF3FF), LV_PART_MAIN);
    lv_obj_set_pos(g_rxLbl, 14, 12);

    lv_obj_t* seqPanel = make_panel(g_obstacleCard, 516, 418, 180, 42, lv_color_hex(0x101A26), lv_color_hex(0x22344A), 16);
    g_seqLbl = lv_label_create(seqPanel);
    lv_label_set_text(g_seqLbl, "SEQ 0");
    lv_obj_set_style_text_font(g_seqLbl, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_text_color(g_seqLbl, lv_color_hex(0xEAF3FF), LV_PART_MAIN);
    lv_obj_set_pos(g_seqLbl, 14, 12);
}

static float lvgl_studio_demo_speed_mph() {
    uint32_t ms = static_cast<uint32_t>(esp_timer_get_time() / 1000ULL);
    static uint32_t t0 = 0;
    if (t0 == 0) t0 = ms;
    const uint32_t t = ms - t0;
    return static_cast<float>(25 + ((t / 100) % 75));
}

static void handle_sensor_packet(const uint8_t* data, int len, uint8_t source) {
    if (len != static_cast<int>(sizeof(SensorPacket))) return;

    SensorPacket pkt;
    memcpy(&pkt, data, sizeof(pkt));
    if (pkt.magic != PACKET_MAGIC) return;

    const uint32_t now = millis();
    if (source == 1) {
        g_wifiRxCount = g_wifiRxCount + 1;
        g_lastWifiRxMs = now;
    } else if (source == 2) {
        g_uartRxCount = g_uartRxCount + 1;
        g_lastUartRxMs = now;
    }

    // ESP-NOW and UART can deliver the same C3 packet. Count source health for
    // both, but only advance the dashboard/data counters once per sequence.
    if (g_lastSeq != 0 && pkt.seq <= g_lastSeq) {
        g_lastRxMs = now;
        g_lastRxSource = source;
        return;
    }

    g_distanceCm = pkt.distanceCm;
    g_tempC = pkt.tempC;
    g_humidity = pkt.humidity;
    g_lightFlags = pkt.lights;
    g_rpm = pkt.rpm;
    g_mph = pkt.mph;
    g_fuelPct = pkt.fuelPct;
    g_statusFlags = pkt.statusFlags;
    if (g_lastSeq != 0 && pkt.seq > g_lastSeq + 1) {
        g_dropCount += (pkt.seq - g_lastSeq - 1);
    }
    g_lastSeq = pkt.seq;
    g_rxCount = g_rxCount + 1;
    g_lastRxMs = now;
    g_lastRxSource = source;
}

static void on_espnow_recv(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
    (void)info;
    handle_sensor_packet(data, len, 1);
}

static void espnow_begin() {
    // Bring up WiFi via Arduino layer (still needed for esp-event registration)
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    delay(50);

    // Standard 11b/g/n protocol — LR mode caused drift in testing.
    esp_wifi_set_protocol(WIFI_IF_STA,
        WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N);

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW init failed");
        return;
    }
    esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_ps(WIFI_PS_NONE);
    esp_now_register_recv_cb(on_espnow_recv);
    Serial.printf("ESP-NOW ready  channel=%u  MAC=%s\n", ESPNOW_CHANNEL, WiFi.macAddress().c_str());
}

static void update_obstacle_field(float closeness, uint32_t now_ms, bool linked) {
    (void)closeness;
    (void)now_ms;
    (void)linked;
}

static void dashboard_tick(uint32_t now_ms) {
    char buf[56];
    const uint32_t link_age_ms = (g_lastRxMs == 0) ? UINT32_MAX : (now_ms - g_lastRxMs);
    // Hysteresis: only flip to "disconnected" after 5s of no packets, not 3s.
    // Brief 1-2s blips look like blinks otherwise.
    bool linked = link_age_ms < 5000;

    uint8_t flags = g_statusFlags;
    bool tach_locked    = linked && (flags & SENSOR_TACH_LOCKED);
    bool vss_active     = linked && (flags & SENSOR_VSS_ACTIVE);
    bool fuel_connected = linked && (flags & SENSOR_FUEL_CONNECTED);

    float rpm_target = tach_locked    ? static_cast<float>(g_rpm)     : 0.0f;
    float mph_target = vss_active     ? static_cast<float>(g_mph)     : 0.0f;
    float fuel_target = fuel_connected ? static_cast<float>(g_fuelPct) : 0.0f;
    uint8_t active_lightFlags = g_lightFlags;
    const uint32_t boot_elapsed_ms = (g_dashboardBootMs == 0) ? 99999U : (now_ms - g_dashboardBootMs);
    const bool booting = boot_elapsed_ms < 2200U;
    const bool lamp_test = boot_elapsed_ms < 1250U;

#if DEMO_MODE
    linked = true;
    tach_locked = true;
    vss_active = true;
    fuel_connected = true;
    const float demo_t = now_ms / 1000.0f;
    rpm_target = 3500.0f + sinf(demo_t * 1.5f) * 2000.0f;
    mph_target = 65.0f + cosf(demo_t * 0.8f) * 15.0f;
    fuel_target = 75.0f + sinf(demo_t * 0.2f) * 10.0f;
    
    // Blink turn signals alternating every second
    if (static_cast<int>(demo_t) % 2 == 0) {
        active_lightFlags = LIGHT_LEFT;
    } else {
        active_lightFlags = LIGHT_RIGHT;
    }
#endif

    if (booting) {
        tach_locked = true;
        vss_active = true;
        fuel_connected = true;
        const float p = clampf(static_cast<float>(boot_elapsed_ms) / 2200.0f, 0.0f, 1.0f);
        if (p < 0.58f) {
            const float up = ease_in_out(p / 0.58f);
            rpm_target = 6800.0f * up;
            mph_target = 160.0f * up;
            fuel_target = 100.0f * up;
        } else {
            const float down = ease_in_out((p - 0.58f) / 0.42f);
            rpm_target = mixf(6800.0f, rpm_target, down);
            mph_target = mixf(160.0f, mph_target, down);
            fuel_target = mixf(100.0f, fuel_target, down);
        }
    }

    if (!g_haveUiFilter || booting) {
        g_smoothRpm = rpm_target;
        g_smoothMph = mph_target;
        g_smoothFuel = fuel_target;
        g_haveUiFilter = true;
    } else {
        // Snappy smoothing: high alpha tracks live packets closely without
        // making the gauges look stepped at 20 Hz input.
        g_smoothRpm = smooth_step(g_smoothRpm, rpm_target, 0.48f);
        g_smoothMph = smooth_step(g_smoothMph, mph_target, 0.48f);
        g_smoothFuel = smooth_step(g_smoothFuel, fuel_target, 0.24f);
    }

    const int rpm_display = static_cast<int>(g_smoothRpm + 0.5f);
    const int mph_display = static_cast<int>(g_smoothMph + 0.5f);

    // RPM number — gray "--" if no tach lock, else live value with redline color
    if (g_speedLbl) {
        if (tach_locked) {
            snprintf(buf, sizeof(buf), "%d", rpm_display);
            lv_label_set_text(g_speedLbl, buf);
            uint32_t color = 0xF7FBFF;
            if      (rpm_display > 6000) color = 0xFF4D4D;
            else if (rpm_display > 5000) color = 0xFFB347;
            lv_obj_set_style_text_color(g_speedLbl, lv_color_hex(color), LV_PART_MAIN);
        } else {
            lv_label_set_text(g_speedLbl, "--");
            lv_obj_set_style_text_color(g_speedLbl, lv_color_hex(0x3D5C78), LV_PART_MAIN);
        }
    }
    if (g_rpmArc) {
        int v = rpm_display > 7000 ? 7000 : (rpm_display < 0 ? 0 : rpm_display);
        lv_arc_set_value(g_rpmArc, v);
        uint32_t arcColor;
        if (!tach_locked)                arcColor = 0x1F2D3F;  // dim gray when no signal
        else if (rpm_display > 6000)     arcColor = 0xFF4D4D;
        else if (rpm_display > 5000)     arcColor = 0xFFB347;
        else                              arcColor = 0x47C0FF;
        lv_obj_set_style_arc_color(g_rpmArc, lv_color_hex(arcColor), LV_PART_INDICATOR);
        lv_obj_set_style_shadow_color(g_rpmArc, lv_color_hex(arcColor), LV_PART_INDICATOR);
    }

    // MPH number — gray "--" if no VSS signal, else live value
    if (g_distanceLbl) {
        if (vss_active) {
            snprintf(buf, sizeof(buf), "%d", mph_display);
            lv_label_set_text(g_distanceLbl, buf);
            lv_obj_set_style_text_color(g_distanceLbl, lv_color_hex(0xF7FBFF), LV_PART_MAIN);
        } else {
            lv_label_set_text(g_distanceLbl, "--");
            lv_obj_set_style_text_color(g_distanceLbl, lv_color_hex(0x3D5C78), LV_PART_MAIN);
        }
    }
    if (g_mphArc) {
        int v = mph_display > 160 ? 160 : (mph_display < 0 ? 0 : mph_display);
        lv_arc_set_value(g_mphArc, v);
        const uint32_t mphColor = vss_active ? 0x68E2A0 : 0x1F2D3F;
        lv_obj_set_style_arc_color(g_mphArc, lv_color_hex(mphColor), LV_PART_INDICATOR);
        lv_obj_set_style_shadow_color(g_mphArc, lv_color_hex(mphColor), LV_PART_INDICATOR);
    }

    // Fuel bar — smoothed on the receiver so packet steps feel less digital.
    const int fuel_display = static_cast<int>(g_smoothFuel + 0.5f);
    if (g_fuelBar) {
        if (fuel_connected) {
            lv_bar_set_value(g_fuelBar, fuel_display, LV_ANIM_OFF);
            uint32_t fc = 0x68E2A0;
            if      (fuel_display < 10) fc = 0xFF4D4D;
            else if (fuel_display < 25) fc = 0xFFB347;
            lv_obj_set_style_bg_color(g_fuelBar, lv_color_hex(fc), LV_PART_INDICATOR);
        } else {
            lv_bar_set_value(g_fuelBar, 0, LV_ANIM_OFF);
            lv_obj_set_style_bg_color(g_fuelBar, lv_color_hex(0x1F2D3F), LV_PART_INDICATOR);
        }
    }
    if (g_fuelLbl) {
        if (fuel_connected) {
            snprintf(buf, sizeof(buf), "%d%%", fuel_display);
            lv_label_set_text(g_fuelLbl, buf);
            lv_obj_set_style_text_color(g_fuelLbl, lv_color_hex(0xEAF3FF), LV_PART_MAIN);
        } else {
            lv_label_set_text(g_fuelLbl, "-- %");
            lv_obj_set_style_text_color(g_fuelLbl, lv_color_hex(0x3D5C78), LV_PART_MAIN);
        }
    }

    // ---- Link health indicator ----
    // Track inter-packet GAP (not "age" — which jitters between renders).
    // Update label at fixed cadence so the user sees a stable readout.
    static uint32_t s_lastSeenRxMs   = 0;    // mirror of g_lastRxMs from previous tick
    static float    s_avgGapMs       = 0.0f; // EMA of inter-packet interval
    static uint32_t s_maxGapMs       = 0;
    static uint32_t s_lastLabelMs    = 0;
    static uint32_t s_pktsThisWindow = 0;

    const uint32_t lastRx = g_lastRxMs;
    if (lastRx != 0 && lastRx != s_lastSeenRxMs) {
        const uint32_t gap = (s_lastSeenRxMs == 0) ? 0 : (lastRx - s_lastSeenRxMs);
        s_lastSeenRxMs = lastRx;
        s_pktsThisWindow++;
        if (gap > 0 && gap < 5000) {
            if (s_avgGapMs < 0.5f) s_avgGapMs = static_cast<float>(gap);
            else                    s_avgGapMs += 0.20f * (static_cast<float>(gap) - s_avgGapMs);
            if (gap > s_maxGapMs) s_maxGapMs = gap;
        }
    }

    // Refresh label only every 500ms = no flicker
    if (g_linkLbl && (now_ms - s_lastLabelMs) > 500) {
        s_lastLabelMs = now_ms;
        const bool wifi_recent = (g_lastWifiRxMs != 0) && ((now_ms - g_lastWifiRxMs) < 1200);
        const bool uart_recent = (g_lastUartRxMs != 0) && ((now_ms - g_lastUartRxMs) < 1200);
        if (!linked) {
            snprintf(buf, sizeof(buf), "SYSTEM CHECK");
            lv_label_set_text(g_linkLbl, buf);
            lv_obj_set_style_text_color(g_linkLbl, lv_color_hex(0x3D5C78), LV_PART_MAIN);
            if (g_transportLbl) {
                lv_label_set_text(g_transportLbl, "SEARCHING");
                lv_obj_set_style_text_color(g_transportLbl, lv_color_hex(0x6F86A4), LV_PART_MAIN);
            }
            if (g_warningStrip) {
                lv_obj_set_style_border_color(g_warningStrip, lv_color_hex(0x243246), LV_PART_MAIN);
            }
        } else {
            const int avg = static_cast<int>(s_avgGapMs + 0.5f);
            const int hz = (avg > 0) ? (1000 / avg) : 0;
            const char* source = "LINK";
            if (wifi_recent && uart_recent) source = "BOTH";
            else if (uart_recent)           source = "UART";
            else if (wifi_recent)           source = "WIFI";
            // Color by jitter quality: avg<60ms = green, <200ms = amber, >=200ms = red
            uint32_t color = 0x68E2A0;
            if      (avg >= 200) color = 0xFF4D4D;
            else if (avg >=  60) color = 0xFFB347;
            snprintf(buf, sizeof(buf), "%s", (wifi_recent && uart_recent) ? "ALL SYSTEMS CLEAR" : "LINK DEGRADED");
            lv_label_set_text(g_linkLbl, buf);
            lv_obj_set_style_text_color(g_linkLbl, lv_color_hex((wifi_recent && uart_recent) ? 0x68E2A0 : 0xFFB347), LV_PART_MAIN);
            if (g_transportLbl) {
                if (uart_recent) {
                    snprintf(buf, sizeof(buf), wifi_recent ? "UART + WIFI" : "UART");
                } else {
                    snprintf(buf, sizeof(buf), "%s %dHz %dms", source, hz, avg);
                }
                lv_label_set_text(g_transportLbl, buf);
                lv_obj_set_style_text_color(g_transportLbl, lv_color_hex(color), LV_PART_MAIN);
            }
            if (g_warningStrip) {
                lv_obj_set_style_border_color(g_warningStrip,
                    lv_color_hex((wifi_recent && uart_recent) ? 0x2F6F55 : 0x705A24), LV_PART_MAIN);
            }
            // Decay max over time so a single spike doesn't dominate forever
            if (s_maxGapMs > 0) s_maxGapMs = (s_maxGapMs * 9) / 10;
        }
        s_pktsThisWindow = 0;
    }

    const lv_color_t red = lv_color_hex(0x44161A);
    const lv_color_t red_fg = lv_color_hex(0xFF5C5C);
    const lv_color_t amber = lv_color_hex(0x3B2A12);
    const lv_color_t amber_fg = lv_color_hex(0xFFC15B);
    for (uint8_t i = 0; i < 8; ++i) {
        const bool active = lamp_test;
        set_warning_lamp(i, active, (i == 1 || i == 2 || i == 5) ? red : amber,
                         (i == 1 || i == 2 || i == 5) ? red_fg : amber_fg);
    }

    if (g_leftTurnLbl && g_rightTurnLbl) {
        const bool hazard_on = (active_lightFlags & LIGHT_HAZARD) != 0;
        const bool blink_phase = ((now_ms / 520U) % 2U) == 0U;
        const bool left_on = lamp_test || (hazard_on ? blink_phase : (((active_lightFlags & LIGHT_LEFT) != 0) && blink_phase));
        const bool right_on = lamp_test || (hazard_on ? blink_phase : (((active_lightFlags & LIGHT_RIGHT) != 0) && blink_phase));
        lv_obj_set_style_text_color(g_leftTurnLbl, lv_color_hex(left_on ? 0xFFC15B : 0x1A2230), LV_PART_MAIN);
        lv_obj_set_style_text_color(g_rightTurnLbl, lv_color_hex(right_on ? 0xFFC15B : 0x1A2230), LV_PART_MAIN);
    }
}

static void dashboard_tick_legacy_unused(uint32_t now_ms) {
    char buf[48];
    const float distance_cm = g_distanceCm;
    const float temp_c = g_tempC;
    const float humidity = g_humidity;
    const int demo_speed = static_cast<int>(roundf(lvgl_studio_demo_speed_mph()));
    const uint32_t link_age_ms = (g_lastRxMs == 0) ? UINT32_MAX : (now_ms - g_lastRxMs);
    const bool has_valid_sample = !std::isnan(distance_cm) && !std::isnan(temp_c) && !std::isnan(humidity);
    const bool linked = has_valid_sample && link_age_ms < 3000;

    if (has_valid_sample) {
        if (!g_haveFilter) {
            g_filtDistCm = distance_cm;
            g_filtTempC = temp_c;
            g_filtHum = humidity;
            g_haveFilter = true;
        } else {
            g_filtDistCm += (distance_cm - g_filtDistCm) * 0.24f;
            g_filtTempC += (temp_c - g_filtTempC) * 0.18f;
            g_filtHum += (humidity - g_filtHum) * 0.18f;
        }
    }

    const uint16_t live_rpm = g_rpm;
    const uint32_t rx_age = link_age_ms;
    const int rpm_display = (rx_age < 3000) ? static_cast<int>(live_rpm) : 0;
    snprintf(buf, sizeof(buf), "%d", rpm_display);
    if (g_speedLbl) lv_label_set_text(g_speedLbl, buf);
    (void)demo_speed;
    if (g_gearLbl) lv_label_set_text(g_gearLbl, "D");

    if ((linked || (g_haveFilter && link_age_ms < 8000)) && g_haveFilter) {
        if (!g_haveUiFilter) {
            g_uiDistCm = g_filtDistCm;
            g_uiTempC = g_filtTempC;
            g_uiHum = g_filtHum;
            g_haveUiFilter = true;
        } else {
            g_uiDistCm = smooth_step(g_uiDistCm, g_filtDistCm, 0.18f);
            g_uiTempC = smooth_step(g_uiTempC, g_filtTempC, 0.14f);
            g_uiHum = smooth_step(g_uiHum, g_filtHum, 0.14f);
        }
    } else {
        g_haveUiFilter = false;
    }

    const bool show_live = g_haveUiFilter && (linked || link_age_ms < 8000);
    const float active_distance = show_live ? g_uiDistCm : NAN;
    const float closeness = distance_closeness(active_distance);
    apply_distance_theme(active_distance);

    if (show_live) {
        const int temp_f = static_cast<int>(roundf((g_uiTempC * 9.0f / 5.0f) + 32.0f));
        const float dist_in = g_uiDistCm / 2.54f;
        snprintf(buf, sizeof(buf), "%.1f in", dist_in);
        if (g_distanceLbl) lv_label_set_text(g_distanceLbl, buf);

        const lv_coord_t fill_width = static_cast<lv_coord_t>(roundf(24.0f + closeness * 168.0f));
        if (g_rangeFill) {
            lv_obj_set_width(g_rangeFill, fill_width);
            lv_obj_set_style_bg_color(g_rangeFill, mix_color(lv_color_hex(0x44BFFF), lv_color_hex(0xFF6A4F), closeness), LV_PART_MAIN);
        }

        if (closeness > 0.78f) {
            set_badge(g_statusPill, g_statusPillLbl, LV_SYMBOL_WARNING " STOP", lv_color_hex(0x33171A), lv_color_hex(0xFF8A7A));
            set_badge(g_obstacleBadge, g_obstacleBadgeLbl, LV_SYMBOL_WARNING " CLOSE", lv_color_hex(0x33171A), lv_color_hex(0xFF8A7A));
        } else if (closeness > 0.38f) {
            set_badge(g_statusPill, g_statusPillLbl, LV_SYMBOL_WARNING " CAUTION", lv_color_hex(0x352814), lv_color_hex(0xFFC46E));
            set_badge(g_obstacleBadge, g_obstacleBadgeLbl, LV_SYMBOL_DOWN " APPROACH", lv_color_hex(0x352814), lv_color_hex(0xFFC46E));
        } else {
            set_badge(g_statusPill, g_statusPillLbl, LV_SYMBOL_OK " CLEAR", lv_color_hex(0x15281E), lv_color_hex(0x67E2A0));
            set_badge(g_obstacleBadge, g_obstacleBadgeLbl, LV_SYMBOL_OK " OPEN", lv_color_hex(0x15281E), lv_color_hex(0x67E2A0));
        }

        snprintf(buf, sizeof(buf), "TEMP %d F", temp_f);
        if (g_tempLbl) lv_label_set_text(g_tempLbl, buf);
        snprintf(buf, sizeof(buf), "HUM %d%%", static_cast<int>(roundf(clampf(g_uiHum, 0.0f, 100.0f))));
        if (g_humLbl) lv_label_set_text(g_humLbl, buf);
    } else {
        if (g_distanceLbl) lv_label_set_text(g_distanceLbl, "-- in");
        if (g_rangeFill) {
            lv_obj_set_width(g_rangeFill, 40);
            lv_obj_set_style_bg_color(g_rangeFill, lv_color_hex(0x3D5C78), LV_PART_MAIN);
        }
        set_badge(g_statusPill, g_statusPillLbl, LV_SYMBOL_WIFI " SEARCH", lv_color_hex(0x14202C), lv_color_hex(0x9CB4D0));
        set_badge(g_obstacleBadge, g_obstacleBadgeLbl, LV_SYMBOL_CLOSE " NO TARGET", lv_color_hex(0x14202C), lv_color_hex(0x9CB4D0));
        if (g_tempLbl) lv_label_set_text(g_tempLbl, "TEMP -- F");
        if (g_humLbl) lv_label_set_text(g_humLbl, "HUM --%");
    }

    update_obstacle_field(closeness, now_ms, show_live);

    if (g_scanLine) {
        const uint32_t sweep_ms = now_ms % 3200U;
        const lv_coord_t y = static_cast<lv_coord_t>(20 + ((sweep_ms / 3200.0f) * 214.0f));
        lv_obj_set_y(g_scanLine, y);
        lv_obj_set_style_bg_opa(g_scanLine, show_live ? static_cast<lv_opa_t>(clampf(30.0f + closeness * 90.0f, 24.0f, 120.0f)) : LV_OPA_20, LV_PART_MAIN);
    }

    snprintf(buf, sizeof(buf), "RX %lu", static_cast<unsigned long>(g_rxCount));
    if (g_rxLbl) lv_label_set_text(g_rxLbl, buf);
    snprintf(buf, sizeof(buf), "SEQ %lu", static_cast<unsigned long>(g_lastSeq));
    if (g_seqLbl) lv_label_set_text(g_seqLbl, buf);

    if (g_linkLbl) {
        if (linked) {
            snprintf(buf, sizeof(buf), LV_SYMBOL_WIFI " LIVE %lums", static_cast<unsigned long>(link_age_ms));
            lv_obj_set_style_text_color(g_linkLbl, lv_color_hex(0x68E2A0), LV_PART_MAIN);
        } else if (g_rxCount > 0) {
            snprintf(buf, sizeof(buf), LV_SYMBOL_WIFI " HOLD %lums", static_cast<unsigned long>(link_age_ms));
            lv_obj_set_style_text_color(g_linkLbl, lv_color_hex(0xFFC46E), LV_PART_MAIN);
        } else {
            snprintf(buf, sizeof(buf), LV_SYMBOL_WIFI " SEARCH");
            lv_obj_set_style_text_color(g_linkLbl, lv_color_hex(0x90A7C4), LV_PART_MAIN);
        }
        lv_label_set_text(g_linkLbl, buf);
    }

    const uint8_t lf = g_lightFlags;
    const bool hazard_on = (lf & LIGHT_HAZARD) != 0;
    const bool blink_phase = ((now_ms / 520U) % 2U) == 0U;
    const bool left_on = hazard_on ? blink_phase : (((lf & LIGHT_LEFT) != 0) && blink_phase);
    const bool right_on = hazard_on ? blink_phase : (((lf & LIGHT_RIGHT) != 0) && blink_phase);
    set_signal_chip(g_leftSignalChip, left_on, lv_color_hex(0x3B2A12), lv_color_hex(0xFFC15B));
    set_signal_chip(g_rightSignalChip, right_on, lv_color_hex(0x3B2A12), lv_color_hex(0xFFC15B));
    set_signal_chip(g_headChip, (lf & LIGHT_HEAD) != 0, lv_color_hex(0x142534), lv_color_hex(0x80D5FF));
    set_signal_chip(g_brakeChip, (lf & LIGHT_BRAKE) != 0, lv_color_hex(0x37161A), lv_color_hex(0xFF7A7A));
}

static void poll_uart_stream(HardwareSerial& uart,
                             uint8_t* rx_buffer,
                             size_t& rx_index,
                             volatile uint32_t& byte_counter) {
    while (uart.available() > 0) {
        byte_counter++;
        rx_buffer[rx_index] = uart.read();
        rx_index++;
        if (rx_index == sizeof(SensorPacket)) {
            SensorPacket* pkt = (SensorPacket*)rx_buffer;
            if (pkt->magic == PACKET_MAGIC) {
                handle_sensor_packet(rx_buffer, sizeof(SensorPacket), 2);
                rx_index = 0;
            } else {
                g_uartBadFrameCount++;
                // Shift buffer by 1 byte to resync
                memmove(rx_buffer, rx_buffer + 1, sizeof(SensorPacket) - 1);
                rx_index--;
            }
        }
    }
}

static void poll_uart() {
    static uint8_t uart2_buffer[sizeof(SensorPacket)];
    static uint8_t alt_buffer[sizeof(SensorPacket)];
    static size_t uart2_index = 0;
    static size_t alt_index = 0;

    // Primary: the Waveshare UART1/UART2 terminal pins.
    poll_uart_stream(Serial1, uart2_buffer, uart2_index, g_uart2ByteCount);
    // Diagnostic fallback: old RS485 pin assumption. This tells us immediately
    // if the board/header labeling differs from the schematic expectation.
    poll_uart_stream(Serial2, alt_buffer, alt_index, g_altUartByteCount);
}

void setup() {
    static esp_lcd_panel_handle_t panel_handle = nullptr;
    static esp_lcd_touch_handle_t tp_handle = nullptr;

    Serial.begin(115200);
    delay(300);
    Serial.println("LVGL Studio runtime boot");

    // Start UART bridge to C3
    Serial1.begin(UART_BRIDGE_BAUD, SERIAL_8N1, UART_BRIDGE_RX_PIN, UART_BRIDGE_TX_PIN);
    Serial2.begin(UART_BRIDGE_BAUD, SERIAL_8N1, 16, 15);
    Serial.printf("UART bridge primary RX=GPIO%u TX=GPIO%u baud=%u; alt RX=GPIO16 TX=GPIO15\n",
                  UART_BRIDGE_RX_PIN,
                  UART_BRIDGE_TX_PIN,
                  UART_BRIDGE_BAUD);

    DEV_I2C_Init();
    IO_EXTENSION_Init();

    panel_handle = waveshare_esp32_s3_rgb_lcd_init();
    if (panel_handle == nullptr) {
        Serial.println("LCD init failed");
        return;
    }

    wavesahre_rgb_lcd_bl_on();
    tp_handle = nullptr;

    esp_err_t lvret = lvgl_port_init(panel_handle, tp_handle);
    if (lvret != ESP_OK) {
        Serial.printf("lvgl_port_init failed: %d\n", static_cast<int>(lvret));
        return;
    }

    if (lvgl_port_lock(-1)) {
        if (kEnableParticleTunnel) {
            particle_tunnel_init(lv_scr_act());
        }
        custom_dashboard_build(lv_scr_act());
        lvgl_port_unlock();
    }

    espnow_begin();

    Serial.println("Exterior WS2812 is controlled by ESP32-C3 (S3 does not drive GPIO9/10)");
    Serial.println("Native dashboard ready");
}

void loop() {
    static uint32_t last_ui_ms = 0;
    static uint32_t last_rx_diag_ms = 0;
    static uint32_t last_rx_count_snapshot = 0;
    uint32_t now = millis();

    // Read any pending packets from C3 over hardwired UART
    poll_uart();
    now = millis();

    // RX diagnostic — every 1s OUTSIDE the LVGL lock.
    // Also re-asserts WiFi config periodically so any silent drift (PS/channel) gets recovered.
    if (now - last_rx_diag_ms >= 1000) {
        const uint32_t rxNow = g_rxCount;
        const uint32_t rxRate = rxNow - last_rx_count_snapshot;
        last_rx_count_snapshot = rxNow;
        const uint32_t age = (g_lastRxMs == 0) ? 9999 : (now - g_lastRxMs);
        uint8_t primary = 0;
        wifi_second_chan_t second;
        esp_wifi_get_channel(&primary, &second);
        const bool wifi_recent = (g_lastWifiRxMs != 0) && ((now - g_lastWifiRxMs) < 1200);
        const bool uart_recent = (g_lastUartRxMs != 0) && ((now - g_lastUartRxMs) < 1200);
        const char* source = "NONE";
        if (wifi_recent && uart_recent) source = "BOTH";
        else if (uart_recent)           source = "UART";
        else if (wifi_recent)           source = "WIFI";
        Serial.printf("RX rate=%luHz total=%lu lastAgeMs=%lu dropCount=%lu src=%s wifi=%lu uart=%lu uart2Bytes=%lu altBytes=%lu uartBad=%lu ch=%u\n",
                      static_cast<unsigned long>(rxRate),
                      static_cast<unsigned long>(rxNow),
                      static_cast<unsigned long>(age),
                      static_cast<unsigned long>(g_dropCount),
                      source,
                      static_cast<unsigned long>(g_wifiRxCount),
                      static_cast<unsigned long>(g_uartRxCount),
                      static_cast<unsigned long>(g_uart2ByteCount),
                      static_cast<unsigned long>(g_altUartByteCount),
                      static_cast<unsigned long>(g_uartBadFrameCount),
                      primary);
        last_rx_diag_ms = now;

        // Soft watchdog only: re-assert PS/channel if quiet for >3s.
        // (Heavy ESP-NOW reinit removed — its 50ms blocking made things worse.)
        static uint32_t last_force_ms = 0;
        if (age > 3000 && now - last_force_ms > 3000) {
            last_force_ms = now;
            esp_wifi_set_ps(WIFI_PS_NONE);
            esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
            Serial.println("[wifi watchdog] re-asserted PS_NONE + channel");
        }
    }

    // UI tick — request ~120 Hz. The RGB panel/vsync will cap actual output,
    // but this keeps LVGL interpolation and boot sweep feeling immediate.
    if (now - last_ui_ms >= 8) {
        last_ui_ms = now;
        if (lvgl_port_lock(5)) {
            const float d = g_distanceCm;
            const bool d_ok = (g_lastRxMs != 0) && !std::isnan(d) && (now - g_lastRxMs < 1500);
            if (kEnableParticleTunnel) {
                particle_tunnel_step(lvgl_studio_demo_speed_mph(), d, d_ok, now);
            }
            dashboard_tick(now);
            lvgl_port_unlock();
        }
    }
    delay(1);
}
