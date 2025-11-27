/*
 * CarPlay Dashboard Configuration
 * Easy customization for different displays and preferences
 */

#ifndef CARPLAY_CONFIG_H
#define CARPLAY_CONFIG_H

// Display Configuration
#define DISPLAY_WIDTH              1024
#define DISPLAY_HEIGHT             600
#define DISPLAY_ROTATION           0

// Color Scheme (Apple CarPlay Dark Theme)
#define COLOR_BLACK                0x0000
#define COLOR_DARK_GRAY            0x4208
#define COLOR_GRAY                 0x8410
#define COLOR_LIGHT_GRAY           0xC618
#define COLOR_WHITE                0xFFFF
#define COLOR_BLUE                 0x1C7F
#define COLOR_GREEN                0x07E0
#define COLOR_RED                  0xF800
#define COLOR_ORANGE               0xFC00
#define COLOR_YELLOW               0xFFE0
#define COLOR_PURPLE               0x8010

// Layout Configuration
#define TOP_BAR_HEIGHT             80
#define BOTTOM_BAR_HEIGHT          100
#define SIDE_MARGIN                20
#define GAUGE_AREA_WIDTH           300
#define CENTER_AREA_WIDTH          (DISPLAY_WIDTH - (GAUGE_AREA_WIDTH * 2) - (SIDE_MARGIN * 3))

// Gauge Settings
#define SPEEDOMETER_MAX_SPEED      200     // km/h
#define TACHOMETER_MAX_RPM         6000    // RPM
#define FUEL_GAUGE_MIN             0       // Percentage
#define FUEL_GAUGE_MAX             100     // Percentage
#define TEMP_GAUGE_MIN             70      // Celsius
#define TEMP_GAUGE_MAX             110     // Celsius

// Update Intervals (milliseconds)
#define SPEED_UPDATE_INTERVAL      100
#define RPM_UPDATE_INTERVAL        100
#define FUEL_UPDATE_INTERVAL       1000
#define TEMP_UPDATE_INTERVAL       1000
#define TIME_UPDATE_INTERVAL       60000   // 1 minute

// Animation Settings
#define NEEDLE_ANIMATION_SPEED     5       // Lower = faster animation
#define GAUGE_UPDATE_DELAY         10      // milliseconds

// Font Sizes
#define FONT_SIZE_TITLE            &Font20
#define FONT_SIZE_SUBTITLE         &Font16
#define FONT_SIZE_BODY             &Font12
#define FONT_SIZE_LARGE            &Font24

// Dashboard Theme
#define THEME_PRIMARY              COLOR_WHITE
#define THEME_SECONDARY            COLOR_GRAY
#define THEME_BACKGROUND           COLOR_BLACK
#define THEME_ACCENT               COLOR_BLUE

// Gauge Colors
#define GAUGE_ACTIVE_COLOR         COLOR_WHITE
#define GAUGE_INACTIVE_COLOR       COLOR_GRAY
#define GAUGE_BACKGROUND_COLOR     COLOR_DARK_GRAY
#define GAUGE_NEEDLE_COLOR         COLOR_RED

// Warning Colors
#define WARNING_CRITICAL           COLOR_RED
#define WARNING_MODERATE           COLOR_ORANGE
#define WARNING_INFO               COLOR_BLUE

// LED Simulation
#define LED_UPDATE_INTERVAL        500
#define SIGNAL_BLINK_INTERVAL      500

// Screen Saver Settings
#define SCREEN_SAVER_TIMEOUT       300000  // 5 minutes of inactivity
#define DIM_BRIGHTNESS             0.3     // 30% brightness for screen saver

// Debug Settings
#define DEBUG_SERIAL               1       // Enable Serial debugging
#define DEBUG_LEVEL                2       // 0=Errors only, 1=Info, 2=Verbose

// Mock Data Ranges (for simulation)
#define MOCK_SPEED_MIN             0
#define MOCK_SPEED_MAX             200
#define MOCK_RPM_MIN               800
#define MOCK_RPM_MAX               6000
#define MOCK_FUEL_MIN              0
#define MOCK_FUEL_MAX              100
#define MOCK_TEMP_MIN              70
#define MOCK_TEMP_MAX              110

// Car Settings
#define DEFAULT_UNITS_METRIC       1       // 1=Metric, 0=Imperial
#define DEFAULT_TEMPERATURE_C      1       // 1=Celsius, 0=Fahrenheit
#define DEFAULT_CLOCK_24H          1       // 1=24h format, 0=12h format

// Layout Positions
#define POS_TIME_X                 20
#define POS_TIME_Y                 25

#define POS_SPEEDOMETER_X          (SIDE_MARGIN + GAUGE_AREA_WIDTH/2)
#define POS_SPEEDOMETER_Y          (DISPLAY_HEIGHT/2)

#define POS_TACHOMETER_X           (DISPLAY_WIDTH - SIDE_MARGIN - GAUGE_AREA_WIDTH/2)
#define POS_TACHOMETER_Y           (DISPLAY_HEIGHT/2)

#define POS_FUEL_X                 (SIDE_MARGIN + 20)
#define POS_FUEL_Y                 (TOP_BAR_HEIGHT + 20)

#define POS_TEMP_X                 (DISPLAY_WIDTH - SIDE_MARGIN - 60)
#define POS_TEMP_Y                 (TOP_BAR_HEIGHT + 20)

#define POS_CENTER_X               (SIDE_MARGIN * 2 + GAUGE_AREA_WIDTH)
#define POS_CENTER_Y               (TOP_BAR_HEIGHT)

#endif // CARPLAY_CONFIG_H