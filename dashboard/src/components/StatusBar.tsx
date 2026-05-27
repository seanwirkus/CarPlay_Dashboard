import { useEffect, useState } from 'react';
import { LIGHT_BRAKE, LIGHT_HAZARD, LIGHT_HEAD, LIGHT_LEFT, LIGHT_RIGHT } from '../types';

interface StatusBarProps {
  lightFlags: number;
  espConnected: boolean;
  rxCount: number;
  rxRateHz: number;
  txSeq: number;
  packetAgeMs: number | null;
  packetLossPct: number;
  lastSeqGap: number;
}

export default function StatusBar({
  lightFlags,
  espConnected,
  rxCount,
  rxRateHz,
  txSeq,
  packetAgeMs,
  packetLossPct,
  lastSeqGap,
}: StatusBarProps) {
  const [now, setNow] = useState(() => new Date());

  useEffect(() => {
    const timer = window.setInterval(() => setNow(new Date()), 30000);
    return () => window.clearInterval(timer);
  }, []);

  const h = now.getHours() % 12 || 12;
  const m = now.getMinutes().toString().padStart(2, '0');
  const ampm = now.getHours() >= 12 ? 'PM' : 'AM';

  const headOn = !!(lightFlags & LIGHT_HEAD);
  const leftOn = !!(lightFlags & LIGHT_LEFT) || !!(lightFlags & LIGHT_HAZARD);
  const brakeOn = !!(lightFlags & LIGHT_BRAKE);
  const rightOn = !!(lightFlags & LIGHT_RIGHT) || !!(lightFlags & LIGHT_HAZARD);

  return (
    <div className="status-bar-shell" id="status-bar">
      <div className="status-group">
        <div>
          <div className="eyebrow">CarPlay Cluster</div>
          <div className="status-caption">Dual-board telemetry bridge</div>
        </div>
      </div>

      <div className="status-time">
        <span className="status-time-value">{h}:{m}</span>
        <span className="status-time-period">{ampm}</span>
      </div>

      <div className="status-group end">
        <div className="light-cluster compact" aria-label="Exterior light state">
          <div className="light-token">
            <span className={`signal-dot ${headOn ? 'is-live' : 'is-idle'}`} />
            <span>Head</span>
          </div>
          <div className="light-token">
            <span className={`signal-dot ${leftOn ? 'is-warn' : 'is-idle'}`} />
            <span>Left</span>
          </div>
          <div className="light-token">
            <span className={`signal-dot ${brakeOn ? 'is-critical' : 'is-idle'}`} />
            <span>Brake</span>
          </div>
          <div className="light-token">
            <span className={`signal-dot ${rightOn ? 'is-warn' : 'is-idle'}`} />
            <span>Right</span>
          </div>
        </div>

        <span className={`status-pill ${espConnected ? 'is-live' : 'is-idle'}`}>
          {espConnected ? `Live ${packetAgeMs ?? 0} ms` : 'No link'}
        </span>

        <span className="status-meta">
          SEQ {txSeq} · RX {rxCount} · {rxRateHz.toFixed(0)} Hz · LOSS {packetLossPct.toFixed(1)}%{lastSeqGap > 0 ? ` +${lastSeqGap}` : ''}
        </span>
      </div>
    </div>
  );
}
