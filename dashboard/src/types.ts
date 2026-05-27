// ============================================================
// Shared type definitions for the CarPlay Dashboard
// Maps to the SensorPacket from ESP32-C3 + ESP32-S3 bridge
// ============================================================

/** Raw sensor data received from ESP32-S3 (via SSE or polling) */
export interface SensorData {
  connected: boolean;
  rxCount: number;
  rxRateHz?: number;
  seq: number;
  lights: number;
  distanceCm: number | null;
  distanceCmRaw: number | null;
  tempC: number | null;
  humidity: number | null;
  packetAgeMs: number | null;
  uptimeMs: number;
  ip: string;
  lostPackets?: number;
  duplicatePackets?: number;
  outOfOrderPackets?: number;
  lastSeqGap?: number;
  packetLossPct?: number;
  tachLocked?: boolean;
  vssActive?: boolean;
  distanceValid?: boolean;
  speed?: number;
  rpm?: number;
  fuel?: number;
  fuelConnected?: boolean;
}

/** Light flag bits — matches LightBits enum in sensor_packet.h */
export const LIGHT_HEAD   = 1 << 0;
export const LIGHT_LEFT   = 1 << 1;
export const LIGHT_RIGHT  = 1 << 2;
export const LIGHT_BRAKE  = 1 << 3;
export const LIGHT_HAZARD = 1 << 4;

/** Dashboard vehicle state — populated from sensor data + simulation */
export interface VehicleData {
  // Driving gauges
  speed: number;          // mph (or mapped from distance sensor)
  rpm: number;            // x1000 RPM
  fuel: number;           // 0-100%
  temperature: number;    // °F engine temp
  gear: string;           // P, R, N, D, 1, 2, 3
  ecoMode: boolean;
  odometer: number;

  // From ESP32-C3 sensors
  distanceCm: number | null;       // Ultrasonic smoothed distance
  distanceCmRaw: number | null;    // Ultrasonic raw distance
  tempC: number | null;            // DHT temperature (Celsius)
  humidity: number | null;         // DHT humidity %

  // Exterior lights (S3 WS2812 strips)
  lightFlags: number;              // Bitmask of LIGHT_* flags

  // Connection
  espConnected: boolean;
  lastSensorUpdate: number | null; // timestamp ms
  rxCount: number;
  rxRateHz: number;
  txSeq: number;
  packetAgeMs: number | null;
  lostPackets: number;
  duplicatePackets: number;
  outOfOrderPackets: number;
  lastSeqGap: number;
  packetLossPct: number;
  tachLocked: boolean;
  vssActive: boolean;
  distanceValid: boolean;
  bridgeIp: string | null;
}

/** Default initial state */
export const DEFAULT_VEHICLE_DATA: VehicleData = {
  speed: 0,
  rpm: 0.8,
  fuel: 72,
  temperature: 195,
  gear: 'P',
  ecoMode: true,
  odometer: 14175,

  distanceCm: null,
  distanceCmRaw: null,
  tempC: null,
  humidity: null,

  lightFlags: 0,

  espConnected: false,
  lastSensorUpdate: null,
  rxCount: 0,
  rxRateHz: 0,
  txSeq: 0,
  packetAgeMs: null,
  lostPackets: 0,
  duplicatePackets: 0,
  outOfOrderPackets: 0,
  lastSeqGap: 0,
  packetLossPct: 0,
  tachLocked: false,
  vssActive: false,
  distanceValid: false,
  bridgeIp: null,
};
