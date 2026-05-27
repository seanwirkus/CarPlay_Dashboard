#pragma once

#include <stdint.h>

// ============================================================
// ESP-NOW control packet: ESP32-S3 (dashboard) → ESP32-C3
// Commands exterior light flags; C3 merges into lightFlags and
// re-broadcasts them on the next SensorPacket.
// ============================================================

constexpr uint32_t CONTROL_MAGIC   = 0x4354524C;  // 'CTRL'
constexpr uint8_t  CONTROL_VERSION = 1;

struct __attribute__((packed)) ControlPacket {
    uint32_t magic;
    uint8_t  version;
    uint8_t  lights;   // same LightBits as SensorPacket
    uint8_t  _pad[2];
};
