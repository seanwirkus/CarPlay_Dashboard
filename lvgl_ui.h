#ifndef LVGL_UI_H
#define LVGL_UI_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

void lvgl_ui_init(void);
void lvgl_ui_tick(void);

// Vehicle telemetry
void ui_update_speed(int speed);
void ui_update_rpm(int rpm);
void ui_update_gear(const char* gear);
void ui_update_fuel(int pct);
void ui_update_temperature(int tempF);

// Media — kept as no-ops for legacy callers, dashboard has no media tile now
void ui_update_song(const char* title, const char* artist);
void ui_update_album(const char* album);
void ui_update_play_state(bool playing);
void ui_update_track_progress(int elapsed_sec, int duration_sec);

// Instrument cluster extras
void ui_update_oil(int pct);
void ui_update_eta(const char* mins, const char* arrival, const char* distance);
void ui_update_compass(int degrees, const char* cardinal); // 0..359, "NW" etc.

// Middle vertical gauges
void ui_update_boost(int psi);          // -10..30 typical
void ui_update_battery(int millivolts); // 11000..15000
void ui_update_oilpress(int psi);       // 0..90

// Status bar
void ui_update_clock(const char* time_str);
void ui_update_outside_temp(int tempF);
void ui_update_trip_odo(const char* trip, const char* odo);
void ui_set_blinkers(bool left, bool right);
void ui_set_headlights(bool on);
void ui_set_bt_connected(bool connected);
void ui_set_bt_status(const char* status);  // e.g. "Scanning", "Connected: iPhone"

#ifdef __cplusplus
}
#endif

#endif
