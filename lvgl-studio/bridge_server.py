#!/usr/bin/env python3
"""
LVGL Studio local bridge:
- Receives exported LVGL C from browser
- Writes it into runtime sketch
- Compiles and flashes ESP32-S3 using arduino-cli
"""

from __future__ import annotations

import json
import os
import shutil
import subprocess
import re
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from urllib.parse import urlparse

ROOT = Path(__file__).resolve().parent
REPO_ROOT = ROOT.parent
RUNTIME_SKETCH = ROOT / "runtime" / "LVGL_Studio_Flash"
GENERATED_UI = RUNTIME_SKETCH / "generated_ui.h"
CARPLAY_DIR = REPO_ROOT / "CarPlay_Dashboard"

# Waveshare 7B: 16MB flash + OPI PSRAM, native USB-Serial/JTAG (hwcdc).
# Must use a 16MB partition scheme — default "4MB" table leaves stale app / wrong offsets.
FQBN_BASE = (
    "esp32:esp32:esp32s3:"
    "USBMode=hwcdc,CDCOnBoot=cdc,UploadMode=default,"
    "FlashSize=16M,PSRAM=opi,PartitionScheme=app3M_fat9M_16MB"
)


def fqbn_compile() -> str:
    return FQBN_BASE + ",UploadSpeed=921600"


def fqbn_upload(upload_speed: str, erase_all: bool) -> str:
    f = FQBN_BASE + f",UploadSpeed={upload_speed}"
    if erase_all:
        f += ",EraseFlash=all"
    return f


def upload_with_retries(sketch: Path, port: str, erase_all: bool) -> tuple[int, str]:
    """ESP32-S3 USB-JTAG: try slower speeds + long discovery so BOOT is not needed."""
    speeds = ["460800", "115200", "230400", "921600"]
    last_out = ""
    for speed in speeds:
        fqbn = fqbn_upload(speed, erase_all)
        code, out = run_cmd(
            [
                "arduino-cli",
                "upload",
                "-p",
                port,
                "--fqbn",
                fqbn,
                "--discovery-timeout",
                "45s",
                "-v",
                str(sketch),
            ],
            timeout=480,
        )
        last_out += f"\n\n=== UPLOAD try UploadSpeed={speed} ===\n" + out
        if code == 0:
            return 0, last_out
    return 1, last_out
SYNC_FILES = [
    "lvgl_port.h",
    "lvgl_port.cpp",
    "rgb_lcd_port.h",
    "rgb_lcd_port.cpp",
    "i2c.h",
    "i2c.cpp",
    "io_extension.h",
    "io_extension.cpp",
    "Debug.h",
    "gt911_touch.h",
    "gt911_touch.cpp",
    "touch.h",
    "touch.cpp",
    "gt911.h",
    "gt911.cpp",
    "ref/i2c.h",
    "ref/i2c.cpp",
    "ref/io_extension.h",
    "ref/io_extension.cpp",
    "ref/Debug.h",
    "ref/rgb_lcd_port.h",
    "ref/rgb_lcd_port.cpp",
    "ref/font8.cpp",
    "ref/font12.cpp",
    "ref/font16.cpp",
    "ref/font20.cpp",
    "ref/font24.cpp",
    "ref/font48.cpp",
    "ref/font8_CN.cpp",
    "ref/font12_CN.cpp",
    "ref/font24_CN.cpp",
    "ref/font48_CN.cpp",
    "ref/fonts.h",
    "ref/gui_paint.h",
    "ref/gui_paint.cpp",
    "ref/image.h",
    "ref/image.cpp",
]


def run_cmd(cmd: list[str], cwd: Path | None = None, timeout: int = 240) -> tuple[int, str]:
    proc = subprocess.run(
        cmd,
        cwd=str(cwd) if cwd else None,
        capture_output=True,
        text=True,
        timeout=timeout,
    )
    out = (proc.stdout or "") + (proc.stderr or "")
    return proc.returncode, out


_JUNK_SUBSTR = (
    "Bluetooth",
    "debug-console",
    "wlan-debug",
    "SOFTWARE",
)


def detect_ports() -> list[str]:
    """Prefer Espressif USB-JTAG (VID 0x303A) so we don't flash the wrong board when several USB serial devices exist."""
    esp: list[str] = []
    other: list[str] = []
    try:
        code, raw = run_cmd(["arduino-cli", "board", "list", "--format", "json"], timeout=45)
        if code == 0 and raw.strip():
            data = json.loads(raw)
            for entry in data.get("detected_ports", []):
                port = entry.get("port") or {}
                addr = port.get("address") or ""
                if not addr.startswith("/dev/cu."):
                    continue
                if any(bad in addr for bad in _JUNK_SUBSTR):
                    continue
                props = port.get("properties") or {}
                vid = str(props.get("vid", "")).lower()
                if vid == "0x303a":
                    esp.append(addr)
                elif "usbmodem" in addr or "usbserial" in addr or "wchusbserial" in addr.lower():
                    other.append(addr)
    except Exception:
        pass

    esp = sorted(set(esp))
    other = sorted(set(other))
    ordered = esp + [p for p in other if p not in esp]

    if not ordered:
        for pattern in ("cu.usbmodem*", "cu.usbserial*", "cu.wchusbserial*"):
            ordered.extend(str(p) for p in Path("/dev").glob(pattern))
    seen: set[str] = set()
    out: list[str] = []
    for p in ordered:
        if p not in seen and not any(bad in p for bad in _JUNK_SUBSTR):
            seen.add(p)
            out.append(p)
    return out


def sync_runtime_support_files() -> list[str]:
    copied = []
    for name in SYNC_FILES:
        src = CARPLAY_DIR / name
        dst = RUNTIME_SKETCH / name
        dst.parent.mkdir(parents=True, exist_ok=True)
        if src.exists():
            shutil.copy2(src, dst)
            copied.append(name)
    return copied


LV_CONF_H = Path(os.path.expanduser("~/Documents/Arduino/libraries/lv_conf.h"))

# Sizes the Studio exporter may reference; must stay on for compile even if hwConfig is empty.
STUDIO_MONTSERRAT_SIZES = (
    12,
    14,
    16,
    18,
    20,
    22,
    24,
    26,
    28,
    30,
    32,
    36,
    40,
    44,
    48,
)


def patch_lv_conf_studio_fonts() -> None:
    """Turn on Montserrat sizes used by exported UI. Run on every push/build so compile matches generated_ui.h."""
    if not LV_CONF_H.exists():
        return
    content = LV_CONF_H.read_text()
    for size in STUDIO_MONTSERRAT_SIZES:
        content = re.sub(
            rf"#define LV_FONT_MONTSERRAT_{size}\s+0",
            f"#define LV_FONT_MONTSERRAT_{size} 1",
            content,
        )
    LV_CONF_H.write_text(content)


def patch_hardware_config(hw_config: dict):
    if not hw_config:
        return

    # Patch lvgl_port.h
    lvgl_port_h = RUNTIME_SKETCH / "lvgl_port.h"
    if lvgl_port_h.exists():
        content = lvgl_port_h.read_text()
        if "tearingMode" in hw_config:
            content = re.sub(r'#define LVGL_PORT_AVOID_TEAR_MODE\s+\(\d+\)', f'#define LVGL_PORT_AVOID_TEAR_MODE       ({hw_config["tearingMode"]})', content)
        lvgl_port_h.write_text(content)

    # Patch rgb_lcd_port.h
    rgb_lcd_port_h = RUNTIME_SKETCH / "rgb_lcd_port.h"
    if rgb_lcd_port_h.exists():
        content = rgb_lcd_port_h.read_text()
        if "pclkMHz" in hw_config:
            content = re.sub(r'#define EXAMPLE_LCD_PIXEL_CLOCK_HZ\s+\(\d+\s*\*\s*1000\s*\*\s*1000\)', f'#define EXAMPLE_LCD_PIXEL_CLOCK_HZ      ({hw_config["pclkMHz"]} * 1000 * 1000)', content)
        if "bounceLines" in hw_config:
            lines = hw_config["bounceLines"]
            if lines == 0:
                content = re.sub(r'#define EXAMPLE_RGB_BOUNCE_BUFFER_SIZE\s+\(EXAMPLE_LCD_H_RES\s*\*\s*\d+\)', '#define EXAMPLE_RGB_BOUNCE_BUFFER_SIZE  0', content)
            else:
                content = re.sub(r'#define EXAMPLE_RGB_BOUNCE_BUFFER_SIZE\s+(?:0|\(EXAMPLE_LCD_H_RES\s*\*\s*\d+\))', f'#define EXAMPLE_RGB_BOUNCE_BUFFER_SIZE  (EXAMPLE_LCD_H_RES * {lines})', content)
        rgb_lcd_port_h.write_text(content)

    # Patch global lv_conf.h (refresh + demos when hardware panel sends hwConfig)
    if LV_CONF_H.exists():
        content = LV_CONF_H.read_text()
        if "refrPeriod" in hw_config:
            content = re.sub(r'#define LV_DISP_DEF_REFR_PERIOD\s+\d+', f'#define LV_DISP_DEF_REFR_PERIOD {hw_config["refrPeriod"]}', content)

        content = re.sub(r'#define LV_USE_DEMO_WIDGETS\s+0', '#define LV_USE_DEMO_WIDGETS 1', content)
        content = re.sub(r'#define LV_USE_DEMO_BENCHMARK\s+0', '#define LV_USE_DEMO_BENCHMARK 1', content)
        content = re.sub(r'#define LV_USE_DEMO_STRESS\s+0', '#define LV_USE_DEMO_STRESS 1', content)
        content = re.sub(r'#define LV_USE_DEMO_MUSIC\s+0', '#define LV_USE_DEMO_MUSIC 1', content)

        LV_CONF_H.write_text(content)


def sanitize_generated_code(code: str) -> str:
    if "build_dashboard(" not in code:
        raise ValueError("Generated code must include build_dashboard(lv_obj_t *scr)")
    # Ensure include once + pragma guard
    body = code.strip()
    return "#pragma once\n\n" + body + "\n"


class Handler(BaseHTTPRequestHandler):
    def _send(self, status: int, payload: dict):
        b = json.dumps(payload).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "application/json")
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET,POST,OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.send_header("Content-Length", str(len(b)))
        self.end_headers()
        self.wfile.write(b)

    def _read_json(self) -> dict:
        length = int(self.headers.get("Content-Length", "0"))
        raw = self.rfile.read(length) if length else b"{}"
        return json.loads(raw.decode("utf-8"))

    def do_OPTIONS(self):
        self._send(200, {"ok": True})

    @staticmethod
    def _api_path(raw_path: str) -> str:
        return urlparse(raw_path).path.rstrip("/") or "/"

    def _send_static(self, path: Path, content_type: str):
        if not path.is_file():
            body = (
                f"Missing {path.name}. From lvgl-studio run: npm install && npm run build:css\n"
            ).encode("utf-8")
            self.send_response(404)
            self.send_header("Content-Type", "text/plain; charset=utf-8")
            self.send_header("Access-Control-Allow-Origin", "*")
            self.send_header("Content-Length", str(len(body)))
            self.end_headers()
            self.wfile.write(body)
            return
        data = path.read_bytes()
        self.send_response(200)
        self.send_header("Content-Type", content_type)
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Cache-Control", "no-cache" if path.suffix == ".html" else "public, max-age=300")
        self.send_header("Content-Length", str(len(data)))
        self.end_headers()
        self.wfile.write(data)

    def do_GET(self):
        req_path = self._api_path(self.path)
        static_routes = {
            "/": (ROOT / "index.html", "text/html; charset=utf-8"),
            "/index.html": (ROOT / "index.html", "text/html; charset=utf-8"),
            "/studio.css": (ROOT / "studio.css", "text/css; charset=utf-8"),
        }
        if req_path in static_routes:
            self._send_static(*static_routes[req_path])
            return

        if req_path == "/api/status":
            code, out = run_cmd(["arduino-cli", "version"])
            ports = detect_ports()
            self._send(
                200,
                {
                    "ok": True,
                    "arduino_cli": out.strip() if code == 0 else "not found",
                    "ports": ports,
                    "multiple_usb_esp": sum(1 for p in ports if "usbmodem" in p) > 1,
                    "runtime_sketch": str(RUNTIME_SKETCH),
                    "fqbn_compile": fqbn_compile(),
                    "fqbn_upload_example": fqbn_upload("460800", False),
                },
            )
            return
        self._send(404, {"ok": False, "error": "Unknown endpoint"})

    def do_POST(self):
        try:
            data = self._read_json()
            post_path = self._api_path(self.path)
            if post_path == "/api/push-code":
                code = data.get("code", "")
                hw_config = data.get("hwConfig", {})
                sanitized = sanitize_generated_code(code)
                sync_runtime_support_files()
                patch_hardware_config(hw_config)
                patch_lv_conf_studio_fonts()
                GENERATED_UI.write_text(sanitized, encoding="utf-8")
                self._send(200, {"ok": True, "written": str(GENERATED_UI)})
                return

            if post_path in ("/api/build", "/api/build-flash"):
                sync_runtime_support_files()
                patch_lv_conf_studio_fonts()
                c, out = run_cmd(
                    [
                        "arduino-cli",
                        "compile",
                        "--fqbn",
                        fqbn_compile(),
                        str(RUNTIME_SKETCH),
                    ],
                    timeout=480,
                )
                if c != 0:
                    self._send(500, {"ok": False, "error": "Compile failed", "output": out})
                    return
                if post_path == "/api/build":
                    self._send(200, {"ok": True, "output": out, "ports": detect_ports()})
                    return

                port = data.get("port", "").strip()
                ports = detect_ports()
                if not port:
                    if len(ports) == 1:
                        port = ports[0]
                    elif len(ports) > 1:
                        self._send(
                            500,
                            {
                                "ok": False,
                                "error": (
                                    "Multiple USB serial devices found — pick the ESP32-S3 port in LVGL Studio "
                                    "(Serial Port), then flash again."
                                ),
                                "output": out,
                                "ports": ports,
                            },
                        )
                        return
                if not port:
                    self._send(500, {"ok": False, "error": "No serial port detected", "output": out, "ports": ports})
                    return

                erase_all = bool(data.get("eraseFlash", False))
                ucode, uout = upload_with_retries(RUNTIME_SKETCH, port, erase_all)
                merged = out + "\n\n=== UPLOAD ===\n" + uout
                if ucode != 0:
                    self._send(
                        500,
                        {
                            "ok": False,
                            "error": f"Upload failed on {port} (tried multiple baud rates). Use USB port labeled UART/USB, data cable, or enable Full chip erase once.",
                            "output": merged,
                            "ports": detect_ports(),
                        },
                    )
                    return
                self._send(200, {"ok": True, "output": merged, "port": port, "ports": detect_ports()})
                return

            self._send(404, {"ok": False, "error": "Unknown endpoint"})
        except Exception as e:
            self._send(500, {"ok": False, "error": str(e)})


def main():
    host = os.environ.get("LVGL_STUDIO_BRIDGE_HOST", "127.0.0.1")
    port = int(os.environ.get("LVGL_STUDIO_BRIDGE_PORT", "8765"))
    server = ThreadingHTTPServer((host, port), Handler)
    print(f"LVGL Studio bridge running at http://{host}:{port}")
    print(f"Open studio UI in the browser (avoid file://): http://{host}:{port}/")
    print(f"Runtime sketch: {RUNTIME_SKETCH}")
    print("Endpoints: GET /api/status, POST /api/push-code, POST /api/build, POST /api/build-flash")
    print("Restart cleanly: ./restart_bridge.sh (from this directory)")
    server.serve_forever()


if __name__ == "__main__":
    main()
