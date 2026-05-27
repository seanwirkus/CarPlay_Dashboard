#include "BatteryMonitor.h"

// ============================================================
// LiPo discharge curve lookup table
// Voltage steps from 4.20V down to 3.10V → 100% down to 0%
// ============================================================
static const float   kVoltTable[] = { 4.20f, 4.10f, 4.00f, 3.90f, 3.80f,
                                       3.70f, 3.60f, 3.50f, 3.40f, 3.30f,
                                       3.20f, 3.10f };
static const uint8_t kPctTable[]  = {   100,    90,    80,    70,    60,
                                          50,    40,    30,    20,    10,
                                           5,     0 };
static const uint8_t kTableLen    = sizeof(kPctTable);

BatteryMonitor::BatteryMonitor(uint8_t adcPin,
                               uint8_t chrgPin,
                               uint8_t stdbyPin,
                               float   dividerRatio)
    : _adcPin(adcPin),
      _chrgPin(chrgPin),
      _stdbyPin(stdbyPin),
      _dividerRatio(dividerRatio)
{}

void BatteryMonitor::begin() {
    // ADC pin is input-only on ESP32-C3; nothing to configure
    if (_chrgPin  != 255) { pinMode(_chrgPin,  INPUT_PULLUP); }
    if (_stdbyPin != 255) { pinMode(_stdbyPin, INPUT_PULLUP); }
}

float BatteryMonitor::readVoltage() {
    // Average 16 samples to reduce ADC noise
    uint32_t sum = 0;
    for (uint8_t i = 0; i < 16; i++) {
        sum += analogReadMilliVolts(_adcPin);
        delayMicroseconds(200);
    }
    float mvAvg = sum / 16.0f;

    // Restore full battery voltage by reversing the divider
    return (mvAvg / 1000.0f) * _dividerRatio;
}

uint8_t BatteryMonitor::readPercent() {
    return voltageToPercent(readVoltage());
}

BatteryStatus BatteryMonitor::readStatus() {
    if (_chrgPin == 255 && _stdbyPin == 255) {
        return BAT_UNKNOWN;
    }
    if (_chrgPin  != 255 && digitalRead(_chrgPin)  == LOW) return BAT_CHARGING;
    if (_stdbyPin != 255 && digitalRead(_stdbyPin) == LOW) return BAT_FULL;
    return BAT_DISCHARGING;
}

const char* BatteryMonitor::statusString() {
    switch (readStatus()) {
        case BAT_CHARGING:    return "CHARGING";
        case BAT_FULL:        return "FULL";
        case BAT_DISCHARGING: return "IN USE";
        default:              return "------";
    }
}

// ============================================================
// Private: interpolate battery % from voltage lookup table
// ============================================================
uint8_t BatteryMonitor::voltageToPercent(float v) {
    if (v >= kVoltTable[0])           return 100;
    if (v <= kVoltTable[kTableLen-1]) return 0;

    for (uint8_t i = 0; i < kTableLen - 1; i++) {
        if (v <= kVoltTable[i] && v > kVoltTable[i+1]) {
            // Linear interpolation between table entries
            float ratio = (v - kVoltTable[i+1]) /
                          (kVoltTable[i] - kVoltTable[i+1]);
            return (uint8_t)(kPctTable[i+1] +
                             ratio * (kPctTable[i] - kPctTable[i+1]));
        }
    }
    return 0;
}
