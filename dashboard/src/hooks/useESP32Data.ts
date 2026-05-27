import { useCallback, useEffect, useRef, useState } from 'react';
import type { VehicleData, SensorData } from '../types';

interface SmoothState {
  speed?: number;
  rpm?: number;
  fuel?: number;
  distanceCm?: number;
}

function smoothNumber(
  state: SmoothState,
  key: keyof SmoothState,
  next: number,
  alpha: number,
  snapDelta: number
) {
  const current = state[key];
  if (current === undefined || Number.isNaN(current) || Math.abs(next - current) >= snapDelta) {
    state[key] = next;
  } else {
    state[key] = current + alpha * (next - current);
  }
  return state[key] ?? next;
}

/**
 * Custom hook that connects to the ESP32-S3 dashboard receiver
 * via SSE (Server-Sent Events) with polling fallback.
 *
 * Stream: C3 sensors → ESP-NOW → S3 → SSE → this hook → React state
 */
export function useESP32Data(
  updateVehicleData: (data: Partial<VehicleData>) => void
) {
  const [connected, setConnected] = useState(false);
  const [lastUpdate, setLastUpdate] = useState<number | null>(null);
  const eventSourceRef = useRef<EventSource | null>(null);
  const pollRef = useRef<ReturnType<typeof setInterval> | null>(null);
  const reconnectTimerRef = useRef<number | null>(null);
  const connectSSERef = useRef<() => void>(() => {});
  const smoothRef = useRef<SmoothState>({});

  const getBaseUrl = useCallback(() => {
    const isLocal = window.location.hostname === 'localhost' ||
                    window.location.hostname === '127.0.0.1';
    return isLocal ? 'http://192.168.4.1' : '';
  }, []);

  /** Map raw sensor payload to VehicleData partial */
  const mapSensorData = useCallback((data: SensorData) => {
    const update: Partial<VehicleData> = {
      espConnected: data.connected,
      rxCount: data.rxCount,
      rxRateHz: data.rxRateHz ?? 0,
      txSeq: data.seq,
      lightFlags: data.lights,
      packetAgeMs: data.packetAgeMs,
      lostPackets: data.lostPackets ?? 0,
      duplicatePackets: data.duplicatePackets ?? 0,
      outOfOrderPackets: data.outOfOrderPackets ?? 0,
      lastSeqGap: data.lastSeqGap ?? 0,
      packetLossPct: data.packetLossPct ?? 0,
      tachLocked: !!data.tachLocked,
      vssActive: !!data.vssActive,
      distanceValid: !!data.distanceValid,
      bridgeIp: data.ip,
      lastSensorUpdate: Date.now(),
    };

    // Ultrasonic distance (direct pass-through)
    if (data.distanceCm !== null && data.distanceCm !== undefined) {
      update.distanceCm = smoothNumber(smoothRef.current, 'distanceCm', data.distanceCm, 0.35, 45);
    } else if (data.distanceValid === false) {
      smoothRef.current.distanceCm = undefined;
      update.distanceCm = null;
    }
    if (data.distanceCmRaw !== null && data.distanceCmRaw !== undefined) {
      update.distanceCmRaw = data.distanceCmRaw;
    }

    // DHT temperature → Celsius pass-through + Fahrenheit for engine display
    if (data.tempC !== null && data.tempC !== undefined) {
      update.tempC = data.tempC;
      update.temperature = Math.round(data.tempC * 9 / 5 + 32);
    }

    // DHT humidity → direct pass-through
    if (data.humidity !== null && data.humidity !== undefined) {
      update.humidity = data.humidity;
    }

    if (typeof data.speed === 'number') {
      update.speed = smoothNumber(smoothRef.current, 'speed', data.speed, 0.40, 35);
    }
    if (typeof data.rpm === 'number') {
      update.rpm = smoothNumber(smoothRef.current, 'rpm', data.rpm, data.tachLocked ? 0.45 : 0.65, 2.0);
    }
    if (data.fuelConnected && typeof data.fuel === 'number') {
      update.fuel = smoothNumber(smoothRef.current, 'fuel', data.fuel, 0.08, 30);
    }

    return update;
  }, []);

  /** Connect via SSE */
  const connectSSE = useCallback(() => {
    if (eventSourceRef.current) {
      eventSourceRef.current.close();
    }

    const baseUrl = getBaseUrl();

    try {
      const es = new EventSource(`${baseUrl}/events`);
      eventSourceRef.current = es;

      es.onopen = () => {
        setConnected(true);
      };

      es.onmessage = (event) => {
        try {
          const data: SensorData = JSON.parse(event.data);
          setLastUpdate(Date.now());
          setConnected(true);
          updateVehicleData(mapSensorData(data));
        } catch (e) {
          console.warn('Failed to parse ESP32 SSE data:', e);
        }
      };

      es.onerror = () => {
        setConnected(false);
        es.close();
        if (reconnectTimerRef.current !== null) {
          window.clearTimeout(reconnectTimerRef.current);
        }
        reconnectTimerRef.current = window.setTimeout(() => {
          connectSSERef.current();
        }, 3000);
      };
    } catch (e) {
      console.warn('SSE connection failed:', e);
      setConnected(false);
    }
  }, [getBaseUrl, mapSensorData, updateVehicleData]);

  useEffect(() => {
    connectSSERef.current = connectSSE;
  }, [connectSSE]);

  /** Polling fallback */
  useEffect(() => {
    const baseUrl = getBaseUrl();

    pollRef.current = setInterval(async () => {
      if (connected) return; // SSE is working, skip polling
      try {
        const res = await fetch(`${baseUrl}/api/status`, {
          signal: AbortSignal.timeout(2000),
        });
        if (res.ok) {
          const data: SensorData = await res.json();
          setConnected(true);
          setLastUpdate(Date.now());
          updateVehicleData(mapSensorData(data));
        }
      } catch {
        // ESP32 not reachable — silent
      }
    }, 750);

    return () => {
      if (pollRef.current) clearInterval(pollRef.current);
    };
  }, [connected, getBaseUrl, mapSensorData, updateVehicleData]);

  /** Init SSE on mount */
  useEffect(() => {
    const initialConnect = window.setTimeout(() => {
      connectSSERef.current();
    }, 0);

    return () => {
      window.clearTimeout(initialConnect);
      if (reconnectTimerRef.current !== null) {
        window.clearTimeout(reconnectTimerRef.current);
      }
      if (eventSourceRef.current) eventSourceRef.current.close();
    };
  }, []);

  return { connected, lastUpdate, reconnect: connectSSE };
}
