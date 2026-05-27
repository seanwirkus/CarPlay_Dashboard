#pragma once

// ============================================================
// ESP32-S3 Dashboard Board Pin Definitions
// ============================================================

// On Waveshare ESP32-S3-Touch-LCD-7B, GPIO2 is an RGB data line (R4). Do not
// use it as a “status LED” — toggling it corrupts the panel.

// Exterior WS2812 strips are wired to the ESP32-C3 sensor hub (see c3_pins.h).

// UART bridge to ESP32-C3 (hardwired serial link, parallel to ESP-NOW).
// Waveshare UART2 terminal is direct ESP32-S3 UART0:
//   display UART2 RX -> GPIO44
//   display UART2 TX -> GPIO43
// Wiring for C3 -> S3 data:
//   C3 TX GPIO1 -> display UART2 RX
//   display GND -> C3 GND
// C3 RX is optional; connect display UART2 TX -> C3 RX GPIO0 only if we later
// need return traffic.
#define UART_BRIDGE_TX_PIN   43
#define UART_BRIDGE_RX_PIN   44
#define UART_BRIDGE_BAUD     115200
