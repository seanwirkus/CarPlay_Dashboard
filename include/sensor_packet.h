#pragma once

#include <stdint.h>

// ============================================================
// Shared ESP-NOW Packet Definition
// Used by both ESP32-C3 (sender) and ESP32-S3 (receiver)
// ============================================================

// Both sender (C3) and receiver (S3) must share this channel.
constexpr uint8_t  ESPNOW_CHANNEL  = 11;

// 4-byte magic lets the receiver ignore unrelated ESP-NOW traffic.
constexpr uint32_t PACKET_MAGIC    = 0x45535052;  // 'ESPR'
constexpr uint8_t  PACKET_VERSION  = 2;

// SensorPacket.lights — bit flags for S3 WS2812 strips.
enum LightBits : uint8_t {
    LIGHT_HEAD   = 1 << 0,
    LIGHT_LEFT   = 1 << 1,
    LIGHT_RIGHT  = 1 << 2,
    LIGHT_BRAKE  = 1 << 3,
    LIGHT_HAZARD = 1 << 4,
};

enum SensorStatusBits : uint8_t {
    SENSOR_FUEL_CONNECTED = 1 << 0,
    SENSOR_TACH_LOCKED    = 1 << 1,
    SENSOR_VSS_ACTIVE     = 1 << 2,
    SENSOR_DISTANCE_VALID = 1 << 3,
    SENSOR_DHT_VALID      = 1 << 4,
};

struct __attribute__((packed)) SensorPacket {
    uint32_t magic;
    uint8_t  version;
    uint8_t  lights;          // LIGHT_* flags
    uint16_t rpm;             // engine RPM from tach (0 = stalled/no signal)
    uint32_t seq;
    uint32_t millisSent;
    float    distanceCm;      // smoothed
    float    distanceCmRaw;   // pre-filter (NAN if no echo)
    float    tempC;
    float    humidity;
    uint16_t mph;             // vehicle speed from VSS pulses
    uint8_t  fuelPct;         // fuel tank level 0..100
    uint8_t  statusFlags;     // SENSOR_* flags
};
