/**
 * @file WRX_Cluster_gen.h
 */

#ifndef WRX_CLUSTER_GEN_H
#define WRX_CLUSTER_GEN_H

#ifndef UI_SUBJECT_STRING_LENGTH
#define UI_SUBJECT_STRING_LENGTH 256
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
    #include "lvgl_private.h"
#else
    #include "lvgl/lvgl.h"
    #include "lvgl/lvgl_private.h"
#endif

#ifdef LV_USE_XML
    #include "lv_xml/lv_xml.h"
#endif



/*********************
 *      DEFINES
 *********************/

#define COL_BG lv_color_hex(0x000000)

#define COL_WHITE lv_color_hex(0xFFFFFF)

#define COL_MUTED lv_color_hex(0x6E7178)

#define COL_SUB lv_color_hex(0xA8ACB3)

#define COL_CHIP lv_color_hex(0x3A3D44)

#define COL_ORANGE lv_color_hex(0xFD7E2A)

#define COL_BLUE lv_color_hex(0x0A84FF)

#define COL_GREEN lv_color_hex(0x34C759)

#define COL_YELLOW lv_color_hex(0xFFCC00)

#define COL_RED lv_color_hex(0xFF3B30)

#define COL_CYAN lv_color_hex(0x32D9FF)

#define COL_MAGENTA lv_color_hex(0xC436FF)

#define COL_TRACK lv_color_hex(0x14171C)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL VARIABLES
 **********************/

/*-------------------
 * Permanent screens
 *------------------*/

/*----------------
 * Global styles
 *----------------*/

extern lv_style_t st_screen;
extern lv_style_t st_bar_track;
extern lv_style_t st_bar_cool;
extern lv_style_t st_bar_warm;
extern lv_style_t st_bar_nav;
extern lv_style_t st_bar_range;
extern lv_style_t st_circle_chip;
extern lv_style_t st_ghost;
extern lv_style_t st_ff_scene;
extern lv_style_t st_ff_sky;
extern lv_style_t st_ff_road_far;
extern lv_style_t st_ff_road_mid;
extern lv_style_t st_ff_road_near;
extern lv_style_t st_ff_lane;
extern lv_style_t st_ff_edge_yellow;
extern lv_style_t st_ff_edge_white;
extern lv_style_t st_ff_sensor;
extern lv_style_t st_ff_lidar;
extern lv_style_t st_ff_detect;
extern lv_style_t st_ff_ego_body;
extern lv_style_t st_ff_ego_roof;
extern lv_style_t st_ff_glass;
extern lv_style_t st_ff_target_body;
extern lv_style_t st_ff_alert_body;
extern lv_style_t st_ff_car_shadow;
extern lv_style_t st_ff_car_wheel;
extern lv_style_t st_ff_body_hi;
extern lv_style_t st_ff_body_lo;
extern lv_style_t st_ff_bumper;
extern lv_style_t st_ff_headlight;
extern lv_style_t st_ff_taillight;
extern lv_style_t st_ff_plate;

/*----------------
 * Fonts
 *----------------*/

extern lv_font_t * font_12;

extern lv_font_t * font_14;

extern lv_font_t * font_16;

extern lv_font_t * font_18;

extern lv_font_t * font_22;

extern lv_font_t * font_48;

/*----------------
 * Images
 *----------------*/

/*----------------
 * Subjects
 *----------------*/

extern lv_subject_t sp_speed;
extern lv_subject_t sp_rpm;
extern lv_subject_t sp_range_mi;
extern lv_subject_t sp_heading;
extern lv_subject_t sp_street;
extern lv_subject_t sp_dist;
extern lv_subject_t sp_eta;
extern lv_subject_t ff_hud_mode;
extern lv_subject_t ff_status;
extern lv_subject_t ff_target_range;
extern lv_subject_t ff_lidar;
extern lv_subject_t ff_left_sensor;
extern lv_subject_t ff_right_sensor;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*----------------
 * Event Callbacks
 *----------------*/

/**
 * Initialize the component library
 */

void WRX_Cluster_init_gen(const char * asset_path);

/**********************
 *      MACROS
 **********************/

/**********************
 *   POST INCLUDES
 **********************/

/*Include all the widgets, components and screens of this library*/
#include "components/freeway_3d_view_gen.h"
#include "screens/dashboard_v2_gen.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*WRX_CLUSTER_GEN_H*/