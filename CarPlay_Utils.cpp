/*
 * CarPlay Dashboard Utilities - Clean Version
 * Helper functions for gauge drawing, animations, and data processing
 */

#include "CarPlay_Config.h"
#include "gui_paint.h"
#include <math.h>
#include <string.h>
#include <Arduino.h>

// Math utilities
#define PI 3.14159265359
#define DEG_TO_RAD(angle) ((angle) * PI / 180.0)

// Gauge animation state
typedef struct {
    float currentValue;
    float targetValue;
    float lastUpdateTime;
    bool isAnimating;
} GaugeAnimation;

// Global gauge animations
GaugeAnimation speedGaugeAnim = {0, 0, 0, false};
GaugeAnimation rpmGaugeAnim = {800, 800, 0, false};

// Function declarations
float interpolateValue(float start, float end, float factor);
float constrainValue(float value, float min, float max);
void initGaugeAnimation(GaugeAnimation *anim, float initialValue);
void updateGaugeAnimation(GaugeAnimation *anim, float newTarget);
float getAnimatedValue(GaugeAnimation *anim);
void drawGaugeNeedle(int centerX, int centerY, float angle, float length, uint16_t color);
void drawProgressArc(int centerX, int centerY, int radius, float startAngle, float endAngle, float progress, uint16_t color);
void drawDigitalDisplay(int x, int y, int width, int height, const char* text, uint16_t bgColor, uint16_t textColor, uint16_t borderColor);
void drawLED(int x, int y, bool state, uint16_t colorOn, uint16_t colorOff);
void drawProgressBar(int x, int y, int width, int height, float progress, uint16_t bgColor, uint16_t fillColor, uint16_t borderColor);
void drawSegmentedDisplay(int x, int y, const char* text, uint16_t color, uint16_t bgColor);
void drawSevenSegment(int x, int y, char digit, uint16_t color, uint16_t bgColor);
void drawSegment(int x, int y, int width, int height, uint16_t color);
void drawWarningIndicator(int x, int y, const char* text, bool isActive);
void formatNumberForDisplay(char* buffer, int value, int digits);
float convertTemperature(float tempC, bool useFahrenheit);
float convertSpeed(float speedKmh, bool useMiles);
void debugLog(int level, const char* message);
void debugLogValue(int level, const char* message, float value);

// Utility function to interpolate between values
float interpolateValue(float start, float end, float factor) {
    return start + (end - start) * factor;
}

// Utility function to constrain a value between min and max
float constrainValue(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

// Initialize gauge animation
void initGaugeAnimation(GaugeAnimation *anim, float initialValue) {
    anim->currentValue = initialValue;
    anim->targetValue = initialValue;
    anim->lastUpdateTime = millis();
    anim->isAnimating = false;
}

// Update gauge animation
void updateGaugeAnimation(GaugeAnimation *anim, float newTarget) {
    anim->targetValue = newTarget;
    anim->lastUpdateTime = millis();
    anim->isAnimating = true;
}

// Get current animated value
float getAnimatedValue(GaugeAnimation *anim) {
    unsigned long currentTime = millis();
    unsigned long elapsed = currentTime - anim->lastUpdateTime;
    
    if (!anim->isAnimating) {
        return anim->currentValue;
    }
    
    // Calculate animation progress (0.0 to 1.0)
    float progress = constrainValue((float)elapsed / 500.0, 0.0, 1.0); // 500ms animation
    
    // Easing function for smooth animation
    float easedProgress = progress * progress * (3.0 - 2.0 * progress); // Smoothstep
    
    anim->currentValue = interpolateValue(anim->currentValue, anim->targetValue, easedProgress);
    
    if (progress >= 1.0) {
        anim->currentValue = anim->targetValue;
        anim->isAnimating = false;
    }
    
    return anim->currentValue;
}

// Draw animated gauge needle
void drawGaugeNeedle(int centerX, int centerY, float angle, float length, uint16_t color) {
    float rad = DEG_TO_RAD(angle);
    int endX = centerX + (length * cos(rad));
    int endY = centerY + (length * sin(rad));
    Paint_DrawLine(centerX, centerY, endX, endY, color, DOT_PIXEL_3X3, LINE_STYLE_SOLID);
}

// Draw circular progress arc
void drawProgressArc(int centerX, int centerY, int radius, float startAngle, float endAngle, float progress, uint16_t color) {
    int steps = 50;
    int lastX = -1, lastY = -1;
    
    for (int i = 0; i <= steps * progress; i++) {
        float angle = startAngle + (endAngle - startAngle) * (float)i / steps;
        float rad = DEG_TO_RAD(angle);
        int x = centerX + (radius * cos(rad));
        int y = centerY + (radius * sin(rad));
        
        if (lastX != -1) {
            Paint_DrawLine(lastX, lastY, x, y, color, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
        }
        lastX = x;
        lastY = y;
    }
}

// Draw digital display box
void drawDigitalDisplay(int x, int y, int width, int height, const char* text, uint16_t bgColor, uint16_t textColor, uint16_t borderColor) {
    // Background
    Paint_DrawRectangle(x, y, x + width, y + height, bgColor, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    
    // Border
    Paint_DrawRectangle(x, y, x + width, y + height, borderColor, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    
    // Text
    if (text != NULL) {
        int textWidth = strlen(text) * 8; // Approximate width per character
        int textX = x + (width - textWidth) / 2;
        int textY = y + (height - 16) / 2;
        Paint_DrawString_EN(textX, textY, text, FONT_SIZE_BODY, textColor, bgColor);
    }
}

// Draw LED indicator
void drawLED(int x, int y, bool state, uint16_t colorOn, uint16_t colorOff) {
    uint16_t color = state ? colorOn : colorOff;
    Paint_DrawCircle(x, y, 8, color, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawCircle(x, y, 8, COLOR_WHITE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
}

// Draw progress bar
void drawProgressBar(int x, int y, int width, int height, float progress, uint16_t bgColor, uint16_t fillColor, uint16_t borderColor) {
    // Background
    Paint_DrawRectangle(x, y, x + width, y + height, bgColor, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    
    // Fill
    int fillWidth = (int)(width * constrainValue(progress, 0.0, 1.0));
    if (fillWidth > 0) {
        Paint_DrawRectangle(x + 1, y + 1, x + fillWidth - 1, y + height - 1, fillColor, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    }
    
    // Border
    Paint_DrawRectangle(x, y, x + width, y + height, borderColor, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
}

// Draw segmented display (simplified version)
void drawSegmentedDisplay(int x, int y, const char* text, uint16_t color, uint16_t bgColor) {
    // For now, just draw as regular text - can be enhanced later
    Paint_DrawString_EN(x, y, text, FONT_SIZE_BODY, color, bgColor);
}

// Draw seven-segment digit (simplified)
void drawSevenSegment(int x, int y, char digit, uint16_t color, uint16_t bgColor) {
    // Simple implementation - draw basic rectangle for each digit
    char str[2] = {digit, '\0'};
    drawDigitalDisplay(x, y, 12, 20, str, bgColor, color, color);
}

// Helper function to draw a segment
void drawSegment(int x, int y, int width, int height, uint16_t color) {
    Paint_DrawRectangle(x, y, x + width, y + height, color, DOT_PIXEL_1X1, DRAW_FILL_FULL);
}

// Draw warning indicator with animation
void drawWarningIndicator(int x, int y, const char* text, bool isActive) {
    uint16_t bgColor = isActive ? WARNING_CRITICAL : COLOR_DARK_GRAY;
    uint16_t textColor = isActive ? COLOR_WHITE : COLOR_GRAY;
    
    if (isActive) {
        // Flashing effect
        static unsigned long lastFlash = 0;
        unsigned long currentTime = millis();
        if (currentTime - lastFlash > 500) {
            lastFlash = currentTime;
            // Toggle background for flashing effect
            bgColor = (currentTime / 500) % 2 ? WARNING_CRITICAL : COLOR_DARK_GRAY;
        }
    }
    
    drawDigitalDisplay(x, y, 80, 30, text, bgColor, textColor, COLOR_WHITE);
}

// Format number for display (adds leading zeros)
void formatNumberForDisplay(char* buffer, int value, int digits) {
    sprintf(buffer, "%0*d", digits, value);
}

// Convert temperature between C and F
float convertTemperature(float tempC, bool useFahrenheit) {
    if (useFahrenheit) {
        return (tempC * 9.0/5.0) + 32.0;
    }
    return tempC;
}

// Convert speed between km/h and mph
float convertSpeed(float speedKmh, bool useMiles) {
    if (useMiles) {
        return speedKmh * 0.621371;
    }
    return speedKmh;
}

// Debug logging function
void debugLog(int level, const char* message) {
    #if DEBUG_SERIAL
    if (level <= DEBUG_LEVEL) {
        Serial.println(message);
    }
    #endif
}

// Debug logging with value
void debugLogValue(int level, const char* message, float value) {
    #if DEBUG_SERIAL
    if (level <= DEBUG_LEVEL) {
        Serial.print(message);
        Serial.println(value);
    }
    #endif
}