#include "waveshare_7b_lcd.h"

#include <string.h>

#include "esp_heap_caps.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_log.h"

#include "waveshare_7b_rgb_hw.h"
#include "waveshare_io_extension.h"
#include "waveshare_i2c.h"

static const char *TAG = "ws7b_lcd";

static esp_lcd_panel_handle_t s_panel = nullptr;

static IRAM_ATTR bool rgb_vsync_noop(esp_lcd_panel_handle_t panel,
                                     const esp_lcd_rgb_panel_event_data_t *edata,
                                     void *user_ctx)
{
    (void)panel;
    (void)edata;
    (void)user_ctx;
    return false;
}

static void ws7b_backlight_on()
{
    IO_EXTENSION_Output(IO_EXTENSION_IO_2, 1);
}

bool waveshare_7b_display_begin()
{
    DEV_I2C_Init();
    IO_EXTENSION_Init();

    ESP_LOGI(TAG, "Install RGB LCD panel driver");

    esp_lcd_rgb_panel_config_t panel_config = {
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .timings = {
            .pclk_hz = WS7B_LCD_PIXEL_CLOCK_HZ,
            .h_res = WS7B_LCD_H_RES,
            .v_res = WS7B_LCD_V_RES,
            .hsync_pulse_width = 162,
            .hsync_back_porch = 152,
            .hsync_front_porch = 48,
            .vsync_pulse_width = 45,
            .vsync_back_porch = 13,
            .vsync_front_porch = 3,
            .flags = {
                .pclk_active_neg = 1,
            },
        },
        .data_width = WS7B_RGB_DATA_WIDTH,
        .bits_per_pixel = WS7B_RGB_BIT_PER_PIXEL,
        .num_fbs = WS7B_LCD_RGB_BUFFER_NUMS,
        .bounce_buffer_size_px = WS7B_RGB_BOUNCE_BUFFER_SIZE,
        .sram_trans_align = 4,
        .psram_trans_align = 64,
        .hsync_gpio_num = WS7B_LCD_IO_RGB_HSYNC,
        .vsync_gpio_num = WS7B_LCD_IO_RGB_VSYNC,
        .de_gpio_num = WS7B_LCD_IO_RGB_DE,
        .pclk_gpio_num = WS7B_LCD_IO_RGB_PCLK,
        .disp_gpio_num = WS7B_LCD_IO_RGB_DISP,
        .data_gpio_nums = {
            WS7B_LCD_IO_RGB_DATA0,
            WS7B_LCD_IO_RGB_DATA1,
            WS7B_LCD_IO_RGB_DATA2,
            WS7B_LCD_IO_RGB_DATA3,
            WS7B_LCD_IO_RGB_DATA4,
            WS7B_LCD_IO_RGB_DATA5,
            WS7B_LCD_IO_RGB_DATA6,
            WS7B_LCD_IO_RGB_DATA7,
            WS7B_LCD_IO_RGB_DATA8,
            WS7B_LCD_IO_RGB_DATA9,
            WS7B_LCD_IO_RGB_DATA10,
            WS7B_LCD_IO_RGB_DATA11,
            WS7B_LCD_IO_RGB_DATA12,
            WS7B_LCD_IO_RGB_DATA13,
            WS7B_LCD_IO_RGB_DATA14,
            WS7B_LCD_IO_RGB_DATA15,
        },
        .flags = {
            .fb_in_psram = 1,
        },
    };

    esp_err_t err = esp_lcd_new_rgb_panel(&panel_config, &s_panel);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_lcd_new_rgb_panel failed: %d", (int)err);
        return false;
    }

    ESP_ERROR_CHECK(esp_lcd_panel_init(s_panel));

    esp_lcd_rgb_panel_event_callbacks_t cbs = {
#if WS7B_RGB_BOUNCE_BUFFER_SIZE > 0
        .on_bounce_frame_finish = rgb_vsync_noop,
#else
        .on_vsync = rgb_vsync_noop,
#endif
    };
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_register_event_callbacks(s_panel, &cbs, nullptr));

    ws7b_backlight_on();
    ESP_LOGI(TAG, "RGB LCD ready, backlight on");
    return true;
}

static void fb_fill_rect(uint16_t *fb, int x0, int y0, int x1, int y1, uint16_t c)
{
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > WS7B_LCD_H_RES) x1 = WS7B_LCD_H_RES;
    if (y1 > WS7B_LCD_V_RES) y1 = WS7B_LCD_V_RES;
    for (int y = y0; y < y1; ++y) {
        uint16_t *row = fb + y * WS7B_LCD_H_RES;
        for (int x = x0; x < x1; ++x) row[x] = c;
    }
}

// Classic 7-seg patterns (abcdefg), a=bit0 .. g=bit6
static const uint8_t kSegDigits[10] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F,
};

static void draw_seg_h(uint16_t *fb, int x, int y, int len, int t, uint16_t c)
{
    fb_fill_rect(fb, x, y, x + len, y + t, c);
}

static void draw_seg_v(uint16_t *fb, int x, int y, int len, int t, uint16_t c)
{
    fb_fill_rect(fb, x, y, x + t, y + len, c);
}

static void draw_seven_digit(uint16_t *fb, int ox, int oy, int d, uint16_t on, uint16_t off, int w, int h, int t)
{
    (void)off;
    if (d < 0 || d > 9) return;
    const uint8_t pat = kSegDigits[d];
    const int m = t;  // margin from digit box
    // a — top
    if (pat & 0x01) draw_seg_h(fb, ox + m, oy, w - 2 * m, t, on);
    // b — upper right
    if (pat & 0x02) draw_seg_v(fb, ox + w - t, oy + m, (h / 2) - 2 * m, t, on);
    // c — lower right
    if (pat & 0x04) draw_seg_v(fb, ox + w - t, oy + h / 2 + m, (h / 2) - 2 * m, t, on);
    // d — bottom
    if (pat & 0x08) draw_seg_h(fb, ox + m, oy + h - t, w - 2 * m, t, on);
    // e — lower left
    if (pat & 0x10) draw_seg_v(fb, ox, oy + h / 2 + m, (h / 2) - 2 * m, t, on);
    // f — upper left
    if (pat & 0x20) draw_seg_v(fb, ox, oy + m, (h / 2) - 2 * m, t, on);
    // g — middle
    if (pat & 0x40) draw_seg_h(fb, ox + m, oy + h / 2 - t / 2, w - 2 * m, t, on);
}

static void draw_ip_seven(uint16_t *fb, const char *ip, int cy, uint16_t on, uint16_t bg)
{
    (void)bg;
    int digitW = 56;
    int digitH = 88;
    int thick = 10;
    int gap = 10;
    int dotR = 6;

    int len = (int)strlen(ip);
    int totalW = 0;
    for (int i = 0; i < len; ++i) {
        char ch = ip[i];
        if (ch >= '0' && ch <= '9') totalW += digitW + gap;
        else if (ch == '.') totalW += dotR * 2 + gap;
        else totalW += gap;
    }
    int startX = (WS7B_LCD_H_RES - totalW) / 2;
    int x = startX;
    for (int i = 0; i < len; ++i) {
        char ch = ip[i];
        if (ch >= '0' && ch <= '9') {
            draw_seven_digit(fb, x, cy, ch - '0', on, 0, digitW, digitH, thick);
            x += digitW + gap;
        } else if (ch == '.') {
            int cx = x + dotR;
            int cyy = cy + digitH - dotR * 3;
            fb_fill_rect(fb, cx - dotR, cyy - dotR, cx + dotR, cyy + dotR, on);
            x += dotR * 2 + gap;
        } else {
            x += gap;
        }
    }
}

// Minimal 5×7 font (rows top→bottom, bits = columns MSB=left pixel)
static void draw_glyph57(uint16_t *fb, int x, int y, char ch, uint16_t fg, int scale)
{
    uint8_t cols[7] = {0};
    if (ch == ' ') {
        return;
    }
    if (ch >= 'a' && ch <= 'z') {
        ch = (char)(ch - 'a' + 'A');
    }
    if (ch >= '0' && ch <= '9') {
        static const uint8_t n[][7] = {
            {0x3E,0x51,0x49,0x45,0x3E,0x00,0x00}, //0
            {0x00,0x42,0x7F,0x40,0x00,0x00,0x00}, //1
            {0x42,0x61,0x51,0x49,0x46,0x00,0x00}, //2
            {0x21,0x41,0x45,0x4B,0x31,0x00,0x00}, //3
            {0x18,0x14,0x12,0x7F,0x10,0x00,0x00}, //4
            {0x27,0x45,0x45,0x45,0x39,0x00,0x00}, //5
            {0x3C,0x4A,0x49,0x49,0x30,0x00,0x00}, //6
            {0x01,0x71,0x09,0x05,0x03,0x00,0x00}, //7
            {0x36,0x49,0x49,0x49,0x36,0x00,0x00}, //8
            {0x06,0x49,0x49,0x29,0x1E,0x00,0x00}, //9
        };
        memcpy(cols, n[ch - '0'], 7);
    } else if (ch >= 'A' && ch <= 'Z') {
        static const uint8_t A[] = {0x7E,0x11,0x11,0x11,0x7E,0x00,0x00};
        static const uint8_t B[] = {0x7F,0x49,0x49,0x49,0x36,0x00,0x00};
        static const uint8_t C[] = {0x3E,0x41,0x41,0x41,0x22,0x00,0x00};
        static const uint8_t D[] = {0x7F,0x41,0x41,0x22,0x1C,0x00,0x00};
        static const uint8_t E[] = {0x7F,0x49,0x49,0x49,0x41,0x00,0x00};
        static const uint8_t F[] = {0x7F,0x09,0x09,0x09,0x01,0x00,0x00};
        static const uint8_t G[] = {0x3E,0x41,0x49,0x49,0x7A,0x00,0x00};
        static const uint8_t H[] = {0x7F,0x08,0x08,0x08,0x7F,0x00,0x00};
        static const uint8_t I[] = {0x00,0x41,0x7F,0x41,0x00,0x00,0x00};
        static const uint8_t L[] = {0x7F,0x40,0x40,0x40,0x40,0x00,0x00};
        static const uint8_t N[] = {0x7F,0x02,0x04,0x08,0x7F,0x00,0x00};
        static const uint8_t O[] = {0x3E,0x41,0x41,0x41,0x3E,0x00,0x00};
        static const uint8_t P[] = {0x7F,0x09,0x09,0x09,0x06,0x00,0x00};
        static const uint8_t K[] = {0x7F,0x08,0x14,0x22,0x41,0x00,0x00};
        static const uint8_t R[] = {0x7F,0x09,0x19,0x29,0x46,0x00,0x00};
        static const uint8_t S[] = {0x46,0x49,0x49,0x49,0x31,0x00,0x00};
        static const uint8_t T[] = {0x01,0x01,0x7F,0x01,0x01,0x00,0x00};
        static const uint8_t U[] = {0x3F,0x40,0x40,0x40,0x3F,0x00,0x00};
        static const uint8_t W[] = {0x1F,0x20,0x40,0x20,0x1F,0x00,0x00};
        static const uint8_t Y[] = {0x07,0x08,0x70,0x08,0x07,0x00,0x00};
        const uint8_t *p = A;
        switch (ch) {
            case 'A': p = A; break;
            case 'B': p = B; break;
            case 'C': p = C; break;
            case 'D': p = D; break;
            case 'E': p = E; break;
            case 'F': p = F; break;
            case 'G': p = G; break;
            case 'H': p = H; break;
            case 'I': p = I; break;
            case 'K': p = K; break;
            case 'L': p = L; break;
            case 'N': p = N; break;
            case 'O': p = O; break;
            case 'P': p = P; break;
            case 'R': p = R; break;
            case 'S': p = S; break;
            case 'T': p = T; break;
            case 'U': p = U; break;
            case 'W': p = W; break;
            case 'Y': p = Y; break;
            default: p = A; break;
        }
        memcpy(cols, p, 7);
    } else if (ch == ':') {
        uint8_t p[] = {0x00,0x36,0x36,0x00,0x36,0x36,0x00};
        memcpy(cols, p, 7);
    } else if (ch == '-') {
        uint8_t p[] = {0x08,0x08,0x08,0x08,0x08,0x00,0x00};
        memcpy(cols, p, 7);
    } else if (ch == '.') {
        uint8_t p[] = {0x00,0x60,0x60,0x00,0x00,0x00,0x00};
        memcpy(cols, p, 7);
    }

    for (int col = 0; col < 7; ++col) {
        uint8_t b = cols[col];
        for (int bit = 0; bit < 5; ++bit) {
            if (b & (1 << bit)) {
                int px = x + (4 - bit) * scale;
                int py = y + col * scale;
                fb_fill_rect(fb, px, py, px + scale, py + scale, fg);
            }
        }
    }
}

static void draw_label57(uint16_t *fb, int x, int y, const char *s, uint16_t fg, int scale)
{
    int cx = x;
    for (; *s; ++s) {
        draw_glyph57(fb, cx, y, *s, fg, scale);
        cx += 6 * scale;
    }
}

void waveshare_7b_show_splash(const char *wifiSsid, const char *wifiPass, const char *ipAddr)
{
    if (!s_panel || !ipAddr) return;

    const size_t npx = (size_t)WS7B_LCD_H_RES * (size_t)WS7B_LCD_V_RES;
    const size_t pxBytes = npx * 2;
    uint16_t *fb = (uint16_t *)heap_caps_malloc(pxBytes, MALLOC_CAP_SPIRAM);
    if (!fb) {
        ESP_LOGE(TAG, "splash: framebuffer alloc failed");
        return;
    }

    const uint16_t bg = 0x1082;   // dark blue-gray RGB565
    const uint16_t hi = 0x051d;   // header strip
    const uint16_t acc = 0x5D9C;  // muted teal
    const uint16_t warn = 0xFD20; // orange
    for (size_t i = 0; i < npx; ++i) {
        fb[i] = bg;
    }

    fb_fill_rect(fb, 0, 0, WS7B_LCD_H_RES, 72, hi);
    fb_fill_rect(fb, 0, 72, WS7B_LCD_H_RES, 76, acc);
    fb_fill_rect(fb, 40, 92, WS7B_LCD_H_RES - 40, 120, bg);

    draw_label57(fb, 48, 84, "BRIDGE  READY", warn, 4);

    draw_ip_seven(fb, ipAddr, 200, 0xFFFF, bg);

    draw_label57(fb, 48, 400, "WIFI", 0xAD75, 3);
    if (wifiSsid) {
        String line = String(wifiSsid);
        draw_label57(fb, 48, 430, line.c_str(), 0xFFFF, 3);
    }
    draw_label57(fb, 48, 470, "PASSWORD", 0xAD75, 3);
    if (wifiPass) {
        String p = String(wifiPass);
        draw_label57(fb, 48, 500, p.c_str(), 0xFFFF, 3);
    }

    draw_label57(fb, 48, 540, "GO TO IP FOR DASHBOARD", 0x07FF, 3);

    esp_err_t err = esp_lcd_panel_draw_bitmap(s_panel, 0, 0, WS7B_LCD_H_RES, WS7B_LCD_V_RES, fb);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "draw_bitmap failed: %d", (int)err);
    }
    delay(50);
    heap_caps_free(fb);
    ESP_LOGI(TAG, "Splash drawn for IP %s", ipAddr);
}