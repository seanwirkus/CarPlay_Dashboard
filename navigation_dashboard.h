#ifndef __NAVIGATION_DASHBOARD_H
#define __NAVIGATION_DASHBOARD_H

#include "gui_paint.h"
#include "fonts.h"
#include <stdint.h>
#include <stdbool.h>

// Navigation component configuration
#define NAV_DASHBOARD_WIDTH    1497
#define NAV_DASHBOARD_HEIGHT   582
#define NAV_DISPLAY_OFFSET_X   0
#define NAV_DISPLAY_OFFSET_Y   10

// UI Element positions based on UIDL specification
#define NAV_BACKGROUND_X       0
#define NAV_BACKGROUND_Y       10
#define NAV_BACKGROUND_WIDTH   1497
#define NAV_BACKGROUND_HEIGHT  572

// Navigation text positions
#define NAV_DISTANCE_X         602
#define NAV_DISTANCE_Y         195
#define NAV_INSTRUCTION_X      602
#define NAV_INSTRUCTION_Y      238
#define NAV_TOTAL_MILEAGE_X    690
#define NAV_TOTAL_MILEAGE_Y    414
#define NAV_MODE_X             534
#define NAV_MODE_Y             414
#define NAV_TEMPERATURE_X      243
#define NAV_TEMPERATURE_Y      296

// Gauge positions
#define NAV_COMPASS_CENTER_X   281
#define NAV_COMPASS_CENTER_Y   319
#define NAV_COMPASS_RADIUS     56

// Progress bar positions
#define NAV_PROGRESS_X         525
#define NAV_PROGRESS_Y         378
#define NAV_PROGRESS_WIDTH     447
#define NAV_PROGRESS_HEIGHT    27

// Color definitions for navigation dashboard
#define NAV_BACKGROUND_COLOR   0x1818  // Dark gray background
#define NAV_TEXT_WHITE         WHITE
#define NAV_TEXT_GREEN         0x00C0  // Green for ECO mode
#define NAV_TEXT_GRAY          0x5555  // Gray for secondary text
#define NAV_ACCENT_COLOR       0x3636  // Dark accent
#define NAV_PROGRESS_COLOR     0x2424  // Progress bar color

// Navigation state structure
typedef struct {
    // Navigation data
    char distance[16];           // e.g., "4.2 mi"
    char instruction[64];        // e.g., "Take a slight left turn for I-395 North"
    uint32_t totalMileage;       // e.g., 14175
    char drivingMode[8];         // e.g., "ECO"
    
    // Vehicle data
    uint16_t temperature;        // e.g., 161 (degrees)
    uint8_t batteryLevel;        // e.g., 85 (percent)
    int16_t compassHeading;      // e.g., 161 (degrees)
    
    // UI state
    bool showNavigation;         // Whether to show navigation
    bool isNavigating;           // Active navigation state
    uint8_t progressPercent;     // Route completion 0-100
    
} NavigationState;

// Navigation component structure
typedef struct {
    NavigationState state;
    bool initialized;
} NavigationDashboard;

// Function declarations

/**
 * @brief Initialize the navigation dashboard component
 * @param nav Pointer to navigation dashboard structure
 */
void NavigationDashboard_Init(NavigationDashboard* nav);

/**
 * @brief Update navigation state with new data
 * @param nav Pointer to navigation dashboard structure
 * @param newState New navigation state data
 */
void NavigationDashboard_UpdateState(NavigationDashboard* nav, NavigationState* newState);

/**
 * @brief Render the complete navigation dashboard
 * @param nav Pointer to navigation dashboard structure
 * @param x Top-left X coordinate for the component
 * @param y Top-left Y coordinate for the component
 */
void NavigationDashboard_Render(NavigationDashboard* nav, uint16_t x, uint16_t y);

/**
 * @brief Render navigation background and container
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 */
void NavigationDashboard_DrawBackground(uint16_t x, uint16_t y);

/**
 * @brief Render navigation text elements
 * @param nav Pointer to navigation dashboard structure
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 */
void NavigationDashboard_DrawNavigationText(NavigationDashboard* nav, uint16_t x, uint16_t y);

/**
 * @brief Render vehicle information display
 * @param nav Pointer to navigation dashboard structure
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 */
void NavigationDashboard_DrawVehicleInfo(NavigationDashboard* nav, uint16_t x, uint16_t y);

/**
 * @brief Render compass/gauge component
 * @param nav Pointer to navigation dashboard structure
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 */
void NavigationDashboard_DrawCompass(NavigationDashboard* nav, uint16_t x, uint16_t y);

/**
 * @brief Render progress bar for route completion
 * @param percent Progress percentage (0-100)
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 */
void NavigationDashboard_DrawProgressBar(uint8_t percent, uint16_t x, uint16_t y);

/**
 * @brief Render directional arrow icon
 * @param x Center X coordinate
 * @param y Center Y coordinate
 * @param size Arrow size
 * @param color Arrow color
 */
void NavigationDashboard_DrawArrow(uint16_t x, uint16_t y, uint16_t size, uint16_t color);

/**
 * @brief Render circular gauge with temperature
 * @param value Temperature value
 * @param x Center X coordinate
 * @param y Center Y coordinate
 * @param radius Gauge radius
 */
void NavigationDashboard_DrawTemperatureGauge(uint16_t value, uint16_t x, uint16_t y, uint16_t radius);

/**
 * @brief Render battery indicator
 * @param level Battery level (0-100)
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 */
void NavigationDashboard_DrawBatteryIndicator(uint8_t level, uint16_t x, uint16_t y);

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
                                       uint16_t radius, uint16_t fillColor, uint16_t borderColor);

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
                                      uint16_t color, uint8_t width);

/**
 * @brief Convert temperature value to display string
 * @param temp Temperature value
 * @param buffer Output buffer
 */
void NavigationDashboard_FormatTemperature(uint16_t temp, char* buffer);

/**
 * @brief Convert mileage to display string with commas
 * @param mileage Mileage value
 * @param buffer Output buffer
 */
void NavigationDashboard_FormatMileage(uint32_t mileage, char* buffer);

#endif /* __NAVIGATION_DASHBOARD_H */