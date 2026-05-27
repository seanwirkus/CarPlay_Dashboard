#pragma once

#include <stdint.h>

/** Bit flags — keep in sync with ESP32-C3 sender / Serial (L:xx). */
enum exterior_light_bits : uint8_t {
    LIGHT_HEAD   = 1 << 0,
    LIGHT_LEFT   = 1 << 1,
    LIGHT_RIGHT  = 1 << 2,
    LIGHT_BRAKE  = 1 << 3,
    LIGHT_HAZARD = 1 << 4,
};

#ifdef __cplusplus
extern "C" {
#endif

void exterior_leds_init(void);
void exterior_leds_set_flags(uint8_t flags);
void exterior_leds_poll_serial(void);
void exterior_leds_tick(uint32_t ms);

#ifdef __cplusplus
}
#endif
