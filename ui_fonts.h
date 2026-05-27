#ifndef UI_FONTS_H
#define UI_FONTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_font_t inter_16;
extern const lv_font_t inter_22;
extern const lv_font_t inter_32;
extern const lv_font_t inter_64;
extern const lv_font_t inter_96;
extern const lv_font_t lucide_28;
extern const lv_font_t lucide_40;

#ifdef __cplusplus
}
#endif

// Lucide icon glyphs (UTF-8 literals)
#define ICO_ARROW_LEFT     "\xEE\x81\x88"
#define ICO_ARROW_RIGHT    "\xEE\x81\x89"
#define ICO_PLAY           "\xEE\x84\xBC"
#define ICO_PAUSE          "\xEE\x84\xAE"
#define ICO_SKIP_FORWARD   "\xEE\x85\xA0"
#define ICO_SKIP_BACK      "\xEE\x85\x9F"
#define ICO_MUSIC          "\xEE\x84\xA2"
#define ICO_SIGNAL         "\xEE\x89\x9F"
#define ICO_BATTERY        "\xEE\x81\x93"
#define ICO_FUEL           "\xEE\x8A\xAF"
#define ICO_CAR            "\xEE\x87\x95"
#define ICO_THERMOMETER    "\xEE\x86\x86"
#define ICO_SUN            "\xEE\x85\xB8"
#define ICO_MOON           "\xEE\x84\x9E"
#define ICO_NAVIGATION     "\xEE\x84\xA3"
#define ICO_HEADPHONES     "\xEE\x83\xB1"
#define ICO_WIFI           "\xEE\x86\xAE"
#define ICO_BLUETOOTH      "\xEE\x81\x9C"
#define ICO_PHONE          "\xEE\x84\xB3"
#define ICO_VOLUME         "\xEE\x86\xAB"
#define ICO_GAUGE          "\xEE\x86\xBF"
#define ICO_LOCATE         "\xEE\x87\x9B"
#define ICO_COMPASS        "\xEE\x82\x9B"
#define ICO_CLOCK          "\xEE\x82\x87"

#endif
