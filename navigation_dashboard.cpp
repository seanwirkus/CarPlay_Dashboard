#include "navigation_dashboard.h"
#include <string.h>
#include <math.h>

// Helper macros for trigonometric calculations
#define DEG_TO_RAD(deg) ((deg) * M_PI / 180.0)
#define RAD_TO_DEG(rad) ((rad) * 180.0 / M_PI)

/**
 * @brief Initialize the navigation dashboard component
 * @param nav Pointer to navigation dashboard structure
 */
void NavigationDashboard_Init(NavigationDashboard* nav) {
    if (nav == NULL) return;

    // Initialize default state
    memset(nav, 0, sizeof(NavigationDashboard));

    // Set default values
    strcpy(nav->state.distance, "4.2 mi");
    strcpy(nav->state.instruction, "Take a slight left turn for I-395 North");
    nav->state.totalMileage = 14175;
    strcpy(nav->state.drivingMode, "ECO");
    nav->state.temperature = 161;
    nav->state.batteryLevel = 85;
    nav->state.compassHeading = 161;
    nav->state.showNavigation = true;
    nav->state.isNavigating = true;
    nav->state.progressPercent = 65;

    nav->initialized = true;
}

/**
 * @brief Update navigation state with new data
 * @param nav Pointer to navigation dashboard structure
 * @param newState New navigation state data
 */
void NavigationDashboard_UpdateState(NavigationDashboard* nav, NavigationState* newState) {
    if (nav == NULL || newState == NULL) return;

    memcpy(&nav->state, newState, sizeof(NavigationState));
}

/**
 * @brief Render the complete navigation dashboard
 * @param nav Pointer to navigation dashboard structure
 * @param x Top-left X coordinate for the component
 * @param y Top-left Y coordinate for the component
 */
void NavigationDashboard_Render(NavigationDashboard* nav, uint16_t x, uint16_t y) {
    if (nav == NULL || !nav->initialized) return;

    // Draw background first
    NavigationDashboard_DrawBackground(x, y);

    // Draw navigation elements
    NavigationDashboard_DrawNavigationText(nav, x, y);
    NavigationDashboard_DrawVehicleInfo(nav, x, y);
    NavigationDashboard_DrawCompass(nav, x, y);
    NavigationDashboard_DrawProgressBar(nav->state.progressPercent, x + NAV_PROGRESS_X, y + NAV_PROGRESS_Y);
    NavigationDashboard_DrawTemperatureGauge(nav->state.temperature, x + NAV_COMPASS_CENTER_X, y + NAV_COMPASS_CENTER_Y, NAV_COMPASS_RADIUS);
    NavigationDashboard_DrawBatteryIndicator(nav->state.batteryLevel, x + 50, y + 50); // Position to be adjusted
}

/**
 * @brief Render navigation background and container
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 */
void NavigationDashboard_DrawBackground(uint16_t x, uint16_t y) {
    // Draw main background rectangle
    Paint_DrawRectangle(x + NAV_BACKGROUND_X, y + NAV_BACKGROUND_Y,
                       x + NAV_BACKGROUND_X + NAV_BACKGROUND_WIDTH,
                       y + NAV_BACKGROUND_Y + NAV_BACKGROUND_HEIGHT,
                       NAV_BACKGROUND_COLOR, DOT_PIXEL_1X1, DRAW_FILL_FULL);
}

/**
 * @brief Render navigation text elements
 * @param nav Pointer to navigation dashboard structure
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 */
void NavigationDashboard_DrawNavigationText(NavigationDashboard* nav, uint16_t x, uint16_t y) {
    if (!nav->state.showNavigation) return;

    // Draw distance
    Paint_DrawString_EN(x + NAV_DISTANCE_X, y + NAV_DISTANCE_Y,
                       nav->state.distance, &Font24, NAV_TEXT_WHITE, NAV_BACKGROUND_COLOR);

    // Draw instruction
    Paint_DrawString_EN(x + NAV_INSTRUCTION_X, y + NAV_INSTRUCTION_Y,
                       nav->state.instruction, &Font16, NAV_TEXT_WHITE, NAV_BACKGROUND_COLOR);
}

/**
 * @brief Render vehicle information display
 * @param nav Pointer to navigation dashboard structure
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 */
void NavigationDashboard_DrawVehicleInfo(NavigationDashboard* nav, uint16_t x, uint16_t y) {
    char mileageStr[16];
    NavigationDashboard_FormatMileage(nav->state.totalMileage, mileageStr);

    // Draw total mileage
    Paint_DrawString_EN(x + NAV_TOTAL_MILEAGE_X, y + NAV_TOTAL_MILEAGE_Y,
                       mileageStr, &Font12, NAV_TEXT_GRAY, NAV_BACKGROUND_COLOR);

    // Draw driving mode
    Paint_DrawString_EN(x + NAV_MODE_X, y + NAV_MODE_Y,
                       nav->state.drivingMode, &Font16, NAV_TEXT_GREEN, NAV_BACKGROUND_COLOR);
}

/**
 * @brief Render compass/gauge component
 * @param nav Pointer to navigation dashboard structure
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 */
void NavigationDashboard_DrawCompass(NavigationDashboard* nav, uint16_t x, uint16_t y) {
    uint16_t centerX = x + NAV_COMPASS_CENTER_X;
    uint16_t centerY = y + NAV_COMPASS_CENTER_Y;
    uint16_t radius = NAV_COMPASS_RADIUS;

    // Draw compass circle
    Paint_DrawCircle(centerX, centerY, radius, NAV_ACCENT_COLOR, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);

    // Draw cardinal directions (N, E, S, W)
    Paint_DrawString_EN(centerX - 4, centerY - radius - 15, "N", &Font8, NAV_TEXT_WHITE, NAV_BACKGROUND_COLOR);
    Paint_DrawString_EN(centerX + radius + 2, centerY - 4, "E", &Font8, NAV_TEXT_WHITE, NAV_BACKGROUND_COLOR);
    Paint_DrawString_EN(centerX - 4, centerY + radius + 2, "S", &Font8, NAV_TEXT_WHITE, NAV_BACKGROUND_COLOR);
    Paint_DrawString_EN(centerX - radius - 10, centerY - 4, "W", &Font8, NAV_TEXT_WHITE, NAV_BACKGROUND_COLOR);

    // Draw heading indicator
    int16_t heading = nav->state.compassHeading;
    uint16_t needleLength = radius - 5;

    // Calculate needle endpoint
    float rad = DEG_TO_RAD(heading);
    uint16_t endX = centerX + (int16_t)(needleLength * sin(rad));
    uint16_t endY = centerY - (int16_t)(needleLength * cos(rad));

    // Draw needle
    Paint_DrawLine(centerX, centerY, endX, endY, NAV_TEXT_WHITE, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
}

/**
 * @brief Render progress bar for route completion
 * @param percent Progress percentage (0-100)
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 */
void NavigationDashboard_DrawProgressBar(uint8_t percent, uint16_t x, uint16_t y) {
    uint16_t width = NAV_PROGRESS_WIDTH;
    uint16_t height = NAV_PROGRESS_HEIGHT;

    // Draw background
    Paint_DrawRectangle(x, y, x + width, y + height, NAV_ACCENT_COLOR, DOT_PIXEL_1X1, DRAW_FILL_FULL);

    // Draw progress fill
    uint16_t fillWidth = (uint16_t)((percent * width) / 100);
    if (fillWidth > 0) {
        Paint_DrawRectangle(x, y, x + fillWidth, y + height, NAV_PROGRESS_COLOR, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    }

    // Draw border
    Paint_DrawRectangle(x, y, x + width, y + height, NAV_TEXT_WHITE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
}

/**
 * @brief Render directional arrow icon
 * @param x Center X coordinate
 * @param y Center Y coordinate
 * @param size Arrow size
 * @param color Arrow color
 */
void NavigationDashboard_DrawArrow(uint16_t x, uint16_t y, uint16_t size, uint16_t color) {
    // Simple arrow pointing right
    uint16_t halfSize = size / 2;

    // Draw arrow shaft
    Paint_DrawLine(x - halfSize, y, x + halfSize, y, color, DOT_PIXEL_2X2, LINE_STYLE_SOLID);

    // Draw arrow head
    Paint_DrawLine(x + halfSize, y, x + halfSize - 5, y - 5, color, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
    Paint_DrawLine(x + halfSize, y, x + halfSize - 5, y + 5, color, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
}

/**
 * @brief Render circular gauge with temperature
 * @param value Temperature value
 * @param x Center X coordinate
 * @param y Center Y coordinate
 * @param radius Gauge radius
 */
void NavigationDashboard_DrawTemperatureGauge(uint16_t value, uint16_t x, uint16_t y, uint16_t radius) {
    // Draw gauge background
    Paint_DrawCircle(x, y, radius, NAV_ACCENT_COLOR, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);

    // Draw temperature value in center
    char tempStr[8];
    sprintf(tempStr, "%d", value);
    Paint_DrawString_EN(x - 15, y - 8, tempStr, &Font12, NAV_TEXT_WHITE, NAV_BACKGROUND_COLOR);
    Paint_DrawString_EN(x + 5, y - 8, "°", &Font8, NAV_TEXT_WHITE, NAV_BACKGROUND_COLOR);
}

/**
 * @brief Render battery indicator
 * @param level Battery level (0-100)
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 */
void NavigationDashboard_DrawBatteryIndicator(uint8_t level, uint16_t x, uint16_t y) {
    uint16_t width = 30;
    uint16_t height = 15;

    // Draw battery outline
    Paint_DrawRectangle(x, y, x + width, y + height, NAV_TEXT_WHITE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);

    // Draw battery terminal
    Paint_DrawRectangle(x + width, y + 4, x + width + 2, y + height - 4, NAV_TEXT_WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);

    // Draw battery level
    uint16_t fillWidth = (uint16_t)((level * (width - 2)) / 100);
    if (fillWidth > 0) {
        uint16_t color = (level > 20) ? NAV_TEXT_GREEN : NAV_ACCENT_COLOR;
        Paint_DrawRectangle(x + 1, y + 1, x + 1 + fillWidth, y + height - 1, color, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    }

    // Draw percentage text
    char levelStr[8];
    sprintf(levelStr, "%d%%", level);
    Paint_DrawString_EN(x + width + 5, y, levelStr, &Font8, NAV_TEXT_WHITE, NAV_BACKGROUND_COLOR);
}

/**
 * @brief Draw a rounded rectangle with shadow
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 * @param width Rectangle width
 * @param height Rectangle height
 * @param radius Corner radius
 * @param fillColor Fill color
 * @param borderColor Border color
 */
void NavigationDashboard_DrawRoundedRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                                       uint16_t radius, uint16_t fillColor, uint16_t borderColor) {
    // For simplicity, draw regular rectangle since Paint doesn't have rounded rect
    // This could be enhanced with custom drawing using lines and arcs
    Paint_DrawRectangle(x, y, x + width, y + height, fillColor, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    if (borderColor != fillColor) {
        Paint_DrawRectangle(x, y, x + width, y + height, borderColor, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    }
}

/**
 * @brief Utility function to draw a line with specified angle and length
 * @param x1 Starting X coordinate
 * @param y1 Starting Y coordinate
 * @param length Line length
 * @param angle Angle in degrees
 * @param color Line color
 * @param width Line width
 */
void NavigationDashboard_DrawAngledLine(uint16_t x1, uint16_t y1, uint16_t length, int16_t angle,
                                      uint16_t color, uint8_t width) {
    float rad = DEG_TO_RAD(angle);
    uint16_t x2 = x1 + (uint16_t)(length * cos(rad));
    uint16_t y2 = y1 + (uint16_t)(length * sin(rad));

    DOT_PIXEL dotPixel = (width == 1) ? DOT_PIXEL_1X1 : DOT_PIXEL_2X2;
    Paint_DrawLine(x1, y1, x2, y2, color, dotPixel, LINE_STYLE_SOLID);
}

/**
 * @brief Convert temperature value to display string
 * @param temp Temperature value
 * @param buffer Output buffer
 */
void NavigationDashboard_FormatTemperature(uint16_t temp, char* buffer) {
    sprintf(buffer, "%d°", temp);
}

/**
 * @brief Convert mileage to display string with commas
 * @param mileage Mileage value
 * @param buffer Output buffer
 */
void NavigationDashboard_FormatMileage(uint32_t mileage, char* buffer) {
    // Simple formatting - could be enhanced for comma separation
    sprintf(buffer, "%lu", mileage);
}
