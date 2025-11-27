# 🚀 Quick Installation Guide

## Step 1: Arduino IDE Setup
1. Open Arduino IDE
2. Go to **File → Preferences**
3. Add this URL to Additional Board Manager URLs:
   ```
   https://dl.espressif.com/dl/package_esp32_index.json
   ```
4. Go to **Tools → Board → Boards Manager**
5. Search for "ESP32" and install "ESP32 by Espressif Systems"

## Step 2: Hardware Connections
```
ESP32-S3 GPIO    →    LCD Component
GPIO 3          →    VSYNC
GPIO 46         →    HSYNC  
GPIO 5          →    DE (Data Enable)
GPIO 7          →    PCLK (Pixel Clock)

GPIO 14-40      →    RGB Data Lines (16-bit parallel)

GPIO 8          →    I2C SDA (Backlight Control)
GPIO 9          →    I2C SCL (Backlight Control)
```

## Step 3: Upload Code
1. Connect ESP32-S3 to computer via USB
2. Select **ESP32S3 Dev Module** in Tools → Board
3. Select correct COM port in Tools → Port
4. Upload `CarPlay_Dashboard.ino`

## Step 4: Test & Monitor
1. Open Serial Monitor (Tools → Serial Monitor)
2. Set baud rate to **115200**
3. You should see initialization messages
4. Dashboard should display on LCD

## ✅ Success Indicators
- Serial shows "Dashboard initialization complete!"
- LCD displays black background with white gauges
- Speed and RPM needles animate smoothly
- Color test patterns display correctly

## 🔧 If Issues Occur
- Check all wire connections
- Verify GPIO pin assignments
- Try different timing parameters in config
- Monitor Serial output for error messages

## 📱 Customization
Edit `CarPlay_Config.h` to adjust:
- Display colors and themes
- Gauge ranges and units
- Update frequencies
- Animation speeds

---
**Ready to drive with your new CarPlay dashboard! 🚗✨**