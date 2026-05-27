#include "exterior_leds.h"

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <cstdio>

// Front strip (GPIO 9): outer zones = turn signals, center = headlights.
// Rear strip (GPIO 10): outer zones = rear turns, center = brake.
// Tune *_NUM to match your physical LED count.
static constexpr uint8_t  FRONT_PIN  = 9;
static constexpr uint8_t  REAR_PIN   = 10;
static constexpr uint16_t FRONT_NUM  = 24;
static constexpr uint16_t REAR_NUM   = 24;
static constexpr uint16_t ZONE       = 8;  // LEDs per side (must be <= strip/3)

static Adafruit_NeoPixel g_front(FRONT_NUM, FRONT_PIN, NEO_GRB + NEO_KHZ800);
static Adafruit_NeoPixel g_rear(REAR_NUM, REAR_PIN, NEO_GRB + NEO_KHZ800);

static uint8_t  s_flags_hw = 0;
static uint8_t  s_flags_serial = 0;
static uint32_t s_serial_until_ms = 0;

void exterior_leds_init() {
    g_front.begin();
    g_rear.begin();
    g_front.setBrightness(200);
    g_rear.setBrightness(240);
    g_front.clear();
    g_rear.clear();
    g_front.show();
    g_rear.show();
}

void exterior_leds_set_flags(uint8_t flags) { s_flags_hw = flags; }

static uint8_t active_flags(uint32_t ms) {
    if (s_serial_until_ms != 0 && (int32_t)(ms - s_serial_until_ms) < 0) return s_flags_serial;
    return s_flags_hw;
}

static uint32_t amb() { return g_front.Color(255, 120, 0); }
static uint32_t head(bool on) { return on ? g_front.Color(255, 255, 255) : g_front.Color(40, 42, 48); }
static uint32_t rear_run() { return g_rear.Color(80, 0, 0); }
static uint32_t brake_hi() { return g_rear.Color(255, 0, 0); }
static uint32_t turn_rear(bool on) { return on ? g_rear.Color(255, 48, 0) : rear_run(); }

void exterior_leds_tick(uint32_t ms) {
    const uint8_t s_flags = active_flags(ms);
    const bool hz  = (s_flags & LIGHT_HAZARD) != 0;
    const bool phase = ((ms / 420) & 1u) != 0;
    const bool left_on =
        hz ? phase : (((s_flags & LIGHT_LEFT) != 0) && phase);
    const bool right_on =
        hz ? phase : (((s_flags & LIGHT_RIGHT) != 0) && phase);
    const bool head_on = (s_flags & LIGHT_HEAD) != 0;
    const bool brake   = (s_flags & LIGHT_BRAKE) != 0;

    const uint16_t fz = (ZONE < FRONT_NUM / 3) ? ZONE : (FRONT_NUM / 3);
    const uint16_t rz = (ZONE < REAR_NUM / 3) ? ZONE : (REAR_NUM / 3);

    for (uint16_t i = 0; i < FRONT_NUM; i++) {
        uint32_t c = 0;
        if (i < fz) {
            c = left_on ? amb() : 0;
        } else if (i >= FRONT_NUM - fz) {
            c = right_on ? amb() : 0;
        } else {
            c = head(head_on);
        }
        g_front.setPixelColor(i, c);
    }

    for (uint16_t i = 0; i < REAR_NUM; i++) {
        uint32_t c;
        if (i < rz) {
            c = turn_rear(left_on);
        } else if (i >= REAR_NUM - rz) {
            c = turn_rear(right_on);
        } else {
            c = brake ? brake_hi() : rear_run();
        }
        g_rear.setPixelColor(i, c);
    }

    g_front.show();
    g_rear.show();
}

void exterior_leds_poll_serial() {
    static char line[24];
    static uint8_t len = 0;
    while (Serial.available()) {
        char ch = (char)Serial.read();
        if (ch == '\r') continue;
        if (ch == '\n') {
            line[len] = '\0';
            len = 0;
            if (line[0] == 'L' && line[1] == ':') {
                unsigned v = 0;
                if (sscanf(line + 2, "%x", &v) == 1) {
                    s_flags_serial = (uint8_t)v;
                    s_serial_until_ms = millis() + 8000;
                    Serial.printf("exterior_leds (8s override) flags=0x%02x\n", s_flags_serial);
                }
            }
            continue;
        }
        if (len < sizeof(line) - 1) line[len++] = ch;
    }
}
