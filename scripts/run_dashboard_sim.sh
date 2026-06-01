#!/usr/bin/env bash
# Build and run the CarPlay LVGL dashboard SDL simulator (1024x600) on macOS/Linux.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
PORT="${ROOT}/lv_port_pc_vscode"
BUILD="${PORT}/build"
EDITOR="${ROOT}/lvgl_editor"

if ! command -v cmake >/dev/null 2>&1; then
  echo "cmake is required. On macOS: brew install cmake sdl2"
  exit 1
fi

if ! command -v sdl2-config >/dev/null 2>&1; then
  echo "SDL2 is required. On macOS: brew install sdl2"
  exit 1
fi

echo "Configuring simulator (lvgl_editor @ 1024x600)..."
cmake -S "${PORT}" -B "${BUILD}" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCARPLAY_LVGL_EDITOR_DIR="${EDITOR}"

echo "Building dashboard_sim..."
cmake --build "${BUILD}" --target dashboard_sim -j"$(sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 4)"

SIM_BIN="${PORT}/bin/dashboard_sim"
echo "Launching ${SIM_BIN}"
exec "${SIM_BIN}"
