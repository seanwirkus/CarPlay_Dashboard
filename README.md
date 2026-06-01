# CarPlay Dashboard (ESP-NOW bridge)

Firmware for **ESP32-C3** sensor hub and **ESP32-S3** Waveshare 7″ display.

## Flash

```bash
# C3 sensor hub (ultrasonic, DHT, OLED, NeoPixels, ESP-NOW TX)
pio run -e c3-sensor -t upload

# S3 native LVGL dashboard (lvgl_editor UI + ESP-NOW)
pio run -e s3-lvgl-studio -t upload

# S3 Wi-Fi dashboard (React UI on SPIFFS + ESP-NOW)
pio run -e s3-dashboard -t upload
pio run -e s3-dashboard -t uploadfs
```

## Layout

| Path | Purpose |
|------|---------|
| `firmware/c3-sensor/` | C3 firmware |
| `firmware/s3-dashboard/` | S3 web/AP dashboard |
| `firmware/lvgl_studio_flash/` | S3 LVGL runtime (`LVGL_Studio_Flash.ino`) |
| `lvgl_editor/` | LVGL Editor Pro UI export |
| `lv_port_pc_vscode/` | LVGL 9 library |
| `include/` | Shared packets (`sensor_packet.h`, etc.) |
| `data/` | SPIFFS assets for `s3-dashboard` |
| `15_LVGL_SLIDER/boards/` | Waveshare board definition |
| `15_LVGL_SLIDER/lib/i2c`, `io_extension/` | Panel I2C / IO expander |

## Main sketch

Edit **`lvgl-studio/runtime/LVGL_Studio_Flash/LVGL_Studio_Flash.ino`** (symlinked as `firmware/lvgl_studio_flash/main.cpp`).

## Build all targets

```bash
pio run -e c3-sensor -e s3-lvgl-studio -e s3-dashboard
```

(`s3-lvgl-studio` uses `lv_port_pc_vscode/lv_conf.h` with ESP-specific settings: no SDL/ThorVG/sysmon, 96KB LV heap.)

## Mac / PC simulator (layout preview)

Preview the **1024×600** LVGL Editor dashboard on your Mac without flashing hardware. This checks XML layout, fonts, and centering. **RGB porch timing** (Waveshare panel shift) is still tuned only in `firmware/lvgl_studio_flash/rgb_lcd_port.cpp` on the S3.

**Prerequisites (macOS):**

```bash
brew install sdl2 cmake
```

**Run:**

```bash
./scripts/run_dashboard_sim.sh
```

- SDL window at **1024×600** with demo gauge values
- Press **G** to toggle red center cross / border guides
- After editing `lvgl_editor/`, regenerate in LVGL Editor, then re-run the script

**LVGL Editor** (installed app) also has a built-in preview; use that for live XML edits. The SDL sim matches what the S3 firmware draws (same `lvgl_editor` C export).
