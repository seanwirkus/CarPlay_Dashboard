#pragma once

#include <Arduino.h>

namespace projector {

constexpr uint16_t kDmxStartAddress = 1;
constexpr size_t kUiSlotCount = 16;
constexpr uint32_t kDmxFrameIntervalMs = 33;
constexpr uint32_t kSceneHoldMs = 2400;
constexpr uint32_t kCrossfadeMs = 1400;
constexpr uint32_t kDisplayRefreshMs = 50;
constexpr uint32_t kButtonDebounceMs = 40;
constexpr uint32_t kDisplayPageHoldMs = 2200;

constexpr uint32_t kProximitySampleIntervalMs = 35;
constexpr float kProximityMinValidCm = 8.0f;
constexpr float kProximityMaxValidCm = 250.0f;
constexpr float kProximityBarMinCm = 2.0f;
constexpr float kProximityTripDistanceCm = 70.0f;
constexpr float kProximityWarnDistanceCm = 120.0f;
constexpr unsigned long kProximityEchoTimeoutUs = 16000UL;
constexpr size_t kProximityFilterSize = 3;
constexpr size_t kProximityMinSamples = 2;

// Monster inline-controller strip control.
// These defaults are placeholders until the actual remote codes are learned.
// The current implementation assumes a NEC-style demodulated IR signal injected
// into the controller's IR receiver output node via GPIO7.
constexpr bool kStripControlEnabled = true;
constexpr bool kStripIrActiveLow = true;
constexpr uint32_t kStripPowerOnCode = 0x00000000;
constexpr uint32_t kStripPowerOffCode = 0x00000000;
constexpr uint32_t kStripSafeColorCode = 0x00000000;  // Green
constexpr uint32_t kStripWarnColorCode = 0x00000000;  // Yellow / amber
constexpr uint32_t kStripTripColorCode = 0x00000000;  // Red
constexpr uint32_t kStripNoDataColorCode = 0x00000000;  // Blue or white
constexpr uint16_t kStripCommandGapMs = 140;

constexpr char kAccessPointSsid[] = "LaserShow-ESP32C3";
constexpr char kAccessPointPassword[] = "laser1234";

// These scenes are starter values only. Most low-cost DMX lasers use vendor-
// specific channel maps, so you should edit these once you have your unit's
// DMX chart in hand. The raw-slot workflow in the web UI still works even when
// these defaults do not match your projector perfectly.
struct DmxScene {
    const char* name;
    uint8_t slots[kUiSlotCount];
};

static constexpr DmxScene kScenes[] = {
    { "Red Fan",      {   0,  12, 128, 128,   0,   0, 200, 150,  16,   0, 0, 0, 0, 0, 0, 0 } },
    { "Green Tunnel", {   0,  40, 128, 128,  24,   0, 180, 128,  64,   0, 0, 0, 0, 0, 0, 0 } },
    { "Blue Spin",    {   0,  74, 128, 128,   0,  80, 190, 180, 112,   0, 0, 0, 0, 0, 0, 0 } },
    { "White Pulse",  {   0,  18, 128, 128,   0,   0, 220, 118, 160,  52, 0, 0, 0, 0, 0, 0 } },
    { "Rainbow Wash", {   0,  54, 128, 128,  36,  18, 160, 112, 208,   0, 0, 0, 0, 0, 0, 0 } },
    { "Party Sweep",  {   0,  88, 128, 128,  80,  24, 150, 200, 240,  22, 0, 0, 0, 0, 0, 0 } },
};

constexpr size_t kSceneCount = sizeof(kScenes) / sizeof(kScenes[0]);

}  // namespace projector
