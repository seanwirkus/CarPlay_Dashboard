#!/usr/bin/env bash
# Kill anything on the LVGL Studio bridge port and start a fresh bridge_server.py
set -euo pipefail
cd "$(dirname "$0")"
pkill -f "bridge_server.py" 2>/dev/null || true
if command -v lsof >/dev/null 2>&1; then
  lsof -ti :8765 | xargs kill -9 2>/dev/null || true
fi
sleep 0.35
exec python3 bridge_server.py
