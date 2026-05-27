# LVGL Studio

A zero-install visual editor for LVGL dashboards. Design for **1024 × 600** in the browser, export **drop-in C code** that builds the same UI on any ESP32 / STM32 / Linux LVGL project (including the `CarPlay_Dashboard` sketch in this repo).

## Quick start

```bash
open lvgl-studio/index.html
```

That's it — it's a single self-contained HTML file (React + Tailwind via CDN, Babel in-browser). No build step, no `npm install`.

## What you get

- **Live preview canvas** — animated simulated data (toggle the `● Live preview` pill).
- **Widgets** — Label, Rectangle, Circle, Bar, Arc, Button.
- **Properties panel** — position, size, opacity, colors, radius, border, shadow, font size/weight, letter spacing, arc angles, bar values.
- **Layers** — reorder, duplicate, delete.
- **Drag to move** on the canvas · arrow keys nudge (Shift = 10 px).
- **Presets** — Tesla EV dashboard, 3-gauge cluster, Blank.
- **Save / Load JSON** for any project.
- **Export LVGL C** that compiles as-is.
- **Build + Flash S3** directly from the web app (via local bridge).

## Build & Flash from web app

The browser cannot run `arduino-cli` directly, so LVGL Studio includes a tiny local bridge.

1. Start the bridge in a terminal:
   ```bash
   cd /Users/sean/Documents/ProjectsPage/lvgl-studio
   python3 bridge_server.py
   ```
2. Open `lvgl-studio/index.html`.
3. Click **Bridge** (should show connected status + detected ports).
4. Click **Build S3** or **Flash S3**.

What happens:
- Editor exports code to `runtime/LVGL_Studio_Flash/generated_ui.h`
- Bridge syncs display driver files from `CarPlay_Dashboard/`
- Bridge runs:
  - `arduino-cli compile ... runtime/LVGL_Studio_Flash`
  - `arduino-cli upload ... runtime/LVGL_Studio_Flash` (for Flash)

## Data bindings

Give a widget a **Bind name** (e.g. `speed`, `battery`, `rpm`) and the exporter will:

1. Declare it globally as `static lv_obj_t *ui_<name>;`
2. Emit setter helpers like `set_speed_text("68")`, `set_rpm_value(42)`, `set_speed_value(70)`.

So once you've exported, driving the UI from your sensor pipeline is one line per value.

## Using the exported code

1. Click **Export LVGL C**, then **Copy**.
2. Paste into a new `.cpp` file in your Arduino sketch (e.g. `dashboard.cpp`) alongside `#include "lvgl.h"`.
3. After `lv_init()` and your display driver is up, call:
   ```c
   build_dashboard(lv_scr_act());
   ```
4. From a timer or RTOS task, call the generated setters:
   ```c
   char buf[16]; snprintf(buf, sizeof(buf), "%d", speed_mph);
   set_speed_text(buf);
   set_battery_value(battery_pct);
   ```

The exporter uses the built-in `lv_font_montserrat_*` fonts, which are enabled by default in most LVGL configs. If you need custom fonts (SF Pro, Font Awesome), replace the `&lv_font_montserrat_xx` reference with your own `LV_FONT_DECLARE`'d font.

## Keyboard shortcuts

| Key | Action |
|---|---|
| `⌘ / Ctrl + Z` | Undo |
| `Shift + ⌘ / Ctrl + Z` | Redo |
| `⌘ / Ctrl + Y` | Redo (Windows-style) |
| `⌘ / Ctrl + S` | Save JSON |
| `⌘ / Ctrl + E` | Export LVGL C |
| `↑ ↓ ← →` | Nudge selected widget 1 px |
| `Shift + arrow` | Nudge 10 px |
| `⌘ / Ctrl + D` | Duplicate |
| `Delete / Backspace` | Remove |
| Click canvas background | Deselect |

## Tips

- Canvas is exactly **1024 × 600**, matching the Waveshare ESP32-S3-Touch-LCD-7B.
- Use **Rectangle** with radius 0 for dividers; with radius == width/2 for pills.
- **Arc** start/sweep in degrees, same convention as LVGL (`lv_arc_set_rotation` + `lv_arc_set_bg_angles`).
- The preview rendering is a faithful approximation of how LVGL will draw, but small rasterization/font-metric differences are expected.
