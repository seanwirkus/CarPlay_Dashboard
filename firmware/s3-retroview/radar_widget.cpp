#include "radar_widget.h"
#include "gui_paint.h"
#include "navigation_dashboard.h"
#include <math.h>

// Paint_DrawCircle is full circle. Approximate upward-opening arc by drawing
// many short line segments — each ring at a different radius, colored by zone.
static void draw_arc(int cx, int cy, int radius, uint16_t color, int dotPx) {
    const float startA = -M_PI;   // 180°
    const float endA   = 0.0f;    // 0°
    const int steps    = 64;
    float prevX = cx + radius * cosf(startA);
    float prevY = cy + radius * sinf(startA);
    for (int i = 1; i <= steps; i++) {
        float t = (float)i / steps;
        float a = startA + (endA - startA) * t;
        float x = cx + radius * cosf(a);
        float y = cy + radius * sinf(a);
        Paint_DrawLine((UWORD)prevX, (UWORD)prevY, (UWORD)x, (UWORD)y,
                       color, (DOT_PIXEL)dotPx, LINE_STYLE_SOLID);
        prevX = x;
        prevY = y;
    }
}

void radar_draw(int cx, int cy, int maxRadius, float distanceCm) {
    const bool haveData = !isnan(distanceCm) && distanceCm > 0.0f && distanceCm < 400.0f;

    // Four ring radii — outer to inner
    const int r4 = maxRadius;
    const int r3 = (int)(maxRadius * 0.75f);
    const int r2 = (int)(maxRadius * 0.50f);
    const int r1 = (int)(maxRadius * 0.28f);

    // Zone colors: far (green) → mid (amber) → near (red). Outline gray if no data.
    const uint16_t outline = NAV_TEXT_GRAY;

    if (!haveData) {
        draw_arc(cx, cy, r4, outline, DOT_PIXEL_1X1);
        draw_arc(cx, cy, r3, outline, DOT_PIXEL_1X1);
        draw_arc(cx, cy, r2, outline, DOT_PIXEL_1X1);
        draw_arc(cx, cy, r1, outline, DOT_PIXEL_1X1);
    } else {
        // d > 100: only outer arc lit
        // 60..100: outer + r3
        // 30..60:  outer + r3 + r2
        // <30:     all four, near = red
        const float d = distanceCm;
        const bool litR3 = (d <= 100.0f);
        const bool litR2 = (d <= 60.0f);
        const bool litR1 = (d <= 30.0f);

        draw_arc(cx, cy, r4, NAV_RADAR_FAR,  DOT_PIXEL_2X2);
        draw_arc(cx, cy, r3, litR3 ? NAV_RADAR_MID  : outline, litR3 ? DOT_PIXEL_2X2 : DOT_PIXEL_1X1);
        draw_arc(cx, cy, r2, litR2 ? NAV_RADAR_NEAR : outline, litR2 ? DOT_PIXEL_3X3 : DOT_PIXEL_1X1);
        draw_arc(cx, cy, r1, litR1 ? NAV_RADAR_NEAR : outline, litR1 ? DOT_PIXEL_3X3 : DOT_PIXEL_1X1);
    }

    // Vehicle marker: rounded rectangle at center, headlight chevron points up
    const int carW = 60;
    const int carH = 80;
    Paint_DrawRectangle(cx - carW/2, cy - carH/2, cx + carW/2, cy + carH/2,
                        NAV_ACCENT_COLOR, DOT_PIXEL_2X2, DRAW_FILL_FULL);
    // Headlights triangle pointing up
    Paint_DrawLine(cx - carW/2, cy - carH/2, cx, cy - carH/2 - 18, WHITE, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
    Paint_DrawLine(cx + carW/2, cy - carH/2, cx, cy - carH/2 - 18, WHITE, DOT_PIXEL_2X2, LINE_STYLE_SOLID);

    // Distance readout below vehicle
    char buf[16];
    if (haveData) {
        snprintf(buf, sizeof(buf), "%.0f cm", distanceCm);
    } else {
        snprintf(buf, sizeof(buf), "-- cm");
    }
    Paint_DrawString_EN(cx - 48, cy + maxRadius + 12, buf, &Font24,
                        NAV_TEXT_WHITE, NAV_BACKGROUND_COLOR);
}
