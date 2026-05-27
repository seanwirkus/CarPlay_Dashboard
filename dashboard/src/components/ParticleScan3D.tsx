import { useEffect, useRef } from 'react';

interface ParticleScan3DProps {
  speed: number;
  distanceCm: number | null;
  width: number;
  height: number;
  nearCm?: number;
  farCm?: number;
}

interface Particle {
  x: number;
  y: number;
  z: number;
  vx: number;
  vy: number;
  vz: number;
  life: number;
  maxLife: number;
  size: number;
}

/**
 * Sonar-style particle tunnel.
 * Speed adds energy, while ultrasonic distance changes how visually close the
 * particle field feels: near objects compress the tunnel and pull particles
 * toward the viewer.
 */
export default function ParticleScan3D({
  speed,
  distanceCm,
  width,
  height,
  nearCm = 15,
  farCm = 120,
}: ParticleScan3DProps) {
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const particlesRef = useRef<Particle[]>([]);
  const frameRef = useRef<number>(0);
  const speedRef = useRef(speed);
  const distanceRef = useRef(distanceCm);

  useEffect(() => {
    speedRef.current = speed;
  }, [speed]);

  useEffect(() => {
    distanceRef.current = distanceCm;
  }, [distanceCm]);

  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;

    const ctx = canvas.getContext('2d');
    if (!ctx) return;

    canvas.width = width;
    canvas.height = height;

    const clamp = (value: number, min: number, max: number) =>
      Math.min(max, Math.max(min, value));

    const lerp = (a: number, b: number, t: number) => a + (b - a) * t;

    const getCloseness = (value: number | null) => {
      if (value === null || Number.isNaN(value)) return 0.16;
      if (value <= nearCm) return 1;
      if (value >= farCm) return 0;
      return 1 - (value - nearCm) / (farCm - nearCm);
    };

    const createParticle = (speedValue: number, closeness: number): Particle => {
      const speedFactor = Math.max(0.2, 0.35 + speedValue / 120);
      const spread = lerp(0.82, 0.36, closeness);
      const depth = lerp(980, 380, closeness);
      const xOrigin = width / 2 + (Math.random() - 0.5) * width * spread;
      const yOrigin = height / 2 + (Math.random() - 0.5) * height * spread;

      return {
        x: xOrigin,
        y: yOrigin,
        z: 30 + Math.random() * depth,
        vx: (Math.random() - 0.5) * lerp(0.9, 1.8, closeness) * speedFactor,
        vy: (Math.random() - 0.5) * lerp(0.6, 1.3, closeness) * speedFactor,
        vz: -(Math.random() * 2.2 + 1.2 + closeness * 2.1) * speedFactor,
        life: 0,
        maxLife: 65 + Math.random() * 130,
        size: Math.random() * (1.4 + closeness * 2.1) + 0.6,
      };
    };

    const seedCloseness = getCloseness(distanceRef.current);
    const seedSpeed = speedRef.current;

    while (particlesRef.current.length < 120) {
      const p = createParticle(seedSpeed, seedCloseness);
      p.life = Math.random() * p.maxLife;
      particlesRef.current.push(p);
    }

    let lastTime = 0;

    const animate = (time: number) => {
      const dt = Math.min(32, time - lastTime) / 16; // normalize to ~60fps
      lastTime = time;

      const liveSpeed = speedRef.current;
      const speedNorm = clamp(liveSpeed / 180, 0, 1);
      const closeness = getCloseness(distanceRef.current);
      const targetParticles = Math.min(340, 72 + Math.floor(liveSpeed * 0.55 + closeness * 120));

      ctx.fillStyle = `rgba(4, 7, 11, ${0.14 + closeness * 0.08})`;
      ctx.fillRect(0, 0, width, height);

      const particles = particlesRef.current;

      while (particles.length < targetParticles) {
        particles.push(createParticle(liveSpeed, closeness));
      }
      while (particles.length > targetParticles) {
        particles.pop();
      }

      const glowRadius = lerp(68, 124, closeness);
      const glowGradient = ctx.createRadialGradient(
        width / 2,
        height / 2,
        0,
        width / 2,
        height / 2,
        glowRadius,
      );
      glowGradient.addColorStop(0, `rgba(255, ${Math.round(165 - closeness * 50)}, ${Math.round(120 - closeness * 56)}, ${0.06 + closeness * 0.08})`);
      glowGradient.addColorStop(1, 'rgba(0, 0, 0, 0)');
      ctx.fillStyle = glowGradient;
      ctx.fillRect(0, 0, width, height);

      for (let i = 0; i < particles.length; i++) {
        const p = particles[i];

        // Update position
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.z += p.vz * dt;
        p.life += dt;

        // Respawn if dead or off-screen
        if (p.life >= p.maxLife || p.z <= 0 || p.x < -20 || p.x > width + 20 || p.y < -20 || p.y > height + 20) {
          const newP = createParticle(liveSpeed, closeness);
          particles[i] = newP;
          continue;
        }

        // Perspective projection
        const perspective = 600 / (600 + p.z);
        const screenX = (p.x - width / 2) * perspective + width / 2;
        const screenY = (p.y - height / 2) * perspective + height / 2;
        const screenSize = p.size * perspective;

        // Life alpha (fade in/out)
        const lifeRatio = p.life / p.maxLife;
        const alpha = lifeRatio < 0.1 ? lifeRatio * 10 :
                      lifeRatio > 0.8 ? (1 - lifeRatio) * 5 :
                      1;

        const r = Math.round(lerp(92, 255, closeness));
        const g = Math.round(lerp(216, 158, closeness) - speedNorm * 18);
        const b = Math.round(lerp(255, 82, closeness) - speedNorm * 10);

        const depthAlpha = Math.max(0.12, 1 - p.z / 1000);

        ctx.beginPath();
        ctx.arc(screenX, screenY, Math.max(0.3, screenSize), 0, Math.PI * 2);
        ctx.fillStyle = `rgba(${r}, ${g}, ${b}, ${alpha * depthAlpha * (0.42 + closeness * 0.32)})`;
        ctx.fill();

        if (p.z < 320 && screenSize > 0.8) {
          ctx.beginPath();
          ctx.arc(screenX, screenY, screenSize * 3, 0, Math.PI * 2);
          ctx.fillStyle = `rgba(${r}, ${g}, ${b}, ${alpha * depthAlpha * (0.08 + closeness * 0.08)})`;
          ctx.fill();
        }
      }

      const gridSpacing = lerp(82, 60, closeness);
      const gridOffset = (time * 0.018 * (1 + speedNorm + closeness * 0.6)) % gridSpacing;
      ctx.strokeStyle = `rgba(${Math.round(lerp(98, 255, closeness))}, ${Math.round(lerp(212, 176, closeness))}, ${Math.round(lerp(255, 122, closeness))}, ${0.04 + closeness * 0.05})`;
      ctx.lineWidth = 0.5;

      for (let x = -gridSpacing + gridOffset; x < width + gridSpacing; x += gridSpacing) {
        ctx.beginPath();
        ctx.moveTo(x, 0);
        ctx.lineTo(x, height);
        ctx.stroke();
      }
      for (let y = -gridSpacing + gridOffset; y < height + gridSpacing; y += gridSpacing) {
        ctx.beginPath();
        ctx.moveTo(0, y);
        ctx.lineTo(width, y);
        ctx.stroke();
      }

      frameRef.current = requestAnimationFrame(animate);
    };

    frameRef.current = requestAnimationFrame(animate);

    return () => {
      cancelAnimationFrame(frameRef.current);
    };
  }, [farCm, height, nearCm, width]);

  return (
    <canvas
      ref={canvasRef}
      width={width}
      height={height}
      className="w-full h-full"
      style={{ width, height }}
    />
  );
}
