# LVGL Editor Project

This folder is the editable LVGL XML version of the Dashboard v2 cluster.

Open `lvgl_editor` as the project folder in LVGL Pro Editor or the LVGL VS Code extension, then open `screens/dashboard_v2.xml`.

Notes:
- `project.xml` defines the 1024x600 ESP32-S3 display preview.
- `globals.xml` contains shared styles, editable design-mode subjects, and local font aliases.
- The screen is built from normal LVGL objects, labels, and bars so it remains editable in design mode.
- `components/freeway_3d_view.xml` is the editable Freeway Frenzy-style center view: lane bands, sensor corridor, target cars, and ego car.
- The firmware still uses the hand-coded `lvgl_ui.cpp` until you export generated C from the LVGL editor and wire it into the Arduino sketch.
