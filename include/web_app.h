#pragma once

#include <pgmspace.h>

static const char kRootPage[] PROGMEM = R"LASER(
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <title>Laser Lab</title>
  <style>
    :root {
      --bg: #071019;
      --panel: rgba(8, 22, 34, 0.78);
      --panel-strong: rgba(10, 28, 42, 0.92);
      --line: rgba(140, 206, 255, 0.18);
      --text: #eff7fb;
      --muted: #8fa7b7;
      --cyan: #68dcff;
      --amber: #ffb84d;
      --pink: #ff5ea9;
      --green: #5cf0a6;
      --warn: #ff6f61;
      --shadow: 0 24px 70px rgba(0, 0, 0, 0.38);
      --radius: 22px;
    }

    * {
      box-sizing: border-box;
    }

    html,
    body {
      min-height: 100%;
      margin: 0;
      color: var(--text);
      background:
        radial-gradient(circle at top, rgba(55, 145, 189, 0.22), transparent 34%),
        radial-gradient(circle at 20% 20%, rgba(251, 110, 159, 0.1), transparent 24%),
        linear-gradient(180deg, #03070c 0%, #071019 46%, #03080f 100%);
      font-family: "Avenir Next", "Helvetica Neue", "Segoe UI", sans-serif;
    }

    body::before {
      content: "";
      position: fixed;
      inset: 0;
      pointer-events: none;
      background:
        linear-gradient(rgba(255, 255, 255, 0.025) 1px, transparent 1px),
        linear-gradient(90deg, rgba(255, 255, 255, 0.025) 1px, transparent 1px);
      background-size: 28px 28px;
      mask-image: linear-gradient(180deg, rgba(0, 0, 0, 0.5), transparent 85%);
    }

    main {
      width: min(1380px, calc(100% - 24px));
      margin: 0 auto;
      padding: 24px 0 56px;
    }

    .hero {
      display: grid;
      grid-template-columns: minmax(0, 1.7fr) minmax(320px, 0.9fr);
      gap: 18px;
      margin-bottom: 18px;
    }

    .hero-card,
    .panel {
      background: var(--panel);
      border: 1px solid var(--line);
      border-radius: var(--radius);
      box-shadow: var(--shadow);
      backdrop-filter: blur(22px);
    }

    .hero-copy {
      padding: 24px;
    }

    .eyebrow {
      margin: 0 0 10px;
      font-size: 0.78rem;
      letter-spacing: 0.22em;
      text-transform: uppercase;
      color: var(--cyan);
    }

    h1 {
      margin: 0;
      font-size: clamp(2rem, 4vw, 3.7rem);
      line-height: 0.96;
      letter-spacing: -0.045em;
    }

    .hero-copy p,
    .panel-copy,
    .microcopy {
      color: var(--muted);
      line-height: 1.6;
      margin: 12px 0 0;
      max-width: 66ch;
      font-size: 0.98rem;
    }

    .status-card {
      padding: 22px;
      display: grid;
      gap: 14px;
      align-content: start;
    }

    .status-grid {
      display: grid;
      grid-template-columns: repeat(2, minmax(0, 1fr));
      gap: 10px;
    }

    .metric {
      padding: 12px 14px;
      border-radius: 16px;
      background: rgba(255, 255, 255, 0.035);
      border: 1px solid rgba(255, 255, 255, 0.07);
    }

    .metric-label {
      color: var(--muted);
      font-size: 0.76rem;
      text-transform: uppercase;
      letter-spacing: 0.18em;
      margin-bottom: 6px;
    }

    .metric strong {
      display: block;
      font-size: 1rem;
      letter-spacing: 0.02em;
    }

    .workspace {
      display: grid;
      grid-template-columns: minmax(0, 1.08fr) minmax(0, 0.92fr);
      gap: 18px;
      margin-bottom: 18px;
    }

    .panel {
      overflow: hidden;
    }

    .panel-head {
      display: flex;
      justify-content: space-between;
      align-items: flex-start;
      gap: 16px;
      padding: 18px 20px 0;
    }

    .panel-head h2,
    .panel-head h3 {
      margin: 0;
      font-weight: 650;
      letter-spacing: -0.03em;
    }

    .panel-head p {
      margin: 6px 0 0;
      font-size: 0.92rem;
      color: var(--muted);
      line-height: 1.5;
      max-width: 42ch;
    }

    .canvas-shell {
      padding: 18px 20px 20px;
    }

    .canvas-wrap {
      position: relative;
      border-radius: 24px;
      overflow: hidden;
      border: 1px solid rgba(124, 209, 255, 0.16);
      background:
        radial-gradient(circle at top, rgba(82, 172, 255, 0.13), transparent 35%),
        linear-gradient(180deg, rgba(4, 14, 24, 0.94), rgba(3, 9, 16, 0.98));
      aspect-ratio: 1 / 1;
      min-height: 320px;
    }

    canvas {
      display: block;
      width: 100%;
      height: 100%;
      touch-action: none;
    }

    .canvas-note {
      position: absolute;
      left: 18px;
      right: 18px;
      bottom: 16px;
      display: flex;
      justify-content: space-between;
      gap: 8px;
      flex-wrap: wrap;
      font-size: 0.76rem;
      letter-spacing: 0.08em;
      text-transform: uppercase;
      color: rgba(223, 241, 255, 0.72);
      pointer-events: none;
    }

    .toolbar,
    .button-row,
    .chip-row,
    .slider-grid,
    .stats-grid,
    .dmx-grid,
    .scene-grid {
      display: grid;
      gap: 12px;
    }

    .toolbar {
      padding: 0 20px 20px;
    }

    .button-row {
      grid-template-columns: repeat(auto-fit, minmax(132px, 1fr));
    }

    .chip-row {
      grid-template-columns: repeat(auto-fit, minmax(160px, 1fr));
    }

    button,
    .toggle,
    input[type="color"] {
      min-height: 46px;
      border: 0;
      border-radius: 15px;
      cursor: pointer;
      transition: transform 160ms ease, background 160ms ease, border-color 160ms ease;
    }

    button {
      padding: 12px 15px;
      background: linear-gradient(180deg, rgba(111, 202, 255, 0.14), rgba(50, 95, 140, 0.12));
      border: 1px solid rgba(122, 209, 255, 0.18);
      color: var(--text);
      font: inherit;
      font-weight: 600;
      letter-spacing: 0.01em;
    }

    button:hover,
    button:focus-visible,
    .toggle:hover,
    .toggle:focus-within {
      transform: translateY(-1px);
      border-color: rgba(136, 220, 255, 0.42);
      outline: none;
    }

    button.alt {
      background: linear-gradient(180deg, rgba(255, 184, 77, 0.14), rgba(164, 95, 22, 0.12));
      border-color: rgba(255, 184, 77, 0.22);
    }

    button.warn {
      background: linear-gradient(180deg, rgba(255, 111, 97, 0.16), rgba(122, 36, 30, 0.14));
      border-color: rgba(255, 111, 97, 0.28);
    }

    button.ghost {
      background: rgba(255, 255, 255, 0.04);
      border-color: rgba(255, 255, 255, 0.1);
    }

    .controls {
      display: grid;
      grid-template-columns: repeat(2, minmax(0, 1fr));
      gap: 18px;
      margin-bottom: 18px;
    }

    .panel-body {
      padding: 18px 20px 20px;
    }

    .slider-grid {
      grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
    }

    .slider-card,
    .stat-card {
      padding: 14px;
      border-radius: 18px;
      background: rgba(255, 255, 255, 0.038);
      border: 1px solid rgba(255, 255, 255, 0.08);
    }

    .slider-card label {
      display: flex;
      justify-content: space-between;
      align-items: center;
      gap: 10px;
      color: var(--muted);
      font-size: 0.86rem;
      letter-spacing: 0.08em;
      text-transform: uppercase;
      margin-bottom: 12px;
    }

    input[type="range"] {
      width: 100%;
      accent-color: var(--cyan);
    }

    .toggle {
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 12px;
      padding: 12px 14px;
      background: rgba(255, 255, 255, 0.038);
      border: 1px solid rgba(255, 255, 255, 0.08);
      color: var(--text);
    }

    .toggle span {
      color: var(--muted);
      font-size: 0.86rem;
      letter-spacing: 0.08em;
      text-transform: uppercase;
    }

    .toggle input {
      width: 18px;
      height: 18px;
    }

    .color-control {
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 14px;
      padding: 12px 14px;
      border-radius: 18px;
      background: rgba(255, 255, 255, 0.038);
      border: 1px solid rgba(255, 255, 255, 0.08);
    }

    .color-control strong {
      font-size: 0.92rem;
      letter-spacing: 0.03em;
    }

    .color-control small,
    .stat-card small {
      display: block;
      margin-top: 4px;
      color: var(--muted);
    }

    input[type="color"] {
      width: 60px;
      padding: 0;
      background: transparent;
      border: 1px solid rgba(255, 255, 255, 0.14);
    }

    .stats-grid {
      grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
      margin-top: 14px;
    }

    .stat-card strong {
      display: block;
      font-size: 1.24rem;
      margin-top: 4px;
    }

    .footer-grid {
      display: grid;
      grid-template-columns: minmax(0, 0.98fr) minmax(0, 1.02fr);
      gap: 18px;
    }

    .scene-grid {
      grid-template-columns: repeat(auto-fit, minmax(132px, 1fr));
      margin-top: 14px;
    }

    .dmx-grid {
      grid-template-columns: repeat(auto-fit, minmax(180px, 1fr));
      margin-top: 14px;
    }

    .dmx-slider {
      padding: 12px 14px;
      border-radius: 18px;
      background: rgba(255, 255, 255, 0.035);
      border: 1px solid rgba(255, 255, 255, 0.08);
    }

    .dmx-slider header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      margin-bottom: 10px;
      color: var(--muted);
      font-size: 0.78rem;
      text-transform: uppercase;
      letter-spacing: 0.12em;
    }

    code,
    pre,
    .mono {
      font-family: "SF Mono", "IBM Plex Mono", "Menlo", monospace;
    }

    .capsule {
      display: inline-flex;
      align-items: center;
      gap: 8px;
      min-height: 36px;
      padding: 8px 12px;
      border-radius: 999px;
      background: rgba(255, 255, 255, 0.055);
      border: 1px solid rgba(255, 255, 255, 0.09);
      color: var(--muted);
      font-size: 0.8rem;
      letter-spacing: 0.08em;
      text-transform: uppercase;
    }

    .capsule strong {
      color: var(--text);
      letter-spacing: 0.03em;
    }

    .notice {
      padding: 14px 16px;
      border-radius: 18px;
      background: rgba(255, 173, 77, 0.08);
      border: 1px solid rgba(255, 173, 77, 0.16);
      color: #ffd8a3;
      font-size: 0.92rem;
      line-height: 1.6;
      margin-top: 14px;
    }

    .sr-only {
      position: absolute;
      width: 1px;
      height: 1px;
      padding: 0;
      margin: -1px;
      overflow: hidden;
      clip: rect(0, 0, 0, 0);
      border: 0;
    }

    @media (max-width: 1120px) {
      .hero,
      .workspace,
      .controls,
      .footer-grid {
        grid-template-columns: 1fr;
      }
    }

    @media (max-width: 760px) {
      main {
        width: min(100%, calc(100% - 16px));
        padding-top: 16px;
      }

      .hero-copy,
      .status-card,
      .canvas-shell,
      .toolbar,
      .panel-body {
        padding-left: 16px;
        padding-right: 16px;
      }

      .panel-head {
        padding-left: 16px;
        padding-right: 16px;
      }

      h1 {
        font-size: 2.25rem;
      }
    }

    @media (prefers-reduced-motion: reduce) {
      button,
      .toggle {
        transition: none;
      }
    }
  </style>
</head>
<body>
  <main>
    <section class="hero">
      <article class="hero-card hero-copy">
        <p class="eyebrow">ESP32-C3 Laser Lab</p>
        <h1>Draw vector paths. Simulate scan travel. Learn what your projector is really doing.</h1>
        <p>
          This editor lets you sketch shapes, tune point dwell and travel speed, and watch a live scan preview that behaves like a laser head sweeping the frame.
          Large drawings take longer to complete. Tight drawings dwell brighter because the beam revisits the same space more often.
        </p>
        <div class="notice">
          The preview is physically inspired, but your current projector still speaks DMX only. That means this page can simulate arbitrary artwork now, while the actual projector
          can still only execute whichever built-in patterns its internal firmware exposes over DMX.
        </div>
      </article>

      <aside class="hero-card status-card">
        <div class="capsule"><span>Transport</span><strong class="mono" id="transportMode">Loading</strong></div>
        <div class="status-grid">
          <div class="metric">
            <div class="metric-label">Access Point</div>
            <strong id="wifiSsid">LaserShow-ESP32C3</strong>
          </div>
          <div class="metric">
            <div class="metric-label">Controller IP</div>
            <strong class="mono" id="wifiIp">192.168.4.1</strong>
          </div>
          <div class="metric">
            <div class="metric-label">DMX State</div>
            <strong id="dmxState">Waiting</strong>
          </div>
          <div class="metric">
            <div class="metric-label">Output</div>
            <strong id="blackoutState">Blackout</strong>
          </div>
        </div>
        <p class="microcopy" id="summaryText">
          Fetching projector status.
        </p>
      </aside>
    </section>

    <section class="workspace">
      <article class="panel">
        <div class="panel-head">
          <div>
            <h2>Design Editor</h2>
            <p>Click to place vertices, or switch on freehand mode and drag. Double-click or use Finish Path when a shape is done.</p>
          </div>
          <div class="capsule"><span>Mode</span><strong id="editorModeLabel">Plot</strong></div>
        </div>
        <div class="canvas-shell">
          <div class="canvas-wrap">
            <canvas id="editorCanvas" aria-label="Laser path editor"></canvas>
            <div class="canvas-note">
              <span>Tap to place points</span>
              <span id="pathStateLabel">0 paths</span>
            </div>
          </div>
        </div>
        <div class="toolbar">
          <div class="chip-row">
            <label class="toggle">
              <span>Freehand Draw</span>
              <input id="freehandMode" type="checkbox">
            </label>
            <label class="toggle">
              <span>Close Shape</span>
              <input id="closePath" type="checkbox" checked>
            </label>
            <div class="color-control">
              <div>
                <strong>Beam Color</strong>
                <small>Preview only unless your projector supports direct vector control.</small>
              </div>
              <input id="pathColor" type="color" value="#68dcff" aria-label="Beam color">
            </div>
          </div>
          <div class="button-row">
            <button id="finishPathBtn">Finish Path</button>
            <button id="undoBtn" class="ghost">Undo</button>
            <button id="clearBtn" class="warn">Clear Design</button>
            <button id="seedBtn" class="alt">Load Demo</button>
          </div>
          <div class="button-row">
            <button id="rectBtn">Rectangle</button>
            <button id="triangleBtn">Triangle</button>
            <button id="circleBtn">Circle</button>
            <button id="starBtn">Star</button>
            <button id="waveBtn">Wave</button>
          </div>
        </div>
      </article>

      <article class="panel">
        <div class="panel-head">
          <div>
            <h2>Scan Preview</h2>
            <p>The glow view accumulates repeated passes so you can see dwell density, blanked travel, and how long large designs take to complete.</p>
          </div>
          <div class="capsule"><span>Cycle</span><strong id="cycleLabel" class="mono">0.00 s</strong></div>
        </div>
        <div class="canvas-shell">
          <div class="canvas-wrap">
            <canvas id="previewCanvas" aria-label="Laser scan preview"></canvas>
            <div class="canvas-note">
              <span>Bright = lit beam</span>
              <span>Grey = blanked travel</span>
            </div>
          </div>
        </div>
        <div class="panel-body">
          <div class="stats-grid">
            <div class="stat-card">
              <small>Lit Path Length</small>
              <strong id="litLengthLabel">0.00</strong>
            </div>
            <div class="stat-card">
              <small>Blank Travel</small>
              <strong id="blankLengthLabel">0.00</strong>
            </div>
            <div class="stat-card">
              <small>Bounding Box</small>
              <strong id="boundsLabel">0% × 0%</strong>
            </div>
            <div class="stat-card">
              <small>Point Count</small>
              <strong id="pointCountLabel">0</strong>
            </div>
          </div>
        </div>
      </article>
    </section>

    <section class="controls">
      <article class="panel">
        <div class="panel-head">
          <div>
            <h3>Scan Behaviour</h3>
            <p>Tune the simulation. Faster beam speed reduces dwell per millimeter. Higher point dwell makes corners hotter and more visible.</p>
          </div>
        </div>
        <div class="panel-body">
          <div class="slider-grid">
            <div class="slider-card">
              <label for="scanSpeed"><span>Beam Speed</span><strong id="scanSpeedValue" class="mono">0.34</strong></label>
              <input id="scanSpeed" type="range" min="0.08" max="0.90" step="0.01" value="0.34">
            </div>
            <div class="slider-card">
              <label for="blankSpeed"><span>Blank Travel Speed</span><strong id="blankSpeedValue" class="mono">1.90×</strong></label>
              <input id="blankSpeed" type="range" min="1.00" max="4.00" step="0.05" value="1.90">
            </div>
            <div class="slider-card">
              <label for="pointDwell"><span>Point Dwell</span><strong id="pointDwellValue" class="mono">48 ms</strong></label>
              <input id="pointDwell" type="range" min="0" max="140" step="2" value="48">
            </div>
            <div class="slider-card">
              <label for="persistence"><span>Glow Persistence</span><strong id="persistenceValue" class="mono">0.18</strong></label>
              <input id="persistence" type="range" min="0.04" max="0.45" step="0.01" value="0.18">
            </div>
          </div>
        </div>
      </article>

      <article class="panel">
        <div class="panel-head">
          <div>
            <h3>Design Library</h3>
            <p>Designs are stored in your browser. Export and import uses JSON so you can tune artwork without reflashing the ESP32.</p>
          </div>
        </div>
        <div class="panel-body">
          <div class="button-row">
            <button id="saveLocalBtn" class="alt">Save Local</button>
            <button id="loadLocalBtn" class="ghost">Load Local</button>
            <button id="exportBtn">Export JSON</button>
            <button id="importBtn" class="ghost">Import JSON</button>
          </div>
          <p class="microcopy" id="designStatus">No saved design yet.</p>
        </div>
      </article>
    </section>

    <section class="footer-grid">
      <article class="panel">
        <div class="panel-head">
          <div>
            <h3>Projector Control</h3>
            <p>The DMX section still controls the real fixture. Use raw channels to discover your unit’s map while the simulator helps you think in vector paths.</p>
          </div>
        </div>
        <div class="panel-body">
          <div class="button-row">
            <button id="manualModeBtn">Manual DMX</button>
            <button id="autoplayBtn" class="alt">Scene Autoplay</button>
            <button id="blackoutBtn" class="warn">Toggle Blackout</button>
            <button id="applyManualBtn" class="ghost">Send DMX Slots</button>
          </div>
          <div class="scene-grid" id="sceneButtons"></div>
          <div class="dmx-grid" id="dmxSlots"></div>
        </div>
      </article>

      <article class="panel">
        <div class="panel-head">
          <div>
            <h3>Reality Check</h3>
            <p>The preview models galvo motion, but your current projector is still a DMX fixture, not an ILDA laser DAC endpoint.</p>
          </div>
        </div>
        <div class="panel-body">
          <div class="capsule"><span>Arbitrary Vector Output</span><strong id="vectorSupportLabel">No</strong></div>
          <p class="panel-copy">
            If you later add ILDA output or direct galvo and laser modulation control, this editor becomes useful as the front-end for actual artwork generation.
            For now it is the right place to design, preview timing, and understand scan geometry before you decide whether to modify the projector hardware.
          </p>
        </div>
      </article>
    </section>
  </main>

  <script>
    (() => {
      const SLOT_COUNT = 16;
      const STORAGE_KEY = 'laser-lab-design-autosave';
      const NAMED_STORAGE_KEY = 'laser-lab-design-saved';

      const editorCanvas = document.getElementById('editorCanvas');
      const previewCanvas = document.getElementById('previewCanvas');
      const editorCtx = editorCanvas.getContext('2d');
      const previewCtx = previewCanvas.getContext('2d');
      const glowCanvas = document.createElement('canvas');
      const glowCtx = glowCanvas.getContext('2d');

      const controls = {
        freehand: document.getElementById('freehandMode'),
        closePath: document.getElementById('closePath'),
        pathColor: document.getElementById('pathColor'),
        scanSpeed: document.getElementById('scanSpeed'),
        blankSpeed: document.getElementById('blankSpeed'),
        pointDwell: document.getElementById('pointDwell'),
        persistence: document.getElementById('persistence')
      };

      const labels = {
        editorMode: document.getElementById('editorModeLabel'),
        pathState: document.getElementById('pathStateLabel'),
        cycle: document.getElementById('cycleLabel'),
        litLength: document.getElementById('litLengthLabel'),
        blankLength: document.getElementById('blankLengthLabel'),
        bounds: document.getElementById('boundsLabel'),
        pointCount: document.getElementById('pointCountLabel'),
        summary: document.getElementById('summaryText'),
        wifiSsid: document.getElementById('wifiSsid'),
        wifiIp: document.getElementById('wifiIp'),
        dmxState: document.getElementById('dmxState'),
        blackout: document.getElementById('blackoutState'),
        transport: document.getElementById('transportMode'),
        vectorSupport: document.getElementById('vectorSupportLabel'),
        scanSpeed: document.getElementById('scanSpeedValue'),
        blankSpeed: document.getElementById('blankSpeedValue'),
        pointDwell: document.getElementById('pointDwellValue'),
        persistence: document.getElementById('persistenceValue'),
        designStatus: document.getElementById('designStatus')
      };

      const dmxSlotsHost = document.getElementById('dmxSlots');
      const sceneButtonsHost = document.getElementById('sceneButtons');

      const state = {
        latest: null,
        manualSlots: Array.from({ length: SLOT_COUNT }, () => 0),
        design: { paths: [] },
        activePath: null,
        drawing: false,
        playback: {
          data: emptyProgramData(),
          playMs: 0,
          lastTick: 0,
          previousPlayMs: 0,
          previousLitPos: null
        }
      };

      function emptyProgramData() {
        return {
          program: [],
          litSegments: [],
          blankSegments: [],
          cycleMs: 0,
          litLength: 0,
          blankLength: 0,
          bounds: null,
          pointCount: 0
        };
      }

      function clamp(value, min, max) {
        return Math.min(max, Math.max(min, value));
      }

      function distance(a, b) {
        return Math.hypot(b.x - a.x, b.y - a.y);
      }

      function lerp(a, b, t) {
        return {
          x: a.x + (b.x - a.x) * t,
          y: a.y + (b.y - a.y) * t
        };
      }

      function withAlpha(hex, alpha) {
        const clean = hex.replace('#', '');
        const value = clean.length === 3
          ? clean.split('').map((ch) => ch + ch).join('')
          : clean;
        const r = parseInt(value.slice(0, 2), 16);
        const g = parseInt(value.slice(2, 4), 16);
        const b = parseInt(value.slice(4, 6), 16);
        return 'rgba(' + r + ',' + g + ',' + b + ',' + alpha + ')';
      }

      function fitCanvas(canvas, ctx) {
        const rect = canvas.getBoundingClientRect();
        const dpr = window.devicePixelRatio || 1;
        const width = Math.max(1, Math.round(rect.width * dpr));
        const height = Math.max(1, Math.round(rect.height * dpr));
        if (canvas.width !== width || canvas.height !== height) {
          canvas.width = width;
          canvas.height = height;
          ctx.setTransform(1, 0, 0, 1, 0, 0);
          ctx.scale(dpr, dpr);
        }
      }

      function fitAllCanvases() {
        fitCanvas(editorCanvas, editorCtx);
        fitCanvas(previewCanvas, previewCtx);
        glowCanvas.width = previewCanvas.width;
        glowCanvas.height = previewCanvas.height;
        glowCtx.setTransform(1, 0, 0, 1, 0, 0);
        glowCtx.scale(window.devicePixelRatio || 1, window.devicePixelRatio || 1);
        glowCtx.clearRect(0, 0, previewCanvas.clientWidth, previewCanvas.clientHeight);
        renderEditor();
      }

      function eventPoint(evt, canvas) {
        const rect = canvas.getBoundingClientRect();
        return {
          x: clamp((evt.clientX - rect.left) / rect.width, 0, 1),
          y: clamp((evt.clientY - rect.top) / rect.height, 0, 1)
        };
      }

      function toCanvasPoint(point, canvas) {
        return {
          x: point.x * canvas.clientWidth,
          y: point.y * canvas.clientHeight
        };
      }

      function nextPlacementCenter() {
        const slots = [
          { x: 0.34, y: 0.34 },
          { x: 0.68, y: 0.34 },
          { x: 0.34, y: 0.68 },
          { x: 0.68, y: 0.68 },
          { x: 0.5, y: 0.5 }
        ];
        return slots[state.design.paths.length % slots.length];
      }

      function startActivePath() {
        state.activePath = {
          color: controls.pathColor.value,
          closed: controls.closePath.checked,
          points: []
        };
      }

      function addPoint(point) {
        if (!state.activePath) {
          startActivePath();
        }

        const points = state.activePath.points;
        if (!points.length || distance(points[points.length - 1], point) > 0.008) {
          points.push(point);
          state.activePath.color = controls.pathColor.value;
          state.activePath.closed = controls.closePath.checked;
          rebuildProgram();
          renderEditor();
          updateStats();
          persistAutosave();
        }
      }

      function commitActivePath() {
        if (!state.activePath) {
          return;
        }

        if (state.activePath.points.length > 1) {
          state.activePath.color = controls.pathColor.value;
          state.activePath.closed = controls.closePath.checked;
          state.design.paths.push({
            color: state.activePath.color,
            closed: state.activePath.closed,
            points: state.activePath.points.slice()
          });
        }

        state.activePath = null;
        rebuildProgram();
        renderEditor();
        updateStats();
        persistAutosave();
      }

      function getRenderPaths() {
        const paths = state.design.paths.slice();
        if (state.activePath && state.activePath.points.length > 0) {
          paths.push(state.activePath);
        }
        return paths;
      }

      function undoStep() {
        if (state.activePath && state.activePath.points.length) {
          state.activePath.points.pop();
          if (!state.activePath.points.length) {
            state.activePath = null;
          }
        } else {
          state.design.paths.pop();
        }
        rebuildProgram();
        renderEditor();
        updateStats();
        persistAutosave();
      }

      function clearDesign() {
        state.design = { paths: [] };
        state.activePath = null;
        state.playback.previousLitPos = null;
        glowCtx.clearRect(0, 0, previewCanvas.clientWidth, previewCanvas.clientHeight);
        rebuildProgram();
        renderEditor();
        updateStats();
        persistAutosave();
      }

      function addPreset(points, closed = true) {
        commitActivePath();
        state.design.paths.push({
          color: controls.pathColor.value,
          closed,
          points
        });
        rebuildProgram();
        renderEditor();
        updateStats();
        persistAutosave();
      }

      function makeRectangle() {
        const c = nextPlacementCenter();
        const hw = 0.16;
        const hh = 0.11;
        addPreset([
          { x: c.x - hw, y: c.y - hh },
          { x: c.x + hw, y: c.y - hh },
          { x: c.x + hw, y: c.y + hh },
          { x: c.x - hw, y: c.y + hh }
        ], true);
      }

      function makeTriangle() {
        const c = nextPlacementCenter();
        addPreset([
          { x: c.x, y: c.y - 0.17 },
          { x: c.x + 0.18, y: c.y + 0.13 },
          { x: c.x - 0.18, y: c.y + 0.13 }
        ], true);
      }

      function makeCircle() {
        const c = nextPlacementCenter();
        const radius = 0.18;
        const points = [];
        for (let i = 0; i < 28; i += 1) {
          const angle = (Math.PI * 2 * i) / 28;
          points.push({
            x: c.x + Math.cos(angle) * radius,
            y: c.y + Math.sin(angle) * radius
          });
        }
        addPreset(points, true);
      }

      function makeStar() {
        const c = nextPlacementCenter();
        const points = [];
        for (let i = 0; i < 10; i += 1) {
          const angle = -Math.PI / 2 + (Math.PI * 2 * i) / 10;
          const radius = i % 2 === 0 ? 0.18 : 0.075;
          points.push({
            x: c.x + Math.cos(angle) * radius,
            y: c.y + Math.sin(angle) * radius
          });
        }
        addPreset(points, true);
      }

      function makeWave() {
        const c = nextPlacementCenter();
        const points = [];
        for (let i = 0; i < 18; i += 1) {
          const t = i / 17;
          points.push({
            x: clamp(c.x - 0.22 + t * 0.44, 0.08, 0.92),
            y: clamp(c.y + Math.sin(t * Math.PI * 2) * 0.11, 0.08, 0.92)
          });
        }
        addPreset(points, false);
      }

      function seedDemo() {
        state.design = {
          paths: [
            {
              color: '#68dcff',
              closed: true,
              points: [
                { x: 0.25, y: 0.28 },
                { x: 0.42, y: 0.22 },
                { x: 0.56, y: 0.31 },
                { x: 0.49, y: 0.48 },
                { x: 0.31, y: 0.49 }
              ]
            },
            {
              color: '#ffb84d',
              closed: false,
              points: [
                { x: 0.61, y: 0.26 },
                { x: 0.78, y: 0.36 },
                { x: 0.68, y: 0.44 },
                { x: 0.82, y: 0.54 },
                { x: 0.62, y: 0.69 }
              ]
            },
            {
              color: '#ff5ea9',
              closed: true,
              points: [
                { x: 0.22, y: 0.68 },
                { x: 0.34, y: 0.58 },
                { x: 0.46, y: 0.68 },
                { x: 0.34, y: 0.82 }
              ]
            }
          ]
        };
        state.activePath = null;
        rebuildProgram();
        renderEditor();
        updateStats();
        persistAutosave();
      }

      function persistAutosave() {
        localStorage.setItem(STORAGE_KEY, JSON.stringify(state.design));
        labels.designStatus.textContent = 'Autosaved in this browser.';
      }

      function saveLocal() {
        commitActivePath();
        localStorage.setItem(NAMED_STORAGE_KEY, JSON.stringify(state.design));
        labels.designStatus.textContent = 'Saved to local browser storage.';
      }

      function loadLocal() {
        const raw = localStorage.getItem(NAMED_STORAGE_KEY) || localStorage.getItem(STORAGE_KEY);
        if (!raw) {
          labels.designStatus.textContent = 'Nothing stored yet.';
          return;
        }
        try {
          state.design = JSON.parse(raw);
          state.activePath = null;
          state.playback.previousLitPos = null;
          glowCtx.clearRect(0, 0, previewCanvas.clientWidth, previewCanvas.clientHeight);
          rebuildProgram();
          renderEditor();
          updateStats();
          labels.designStatus.textContent = 'Loaded from local browser storage.';
        } catch (error) {
          labels.designStatus.textContent = 'Stored design JSON is invalid.';
        }
      }

      function exportJson() {
        commitActivePath();
        window.prompt('Copy the design JSON below.', JSON.stringify(state.design));
      }

      function importJson() {
        const raw = window.prompt('Paste a design JSON object.');
        if (!raw) {
          return;
        }
        try {
          const parsed = JSON.parse(raw);
          if (!parsed || !Array.isArray(parsed.paths)) {
            throw new Error('Invalid design');
          }
          state.design = parsed;
          state.activePath = null;
          rebuildProgram();
          renderEditor();
          updateStats();
          persistAutosave();
          labels.designStatus.textContent = 'Imported JSON design.';
        } catch (error) {
          labels.designStatus.textContent = 'Import failed. JSON format was invalid.';
        }
      }

      function buildProgramData() {
        const paths = getRenderPaths().filter((path) => path.points.length > 0);
        const scanSpeed = Number(controls.scanSpeed.value);
        const blankSpeed = Number(controls.blankSpeed.value);
        const pointDwell = Number(controls.pointDwell.value);
        const data = emptyProgramData();

        let lastPoint = null;
        let minX = 1;
        let minY = 1;
        let maxX = 0;
        let maxY = 0;

        function includePoint(point) {
          minX = Math.min(minX, point.x);
          minY = Math.min(minY, point.y);
          maxX = Math.max(maxX, point.x);
          maxY = Math.max(maxY, point.y);
          data.pointCount += 1;
        }

        function addMove(from, to, lit, color, speedMultiplier) {
          const len = distance(from, to);
          if (len < 0.0005) {
            return;
          }
          const duration = Math.max(16, (len / (scanSpeed * speedMultiplier)) * 1000);
          const item = { type: 'move', from, to, lit, color, duration };
          data.program.push(item);
          if (lit) {
            data.litSegments.push(item);
            data.litLength += len;
          } else {
            data.blankSegments.push(item);
            data.blankLength += len;
          }
        }

        function addDwell(point, color) {
          if (pointDwell <= 0) {
            return;
          }
          data.program.push({ type: 'dwell', point, color, lit: true, duration: pointDwell });
        }

        paths.forEach((path) => {
          if (path.points.length < 2) {
            path.points.forEach(includePoint);
            return;
          }

          path.points.forEach(includePoint);
          const first = path.points[0];

          if (lastPoint) {
            addMove(lastPoint, first, false, '#9bbbd2', blankSpeed);
          }

          addDwell(first, path.color);

          for (let i = 1; i < path.points.length; i += 1) {
            addMove(path.points[i - 1], path.points[i], true, path.color, 1);
            addDwell(path.points[i], path.color);
          }

          if (path.closed && path.points.length > 2) {
            addMove(path.points[path.points.length - 1], first, true, path.color, 1);
            addDwell(first, path.color);
            lastPoint = first;
          } else {
            lastPoint = path.points[path.points.length - 1];
          }
        });

        data.cycleMs = data.program.reduce((total, item) => total + item.duration, 0);
        if (data.pointCount > 0) {
          data.bounds = {
            width: Math.max(0, maxX - minX),
            height: Math.max(0, maxY - minY)
          };
        }
        return data;
      }

      function rebuildProgram() {
        state.playback.data = buildProgramData();
        state.playback.playMs = 0;
        state.playback.previousPlayMs = 0;
        state.playback.previousLitPos = null;
        glowCtx.clearRect(0, 0, previewCanvas.clientWidth, previewCanvas.clientHeight);
      }

      function updateStats() {
        const data = state.playback.data;
        const pathCount = state.design.paths.length + (state.activePath && state.activePath.points.length ? 1 : 0);
        labels.editorMode.textContent = controls.freehand.checked ? 'Freehand' : 'Plot';
        labels.pathState.textContent = pathCount + ' path' + (pathCount === 1 ? '' : 's');
        labels.cycle.textContent = (data.cycleMs / 1000).toFixed(2) + ' s';
        labels.litLength.textContent = data.litLength.toFixed(2);
        labels.blankLength.textContent = data.blankLength.toFixed(2);
        labels.pointCount.textContent = String(data.pointCount);
        labels.bounds.textContent = data.bounds
          ? Math.round(data.bounds.width * 100) + '% × ' + Math.round(data.bounds.height * 100) + '%'
          : '0% × 0%';
        labels.scanSpeed.textContent = Number(controls.scanSpeed.value).toFixed(2);
        labels.blankSpeed.textContent = Number(controls.blankSpeed.value).toFixed(2) + '×';
        labels.pointDwell.textContent = Number(controls.pointDwell.value) + ' ms';
        labels.persistence.textContent = Number(controls.persistence.value).toFixed(2);
      }

      function sampleProgram(timeMs) {
        const data = state.playback.data;
        if (!data.program.length || data.cycleMs <= 0) {
          return null;
        }

        let cursor = timeMs % data.cycleMs;
        for (let i = 0; i < data.program.length; i += 1) {
          const item = data.program[i];
          if (cursor <= item.duration) {
            if (item.type === 'move') {
              const t = item.duration > 0 ? cursor / item.duration : 1;
              return {
                item,
                point: lerp(item.from, item.to, t),
                lit: item.lit,
                color: item.color
              };
            }
            return {
              item,
              point: item.point,
              lit: true,
              color: item.color
            };
          }
          cursor -= item.duration;
        }

        const last = data.program[data.program.length - 1];
        return last
          ? {
              item: last,
              point: last.type === 'move' ? last.to : last.point,
              lit: last.lit !== false,
              color: last.color
            }
          : null;
      }

      function drawGrid(ctx, width, height) {
        ctx.save();
        ctx.strokeStyle = 'rgba(170, 225, 255, 0.08)';
        ctx.lineWidth = 1;
        for (let i = 1; i < 10; i += 1) {
          const x = (width / 10) * i;
          const y = (height / 10) * i;
          ctx.beginPath();
          ctx.moveTo(x, 0);
          ctx.lineTo(x, height);
          ctx.stroke();
          ctx.beginPath();
          ctx.moveTo(0, y);
          ctx.lineTo(width, y);
          ctx.stroke();
        }
        ctx.restore();
      }

      function drawPath(ctx, path, options = {}) {
        if (!path.points.length) {
          return;
        }

        const points = path.points.map((point) => toCanvasPoint(point, ctx.canvas));
        ctx.save();
        ctx.strokeStyle = withAlpha(path.color, options.alpha || 0.9);
        ctx.fillStyle = withAlpha(path.color, 0.95);
        ctx.lineWidth = options.lineWidth || 2.2;
        ctx.lineJoin = 'round';
        ctx.lineCap = 'round';
        if (options.dashed) {
          ctx.setLineDash([8, 8]);
        }

        ctx.beginPath();
        ctx.moveTo(points[0].x, points[0].y);
        for (let i = 1; i < points.length; i += 1) {
          ctx.lineTo(points[i].x, points[i].y);
        }
        if (path.closed && points.length > 2) {
          ctx.closePath();
        }
        ctx.stroke();

        points.forEach((point, index) => {
          ctx.beginPath();
          ctx.arc(point.x, point.y, options.pointRadius || 3.3, 0, Math.PI * 2);
          ctx.fill();
          if (options.numbered) {
            ctx.fillStyle = 'rgba(240, 248, 255, 0.85)';
            ctx.font = '11px "SF Mono", monospace';
            ctx.fillText(String(index + 1), point.x + 6, point.y - 6);
            ctx.fillStyle = withAlpha(path.color, 0.95);
          }
        });
        ctx.restore();
      }

      function renderEditor() {
        fitCanvas(editorCanvas, editorCtx);
        const width = editorCanvas.clientWidth;
        const height = editorCanvas.clientHeight;
        editorCtx.clearRect(0, 0, width, height);

        const gradient = editorCtx.createLinearGradient(0, 0, width, height);
        gradient.addColorStop(0, 'rgba(10, 28, 42, 0.98)');
        gradient.addColorStop(1, 'rgba(4, 12, 20, 1)');
        editorCtx.fillStyle = gradient;
        editorCtx.fillRect(0, 0, width, height);

        drawGrid(editorCtx, width, height);

        editorCtx.save();
        editorCtx.strokeStyle = 'rgba(104, 220, 255, 0.22)';
        editorCtx.lineWidth = 2;
        editorCtx.strokeRect(width * 0.06, height * 0.06, width * 0.88, height * 0.88);
        editorCtx.restore();

        state.design.paths.forEach((path) => drawPath(editorCtx, path, { alpha: 0.88, pointRadius: 3.2 }));
        if (state.activePath && state.activePath.points.length) {
          drawPath(editorCtx, state.activePath, {
            alpha: 1,
            dashed: true,
            pointRadius: 4,
            numbered: true
          });
        }
      }

      function drawSegment(ctx, segment, strokeStyle, width, dashed) {
        const from = toCanvasPoint(segment.from, ctx.canvas);
        const to = toCanvasPoint(segment.to, ctx.canvas);
        ctx.save();
        ctx.strokeStyle = strokeStyle;
        ctx.lineWidth = width;
        ctx.lineCap = 'round';
        if (dashed) {
          ctx.setLineDash([6, 8]);
        }
        ctx.beginPath();
        ctx.moveTo(from.x, from.y);
        ctx.lineTo(to.x, to.y);
        ctx.stroke();
        ctx.restore();
      }

      function renderPreview(sample) {
        fitCanvas(previewCanvas, previewCtx);
        const width = previewCanvas.clientWidth;
        const height = previewCanvas.clientHeight;

        previewCtx.clearRect(0, 0, width, height);

        const gradient = previewCtx.createRadialGradient(width * 0.52, height * 0.48, 30, width * 0.5, height * 0.5, width * 0.78);
        gradient.addColorStop(0, 'rgba(17, 34, 48, 0.86)');
        gradient.addColorStop(1, 'rgba(2, 6, 12, 1)');
        previewCtx.fillStyle = gradient;
        previewCtx.fillRect(0, 0, width, height);

        drawGrid(previewCtx, width, height);

        const data = state.playback.data;
        data.blankSegments.forEach((segment) => drawSegment(previewCtx, segment, 'rgba(187, 212, 232, 0.16)', 1.2, true));
        data.litSegments.forEach((segment) => drawSegment(previewCtx, segment, withAlpha(segment.color, 0.16), 1.35, false));

        previewCtx.drawImage(glowCanvas, 0, 0, width, height);

        if (sample) {
          const point = toCanvasPoint(sample.point, previewCanvas);
          previewCtx.save();
          previewCtx.fillStyle = sample.lit ? withAlpha(sample.color, 0.98) : 'rgba(215, 235, 255, 0.72)';
          previewCtx.shadowColor = sample.lit ? withAlpha(sample.color, 0.95) : 'rgba(220, 240, 255, 0.5)';
          previewCtx.shadowBlur = sample.lit ? 18 : 6;
          previewCtx.beginPath();
          previewCtx.arc(point.x, point.y, sample.lit ? 5.5 : 4, 0, Math.PI * 2);
          previewCtx.fill();
          previewCtx.restore();
        }
      }

      function animate(now) {
        if (!state.playback.lastTick) {
          state.playback.lastTick = now;
        }

        const delta = now - state.playback.lastTick;
        state.playback.lastTick = now;
        const data = state.playback.data;

        if (data.cycleMs > 0) {
          state.playback.playMs = (state.playback.playMs + delta) % data.cycleMs;
          const wrapped = state.playback.playMs < state.playback.previousPlayMs;
          if (wrapped) {
            state.playback.previousLitPos = null;
          }

          const sample = sampleProgram(state.playback.playMs);

          const fade = clamp(Number(controls.persistence.value), 0.04, 0.45);
          glowCtx.fillStyle = 'rgba(2, 7, 13,' + fade + ')';
          glowCtx.fillRect(0, 0, previewCanvas.clientWidth, previewCanvas.clientHeight);

          if (sample) {
            const point = toCanvasPoint(sample.point, previewCanvas);
            if (sample.lit) {
              glowCtx.save();
              glowCtx.globalCompositeOperation = 'screen';
              glowCtx.strokeStyle = withAlpha(sample.color, 0.48);
              glowCtx.lineWidth = 2.8;
              glowCtx.lineCap = 'round';
              glowCtx.shadowBlur = 16;
              glowCtx.shadowColor = withAlpha(sample.color, 0.72);
              if (state.playback.previousLitPos) {
                glowCtx.beginPath();
                glowCtx.moveTo(state.playback.previousLitPos.x, state.playback.previousLitPos.y);
                glowCtx.lineTo(point.x, point.y);
                glowCtx.stroke();
              }
              glowCtx.beginPath();
              glowCtx.fillStyle = withAlpha(sample.color, 0.82);
              glowCtx.arc(point.x, point.y, 2.8, 0, Math.PI * 2);
              glowCtx.fill();
              glowCtx.restore();
              state.playback.previousLitPos = point;
            } else {
              state.playback.previousLitPos = null;
            }
          }

          state.playback.previousPlayMs = state.playback.playMs;
          renderPreview(sample);
        } else {
          glowCtx.clearRect(0, 0, previewCanvas.clientWidth, previewCanvas.clientHeight);
          renderPreview(null);
        }

        requestAnimationFrame(animate);
      }

      function buildDmxSlots(values) {
        dmxSlotsHost.innerHTML = values.map((value, index) => {
          return '' +
            '<div class="dmx-slider">' +
              '<header><span>CH ' + (index + 1) + '</span><strong class="mono" id="dmxVal' + (index + 1) + '">' + value + '</strong></header>' +
              '<input type="range" min="0" max="255" value="' + value + '" data-slot="' + index + '">' +
            '</div>';
        }).join('');

        dmxSlotsHost.querySelectorAll('input[type="range"]').forEach((input) => {
          input.addEventListener('input', (event) => {
            const index = Number(event.target.getAttribute('data-slot'));
            state.manualSlots[index] = Number(event.target.value);
            const label = document.getElementById('dmxVal' + (index + 1));
            if (label) {
              label.textContent = String(state.manualSlots[index]);
            }
          });
        });
      }

      function renderStatus(data) {
        state.latest = data;
        labels.transport.textContent = data.transport || 'DMX';
        labels.vectorSupport.textContent = data.arbitraryDraw ? 'Yes' : 'No';
        labels.wifiSsid.textContent = data.ssid || 'LaserShow-ESP32C3';
        labels.wifiIp.textContent = data.ip || '192.168.4.1';
        labels.dmxState.textContent = data.dmxReady ? 'Online' : 'Unavailable';
        labels.blackout.textContent = data.blackout ? 'Blackout' : 'Live';
        labels.summary.textContent = data.arbitraryDraw
          ? 'This hardware exposes direct vector output.'
          : 'Preview draws custom vectors in the browser. The projector still uses built-in DMX patterns.';

        if (Array.isArray(data.slots) && data.slots.length === SLOT_COUNT) {
          state.manualSlots = data.slots.slice();
          buildDmxSlots(state.manualSlots);
        }

        if (Array.isArray(data.sceneNames)) {
          sceneButtonsHost.innerHTML = data.sceneNames.map((name, index) => {
            return '<button class="' + (data.selectedScene === index ? 'alt' : 'ghost') + '" data-scene="' + index + '">' + name + '</button>';
          }).join('');

          sceneButtonsHost.querySelectorAll('button').forEach((button) => {
            button.addEventListener('click', () => {
              callApi('/api/scene?index=' + button.getAttribute('data-scene'));
            });
          });
        }
      }

      async function refreshStatus() {
        try {
          const response = await fetch('/api/status', { cache: 'no-store' });
          const data = await response.json();
          renderStatus(data);
        } catch (error) {
          labels.summary.textContent = 'ESP32 status fetch failed. The editor still works locally in this browser.';
        }
      }

      async function callApi(path) {
        await fetch(path, { cache: 'no-store' });
        await refreshStatus();
      }

      function applyManualDmx() {
        const params = new URLSearchParams();
        state.manualSlots.forEach((value, index) => params.set('ch' + (index + 1), value));
        callApi('/api/manual?' + params.toString());
      }

      function bindUi() {
        controls.freehand.addEventListener('change', () => {
          labels.editorMode.textContent = controls.freehand.checked ? 'Freehand' : 'Plot';
        });

        [controls.scanSpeed, controls.blankSpeed, controls.pointDwell, controls.persistence].forEach((input) => {
          input.addEventListener('input', () => {
            rebuildProgram();
            updateStats();
          });
        });

        document.getElementById('finishPathBtn').addEventListener('click', commitActivePath);
        document.getElementById('undoBtn').addEventListener('click', undoStep);
        document.getElementById('clearBtn').addEventListener('click', clearDesign);
        document.getElementById('seedBtn').addEventListener('click', seedDemo);
        document.getElementById('rectBtn').addEventListener('click', makeRectangle);
        document.getElementById('triangleBtn').addEventListener('click', makeTriangle);
        document.getElementById('circleBtn').addEventListener('click', makeCircle);
        document.getElementById('starBtn').addEventListener('click', makeStar);
        document.getElementById('waveBtn').addEventListener('click', makeWave);
        document.getElementById('saveLocalBtn').addEventListener('click', saveLocal);
        document.getElementById('loadLocalBtn').addEventListener('click', loadLocal);
        document.getElementById('exportBtn').addEventListener('click', exportJson);
        document.getElementById('importBtn').addEventListener('click', importJson);
        document.getElementById('manualModeBtn').addEventListener('click', () => callApi('/api/mode?value=manual'));
        document.getElementById('autoplayBtn').addEventListener('click', () => callApi('/api/mode?value=autoplay'));
        document.getElementById('blackoutBtn').addEventListener('click', () => {
          const next = !(state.latest && state.latest.blackout);
          callApi('/api/blackout?enabled=' + (next ? '1' : '0'));
        });
        document.getElementById('applyManualBtn').addEventListener('click', applyManualDmx);

        editorCanvas.addEventListener('pointerdown', (event) => {
          if (event.button !== 0) {
            return;
          }

          const point = eventPoint(event, editorCanvas);
          if (controls.freehand.checked) {
            state.drawing = true;
            editorCanvas.setPointerCapture(event.pointerId);
            addPoint(point);
          } else {
            addPoint(point);
          }
        });

        editorCanvas.addEventListener('pointermove', (event) => {
          if (!state.drawing) {
            return;
          }
          addPoint(eventPoint(event, editorCanvas));
        });

        function finishPointerDraw() {
          if (!state.drawing) {
            return;
          }
          state.drawing = false;
          commitActivePath();
        }

        editorCanvas.addEventListener('pointerup', finishPointerDraw);
        editorCanvas.addEventListener('pointercancel', finishPointerDraw);
        editorCanvas.addEventListener('dblclick', () => {
          if (!controls.freehand.checked) {
            commitActivePath();
          }
        });

        window.addEventListener('resize', fitAllCanvases);
      }

      function boot() {
        bindUi();
        fitAllCanvases();
        loadLocal();
        if (!state.design.paths.length) {
          seedDemo();
        }
        updateStats();
        refreshStatus();
        setInterval(refreshStatus, 2500);
        requestAnimationFrame(animate);
      }

      boot();
    })();
  </script>
</body>
</html>
)LASER";
