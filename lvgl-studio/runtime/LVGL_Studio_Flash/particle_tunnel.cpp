#include "particle_tunnel.h"

#include "rgb_lcd_port.h"

#include <Arduino.h>
#include <cmath>
#include <cstdlib>
#include <esp_random.h>

namespace {

constexpr int kMaxParticles = 170;
constexpr float kNearCm = 15.f;
constexpr float kFarCm = 120.f;

struct Particle {
  float x, y, z;
  float vx, vy, vz;
  float life;
  float maxLife;
  float size;
};

static lv_obj_t *s_canvas;
static lv_color_t *s_buf;
static Particle s_parts[kMaxParticles];
static int s_count;
static bool s_inited;
static uint32_t s_last_wall_ms;

static inline float clampf(float v, float lo, float hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

static inline float lerpf(float a, float b, float t) { return a + (b - a) * t; }

static float get_closeness(bool distance_ok, float distance_cm) {
  if (!distance_ok || isnan(distance_cm)) return 0.16f;
  if (distance_cm <= kNearCm) return 1.f;
  if (distance_cm >= kFarCm) return 0.f;
  return 1.f - (distance_cm - kNearCm) / (kFarCm - kNearCm);
}

static Particle create_particle(int w, int h, float speed_value, float closeness) {
  Particle p{};
  float speed_factor = fmaxf(0.2f, 0.35f + speed_value / 120.f);
  float spread = lerpf(0.82f, 0.36f, closeness);
  float depth = lerpf(980.f, 380.f, closeness);
  float x_origin = (float)w * 0.5f + ((float)random(0, 10001) / 10000.f - 0.5f) * (float)w * spread;
  float y_origin = (float)h * 0.5f + ((float)random(0, 10001) / 10000.f - 0.5f) * (float)h * spread;
  p.x = x_origin;
  p.y = y_origin;
  p.z = 30.f + ((float)random(0, 10001) / 10000.f) * depth;
  p.vx = (((float)random(0, 10001) / 10000.f) - 0.5f) * lerpf(0.9f, 1.8f, closeness) * speed_factor;
  p.vy = (((float)random(0, 10001) / 10000.f) - 0.5f) * lerpf(0.6f, 1.3f, closeness) * speed_factor;
  p.vz = -(((float)random(0, 10001) / 10000.f) * 2.2f + 1.2f + closeness * 2.1f) * speed_factor;
  p.life = 0.f;
  p.maxLife = 65.f + ((float)random(0, 10001) / 10000.f) * 130.f;
  p.size = ((float)random(0, 10001) / 10000.f) * (1.4f + closeness * 2.1f) + 0.6f;
  return p;
}

static void draw_filled_circle(lv_obj_t *cv, int cx, int cy, int r, lv_color_t col, lv_opa_t opa) {
  if (r < 1) r = 1;
  lv_draw_rect_dsc_t dsc;
  lv_draw_rect_dsc_init(&dsc);
  dsc.bg_color = col;
  dsc.bg_opa = opa;
  dsc.radius = LV_RADIUS_CIRCLE;
  dsc.border_width = 0;
  lv_canvas_draw_rect(cv, cx - r, cy - r, r * 2, r * 2, &dsc);
}

static void draw_grid(lv_obj_t *cv, int w, int h, float closeness, float speed_norm, uint32_t wall_ms) {
  float grid_spacing = lerpf(82.f, 60.f, closeness);
  float t = (float)wall_ms * 0.018f * (1.f + speed_norm + closeness * 0.6f);
  float grid_offset = fmodf(t, grid_spacing);
  if (grid_offset < 0.f) grid_offset += grid_spacing;

  lv_opa_t opa = (lv_opa_t)clampf(4.f + closeness * 12.f, 4.f, 40.f);
  lv_color_t c = lv_color_make(
      (uint8_t)lerpf(98.f, 255.f, closeness),
      (uint8_t)lerpf(212.f, 176.f, closeness),
      (uint8_t)lerpf(255.f, 122.f, closeness));

  lv_draw_line_dsc_t dsc;
  lv_draw_line_dsc_init(&dsc);
  dsc.color = c;
  dsc.width = 1;
  dsc.opa = opa;

  for (float x = -grid_spacing + grid_offset; x < (float)w + grid_spacing; x += grid_spacing) {
    lv_point_t seg[] = {{(lv_coord_t)x, 0}, {(lv_coord_t)x, (lv_coord_t)h}};
    lv_canvas_draw_line(cv, seg, 2, &dsc);
  }
  for (float y = -grid_spacing + grid_offset; y < (float)h + grid_spacing; y += grid_spacing) {
    lv_point_t seg[] = {{0, (lv_coord_t)y}, {(lv_coord_t)w, (lv_coord_t)y}};
    lv_canvas_draw_line(cv, seg, 2, &dsc);
  }
}

} // namespace

void particle_tunnel_init(lv_obj_t *scr) {
  if (s_inited || !scr) return;
  const int w = EXAMPLE_LCD_H_RES;
  const int h = EXAMPLE_LCD_V_RES;

  s_buf = (lv_color_t *)heap_caps_malloc((size_t)w * (size_t)h * sizeof(lv_color_t),
                                          MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  if (!s_buf) {
    return;
  }

  s_canvas = lv_canvas_create(scr);
  lv_obj_set_size(s_canvas, w, h);
  lv_obj_align(s_canvas, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_clear_flag(s_canvas, LV_OBJ_FLAG_SCROLLABLE);
  lv_canvas_set_buffer(s_canvas, s_buf, w, h, LV_IMG_CF_TRUE_COLOR);
  lv_canvas_fill_bg(s_canvas, lv_color_hex(0x04070b), LV_OPA_COVER);

  randomSeed((uint32_t)esp_random());
  s_count = 0;
  while (s_count < 80) {
    s_parts[s_count] = create_particle(w, h, 40.f, 0.16f);
    s_parts[s_count].life = ((float)random(0, 10001) / 10000.f) * s_parts[s_count].maxLife;
    s_count++;
  }

  s_inited = true;
  s_last_wall_ms = 0;
}

void particle_tunnel_step(float speed_mph, float distance_cm, bool distance_ok, uint32_t wall_ms) {
  if (!s_inited || !s_canvas) return;

  const int w = EXAMPLE_LCD_H_RES;
  const int h = EXAMPLE_LCD_V_RES;

  float dt = 1.f;
  if (s_last_wall_ms != 0) {
    dt = fminf(32.f, (float)(wall_ms - s_last_wall_ms)) / 16.f;
  }
  s_last_wall_ms = wall_ms;

  float speed_norm = clampf(speed_mph / 180.f, 0.f, 1.f);
  float closeness = get_closeness(distance_ok, distance_cm);
  int target = (int)fminf(220.f, 56.f + floorf(speed_mph * 0.40f + closeness * 75.f));
  target = (int)clampf((float)target, 24.f, (float)kMaxParticles);

  while (s_count < target && s_count < kMaxParticles) {
    s_parts[s_count++] = create_particle(w, h, speed_mph, closeness);
  }
  while (s_count > target) {
    s_count--;
  }

  uint8_t bg_r = (uint8_t)clampf(4.f + closeness * 8.f, 0.f, 32.f);
  uint8_t bg_g = (uint8_t)clampf(7.f + closeness * 10.f, 0.f, 40.f);
  uint8_t bg_b = (uint8_t)clampf(11.f + closeness * 14.f, 0.f, 48.f);
  lv_canvas_fill_bg(s_canvas, lv_color_make(bg_r, bg_g, bg_b), LV_OPA_COVER);

  for (int i = 0; i < s_count; i++) {
    Particle &p = s_parts[i];
    p.x += p.vx * dt;
    p.y += p.vy * dt;
    p.z += p.vz * dt;
    p.life += dt;

    if (p.life >= p.maxLife || p.z <= 0.f || p.x < -20.f || p.x > (float)w + 20.f || p.y < -20.f ||
        p.y > (float)h + 20.f) {
      p = create_particle(w, h, speed_mph, closeness);
      continue;
    }

    float perspective = 600.f / (600.f + p.z);
    float screen_x = (p.x - (float)w * 0.5f) * perspective + (float)w * 0.5f;
    float screen_y = (p.y - (float)h * 0.5f) * perspective + (float)h * 0.5f;
    float screen_size = p.size * perspective;

    float life_ratio = p.life / p.maxLife;
    float alpha = 1.f;
    if (life_ratio < 0.1f) alpha = life_ratio * 10.f;
    else if (life_ratio > 0.8f) alpha = (1.f - life_ratio) * 5.f;

    int r = (int)lroundf(lerpf(92.f, 255.f, closeness));
    int g = (int)lroundf(lerpf(216.f, 158.f, closeness) - speed_norm * 18.f);
    int b = (int)lroundf(lerpf(255.f, 82.f, closeness) - speed_norm * 10.f);
    lv_color_t col = lv_color_make((uint8_t)clampf((float)r, 0.f, 255.f), (uint8_t)clampf((float)g, 0.f, 255.f),
                                     (uint8_t)clampf((float)b, 0.f, 255.f));

    float depth_alpha = fmaxf(0.12f, 1.f - p.z / 1000.f);
    lv_opa_t opa =
        (lv_opa_t)clampf(alpha * depth_alpha * (0.42f + closeness * 0.32f) * 255.f, 10.f, 255.f);

    int ir = (int)lroundf(screen_size);
    if (ir < 1) ir = 1;
    draw_filled_circle(s_canvas, (int)lroundf(screen_x), (int)lroundf(screen_y), ir, col, opa);

    if (p.z < 320.f && screen_size > 0.8f) {
      lv_opa_t halo_opa =
          (lv_opa_t)clampf(alpha * depth_alpha * (0.08f + closeness * 0.08f) * 255.f, 4.f, 80.f);
      draw_filled_circle(s_canvas, (int)lroundf(screen_x), (int)lroundf(screen_y), (int)lroundf(screen_size * 3.f),
                         col, halo_opa);
    }
  }

  draw_grid(s_canvas, w, h, closeness, speed_norm, wall_ms);

  lv_obj_invalidate(s_canvas);
}
