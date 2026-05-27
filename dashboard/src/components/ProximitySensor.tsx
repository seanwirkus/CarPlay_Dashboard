interface ProximitySensorProps {
  distanceCm: number | null;
  distanceCmRaw: number | null;
  nearCm?: number;
  farCm?: number;
}

export default function ProximitySensor({
  distanceCm,
  distanceCmRaw,
  nearCm = 15,
  farCm = 120,
}: ProximitySensorProps) {
  const hasData = distanceCm !== null && !Number.isNaN(distanceCm);
  const hasRaw = distanceCmRaw !== null && !Number.isNaN(distanceCmRaw);
  const dist = hasData ? distanceCm! : null;

  const proximity =
    !hasData ? 0 :
    dist! <= nearCm ? 1 :
    dist! >= farCm ? 0 :
    1 - (dist! - nearCm) / (farCm - nearCm);

  const tone =
    !hasData ? 'is-idle' :
    dist! < 20 ? 'is-critical' :
    dist! < 50 ? 'is-warn' :
    'is-live';

  const label =
    !hasData ? 'Awaiting' :
    dist! < 20 ? 'Near field' :
    dist! < 50 ? 'Closing' :
    'Clear';

  const meterColor =
    tone === 'is-critical' ? '#ff6a5f' :
    tone === 'is-warn' ? '#ffb366' :
    tone === 'is-live' ? '#62f5d0' :
    '#2a3350';

  const particleBias =
    !hasData ? 'Idle' :
    proximity > 0.72 ? 'Strong pull' :
    proximity > 0.35 ? 'Medium pull' :
    'Light pull';

  const driftText =
    hasData && hasRaw ? `${Math.abs(dist! - distanceCmRaw!).toFixed(1)} cm drift` : 'No raw echo';

  return (
    <section className="panel-shell proximity-panel">
      <div className="panel-heading">
        <div>
          <p className="eyebrow">Ultrasonic</p>
          <h3 className="section-title">Near-field readout</h3>
        </div>
        <span className={`status-pill ${tone}`}>{label}</span>
      </div>

      <div className="proximity-stack">
        <div className="proximity-value" style={{ color: meterColor }}>
          <span className="font-mono font-black">
            {hasData ? dist!.toFixed(1) : '---'}
          </span>
          <span>cm</span>
        </div>

        <div className="proximity-state-line">
          <span className="metric-subvalue">Tunnel response</span>
          <span className="metric-value compact">{particleBias}</span>
        </div>

        <div className="proximity-meter">
          <div
            className="proximity-meter-fill"
            style={{
              width: `${Math.round(proximity * 100)}%`,
              background: `linear-gradient(90deg, ${meterColor}, rgba(255,255,255,0.9))`,
            }}
          />
        </div>

        <div className="proximity-scale">
          <span>Far</span>
          <span>Near</span>
        </div>

        <div className="summary-grid proximity-meta-grid">
          <div className="metric-cell">
            <span className="metric-label">Raw echo</span>
            <span className="metric-value">
              {hasRaw ? `${distanceCmRaw!.toFixed(1)} cm` : '—'}
            </span>
            <span className="metric-subvalue">{driftText}</span>
          </div>

          <div className="metric-cell">
            <span className="metric-label">Particle pull</span>
            <span className="metric-value">{Math.round(proximity * 100)}%</span>
            <span className="metric-subvalue">Correlated to the tunnel depth</span>
          </div>
        </div>
      </div>
    </section>
  );
}
