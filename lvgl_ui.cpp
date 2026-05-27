#include "lvgl_ui.h"
#include "ui_fonts.h"
#include <stdio.h>
#include <string.h>

// ============================================================
// CarPlay-style instrument cluster — 1024 x 600
//
// Top:    Trip pill (left)  |  Gear + RPM bar (right)
// Mid:    Speed circle (L)  |  3 vertical gauges (C)  |  ETA circle (R)
// Bot:    Mini gauges (L)   |  Compass scale (R)
// ============================================================

// Palette ----------------------------------------------------
#define COL_BG            0x0B1820
#define COL_BG_BOT        0x06101A
#define COL_PILL          0x142028
#define COL_PILL_BORDER   0x223038
#define COL_RING_TRACK    0x132028

#define COL_BLUE          0x0A84FF
#define COL_TEAL          0x64D2FF
#define COL_GREEN         0x32D74B
#define COL_PURPLE        0xBF5AF2
#define COL_PINK          0xFF375F
#define COL_AMBER         0xFF9F0A
#define COL_YELLOW        0xFFD60A
#define COL_RED           0xFF453A

#define COL_TEXT          0xFFFFFF
#define COL_TEXT_2        0xB0B5BD
#define COL_TEXT_3        0x6E7780
#define COL_TEXT_4        0x3D464F

#define SCREEN_W   1024
#define SCREEN_H   600

// Layout constants -------------------------------------------
#define BIG_RING_DIA   240
#define BIG_RING_STROKE 14
#define BIG_RING_CY    240
#define SPEED_CX       180
#define ETA_CX         (SCREEN_W - 180)

#define MID_BAR_CY        BIG_RING_CY
#define MID_BAR_TRACK_W   14
#define MID_BAR_TRACK_H   200
#define MID_BAR_COL_LEFT_CX   444
#define MID_BAR_COL_MID_CX    512
#define MID_BAR_COL_RIGHT_CX  580

// Handles ----------------------------------------------------
static lv_obj_t *speed_label, *speed_arc;
static lv_obj_t *rpm_big_arc, *rpm_big_label;
static lv_obj_t *eta_min_label, *eta_arrive_label, *eta_arc;  // unused now

static lv_obj_t *gear_label_lbl, *rpm_meter_fill, *rpm_label;
static lv_obj_t *trip_eta_min_pill, *trip_eta_time_pill, *trip_dist_pill;

static lv_obj_t *temp_arc, *fuel_arc, *fuel_pct_label;
static lv_obj_t *clock_big_label, *odo_label_w, *trip_label_w, *out_temp_label_w;
static lv_obj_t *compass_bar, *compass_needle, *compass_label;  // unused

static lv_obj_t *boost_fill, *boost_val_label;
static lv_obj_t *batt_fill,  *batt_val_label;
static lv_obj_t *oilp_fill,  *oilp_val_label;

static lv_obj_t *left_blinker, *right_blinker, *headlight_icon;
static lv_obj_t *bt_icon, *bt_status_label;

static int g_rpm_w = 0;
static int g_compass_w = 0;

// Helpers ----------------------------------------------------

// Inter glyph ascender/descender is asymmetric: visual cap-center sits ABOVE
// the line-box center by ~18% of font size. Compensates so digits look
// optically centered inside arcs.
static void center_text_in_arc(lv_obj_t *label, lv_obj_t *arc, int width, int y_extra) {
    lv_obj_set_width(label, width);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(label, arc, LV_ALIGN_CENTER, 0, y_extra);
}

static void set_fmt(lv_obj_t *l, const char *fmt, ...) {
    if (!l) return;
    char b[96]; va_list a; va_start(a, fmt);
    vsnprintf(b, sizeof(b), fmt, a); va_end(a);
    lv_label_set_text(l, b);
}

static lv_obj_t* make_label(lv_obj_t *p, const lv_font_t *f, uint32_t c, const char *t) {
    lv_obj_t *l = lv_label_create(p);
    lv_obj_set_style_text_font(l, f, 0);
    lv_obj_set_style_text_color(l, lv_color_hex(c), 0);
    lv_label_set_text(l, t);
    return l;
}
static lv_obj_t* make_icon(lv_obj_t *p, const lv_font_t *f, uint32_t c, const char *g) {
    return make_label(p, f, c, g);
}

// Center a child object on a coord (uses content size)
static void center_on(lv_obj_t *o, int cx, int cy) {
    lv_obj_update_layout(o);
    int w = lv_obj_get_width(o);
    int h = lv_obj_get_height(o);
    lv_obj_set_pos(o, cx - w/2, cy - h/2);
}

static lv_obj_t* make_panel(lv_obj_t *p, int x, int y, int w, int h, int radius, uint32_t bg) {
    lv_obj_t *o = lv_obj_create(p);
    lv_obj_set_size(o, w, h);
    lv_obj_set_pos(o, x, y);
    lv_obj_set_style_bg_color(o, lv_color_hex(bg), 0);
    lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(o, lv_color_hex(COL_PILL_BORDER), 0);
    lv_obj_set_style_border_width(o, 1, 0);
    lv_obj_set_style_radius(o, radius, 0);
    lv_obj_set_style_pad_all(o, 0, 0);
    lv_obj_set_scrollbar_mode(o, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(o, LV_OBJ_FLAG_SCROLLABLE);
    return o;
}

// Big C-arc ring
static lv_obj_t* make_big_ring(lv_obj_t *parent, int cx, int cy, int diameter,
                               int range_max, uint32_t fill_col) {
    lv_obj_t *a = lv_arc_create(parent);
    lv_obj_set_size(a, diameter, diameter);
    lv_obj_set_pos(a, cx - diameter/2, cy - diameter/2);
    lv_arc_set_rotation(a, 135);
    lv_arc_set_bg_angles(a, 0, 270);
    lv_arc_set_range(a, 0, range_max);
    lv_arc_set_value(a, 0);
    lv_obj_set_style_arc_color(a, lv_color_hex(COL_RING_TRACK), LV_PART_MAIN);
    lv_obj_set_style_arc_width(a, BIG_RING_STROKE, LV_PART_MAIN);
    lv_obj_set_style_arc_color(a, lv_color_hex(fill_col), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(a, BIG_RING_STROKE, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(a, true, LV_PART_INDICATOR);
    lv_obj_remove_style(a, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(a, LV_OBJ_FLAG_CLICKABLE);
    return a;
}

static lv_obj_t* make_mini_ring(lv_obj_t *parent, int cx, int cy, int dia,
                                int range_max, uint32_t fill_col) {
    lv_obj_t *a = lv_arc_create(parent);
    lv_obj_set_size(a, dia, dia);
    lv_obj_set_pos(a, cx - dia/2, cy - dia/2);
    lv_arc_set_rotation(a, 130);
    lv_arc_set_bg_angles(a, 0, 280);
    lv_arc_set_range(a, 0, range_max);
    lv_arc_set_value(a, 0);
    lv_obj_set_style_arc_color(a, lv_color_hex(COL_RING_TRACK), LV_PART_MAIN);
    lv_obj_set_style_arc_width(a, 9, LV_PART_MAIN);
    lv_obj_set_style_arc_color(a, lv_color_hex(fill_col), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(a, 9, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(a, true, LV_PART_INDICATOR);
    lv_obj_remove_style(a, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(a, LV_OBJ_FLAG_CLICKABLE);
    return a;
}

// Vertical bar gauge
static lv_obj_t* build_vbar(lv_obj_t *scr, int cx, const char *icon_glyph,
                            uint32_t fill_col, const char *abbr,
                            lv_obj_t **out_val_label) {
    // Icon on top
    lv_obj_t *ico = make_icon(scr, &lucide_28, fill_col, icon_glyph);
    center_on(ico, cx, MID_BAR_CY - MID_BAR_TRACK_H/2 - 38);

    // Value label
    lv_obj_t *val = make_label(scr, &inter_22, COL_TEXT, "—");
    center_on(val, cx, MID_BAR_CY - MID_BAR_TRACK_H/2 - 12);
    *out_val_label = val;

    // Track
    lv_obj_t *track = lv_obj_create(scr);
    lv_obj_set_size(track, MID_BAR_TRACK_W, MID_BAR_TRACK_H);
    lv_obj_set_pos(track, cx - MID_BAR_TRACK_W/2, MID_BAR_CY - MID_BAR_TRACK_H/2 + 8);
    lv_obj_set_style_bg_color(track, lv_color_hex(COL_RING_TRACK), 0);
    lv_obj_set_style_border_width(track, 0, 0);
    lv_obj_set_style_radius(track, MID_BAR_TRACK_W/2, 0);
    lv_obj_set_style_pad_all(track, 0, 0);
    lv_obj_set_scrollbar_mode(track, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(track, LV_OBJ_FLAG_SCROLLABLE);

    // Fill (anchored to bottom of track, grows up)
    lv_obj_t *fill = lv_obj_create(track);
    lv_obj_set_size(fill, MID_BAR_TRACK_W, 0);
    lv_obj_align(fill, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(fill, lv_color_hex(fill_col), 0);
    lv_obj_set_style_border_width(fill, 0, 0);
    lv_obj_set_style_radius(fill, MID_BAR_TRACK_W/2, 0);
    lv_obj_set_style_pad_all(fill, 0, 0);
    lv_obj_set_scrollbar_mode(fill, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(fill, LV_OBJ_FLAG_SCROLLABLE);

    // Bottom abbrev label
    lv_obj_t *lbl = make_label(scr, &inter_16, COL_TEXT_2, abbr);
    lv_obj_set_style_text_letter_space(lbl, 2, 0);
    center_on(lbl, cx, MID_BAR_CY + MID_BAR_TRACK_H/2 + 22);

    return fill;
}

static void set_vbar(lv_obj_t *fill, int pct) {
    if (!fill) return;
    if (pct < 0) pct = 0; if (pct > 100) pct = 100;
    lv_obj_set_height(fill, (MID_BAR_TRACK_H * pct) / 100);
}

// =========================================================
// PUBLIC SETTERS
// =========================================================
void ui_update_speed(int speed) {
    set_fmt(speed_label, "%d", speed);
    if (speed_arc) lv_arc_set_value(speed_arc, speed);
}

void ui_update_rpm(int rpm) {
    set_fmt(rpm_big_label, "%d", rpm);
    if (rpm_big_arc) lv_arc_set_value(rpm_big_arc, rpm);
    if (rpm_label) set_fmt(rpm_label, "%d rpm", rpm);  // legacy if exists
    if (rpm_meter_fill && g_rpm_w) {
        int v = rpm; if (v > 8000) v = 8000; if (v < 0) v = 0;
        lv_obj_set_width(rpm_meter_fill, (g_rpm_w * v) / 8000);
    }
}

void ui_update_gear(const char *gear) {
    if (gear_label_lbl) lv_label_set_text(gear_label_lbl, gear);
}

void ui_update_fuel(int pct) {
    if (fuel_arc) lv_arc_set_value(fuel_arc, pct);
    set_fmt(fuel_pct_label, "%d%%", pct);
    if (fuel_arc) {
        uint32_t c = pct < 15 ? COL_RED : (pct < 30 ? COL_AMBER : COL_GREEN);
        lv_obj_set_style_arc_color(fuel_arc, lv_color_hex(c), LV_PART_INDICATOR);
    }
}

void ui_update_temperature(int tempF) {
    if (!temp_arc) return;
    int v = tempF - 100; if (v < 0) v = 0; if (v > 100) v = 100;
    lv_arc_set_value(temp_arc, v);
    uint32_t c = (tempF > 220) ? COL_RED : (tempF > 200 ? COL_AMBER : COL_TEAL);
    lv_obj_set_style_arc_color(temp_arc, lv_color_hex(c), LV_PART_INDICATOR);
}

void ui_update_oil(int pct) { (void)pct; } // legacy — replaced by oilpress vbar

void ui_update_eta(const char *mins, const char *arrival, const char *distance) {
    if (trip_eta_min_pill)  lv_label_set_text(trip_eta_min_pill,  mins ? mins : "—");
    if (trip_eta_time_pill) lv_label_set_text(trip_eta_time_pill, arrival ? arrival : "—");
    if (trip_dist_pill)     lv_label_set_text(trip_dist_pill,     distance ? distance : "—");
    if (eta_min_label && mins) lv_label_set_text(eta_min_label, mins);
    if (eta_arrive_label && arrival) lv_label_set_text(eta_arrive_label, arrival);
    if (eta_arc && mins) {
        int v = atoi(mins); if (v > 120) v = 120;
        lv_arc_set_value(eta_arc, v);
    }
}

void ui_update_compass(int degrees, const char *cardinal) {
    (void)degrees; (void)cardinal; // compass removed
}

void ui_update_boost(int psi) {
    set_fmt(boost_val_label, "%d", psi);
    set_vbar(boost_fill, ((psi + 10) * 100) / 40); // -10..30 → 0..100
}

void ui_update_battery(int mv) {
    char b[12]; snprintf(b, sizeof(b), "%d.%dV", mv/1000, (mv%1000)/100);
    if (batt_val_label) lv_label_set_text(batt_val_label, b);
    int pct = ((mv - 11000) * 100) / 4000; // 11..15V → 0..100
    set_vbar(batt_fill, pct);
}

void ui_update_oilpress(int psi) {
    set_fmt(oilp_val_label, "%d", psi);
    set_vbar(oilp_fill, (psi * 100) / 90); // 0..90 → 0..100
}

void ui_update_clock(const char *t) {
    if (clock_big_label) lv_label_set_text(clock_big_label, t ? t : "—");
}
void ui_update_outside_temp(int tempF) {
    if (out_temp_label_w) set_fmt(out_temp_label_w, "%d°F", tempF);
}
void ui_update_trip_odo(const char *trip, const char *odo) {
    if (trip_label_w) set_fmt(trip_label_w, "%s mi", trip);
    if (odo_label_w)  set_fmt(odo_label_w,  "%s mi", odo);
}
void ui_update_song(const char *t, const char *a) { (void)t; (void)a; }
void ui_update_album(const char *a)         { (void)a; }
void ui_update_play_state(bool p)           { (void)p; }
void ui_update_track_progress(int e, int d) { (void)e; (void)d; }

void ui_set_blinkers(bool l, bool r) {
    if (left_blinker)  lv_obj_set_style_text_color(left_blinker,  lv_color_hex(l ? COL_GREEN : COL_TEXT_4), 0);
    if (right_blinker) lv_obj_set_style_text_color(right_blinker, lv_color_hex(r ? COL_GREEN : COL_TEXT_4), 0);
}
void ui_set_headlights(bool on) {
    if (headlight_icon) lv_obj_set_style_text_color(headlight_icon, lv_color_hex(on ? COL_BLUE : COL_TEXT_4), 0);
}
void ui_set_bt_connected(bool c) {
    if (bt_icon) lv_obj_set_style_text_color(bt_icon, lv_color_hex(c ? COL_BLUE : COL_TEXT_3), 0);
}
void ui_set_bt_status(const char *s) {
    if (bt_status_label) lv_label_set_text(bt_status_label, s ? s : "");
}

// =========================================================
// LAYOUT
// =========================================================
static void build_background(lv_obj_t *scr) {
    lv_obj_set_style_bg_color(scr, lv_color_hex(COL_BG), 0);
    lv_obj_set_style_bg_grad_color(scr, lv_color_hex(COL_BG_BOT), 0);
    lv_obj_set_style_bg_grad_dir(scr, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_pad_all(scr, 0, 0);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
}

static void build_top_row(lv_obj_t *scr) {
    // Trip pill (left)
    const int PW = 340, PH = 64;
    lv_obj_t *pill = make_panel(scr, 22, 18, PW, PH, 16, COL_PILL);

    const int COL_W = PW / 3;
    const char *labels[3] = {"min", "arrival", "km"};
    lv_obj_t **targets[3] = {&trip_eta_min_pill, &trip_eta_time_pill, &trip_dist_pill};
    uint32_t cols[3] = {COL_BLUE, COL_TEAL, COL_GREEN};

    for (int i = 0; i < 3; i++) {
        lv_obj_t *v = make_label(pill, &inter_22, COL_TEXT, "—");
        center_on(v, i * COL_W + COL_W/2, 22);
        *targets[i] = v;
        lv_obj_t *u = make_label(pill, &inter_16, cols[i], labels[i]);
        center_on(u, i * COL_W + COL_W/2, 46);
    }

    // Gear circle — top-right, large + bold
    lv_obj_t *gp = lv_obj_create(scr);
    lv_obj_set_size(gp, 68, 68);
    lv_obj_set_pos(gp, SCREEN_W - 22 - 68, 18);
    lv_obj_set_style_bg_color(gp, lv_color_hex(COL_PILL), 0);
    lv_obj_set_style_border_color(gp, lv_color_hex(COL_BLUE), 0);
    lv_obj_set_style_border_width(gp, 2, 0);
    lv_obj_set_style_radius(gp, 34, 0);
    lv_obj_set_style_pad_all(gp, 0, 0);
    lv_obj_set_scrollbar_mode(gp, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(gp, LV_OBJ_FLAG_SCROLLABLE);
    gear_label_lbl = make_label(gp, &inter_32, COL_BLUE, "D");
    lv_obj_center(gear_label_lbl);

    // BT pill — top-right left of gear
    bt_icon = make_icon(scr, &lucide_28, COL_TEXT_3, ICO_BLUETOOTH);
    lv_obj_set_pos(bt_icon, SCREEN_W - 22 - 68 - 50, 38);
    bt_status_label = make_label(scr, &inter_16, COL_TEXT_3, "");
    lv_obj_set_width(bt_status_label, 130);
    lv_obj_set_style_text_align(bt_status_label, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_set_pos(bt_status_label, SCREEN_W - 22 - 68 - 50 - 138, 42);

    // No-op handles
    rpm_meter_fill = nullptr;
    rpm_label = nullptr;
    g_rpm_w = 0;
}

static void build_middle(lv_obj_t *scr) {
    // ---- RPM (left) ----
    rpm_big_arc = make_big_ring(scr, SPEED_CX, BIG_RING_CY, BIG_RING_DIA, 8000, COL_RED);

    rpm_big_label = make_label(scr, &inter_64, COL_TEXT, "0");
    center_text_in_arc(rpm_big_label, rpm_big_arc, 180, -10);

    lv_obj_t *rpmu = make_label(scr, &inter_22, COL_RED, "RPM");
    lv_obj_set_style_text_letter_space(rpmu, 4, 0);
    lv_obj_align_to(rpmu, rpm_big_arc, LV_ALIGN_CENTER, 0, 36);

    // ---- SPEED (right) ----
    speed_arc = make_big_ring(scr, ETA_CX, BIG_RING_CY, BIG_RING_DIA, 180, COL_BLUE);

    speed_label = make_label(scr, &inter_64, COL_TEXT, "0");
    center_text_in_arc(speed_label, speed_arc, 180, -10);

    lv_obj_t *mph = make_label(scr, &inter_22, COL_BLUE, "MPH");
    lv_obj_set_style_text_letter_space(mph, 4, 0);
    lv_obj_align_to(mph, speed_arc, LV_ALIGN_CENTER, 0, 36);

    // ETA fields unused — null
    eta_arc = nullptr;
    eta_min_label = nullptr;
    eta_arrive_label = nullptr;

    // ---- 3 vertical bars (center column) ----
    boost_fill = build_vbar(scr, MID_BAR_COL_LEFT_CX, ICO_GAUGE,      COL_PURPLE, "BOOST", &boost_val_label);
    batt_fill  = build_vbar(scr, MID_BAR_COL_MID_CX,  ICO_BATTERY,    COL_TEAL,   "VOLT",  &batt_val_label);
    oilp_fill  = build_vbar(scr, MID_BAR_COL_RIGHT_CX,ICO_FUEL,       COL_AMBER,  "OIL",   &oilp_val_label);
}

static void build_mini_gauge(lv_obj_t *scr, int cx, int cy, const char *icon_glyph,
                             uint32_t icon_col, const char *left_letter, const char *right_letter,
                             lv_obj_t **out_arc) {
    lv_obj_t *a = make_mini_ring(scr, cx, cy, 92, 100, icon_col);
    *out_arc = a;

    lv_obj_t *ic = make_icon(scr, &lucide_28, icon_col, icon_glyph);
    lv_obj_align_to(ic, a, LV_ALIGN_CENTER, 0, -4);

    lv_obj_t *l = make_label(scr, &inter_16, COL_TEXT_3, left_letter);
    lv_obj_align_to(l, a, LV_ALIGN_CENTER, -26, 28);
    lv_obj_t *r = make_label(scr, &inter_16, COL_TEXT_3, right_letter);
    lv_obj_align_to(r, a, LV_ALIGN_CENTER, 26, 28);
}

// Stat row inside a panel: header + value
static void stat_row(lv_obj_t *parent, int x, int y, const char *header,
                     uint32_t header_col, const lv_font_t *val_font, const char *initial,
                     lv_obj_t **out_value) {
    lv_obj_t *h = make_label(parent, &inter_16, header_col, header);
    lv_obj_set_style_text_letter_space(h, 3, 0);
    lv_obj_set_pos(h, x, y);
    lv_obj_t *v = make_label(parent, val_font, COL_TEXT, initial);
    lv_obj_set_pos(v, x, y + 22);
    *out_value = v;
}

static void build_bottom_row(lv_obj_t *scr) {
    const int Y = 480;

    // ---- Left: TEMP + FUEL mini gauges ----
    build_mini_gauge(scr, 84,  Y, ICO_THERMOMETER, COL_TEAL,  "C", "H", &temp_arc);
    build_mini_gauge(scr, 218, Y, ICO_FUEL,        COL_GREEN, "E", "F", &fuel_arc);

    fuel_pct_label = make_label(scr, &inter_16, COL_TEXT_2, "0%");
    lv_obj_align_to(fuel_pct_label, fuel_arc, LV_ALIGN_CENTER, 0, 60);

    lv_obj_t *temp_lbl = make_label(scr, &inter_16, COL_TEXT_2, "TEMP");
    lv_obj_set_style_text_letter_space(temp_lbl, 2, 0);
    lv_obj_align_to(temp_lbl, temp_arc, LV_ALIGN_CENTER, 0, 60);

    // ---- Center: big clock panel ----
    const int CKW = 270, CKH = 110;
    const int CKX = (SCREEN_W - CKW) / 2;
    lv_obj_t *clock_panel = make_panel(scr, CKX, Y - 12, CKW, CKH, 18, COL_PILL);

    clock_big_label = make_label(clock_panel, &inter_64, COL_TEXT, "—:—");
    lv_obj_align(clock_big_label, LV_ALIGN_CENTER, 0, -8);

    lv_obj_t *date_h = make_label(clock_panel, &inter_16, COL_BLUE, "DRIVE TIME");
    lv_obj_set_style_text_letter_space(date_h, 3, 0);
    lv_obj_align(date_h, LV_ALIGN_BOTTOM_MID, 0, -10);

    // ---- Right: stats panel (odo, trip, outside) ----
    const int RPW = 270, RPH = 130;
    const int RPX = SCREEN_W - 22 - RPW;
    lv_obj_t *r_panel = make_panel(scr, RPX, Y - 12, RPW, RPH, 18, COL_PILL);

    stat_row(r_panel, 20, 14, "ODO", COL_TEAL, &inter_32, "0 mi", &odo_label_w);
    stat_row(r_panel, 20, 70, "TRIP", COL_GREEN, &inter_22, "0 mi", &trip_label_w);
    stat_row(r_panel, 150, 70, "OUTSIDE", COL_AMBER, &inter_22, "—°F", &out_temp_label_w);

    // ---- Indicator row very bottom ----
    const int IY = 575;
    headlight_icon = make_icon(scr, &lucide_28, COL_TEXT_4, ICO_SUN);
    lv_obj_set_pos(headlight_icon, 22, IY);
    left_blinker = make_icon(scr, &lucide_40, COL_TEXT_4, ICO_ARROW_LEFT);
    lv_obj_set_pos(left_blinker, SCREEN_W/2 - 60, IY - 6);
    right_blinker = make_icon(scr, &lucide_40, COL_TEXT_4, ICO_ARROW_RIGHT);
    lv_obj_set_pos(right_blinker, SCREEN_W/2 + 20, IY - 6);
}

// =========================================================
// INIT
// =========================================================
void lvgl_ui_init(void) {
    lv_obj_t *scr = lv_scr_act();
    build_background(scr);
    build_top_row(scr);
    build_middle(scr);
    build_bottom_row(scr);
}

void lvgl_ui_tick(void) {}
