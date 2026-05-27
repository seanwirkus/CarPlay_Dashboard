// Apple Media Service (AMS) client — STUB FOR NOW.
//
// ESP32-S3 Arduino core 3.x ships with NimBLE (not Bluedroid). A real AMS
// client on NimBLE needs: pair (Just Works) → on auth-complete grab peer addr
// from ble_gap_conn_desc → BLEClient::connect to peer → discover AMS service
// 89D3502B-… → subscribe to Entity Update 2F7CABCE-… → register attribute
// interest (Track: Artist/Album/Title/Duration; Player: PlaybackInfo).
//
// Notification payload: [entity, attribute, flags, value-utf8...].
// Drive ui_update_song(title, artist), ui_update_album(album),
// ui_update_play_state(playing), ui_update_track_progress(elapsed, total).
//
// For now this stub:
//   - Advertises the device name so iPhone sees it
//   - Drives a simulator that walks through several songs so the layout
//     can be visually validated
//   - Lets the UI's ui_set_bt_* show "Simulator" state
//
// To wire real AMS, replace ams_tick() body and add NimBLE BLEClient discovery.

#include "carplay_ams.h"
#include "lvgl_ui.h"
#include <Arduino.h>

static bool g_ready = false;

struct Track { const char *title; const char *artist; const char *album; int duration; };

static const Track DEMO[] = {
    {"Espresso",           "Sabrina Carpenter",  "Short n' Sweet",            175},
    {"Good Luck, Babe!",   "Chappell Roan",      "The Rise & Fall…",          218},
    {"Birds of a Feather", "Billie Eilish",      "Hit Me Hard and Soft",      210},
    {"Lose Control",       "Teddy Swims",        "I've Tried Everything…",    211},
    {"Stick Season",       "Noah Kahan",         "Stick Season",              183},
};
static const int DEMO_N = sizeof(DEMO) / sizeof(DEMO[0]);

void ams_begin(const char *device_name) {
    Serial.printf("[AMS-stub] device=%s — simulator mode (no BLE)\n",
                  device_name ? device_name : "CarPlay Dash");
    ui_set_bt_status("Simulator");
    ui_set_bt_connected(false);
    g_ready = true;
}

void ams_tick(void) {
    if (!g_ready) return;
    static uint32_t last_track_change = 0;
    static int idx = -1;
    uint32_t now = millis();

    // Advance track every duration ms (capped 12s for demo pace)
    if (idx < 0 || now - last_track_change > (uint32_t)(DEMO[idx].duration < 12 ? DEMO[idx].duration * 1000 : 12000)) {
        idx = (idx + 1) % DEMO_N;
        last_track_change = now;
        ui_update_song(DEMO[idx].title, DEMO[idx].artist);
        ui_update_album(DEMO[idx].album);
        ui_update_track_progress(0, DEMO[idx].duration);
        ui_update_play_state(true);
    }

    // Progress ticks once per second
    static uint32_t last_progress = 0;
    if (now - last_progress > 1000) {
        last_progress = now;
        int elapsed = (now - last_track_change) / 1000;
        if (elapsed > DEMO[idx].duration) elapsed = DEMO[idx].duration;
        ui_update_track_progress(elapsed, DEMO[idx].duration);
    }
}

bool ams_is_connected(void) { return false; }
