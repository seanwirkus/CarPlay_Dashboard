#pragma once

// Legacy compatibility header for archived experiments.
// Active C3 sensor firmware should include c3_pins.h directly.
#include "c3_pins.h"

// UART0 (available for external use; USB CDC handles serial monitor)
#define UART0_RX        20
#define UART0_TX        21

// DMX512 output via a 3.3V RS-485 transceiver such as MAX3485 / SN65HVD1781.
// ESP32 TX  -> DI
// GPIO10    -> DE and /RE tied together (HIGH = transmit)
// A/B       -> projector DMX IN
#define DMX_TX_PIN      UART0_TX
#define DMX_RX_PIN      UART0_RX
#define DMX_DIR_PIN     10

// TP4056 Battery Charge Board
// Wiring: Battery+ → 100K → GPIO2 junction → 100K → GND  (voltage divider)
//         TP4056 CHRG  pin → GPIO3  (active LOW = currently charging)
//         TP4056 STDBY pin → GPIO4  (active LOW = charge complete / battery full)
#define BAT_ADC_PIN     2   // ADC: battery voltage through 100K/100K divider
#define BAT_CHRG_PIN    3   // TP4056 CHRG  (INPUT_PULLUP, LOW = charging)
#define BAT_STDBY_PIN   4   // TP4056 STDBY (INPUT_PULLUP, LOW = full)

// HC-SR04 Ultrasonic Sensor
// IMPORTANT: Echo output is 5V — use a voltage divider to bring it to 3.3V!
//   Echo pin → 1KΩ → GPIO0 junction → 2KΩ → GND
#define US_TRIG_PIN     1   // Trigger output
#define US_ECHO_PIN     0   // Echo input (through voltage divider!)

// LED Strip / IR Output
// GPIO7 is the general-purpose output currently used to drive the LED strip
// controller's IR input line.
//
// If you wire into the controller's demodulated "IR" signal node, drive it as
// an active-LOW open-drain output so it behaves like the original IR receiver:
// idle HIGH, pulse LOW for marks.
#define LED_STRIP_PIN   7
#define IR_TX_PIN       LED_STRIP_PIN

// Available GPIO pins for general use
// GPIO 10
// ADC capable: GPIO 0-5
