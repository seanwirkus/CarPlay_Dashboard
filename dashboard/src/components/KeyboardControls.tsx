import { useCallback, useEffect } from 'react';

/**
 * Keyboard-driven control layer for testing the dashboard
 * without the ESP32-C3 hardware connected.
 *
 * Controls:
 *   ↑/↓    — Speed
 *   ←/→    — RPM
 *   G      — Cycle gear
 *   E      — Toggle ECO mode
 *   F/⇧F   — Fuel up/down
 *   T/⇧T   — Temperature up/down
 *   D/⇧D   — Distance up/down (ultrasonic)
 */
export default function KeyboardControls() {
  const handleKeyDown = useCallback((e: KeyboardEvent) => {
    if (e.target instanceof HTMLInputElement || e.target instanceof HTMLTextAreaElement) return;

    switch (e.key) {
      case 'ArrowUp':
        e.preventDefault();
        window.dispatchEvent(new Event('increment-speed'));
        break;
      case 'ArrowDown':
        e.preventDefault();
        window.dispatchEvent(new Event('decrement-speed'));
        break;
      case 'ArrowRight':
        e.preventDefault();
        window.dispatchEvent(new Event('increment-rpm'));
        break;
      case 'ArrowLeft':
        e.preventDefault();
        window.dispatchEvent(new Event('decrement-rpm'));
        break;
      case 'g':
      case 'G':
        window.dispatchEvent(new Event('cycle-gear'));
        break;
      case 'e':
      case 'E':
        window.dispatchEvent(new Event('toggle-eco'));
        break;
      case 'f':
        window.dispatchEvent(new Event(e.shiftKey ? 'decrease-fuel' : 'increase-fuel'));
        break;
      case 'F':
        window.dispatchEvent(new Event('decrease-fuel'));
        break;
      case 't':
        window.dispatchEvent(new Event(e.shiftKey ? 'decrease-temp' : 'increase-temp'));
        break;
      case 'T':
        window.dispatchEvent(new Event('decrease-temp'));
        break;
      case 'd':
        window.dispatchEvent(new Event(e.shiftKey ? 'decrease-distance' : 'increase-distance'));
        break;
      case 'D':
        window.dispatchEvent(new Event('decrease-distance'));
        break;
    }
  }, []);

  useEffect(() => {
    window.addEventListener('keydown', handleKeyDown);
    return () => window.removeEventListener('keydown', handleKeyDown);
  }, [handleKeyDown]);

  return null;
}
