import { startTransition, useCallback, useDeferredValue, useEffect, useState } from 'react';
import type { VehicleData } from './types';
import { DEFAULT_VEHICLE_DATA } from './types';
import { useESP32Data } from './hooks/useESP32Data';
import Speedometer from './components/Speedometer';
import RPMGauge from './components/RPMGauge';
import ParticleScan3D from './components/ParticleScan3D';
import ProximitySensor from './components/ProximitySensor';
import StatusBar from './components/StatusBar';
import TelemetryRail from './components/TelemetryRail';
import KeyboardControls from './components/KeyboardControls';

function getDistanceBand(distanceCm: number | null) {
  if (distanceCm === null || Number.isNaN(distanceCm)) {
    return {
      label: 'No target',
      tone: 'is-idle',
      blurb: 'Waiting for an ultrasonic return from the sensor bridge.',
    };
  }

  if (distanceCm < 20) {
    return {
      label: 'Near',
      tone: 'is-critical',
      blurb: 'The particle field pulls forward to mark a close obstruction.',
    };
  }

  if (distanceCm < 50) {
    return {
      label: 'Closing',
      tone: 'is-warn',
      blurb: 'The tunnel compresses as the near field starts to fill.',
    };
  }

  return {
    label: 'Clear',
    tone: 'is-live',
    blurb: 'Open space keeps the tunnel long and calm.',
  };
}

export default function App() {
  const [scale, setScale] = useState(1);
  const [vehicleData, setVehicleData] = useState<VehicleData>(DEFAULT_VEHICLE_DATA);

  // Scale to fit the viewport
  useEffect(() => {
    const handleResize = () => {
      const pad = 20;
      const w = window.innerWidth - pad * 2;
      const h = window.innerHeight - pad * 2;
      setScale(Math.min(w / 1024, h / 600, 1));
    };
    handleResize();
    window.addEventListener('resize', handleResize);
    return () => window.removeEventListener('resize', handleResize);
  }, []);

  const updateVehicleData = useCallback((data: Partial<VehicleData>) => {
    startTransition(() => {
      setVehicleData((prev) => ({ ...prev, ...data }));
    });
  }, []);

  useESP32Data(updateVehicleData);

  useEffect(() => {
    const handlers: Record<string, () => void> = {
      'increment-speed': () => setVehicleData(p => ({ ...p, speed: Math.min(200, p.speed + 5) })),
      'decrement-speed': () => setVehicleData(p => ({ ...p, speed: Math.max(0, p.speed - 5) })),
      'increment-rpm': () => setVehicleData(p => ({ ...p, rpm: Number(Math.min(8.0, p.rpm + 0.5).toFixed(1)) })),
      'decrement-rpm': () => setVehicleData(p => ({ ...p, rpm: Number(Math.max(0, p.rpm - 0.5).toFixed(1)) })),
      'cycle-gear': () => setVehicleData(p => {
        const gears = ['P', 'R', 'N', 'D', '1', '2', '3'];
        const i = (gears.indexOf(p.gear) + 1) % gears.length;
        return { ...p, gear: gears[i] };
      }),
      'toggle-eco': () => setVehicleData(p => ({ ...p, ecoMode: !p.ecoMode })),
      'increase-fuel': () => setVehicleData(p => ({ ...p, fuel: Math.min(100, p.fuel + 5) })),
      'decrease-fuel': () => setVehicleData(p => ({ ...p, fuel: Math.max(0, p.fuel - 5) })),
      'increase-temp': () => setVehicleData(p => ({ ...p, temperature: Math.min(250, p.temperature + 5) })),
      'decrease-temp': () => setVehicleData(p => ({ ...p, temperature: Math.max(0, p.temperature - 5) })),
      'increase-distance': () => setVehicleData(p => ({
        ...p, distanceCm: Math.min(400, (p.distanceCm ?? 100) + 10),
        distanceCmRaw: Math.min(400, (p.distanceCmRaw ?? 100) + 10),
      })),
      'decrease-distance': () => setVehicleData(p => ({
        ...p, distanceCm: Math.max(2, (p.distanceCm ?? 100) - 10),
        distanceCmRaw: Math.max(2, (p.distanceCmRaw ?? 100) - 10),
      })),
    };

    Object.entries(handlers).forEach(([event, handler]) =>
      window.addEventListener(event, handler));
    return () => Object.entries(handlers).forEach(([event, handler]) =>
      window.removeEventListener(event, handler));
  }, []);

  const { speed, rpm, gear, fuel, temperature, ecoMode, odometer,
          distanceCm, distanceCmRaw, tempC, humidity, lightFlags,
          espConnected, rxCount, rxRateHz, txSeq, packetAgeMs, packetLossPct, lastSeqGap } = vehicleData;

  const deferredSpeed = useDeferredValue(speed);
  const deferredDistanceCm = useDeferredValue(distanceCm);
  const distanceBand = getDistanceBand(distanceCm);
  const isLive = espConnected;
  const gearColor = gear === 'R' ? '#ff2222' : gear === 'P' ? '#ff8800' : '#00ffff';
  const devMode =
    typeof window !== 'undefined' &&
    (window.location.hostname === 'localhost' || window.location.hostname === '127.0.0.1');
  const distanceFill =
    deferredDistanceCm === null || Number.isNaN(deferredDistanceCm)
      ? 0
      : Math.max(0, Math.min(100, ((120 - deferredDistanceCm) / 105) * 100));

  return (
    <div className="dashboard-stage no-select">
      <KeyboardControls />

      <div
        className="dashboard-shell"
        style={{
          transform: `scale(${scale})`,
        }}
      >
        <div className="dashboard-backdrop" />

        <StatusBar
          lightFlags={lightFlags}
          espConnected={isLive}
          rxCount={rxCount}
          rxRateHz={rxRateHz}
          txSeq={txSeq}
          packetAgeMs={packetAgeMs}
          packetLossPct={packetLossPct}
          lastSeqGap={lastSeqGap}
        />

        <div className="dashboard-main">
          <section className="panel-shell cluster-panel panel-reveal">
            <div className="panel-heading">
              <div>
                <p className="eyebrow">Drive cluster</p>
                <h2 className="section-title">Primary gauges</h2>
              </div>
              <span className="status-pill is-idle">SIM</span>
            </div>

            <div className="cluster-gauge-stage">
              <div className="cluster-speedometer">
                <Speedometer speed={speed} />
              </div>
              <div className="cluster-rpm">
                <RPMGauge rpm={rpm} />
              </div>
              <div className="gear-badge" style={{ borderColor: gearColor, boxShadow: `0 0 18px ${gearColor}33` }}>
                <span style={{ color: gearColor }}>{gear}</span>
              </div>
            </div>

            <div className="summary-grid">
              <div className="metric-cell">
                <span className="metric-label">Mode</span>
                <span className="metric-value">{ecoMode ? 'Eco assist' : 'Sport bias'}</span>
              </div>
              <div className="metric-cell">
                <span className="metric-label">Fuel</span>
                <span className="metric-value">{fuel}%</span>
              </div>
              <div className="metric-cell">
                <span className="metric-label">Dash temp</span>
                <span className="metric-value">{temperature}°F</span>
              </div>
              <div className="metric-cell">
                <span className="metric-label">Odometer</span>
                <span className="metric-value">{odometer.toLocaleString()} mi</span>
              </div>
            </div>
          </section>

          <section className="panel-shell scan-panel panel-reveal panel-delay-1">
            <div className="panel-heading">
              <div>
                <p className="eyebrow">Ultrasonic tunnel</p>
                <h2 className="section-title">Forward depth</h2>
              </div>
              <span className={`status-pill ${distanceBand.tone}`}>{distanceBand.label}</span>
            </div>

            <div className="scan-viewport">
              <ParticleScan3D
                speed={deferredSpeed}
                distanceCm={deferredDistanceCm}
                width={360}
                height={360}
              />

              <div className="scan-reticle" />

              <div className="scan-overlay">
                <span className="eyebrow">Clearance</span>
                <div className="distance-value">
                  {distanceCm === null || Number.isNaN(distanceCm) ? '---' : distanceCm.toFixed(1)}
                  <span>cm</span>
                </div>
                <p className="scan-note">{distanceBand.blurb}</p>
                <div className="scan-meta">
                  <span>Raw {distanceCmRaw === null || Number.isNaN(distanceCmRaw) ? '—' : `${distanceCmRaw.toFixed(1)} cm`}</span>
                  <span>Temp {tempC === null || Number.isNaN(tempC) ? '—' : `${tempC.toFixed(1)}°C`}</span>
                  <span>Humidity {humidity === null || Number.isNaN(humidity) ? '—' : `${humidity.toFixed(0)}%`}</span>
                </div>
              </div>

              <div className="scan-scale">
                <span>Far</span>
                <div className="scan-scale-track">
                  <div className="scan-scale-fill" style={{ height: `${distanceFill}%` }} />
                </div>
                <span>Near</span>
              </div>
            </div>
          </section>

          <aside className="side-column panel-reveal panel-delay-2">
            <ProximitySensor distanceCm={distanceCm} distanceCmRaw={distanceCmRaw} />
            <TelemetryRail vehicleData={vehicleData} />
          </aside>
        </div>

        {devMode && (
          <div className="dev-hint">
            Arrows speed/rpm · G gear · E eco · D distance
          </div>
        )}
      </div>
    </div>
  );
}
