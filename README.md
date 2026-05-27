# ESP32 Bridge — C3 Sensor Hub + S3 Dashboard Display

A dual-board vehicle telemetry system built on ESP32-C3 and ESP32-S3.

## Architecture

```
┌──────────────────────┐  SensorPacket  ┌──────────────────────────────┐
│  ESP32-C3 Sensor Hub │  ────────────▶   │  ESP32-S3 Display / Bridge   │
│                      │   ESP-NOW ch 1   │                              │
│  • HC-SR04 distance  │                  │  • Receives telemetry (SSE)  │
│  • DHT11 temp/humid  │  ◀────────────   │  • Serves React on SPIFFS    │
│  • WS2812 front/rear │  ControlPacket   │  • Forwards light taps→C3   │
│  • SSD1306 OLED      │  (light flags)   │  • Waveshare 1024×600 UI     │
│  • Wi-Fi AP + Web UI │                  │                              │
└──────────────────────┘                  └──────────────────────────────┘
```

## Quick Start

### 1. Flash the ESP32-C3 Sensor Hub

```bash
pio run -e c3-sensor -t upload
```

Connect to Wi-Fi `ESP32C3-Sensors` (password: `neopixel!`) and open `http://192.168.4.1/` to toggle exterior lights (on the C3 strips) and view raw sensor data.

### 2. Build the React Dashboard

```bash
cd dashboard
npm install
npm run build    # outputs to ../data/ for SPIFFS
```

### 3. Flash the ESP32-S3 Dashboard

```bash
pio run -e s3-dashboard -t uploadfs   # upload React app to SPIFFS
pio run -e s3-dashboard -t upload      # flash firmware
```

If upload fails, put the board in **download mode**: hold **BOOT**, tap **RESET**, release **BOOT**, then run `upload` again.

The Waveshare **7B** panel is driven natively (backlight + RGB). After boot you should see a splash with the AP IP (`192.168.4.1`), Wi‑Fi name, and password — then open that URL in a browser on a phone/tablet (or on the same device if it has a browser). **Do not use GPIO2 as a blinking “status LED” on this board** — it is an LCD data line and will blank or corrupt the image.

Connect to the S3 AP (`ESP32S3-Dashboard`, password: `dashboard1`) and open `http://192.168.4.1/`.

### 4. Development Mode (no hardware)

```bash
cd dashboard
npm run dev
```

Open `http://localhost:5173` — use keyboard controls to test the dashboard:
- `↑`/`↓` — Speed
- `←`/`→` — RPM
- `G` — Cycle gear
- `E` — Toggle ECO
- `F`/`Shift+F` — Fuel up/down
- `T`/`Shift+T` — Temp up/down
- `D`/`Shift+D` — Ultrasonic distance up/down

## Project Structure

```
ESP32-C3/
├── platformio.ini              # Multi-env: c3-sensor + s3-dashboard
├── firmware/
│   ├── c3-sensor/main.cpp      # C3 firmware (sensors + ESP-NOW TX)
│   └── s3-dashboard/main.cpp   # S3 firmware (ESP-NOW RX + web + light relay)
├── dashboard/                  # React/Vite app for Waveshare display
│   ├── src/App.tsx             # Main dashboard layout
│   └── src/components/         # UI components
├── include/
│   ├── c3_pins.h               # Active C3 sensor-hub pin definitions
│   ├── board_pins.h            # Legacy compatibility shim for archives
│   ├── s3_pins.h               # S3 pin definitions
│   ├── sensor_packet.h         # Shared telemetry ESP-NOW struct
│   └── control_packet.h        # S3→C3 light command (ESP-NOW)
├── data/                       # Built dashboard (SPIFFS upload target)
├── src/_archived/              # Older projector experiments kept as reference
└── lib/                        # PlatformIO libraries
```

## ESP-NOW Protocol

The C3 broadcasts a `SensorPacket` at 10 Hz on ESP-NOW channel 1:

| Field | Type | Description |
|-------|------|-------------|
| `magic` | `uint32_t` | `0x45535052` ('ESPR') |
| `version` | `uint8_t` | Protocol version (1) |
| `lights` | `uint8_t` | Bit flags for exterior lights |
| `seq` | `uint32_t` | Packet sequence number |
| `distanceCm` | `float` | Smoothed ultrasonic distance |
| `tempC` | `float` | Temperature in Celsius |
| `humidity` | `float` | Relative humidity % |

The S3 can broadcast a `ControlPacket` (`magic` `0x4354524C`, `lights` field) so the CarPlay web UI can update light flags on the C3 without joining the C3’s Wi‑Fi AP.

### Light Bit Flags

| Bit | Flag | Description |
|-----|------|-------------|
| 0 | `LIGHT_HEAD` | Headlights |
| 1 | `LIGHT_LEFT` | Left turn signal |
| 2 | `LIGHT_RIGHT` | Right turn signal |
| 3 | `LIGHT_BRAKE` | Brake lights |
| 4 | `LIGHT_HAZARD` | Hazard flashers |

## Wiring

### ESP32-C3

See [`include/c3_pins.h`](include/c3_pins.h) for the active sensor-hub pin assignments.

- **OLED**: SDA=GPIO5, SCL=GPIO6
- **Ultrasonic**: TRIG=GPIO1, ECHO=GPIO0 (through voltage divider!)
- **DHT11**: DATA=GPIO3
- **Front WS2812** (head / turns): DATA=GPIO9 (18 LEDs)
- **Rear WS2812** (tail / brake / turns): DATA=GPIO10 (18 LEDs)
- **LED**: GPIO8 (active LOW)

### ESP32-S3

See [`include/s3_pins.h`](include/s3_pins.h). This build uses the board mainly for the LCD-hosted dashboard and Wi‑Fi; **vehicle WS2812 strips are not wired here**.

- **Status LED**: GPIO2 (heartbeat / link)

## Laser Safety

If using the DMX laser projector features (archived firmware), treat a 500 mW projector as an eye hazard. See the archived code for DMX wiring details.
