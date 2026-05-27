import { useMemo } from 'react';

interface RPMGaugeProps {
  rpm: number;       // x1000 (e.g. 2.5 = 2500 RPM)
  maxRpm?: number;   // x1000 (default 8.0)
}

/**
 * CarPlay Ultra RPM Gauge — Smaller arc gauge with red zone,
 * matches the native C++ CarPlayUltra_DrawRPMGauge layout.
 */
export default function RPMGauge({ rpm, maxRpm = 8.0 }: RPMGaugeProps) {
  const cx = 110, cy = 110, rOuter = 95, rInner = 70;
  const startAngle = 135, endAngle = 405;
  const sweepDeg = endAngle - startAngle;

  const rpmPct = Math.min(1, Math.max(0, rpm / maxRpm));
  const activeEnd = startAngle + rpmPct * sweepDeg;
  const isHighRpm = rpm >= 6.0;
  const redZoneStart = startAngle + (6.0 / maxRpm) * sweepDeg;

  const describeArc = (cx: number, cy: number, r: number, start: number, end: number) => {
    const sr = (start * Math.PI) / 180;
    const er = (end * Math.PI) / 180;
    const x1 = cx + r * Math.cos(sr);
    const y1 = cy + r * Math.sin(sr);
    const x2 = cx + r * Math.cos(er);
    const y2 = cy + r * Math.sin(er);
    const la = end - start > 180 ? 1 : 0;
    return `M ${x1} ${y1} A ${r} ${r} 0 ${la} 1 ${x2} ${y2}`;
  };

  // Ticks
  const ticks = useMemo(() => {
    const items = [];
    const count = 24;
    for (let i = 0; i <= count; i++) {
      const t = i / count;
      const angleDeg = startAngle + t * sweepDeg;
      const angleRad = (angleDeg * Math.PI) / 180;
      const isMajor = i % 3 === 0;
      const r1 = isMajor ? rOuter + 5 : rOuter + 2;
      const r2 = rOuter - 3;
      const inRedZone = angleDeg >= redZoneStart;
      items.push({
        x1: cx + r1 * Math.cos(angleRad), y1: cy + r1 * Math.sin(angleRad),
        x2: cx + r2 * Math.cos(angleRad), y2: cy + r2 * Math.sin(angleRad),
        isMajor, inRedZone,
      });
    }
    return items;
  }, [redZoneStart, startAngle, sweepDeg]);

  // Labels 0-8
  const labels = useMemo(() => {
    return [0, 1, 2, 3, 4, 5, 6, 7, 8].map(v => {
      const t = v / maxRpm;
      const angleDeg = startAngle + t * sweepDeg;
      const angleRad = (angleDeg * Math.PI) / 180;
      const lr = rOuter + 16;
      return {
        x: cx + lr * Math.cos(angleRad),
        y: cy + lr * Math.sin(angleRad),
        value: v,
        isRed: v >= 6,
      };
    });
  }, [maxRpm, startAngle, sweepDeg]);

  // Needle
  const needleAngle = startAngle + rpmPct * sweepDeg;
  const needleRad = (needleAngle * Math.PI) / 180;
  const needleLen = rInner - 6;
  const nx = cx + needleLen * Math.cos(needleRad);
  const ny = cy + needleLen * Math.sin(needleRad);
  const ntx = cx - 12 * Math.cos(needleRad);
  const nty = cy - 12 * Math.sin(needleRad);

  return (
    <div className="relative" style={{ width: 220, height: 220 }}>
      <svg viewBox="0 0 220 220" width="220" height="220">
        <defs>
          <linearGradient id="rpm-sweep-grad" x1="0%" y1="0%" x2="100%" y2="100%">
            <stop offset="0%" stopColor="#00b3a0" />
            <stop offset="65%" stopColor="#ff8800" />
            <stop offset="100%" stopColor="#ff2222" />
          </linearGradient>
          <filter id="rpm-glow">
            <feGaussianBlur stdDeviation="2" result="blur" />
            <feMerge><feMergeNode in="blur" /><feMergeNode in="SourceGraphic" /></feMerge>
          </filter>
        </defs>

        {/* Background arc */}
        <path
          d={describeArc(cx, cy, (rOuter + rInner) / 2, startAngle, endAngle)}
          fill="none" stroke="#0c1018" strokeWidth={rOuter - rInner} strokeLinecap="round" opacity={0.7}
        />

        {/* Active sweep */}
        {rpmPct > 0.005 && (
          <path
            d={describeArc(cx, cy, (rOuter + rInner) / 2, startAngle, activeEnd)}
            fill="none" stroke="url(#rpm-sweep-grad)" strokeWidth={rOuter - rInner - 5}
            strokeLinecap="round" filter="url(#rpm-glow)"
            style={{ transition: 'all 0.1s ease-out' }}
          />
        )}

        {/* Red zone indicator (always visible) */}
        <path
          d={describeArc(cx, cy, rOuter, redZoneStart, endAngle)}
          fill="none" stroke="#ff2222" strokeWidth={3} opacity={0.4}
        />

        {/* Ticks */}
        {ticks.map((t, i) => (
          <line key={i} x1={t.x1} y1={t.y1} x2={t.x2} y2={t.y2}
            stroke={t.inRedZone ? '#ff2222' : t.isMajor ? '#7888aa' : '#2a3350'}
            strokeWidth={t.isMajor ? 1.5 : 0.8} opacity={t.inRedZone ? 0.6 : 1}
          />
        ))}

        {/* Labels */}
        {labels.map(l => (
          <text key={l.value} x={l.x} y={l.y} fill={l.isRed ? '#ff2222' : '#4a5570'}
            fontSize="8" fontFamily="'JetBrains Mono', monospace"
            textAnchor="middle" dominantBaseline="central" opacity={l.isRed ? 0.7 : 1}>
            {l.value}
          </text>
        ))}

        {/* Needle */}
        <line x1={ntx} y1={nty} x2={nx} y2={ny}
          stroke="#ffffff" strokeWidth={2} strokeLinecap="round"
          filter="url(#rpm-glow)" style={{ transition: 'all 0.1s ease-out' }}
        />

        {/* Center hub */}
        <circle cx={cx} cy={cy} r={8} fill="#1e2540" />
        <circle cx={cx} cy={cy} r={3} fill="#ffffff" />
      </svg>

      {/* RPM readout */}
      <div className="absolute inset-0 flex flex-col items-center justify-center pointer-events-none"
           style={{ paddingTop: 6 }}>
        <div className={`font-mono font-bold leading-none text-lg ${isHighRpm ? 'text-red-400' : 'text-white'}`}
             style={{ transition: 'color 0.3s' }}>
          {(rpm * 1000).toFixed(0)}
        </div>
        <div className="text-[8px] font-mono tracking-widest uppercase mt-0.5" style={{ color: '#4a5570' }}>
          RPM
        </div>
      </div>
    </div>
  );
}
