#pragma once

#include "lvgl.h"

/** Full-screen particle tunnel (ported from React ParticleScan3D). Call after lv_init, before dashboard widgets. */
void particle_tunnel_init(lv_obj_t *scr);

/**
 * Advance simulation and redraw the canvas. Call from the LVGL lock at ~20 Hz.
 * @param speed_mph Vehicle speed (demo or live).
 * @param distance_cm Ultrasonic distance when distance_ok is true.
 * @param distance_ok False uses the same default “open road” closeness as null distance in the web UI.
 * @param wall_ms Monotonic milliseconds (e.g. millis()) for grid animation.
 */
void particle_tunnel_step(float speed_mph, float distance_cm, bool distance_ok, uint32_t wall_ms);
