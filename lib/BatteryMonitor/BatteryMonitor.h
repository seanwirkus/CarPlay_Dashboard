#pragma once
#include <Arduino.h>

// ============================================================
// BatteryMonitor Library for ESP32-C3
//
// Reads a 3.7V LiPo battery connected through a TP4056
// charge board.
//
// Wiring:
//   Battery+ ─┬─ 100K ─┬─ 100K ─ GND
//              │        └─── ADC pin (GPIO2)
//              └─ (also connects to TP4056 BAT+)
//   TP4056 CHRG  ──────────── GPIO3  (active LOW = charging)
//   TP4056 STDBY ──────────── GPIO4  (active LOW = charge full)
//
// The 100K/100K divider halves the battery voltage so the
// max ~4.2V becomes ~2.1V, safely within the 3.3V ADC range.
// ============================================================

enum BatteryStatus {
    BAT_CHARGING,     // CHRG pin asserted LOW
    BAT_FULL,         // STDBY pin asserted LOW (charging done)
    BAT_DISCHARGING,  // Neither pin asserted
    BAT_UNKNOWN       // Status pins not wired / floating
};

class BatteryMonitor {
public:
    // adcPin:        GPIO with 100K/100K voltage divider to battery+
    // chrgPin:       TP4056 CHRG  pin, 255 = not connected
    // stdbyPin:      TP4056 STDBY pin, 255 = not connected
    // dividerRatio:  voltage multiplier (2.0 for equal-value divider)
    BatteryMonitor(uint8_t adcPin,
                   uint8_t chrgPin      = 255,
                   uint8_t stdbyPin     = 255,
                   float   dividerRatio = 2.0f);

    void begin();

    // Returns averaged battery voltage in volts (e.g. 3.82)
    float readVoltage();

    // Returns battery state-of-charge 0–100 (LiPo curve)
    uint8_t readPercent();

    // Returns BAT_CHARGING / BAT_FULL / BAT_DISCHARGING
    BatteryStatus readStatus();

    // Returns human-readable status string
    const char* statusString();

private:
    uint8_t _adcPin;
    uint8_t _chrgPin;
    uint8_t _stdbyPin;
    float   _dividerRatio;

    // Map voltage to percent via LiPo discharge curve lookup table
    static uint8_t voltageToPercent(float v);
};
