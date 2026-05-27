#pragma once

#include <Arduino.h>

// Initialize I2C IO-expander, RGB LCD, and turn backlight on. Call before WiFi/SPIFFS.
bool waveshare_7b_display_begin();

// Draw boot / bridge instructions (large IP, color bands). Call after softAP is up.
void waveshare_7b_show_splash(const char *wifiSsid, const char *wifiPass, const char *ipAddr);
