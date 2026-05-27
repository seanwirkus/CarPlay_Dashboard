import { useMemo } from 'react';

interface SpeedometerProps {
  speed: number;       // 0-160
  maxSpeed?: number;
}

/**
 * CarPlay Ultra Speedometer — SVG arc gauge with cyan-green sweep,
 * tick marks, needle, and large speed readout.
 * Matches the native C++ CarPlayUltra_DrawSpeedometer layout.
 */
export default function Speedometer({ speed, maxSpeed = 160 }: SpeedometerProps) {
  const cx = 155, cy = 155, rOuter = 140, rInner = 105;
  const startAngle = 135, endAngle = 405; // 270° sweep
  const sweepDeg = endAngle - startAngle;

  const speedPct = Math.min(1, Math.max(0, speed / maxSpeed));
  const activeEnd = startAngle + speedPct * sweepDeg;

  // Convert deg → SVG arc
  const describeArc = (cx: number, cy: number, r: number, start: number, end: number) => {
    const startRad = (start * Math.PI) / 180;
    const endRad = (end * Math.PI) / 180;
    const x1 = cx + r * Math.cos(startRad);
    const y1 = cy + r * Math.sin(startRad);
    const x2 = cx + r * Math.cos(endRad);
    const y2 = cy + r * Math.sin(endRad);
    const largeArc = end - start > 180 ? 1 : 0;
    return `M ${x1} ${y1} A ${r} ${r} 0 ${largeArc} 1 ${x2} ${y2}`;
  };

  // Tick marks
  const ticks = useMemo(() => {
    const items = [];
    const count = 32;
    for (let i = 0; i <= count; i++) {
      const t = i / count;
      const angleDeg = startAngle + t * sweepDeg;
      const angleRad = (angleDeg * Math.PI) / 180;
      const isMajor = i % 4 === 0;
      const r1 = isMajor ? rOuter + 6 : rOuter + 2;
      const r2 = rOuter - 4;
      items.push({
        x1: cx + r1 * Math.cos(angleRad),
        y1: cy + r1 * Math.sin(angleRad),
        x2: cx + r2 * Math.cos(angleRad),
        y2: cy + r2 * Math.sin(angleRad),
        isMajor,
      });
    }
    return items;
  }, [startAngle, sweepDeg]);

  // Speed labels
  const labels = useMemo(() => {
    return [0, 20, 40, 60, 80, 100, 120, 140, 160].map(v => {
      const t = v / maxSpeed;
      const angleDeg = startAngle + t * sweepDeg;
      const angleRad = (angleDeg * Math.PI) / 180;
      const lr = rOuter + 22;
      return {
        x: cx + lr * Math.cos(angleRad),
        y: cy + lr * Math.sin(angleRad),
        value: v,
      };
    });
  }, [maxSpeed, startAngle, sweepDeg]);

  // Needle
  const needleAngleDeg = startAngle + speedPct * sweepDeg;
  const needleRad = (needleAngleDeg * Math.PI) / 180;
  const needleLen = rInner - 8;
  const needleTailLen = 16;
  const nx = cx + needleLen * Math.cos(needleRad);
  const ny = cy + needleLen * Math.sin(needleRad);
  const ntx = cx - needleTailLen * Math.cos(needleRad);
  const nty = cy - needleTailLen * Math.sin(needleRad);

  // Speed color
  const speedColor = speed > 120 ? '#ff2222' : speed > 80 ? '#ff8800' : '#00ffff';

  // Active sweep gradient ID
  const gradId = 'speedo-sweep-grad';

  return (
    <div className="relative" style={{ width: 310, height: 310 }}>
      <svg viewBox="0 0 310 310" width="310" height="310">
        <defs>
          <linearGradient id={gradId} x1="0%" y1="0%" x2="100%" y2="100%">
            <stop offset="0%" stopColor="#00ffff" />
            <stop offset="40%" stopColor="#00ff66" />
            <stop offset="100%" stopColor="#00b3a0" />
          </linearGradient>
          <filter id="speedo-glow">
            <feGaussianBlur stdDeviation="3" result="blur" />
            <feMerge>
              <feMergeNode in="blur" />
              <feMergeNode in="SourceGraphic" />
            </feMerge>
          </filter>
        </defs>

        {/* Background arc track */}
        <path
          d={describeArc(cx, cy, (rOuter + rInner) / 2, startAngle, endAngle)}
          fill="none"
          stroke="#0c1018"
          strokeWidth={rOuter - rInner}
          strokeLinecap="round"
          opacity={0.7}
        />

        {/* Active sweep arc */}
        {speedPct > 0.005 && (
          <path
            d={describeArc(cx, cy, (rOuter + rInner) / 2, startAngle, activeEnd)}
            fill="none"
            stroke={`url(#${gradId})`}
            strokeWidth={rOuter - rInner - 6}
            strokeLinecap="round"
            filter="url(#speedo-glow)"
            style={{ transition: 'all 0.15s ease-out' }}
          />
        )}

        {/* Tick marks */}
        {ticks.map((t, i) => (
          <line
            key={i}
            x1={t.x1} y1={t.y1} x2={t.x2} y2={t.y2}
            stroke={t.isMajor ? '#7888aa' : '#2a3350'}
            strokeWidth={t.isMajor ? 2 : 1}
          />
        ))}

        {/* Speed number labels */}
        {labels.map(l => (
          <text
            key={l.value}
            x={l.x} y={l.y}
            fill="#4a5570"
            fontSize="9"
            fontFamily="'JetBrains Mono', monospace"
            textAnchor="middle"
            dominantBaseline="central"
          >
            {l.value}
          </text>
        ))}

        {/* Needle */}
        <line
          x1={ntx} y1={nty} x2={nx} y2={ny}
          stroke="#ffffff"
          strokeWidth={2.5}
          strokeLinecap="round"
          filter="url(#speedo-glow)"
          style={{ transition: 'all 0.12s ease-out' }}
        />

        {/* Center hub */}
        <circle cx={cx} cy={cy} r={10} fill="#1e2540" />
        <circle cx={cx} cy={cy} r={4} fill="#ffffff" />
      </svg>

      {/* Speed readout overlay */}
      <div className="absolute inset-0 flex flex-col items-center justify-center pointer-events-none"
           style={{ paddingTop: 10 }}>
        <div
          className="font-mono font-black leading-none speed-glow"
          style={{
            fontSize: 56,
            color: speedColor,
            transition: 'color 0.3s',
          }}
        >
          {Math.round(speed)}
        </div>
        <div className="text-[10px] font-mono tracking-[0.3em] uppercase mt-1"
             style={{ color: '#4a5570' }}>
          MPH
        </div>
        <div className="text-[8px] font-mono mt-0.5"
             style={{ color: '#2a3350' }}>
          {Math.round(speed * 1.60934)} km/h
        </div>
      </div>
    </div>
  );
}
