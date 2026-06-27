#pragma once

// ============================================================
// ESP32-C3 Sensor Hub Pin Definitions
// ============================================================

// OLED Display (SSD1306, 72x40 visible in 128x64 buffer)
#define OLED_SDA        5
#define OLED_SCL        6
#define OLED_ADDR       0x3C

// Visible OLED window
#define OLED_WIDTH      72
#define OLED_HEIGHT     40
#define OLED_X_OFFSET   30
#define OLED_Y_OFFSET   12

// Built-in LED (active LOW: write LOW to turn ON)
#define LED_PIN         8

// Added external WiFi/serial module. Do not use GPIO5/GPIO6: those are the
// C3 OLED I2C bus. GPIO20/21 are currently used as a second bridge UART TX/RX
// so the board's TX-labeled pad can also feed the S3 UART2 terminal.
#define EXT_WIFI_MODULE_RX_PIN 20
#define EXT_WIFI_MODULE_TX_PIN 21

// Vehicle exterior WS2812 strips (18 LEDs each). Front = front of car, rear = back.
// Note: GPIO9 is the BOOT strap on many ESP32-C3 modules; your wiring may use a different data pin.
#define C3_FRONT_LED_PIN    9
#define C3_REAR_LED_PIN     10
#define C3_FRONT_LED_COUNT  18
#define C3_REAR_LED_COUNT   18

// HC-SR04 ultrasonic sensor (DISABLED — pins reused for UART bridge below)
// IMPORTANT: Echo output is 5V and must be level-shifted to 3.3V.
#define US_TRIG_PIN     1
#define US_ECHO_PIN     0

// UART bridge to ESP32-S3 (hardwired serial link, parallel to ESP-NOW).
// Primary wiring: C3 GPIO1 TX -> S3 UART2 RX, shared GND.
// Fallback wiring: the same packet is also sent on C3 GPIO21 TX, which is the
// common TX-labeled hardware UART pad on many ESP32-C3 boards.
#define UART_BRIDGE_TX_PIN   1
#define UART_BRIDGE_RX_PIN   0
#define UART_BRIDGE_ALT_TX_PIN 21
#define UART_BRIDGE_ALT_RX_PIN 20
#define UART_BRIDGE_BAUD     115200

// Tachometer input from optocoupler OUT (3.3V logic, pulses falling-edge per ignition event).
// Connect opto board VCC->3V3, GND->GND, OUT->GPIO4. R1 must be sized for 12V input.
#define TACH_PIN        4
// Pulses per crankshaft revolution: 2 for OEM tach output wire (cluster), 0.5 for a single coil-on-plug trigger on a 4-cyl 4-stroke.
#define TACH_PULSES_PER_REV  2.0f

// Short-period rejection: reject pulses arriving faster than this percentage of the
// last accepted period. MUST be below 50% to avoid latch-up after a missed pulse
// (a double-period inflates tachPeriodUs, and real pulses at 50% would be rejected
// if this threshold were ≥50%).


// Vehicle Speed Sensor (VSS) input from second optocoupler OUT.
// Connect opto board VCC->3V3, GND->GND, OUT->GPIO7. + INPUT taps the VSS signal wire from transmission.
#define VSS_PIN         7
// 06 Subaru WRX VSS: ~4000 pulses per mile. Adjust after calibration drive.
#define VSS_PULSES_PER_MILE  4000.0f

// Fuel sender ADC. Wire as voltage divider: 3.3V -> [100 ohm fixed] -> ADC pin -> [fuel sender to GND].
// 06 WRX sender path: roughly 1-5 ohms full -> 92-96 ohms empty.
// Full = low voltage on ADC, empty = higher voltage.
#define FUEL_PIN        2
// Keep this disabled until the sender divider is physically wired. A floating ADC can look like a real fuel value.
#define FUEL_SENDER_ENABLED 0
// Sender resistance bounds (ohms) used for linear mapping to percent.
#define FUEL_R_FULL      3.0f
#define FUEL_R_EMPTY     94.0f
// Pull-up resistor value used in the divider.
#define FUEL_PULLUP_OHMS 100.0f

// Vehicle exterior lighting inputs (optocoupler OUT → GPIO, active LOW when lamp on).
// Enable with -DVEHICLE_LIGHTS_ENABLED=1. Sacrifices DHT (GPIO3), status LED (GPIO8), and
// the ext-WiFi UART pads (GPIO20/21) — do not enable if those peripherals are wired.
#ifndef VEHICLE_LIGHTS_ENABLED
#define VEHICLE_LIGHTS_ENABLED 0
#endif

#if VEHICLE_LIGHTS_ENABLED
#define LIGHT_LEFT_PIN    3   // was DHT — column BLK/GRN turn feed via opto
#define LIGHT_RIGHT_PIN   8   // was status LED — column LT GRN/BLK via opto
#define LIGHT_BRAKE_PIN   20  // brake pedal WHT/BLK via opto (conflicts with EXT_WIFI RX)
#define LIGHT_HEAD_PIN    21  // parking/head circuit via opto (conflicts with EXT_WIFI TX)
// Hazard is derived in firmware when left AND right are simultaneously active.
#endif
