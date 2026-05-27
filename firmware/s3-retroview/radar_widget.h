#pragma once

#include <stdint.h>

// Concentric arcs radar centered at (cx, cy) opening upward toward driver.
// distanceCm: NAN = no link / no data → outline only.
// Renders into the active Paint framebuffer using NAV_RADAR_* palette.
void radar_draw(int cx, int cy, int maxRadius, float distanceCm);
