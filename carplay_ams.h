#ifndef CARPLAY_AMS_H
#define CARPLAY_AMS_H

#ifdef __cplusplus
extern "C" {
#endif

// Apple Media Service (AMS) client for iPhone Now Playing.
// Requires the iPhone to pair (Just Works) when prompted on first run.
//
// Drives ui_update_song / ui_update_play_state / ui_update_track_progress.
//
// Call ams_begin() once after Serial + LVGL are initialized.
// Call ams_tick() from loop() — currently a no-op (callbacks drive state).

void ams_begin(const char *device_name);
void ams_tick(void);
bool ams_is_connected(void);

#ifdef __cplusplus
}
#endif

#endif
