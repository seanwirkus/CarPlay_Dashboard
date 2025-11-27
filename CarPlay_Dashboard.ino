#include "rgb_lcd_port.h"
#include "gui_paint.h"
#include "navigation_dashboard.h"
#include <Arduino.h>

// CarPlay Dashboard display size macros/matches reference
#define DASHBOARD_WIDTH  EXAMPLE_LCD_H_RES
#define DASHBOARD_HEIGHT EXAMPLE_LCD_V_RES

UBYTE *DashboardImage = NULL;

float mockSpeed = 88.0; // km/h, Back-to-the-Future!

// Navigation dashboard instance
NavigationDashboard navDashboard;

void setup() {
  Serial.begin(115200);
  Serial.println("CARPLAY DASHBOARD: setup started...");
  DEV_I2C_Init();
  delay(100);
  IO_EXTENSION_Init();
  delay(100);
  waveshare_esp32_s3_rgb_lcd_init();
  delay(100);
  wavesahre_rgb_lcd_bl_on();
  delay(100);

  // Allocate dashboard image buffer (framebuffer, RGB565)
  UDOUBLE Imagesize = DASHBOARD_WIDTH * DASHBOARD_HEIGHT * 2;
  DashboardImage = (UBYTE *)malloc(Imagesize);
  if (!DashboardImage) {
    Serial.println("Failed to allocate DashboardImage!");
    while(1) { delay(1000); }
  }

  // Set up Paint for the dashboard image
  Paint_NewImage(DashboardImage, DASHBOARD_WIDTH, DASHBOARD_HEIGHT, 0, WHITE);
  Paint_SetRotate(ROTATE_0);
  Paint_SetScale(65);
  Serial.println("CARPLAY DASHBOARD: framebuffer and Paint initialized!");

  // Initialize navigation dashboard
  NavigationDashboard_Init(&navDashboard);
  Serial.println("Navigation dashboard initialized!");
}

void loop() {
  // High refresh-rate update (~10 FPS) for smooth CarPlay-like movement
  uint32_t now = millis();
  NavigationDashboard_Tick(&navDashboard, now);

  // Clear the dashboard to black for CarPlay style
  Paint_Clear(BLACK);

  // Render the navigation dashboard
  NavigationDashboard_Render(&navDashboard, 0, 0);

  // Update the LCD with new buffer contents
  wavesahre_rgb_lcd_display(DashboardImage);

  Serial.println("DASHBOARD: navigation dashboard rendered and display updated");
  delay(100);
}
