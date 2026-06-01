/**
 * @file lvgl_editor_gen.h
 */

#ifndef LVGL_EDITOR_GEN_H
#define LVGL_EDITOR_GEN_H

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

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL VARIABLES
 **********************/

/*-------------------
 * Permanent screens
 *------------------*/

extern lv_obj_t * lvgl_studio_flash;

/*----------------
 * Global styles
 *----------------*/

extern lv_style_t st_black;
extern lv_style_t st_arc_track_rpm;
extern lv_style_t st_arc_fill_rpm;
extern lv_style_t st_arc_track_mph;
extern lv_style_t st_arc_fill_mph;
extern lv_style_t st_arc_knob_hidden;
extern lv_style_t st_panel;
extern lv_style_t st_panel_soft;
extern lv_style_t st_text_primary;
extern lv_style_t st_text_secondary;
extern lv_style_t st_text_muted;
extern lv_style_t st_text_dim;
extern lv_style_t st_speed_text;
extern lv_style_t st_value_text;
extern lv_style_t st_small_text;
extern lv_style_t st_tiny_text;
extern lv_style_t st_bar_track;
extern lv_style_t st_speed_indicator;
extern lv_style_t st_fuel_track;
extern lv_style_t st_fuel_indicator;
extern lv_style_t st_rpm_track;
extern lv_style_t st_rpm_indicator;
extern lv_style_t st_temp_track;
extern lv_style_t st_temp_indicator;
extern lv_style_t st_blue_indicator;
extern lv_style_t st_road;
extern lv_style_t st_road_line;
extern lv_style_t st_lane_shadow;
extern lv_style_t st_car_body;
extern lv_style_t st_car_glass;
extern lv_style_t st_chip;
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

/*----------------
 * Fonts
 *----------------*/

extern lv_font_t * font_12;

extern lv_font_t * font_16;

extern lv_font_t * font_18;

extern lv_font_t * font_20;

extern lv_font_t * font_24;

extern lv_font_t * font_32;

extern lv_font_t * font_48;

extern lv_font_t * font_64;

extern lv_font_t * font_96;

extern lv_font_t * bs_icons_24;

extern lv_font_t * bs_icons_48;

/*----------------
 * Images
 *----------------*/

/*----------------
 * Subjects
 *----------------*/

extern lv_subject_t sp_speed;
extern lv_subject_t sp_speed_bar;
extern lv_subject_t sp_fuel_range;
extern lv_subject_t sp_fuel_bar;
extern lv_subject_t sp_drive_mode;
extern lv_subject_t sp_gear;
extern lv_subject_t sp_rpm_bar;
extern lv_subject_t sp_temp;
extern lv_subject_t sp_temp_bar;
extern lv_subject_t sp_odo;
extern lv_subject_t ff_hud_mode;
extern lv_subject_t ff_status;
extern lv_subject_t ff_target_range;
extern lv_subject_t ff_lidar;
extern lv_subject_t ff_left_sensor;
extern lv_subject_t ff_right_sensor;
extern lv_subject_t flash_rpm;
extern lv_subject_t flash_mph;
extern lv_subject_t flash_fuel;
extern lv_subject_t flash_fuel_bar;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*----------------
 * Event Callbacks
 *----------------*/

/**
 * Initialize the component library
 */

void lvgl_editor_init_gen(const char * asset_path);

/**********************
 *      MACROS
 **********************/

/**********************
 *   POST INCLUDES
 **********************/

/*Include all the widgets, components and screens of this library*/
#include "components/freeway_3d_view_gen.h"
#include "screens/lvgl_studio_flash_gen.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_EDITOR_GEN_H*/