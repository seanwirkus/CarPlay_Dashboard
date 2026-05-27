#pragma once

#include "driver/gpio.h"

// Waveshare ESP32-S3-Touch-LCD-7B (1024×600 RGB) — timing/pins from vendor rgb_lcd_port.h

#define WS7B_LCD_H_RES              1024
#define WS7B_LCD_V_RES              600
#define WS7B_LCD_PIXEL_CLOCK_HZ     (30 * 1000 * 1000)
#define WS7B_LCD_BIT_PER_PIXEL      16
#define WS7B_RGB_BIT_PER_PIXEL      16
#define WS7B_RGB_DATA_WIDTH         16
#define WS7B_LCD_RGB_BUFFER_NUMS    2
#define WS7B_RGB_BOUNCE_BUFFER_SIZE (WS7B_LCD_H_RES * 10)

#define WS7B_LCD_IO_RGB_DISP        (-1)
#define WS7B_LCD_IO_RGB_VSYNC       (GPIO_NUM_3)
#define WS7B_LCD_IO_RGB_HSYNC       (GPIO_NUM_46)
#define WS7B_LCD_IO_RGB_DE          (GPIO_NUM_5)
#define WS7B_LCD_IO_RGB_PCLK        (GPIO_NUM_7)

#define WS7B_LCD_IO_RGB_DATA0       (GPIO_NUM_14)
#define WS7B_LCD_IO_RGB_DATA1       (GPIO_NUM_38)
#define WS7B_LCD_IO_RGB_DATA2       (GPIO_NUM_18)
#define WS7B_LCD_IO_RGB_DATA3       (GPIO_NUM_17)
#define WS7B_LCD_IO_RGB_DATA4       (GPIO_NUM_10)
#define WS7B_LCD_IO_RGB_DATA5       (GPIO_NUM_39)
#define WS7B_LCD_IO_RGB_DATA6       (GPIO_NUM_0)
#define WS7B_LCD_IO_RGB_DATA7       (GPIO_NUM_45)
#define WS7B_LCD_IO_RGB_DATA8       (GPIO_NUM_48)
#define WS7B_LCD_IO_RGB_DATA9       (GPIO_NUM_47)
#define WS7B_LCD_IO_RGB_DATA10      (GPIO_NUM_21)
#define WS7B_LCD_IO_RGB_DATA11      (GPIO_NUM_1)
#define WS7B_LCD_IO_RGB_DATA12      (GPIO_NUM_2)
#define WS7B_LCD_IO_RGB_DATA13      (GPIO_NUM_42)
#define WS7B_LCD_IO_RGB_DATA14      (GPIO_NUM_41)
#define WS7B_LCD_IO_RGB_DATA15      (GPIO_NUM_40)
