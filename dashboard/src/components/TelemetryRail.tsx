import { LIGHT_BRAKE, LIGHT_HAZARD, LIGHT_HEAD, LIGHT_LEFT, LIGHT_RIGHT, type VehicleData } from '../types';

interface TelemetryRailProps {
  vehicleData: VehicleData;
}

interface LightState {
  label: string;
  active: boolean;
}

export default function TelemetryRail({ vehicleData }: TelemetryRailProps) {
  const {
    tempC,
    humidity,
    espConnected,
    packetAgeMs,
    bridgeIp,
    rxCount,
    rxRateHz,
    txSeq,
    packetLossPct,
    lastSeqGap,
    lightFlags,
  } = vehicleData;

  const sensorTempF =
    tempC !== null && !Number.isNaN(tempC) ? Math.round((tempC * 9) / 5 + 32) : null;

  const lightStates: LightState[] = [
    { label: 'Head', active: !!(lightFlags & LIGHT_HEAD) },
    { label: 'Left', active: !!(lightFlags & LIGHT_LEFT) || !!(lightFlags & LIGHT_HAZARD) },
    { label: 'Brake', active: !!(lightFlags & LIGHT_BRAKE) },
    { label: 'Right', active: !!(lightFlags & LIGHT_RIGHT) || !!(lightFlags & LIGHT_HAZARD) },
  ];

  return (
    <div className="telemetry-rail">
      <section className="panel-shell telemetry-panel">
        <div className="panel-heading">
          <div>
            <p className="eyebrow">Bridge</p>
            <h3 className="section-title">Link + environment</h3>
          </div>
          <span className={`status-pill ${espConnected ? 'is-live' : 'is-idle'}`}>
            {espConnected ? 'Stable' : 'Searching'}
          </span>
        </div>

        <div className="metric-grid compact-grid">
          <div className="metric-cell">
            <span className="metric-label">Freshness</span>
            <span className="metric-value">
              {packetAgeMs === null ? '—' : `${packetAgeMs} ms`}
            </span>
          </div>

          <div className="metric-cell">
            <span className="metric-label">Packet</span>
            <span className="metric-value">#{txSeq}</span>
          </div>

          <div className="metric-cell">
            <span className="metric-label">RX count</span>
            <span className="metric-value">{rxCount}</span>
            <span className="metric-subvalue">
              {rxRateHz.toFixed(1)} Hz · {packetLossPct.toFixed(1)}% loss{lastSeqGap > 0 ? `, +${lastSeqGap} gap` : ''}
            </span>
          </div>

          <div className="metric-cell">
            <span className="metric-label">Sensor temp</span>
            <span className="metric-value">
              {tempC === null || Number.isNaN(tempC) ? '—' : `${tempC.toFixed(1)}°C`}
            </span>
            <span className="metric-subvalue">
              {sensorTempF === null ? 'No packet' : `${sensorTempF}°F`}
            </span>
          </div>

          <div className="metric-cell">
            <span className="metric-label">Humidity</span>
            <span className="metric-value">
              {humidity === null || Number.isNaN(humidity) ? '—' : `${humidity.toFixed(0)}%`}
            </span>
          </div>

          <div className="metric-cell">
            <span className="metric-label">AP</span>
            <span className="metric-value">{bridgeIp ?? '192.168.4.1'}</span>
          </div>
        </div>

        <div className="signal-strip" aria-label="Exterior light state">
          {lightStates.map((state) => (
            <span key={state.label} className={`signal-token ${state.active ? 'is-live' : 'is-idle'}`}>
              <span className={`signal-dot ${state.active ? 'is-live' : 'is-idle'}`} />
              <span>{state.label}</span>
            </span>
          ))}
        </div>
      </section>
    </div>
  );
}
