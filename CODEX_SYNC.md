# Codex Sync - ESP32 C3/S3 Dashboard

Last updated: 2026-05-26

## Current Goal

Use the native LVGL dashboard on the Waveshare ESP32-S3 no-touch 7B screen and keep telemetry from the ESP32-C3 stable with low visible latency over ESP-NOW plus wired UART backup.

## Important Target Distinction

- `s3-dashboard` is the Wi-Fi/web/SPIFFS dashboard target. It is not the readable native LVGL screen UI.
- `s3-lvgl-studio` is the native LVGL Waveshare 7B runtime. Flash this for the physical S3 display.
- Do not use `uploadfs` for the LVGL target; it is only relevant for `s3-dashboard`.

## Current Radio State

- ESP-NOW channel is locked to `11` in `include/sensor_packet.h`.
- C3 sends unicast to S3 MAC `10:51:DB:74:F4:B8`.
- C3 TX interval is `50 ms` / `20 Hz`.
- C3 sample mode is currently OFF. The boot sweep is local to the S3 UI only; after startup, values settle to real C3 inputs.
- LR mode is not enabled. Standard `11b/g/n` is used.
- Heavy ESP-NOW deinit/reinit watchdog should stay removed; it caused cutouts.
- Tach input is not causing the RF issue when idle-disconnected: C3 serial showed `pulses=0 reject=0` with tach pin idle HIGH.

## Current Wired UART State

- S3 UART2 terminal RX is working with firmware RX on `GPIO44`, TX on `GPIO43`.
- C3 now sends the same binary `SensorPacket` on both TX candidates:
  - Primary: `GPIO1`
  - Fallback/common board TX pad: `GPIO21`
- Live verified after flashing:
  - S3 `src=BOTH`
  - S3 `uart2Bytes` increasing
  - S3 `uartBad=0`
  - S3 de-duplicates matching WiFi/UART sequence numbers, so displayed RX rate should be near the real C3 sender rate, about `20 Hz`.
- S3 screen transport indicator is now top-center and should read `WIFI`, `UART`, or `BOTH` with rate/latency.
- Current UI behavior after the latest flash:
  - Demo mode remains off on the S3, and C3 sample mode is off.
  - On boot, the gauges do a startup sweep and the warning/service strip does a lamp test, then stop moving unless real inputs move.
  - When WiFi + UART are both live, the lower-right status reads `ALL SYSTEMS CLEAR`; the smaller transport line shows `UART + WIFI` without Hz/ms.
  - Warning/service placeholders use the compiled Font Awesome LVGL font for icons and are present for `CEL`, `OIL`, `BAT`, `ABS`, `AIR`, `BRK`, `TMP`, and `AWD`.
  - LVGL tick is requested at ~120 Hz; the RGB panel/vsync still determines actual visible refresh.

## What Was Observed

- Channel 1 was better than channel 6 in earlier testing.
- Channel 11 was later reported as stable/fine and is the current source setting.
- C3 saw high `failCb` counts when RF was bad, meaning packets were not ACKed after retries.
- Likely root causes are RF/antenna orientation, S3 RGB-DMA/Wi-Fi contention, and local channel congestion.

## Current Code Notes

- Shared packet: `include/sensor_packet.h`
- C3 sender: `firmware/c3-sensor/main.cpp`
- LVGL native S3 runtime: `firmware/lvgl_studio_flash/LVGL_Studio_Flash.ino`
- Web/SPIFFS S3 bridge: `firmware/s3-dashboard/main.cpp`

LVGL runtime already has:

- `ESPNOW_CHANNEL = 11`
- updated `SensorPacket` with `statusFlags`
- status bits matching `include/sensor_packet.h`
- RX count/drop count tracking
- direct C3 packet values for RPM/MPH/fuel so the C3 OLED and S3 screen match
- UART2 raw byte counters in serial diagnostics

## Flash Commands

Flash C3:

```bash
pio run -e c3-sensor -t upload --upload-port /dev/cu.usbmodem2101
```

Flash native LVGL S3 display:

```bash
pio run -e s3-lvgl-studio -t upload --upload-port /dev/cu.usbmodem101
```

If S3 USB is missing:

1. Hold S3 `BOOT`.
2. Tap `RESET`.
3. Release `BOOT`.
4. Re-run `pio device list`.
5. Flash `s3-lvgl-studio` to the new S3 port.

## Do Not Accidentally Flash

Avoid this unless you specifically want the browser dashboard instead of LVGL:

```bash
pio run -e s3-dashboard -t upload
pio run -e s3-dashboard -t uploadfs
```

## Quick Debug Commands

List ports:

```bash
pio device list
```

Read C3 serial:

```bash
pio device monitor -p /dev/cu.usbmodem2101 -b 115200
```

Read S3 serial, after the S3 port exists:

```bash
pio device monitor -p /dev/cu.usbmodem101 -b 115200
```

Expected useful C3 stats:

- `ESPNOW seq=... okCb=... failCb=... queueFail=... ch=11`
- `tach gpio=... raw=... rpm=... pulses=... reject=...`

Expected useful S3 LVGL boot lines:

- `LVGL Studio runtime boot`
- `ESP-NOW ready channel=11`
- `Native dashboard ready`
- `RX rate=... src=BOTH ... uart2Bytes=... uartBad=0`

## Workspace Cleanup Rule

The repo currently has many untracked project files. Do not delete or reset them blindly. Cleanup should be limited to generated build folders such as `.pio/` only when a rebuild is acceptable, and never use `git reset --hard` without explicit approval.
