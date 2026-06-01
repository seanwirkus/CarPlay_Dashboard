/**
 * @file lvgl_studio_flash_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "lvgl_studio_flash_gen.h"
#include "../lvgl_editor.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/***********************
 *  STATIC VARIABLES
 **********************/

/***********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lvgl_studio_flash_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");


    static bool style_inited = false;

    if (!style_inited) {

        style_inited = true;
    }

    if (lvgl_studio_flash == NULL) lvgl_studio_flash = lv_obj_create(NULL);
    lv_obj_t * lv_obj_0 = lvgl_studio_flash;
    lv_obj_set_name_static(lv_obj_0, "lvgl_studio_flash_#");

    lv_obj_t * lv_obj_1 = lv_obj_create(lv_obj_0);
    lv_obj_set_x(lv_obj_1, 0);
    lv_obj_set_y(lv_obj_1, 0);
    lv_obj_set_width(lv_obj_1, 1024);
    lv_obj_set_height(lv_obj_1, 600);
    lv_obj_add_style(lv_obj_1, &st_black, 0);
    lv_obj_t * lv_label_0 = lv_label_create(lv_obj_1);
    lv_obj_set_x(lv_label_0, 432);
    lv_obj_set_y(lv_label_0, 58);
    lv_label_set_text(lv_label_0, "");
    lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0x1a2230), 0);
    lv_obj_set_style_text_font(lv_label_0, bs_icons_48, 0);
    
    lv_obj_t * lv_label_1 = lv_label_create(lv_obj_1);
    lv_obj_set_x(lv_label_1, 544);
    lv_obj_set_y(lv_label_1, 58);
    lv_label_set_text(lv_label_1, "");
    lv_obj_set_style_text_color(lv_label_1, lv_color_hex(0x1a2230), 0);
    lv_obj_set_style_text_font(lv_label_1, bs_icons_48, 0);
    
    lv_obj_t * lv_arc_0 = lv_arc_create(lv_obj_1);
    lv_obj_set_x(lv_arc_0, 0);
    lv_obj_set_y(lv_arc_0, 70);
    lv_obj_set_width(lv_arc_0, 460);
    lv_obj_set_height(lv_arc_0, 460);
    lv_arc_set_bg_start_angle(lv_arc_0, 135);
    lv_arc_set_bg_end_angle(lv_arc_0, 45);
    lv_arc_set_start_angle(lv_arc_0, 135);
    lv_arc_set_end_angle(lv_arc_0, 45);
    lv_arc_set_min_value(lv_arc_0, 0);
    lv_arc_set_max_value(lv_arc_0, 100);
    lv_arc_set_value(lv_arc_0, 0);
    lv_arc_bind_value(lv_arc_0, &sp_rpm_bar);
    lv_obj_add_style(lv_arc_0, &st_arc_track_rpm, 0);
    lv_obj_add_style(lv_arc_0, &st_arc_fill_rpm, LV_PART_INDICATOR);
    lv_obj_add_style(lv_arc_0, &st_arc_knob_hidden, LV_PART_KNOB);
    
    lv_obj_t * lv_label_2 = lv_label_create(lv_obj_1);
    lv_obj_set_x(lv_label_2, 156);
    lv_obj_set_y(lv_label_2, 248);
    lv_obj_set_width(lv_label_2, 200);
    lv_obj_set_height(lv_label_2, 100);
    lv_label_bind_text(lv_label_2, &flash_rpm, NULL);
    lv_obj_set_style_text_align(lv_label_2, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_add_style(lv_label_2, &st_speed_text, 0);
    
    lv_obj_t * lv_label_3 = lv_label_create(lv_obj_1);
    lv_obj_set_x(lv_label_3, 206);
    lv_obj_set_y(lv_label_3, 358);
    lv_obj_set_width(lv_label_3, 100);
    lv_label_set_text(lv_label_3, "RPM");
    lv_obj_set_style_text_align(lv_label_3, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(lv_label_3, lv_color_hex(0x6f86a4), 0);
    lv_obj_set_style_text_font(lv_label_3, font_32, 0);
    lv_obj_set_height(lv_label_3, 40);
    
    lv_obj_t * lv_obj_2 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_2, 471);
    lv_obj_set_y(lv_obj_2, 141);
    lv_obj_set_width(lv_obj_2, 80);
    lv_obj_set_height(lv_obj_2, 80);
    lv_obj_add_style(lv_obj_2, &st_chip, 0);
    lv_obj_t * lv_label_4 = lv_label_create(lv_obj_2);
    lv_obj_set_x(lv_label_4, 7);
    lv_obj_set_y(lv_label_4, 1);
    lv_obj_set_width(lv_label_4, 33);
    lv_obj_set_align(lv_label_4, LV_ALIGN_CENTER);
    lv_label_bind_text(lv_label_4, &sp_gear, NULL);
    lv_obj_set_style_text_color(lv_label_4, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_text_font(lv_label_4, font_48, 0);
    lv_obj_set_height(lv_label_4, 41);
    
    lv_obj_t * lv_label_5 = lv_label_create(lv_obj_1);
    lv_obj_set_x(lv_label_5, 442);
    lv_obj_set_y(lv_label_5, 296);
    lv_obj_set_width(lv_label_5, 140);
    lv_obj_set_align(lv_label_5, LV_ALIGN_CENTER);
    lv_label_bind_text(lv_label_5, &sp_drive_mode, NULL);
    lv_obj_set_style_text_color(lv_label_5, lv_color_hex(0x6f86a4), 0);
    lv_obj_set_style_text_font(lv_label_5, font_16, 0);
    
    lv_obj_t * lv_label_6 = lv_label_create(lv_obj_1);
    lv_obj_set_x(lv_label_6, 442);
    lv_obj_set_y(lv_label_6, 320);
    lv_obj_set_width(lv_label_6, 140);
    lv_obj_set_align(lv_label_6, LV_ALIGN_CENTER);
    lv_label_bind_text(lv_label_6, &sp_odo, NULL);
    lv_obj_set_style_text_color(lv_label_6, lv_color_hex(0x3d5c78), 0);
    lv_obj_set_style_text_font(lv_label_6, font_12, 0);
    
    lv_obj_t * lv_arc_1 = lv_arc_create(lv_obj_1);
    lv_obj_set_x(lv_arc_1, 564);
    lv_obj_set_y(lv_arc_1, 70);
    lv_obj_set_width(lv_arc_1, 460);
    lv_obj_set_height(lv_arc_1, 460);
    lv_arc_set_bg_start_angle(lv_arc_1, 135);
    lv_arc_set_bg_end_angle(lv_arc_1, 45);
    lv_arc_set_start_angle(lv_arc_1, 135);
    lv_arc_set_end_angle(lv_arc_1, 45);
    lv_arc_set_min_value(lv_arc_1, 0);
    lv_arc_set_max_value(lv_arc_1, 100);
    lv_arc_set_value(lv_arc_1, 0);
    lv_arc_bind_value(lv_arc_1, &sp_speed_bar);
    lv_obj_add_style(lv_arc_1, &st_arc_track_mph, 0);
    lv_obj_add_style(lv_arc_1, &st_arc_fill_mph, LV_PART_INDICATOR);
    lv_obj_add_style(lv_arc_1, &st_arc_knob_hidden, LV_PART_KNOB);
    
    lv_obj_t * lv_label_7 = lv_label_create(lv_obj_1);
    lv_obj_set_x(lv_label_7, 668);
    lv_obj_set_y(lv_label_7, 248);
    lv_obj_set_width(lv_label_7, 200);
    lv_obj_set_height(lv_label_7, 100);
    lv_label_bind_text(lv_label_7, &flash_mph, NULL);
    lv_obj_set_style_text_align(lv_label_7, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_add_style(lv_label_7, &st_speed_text, 0);
    
    lv_obj_t * lv_label_8 = lv_label_create(lv_obj_1);
    lv_obj_set_x(lv_label_8, 718);
    lv_obj_set_y(lv_label_8, 358);
    lv_obj_set_width(lv_label_8, 100);
    lv_label_set_text(lv_label_8, "MPH");
    lv_obj_set_style_text_align(lv_label_8, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(lv_label_8, lv_color_hex(0x6f86a4), 0);
    lv_obj_set_style_text_font(lv_label_8, font_32, 0);
    lv_obj_set_height(lv_label_8, 40);
    
    lv_obj_t * lv_label_9 = lv_label_create(lv_obj_1);
    lv_obj_set_x(lv_label_9, 302);
    lv_obj_set_y(lv_label_9, 518);
    lv_label_set_text(lv_label_9, "FUEL");
    lv_obj_set_style_text_color(lv_label_9, lv_color_hex(0x6f86a4), 0);
    lv_obj_set_style_text_font(lv_label_9, font_16, 0);
    
    lv_obj_t * lv_label_10 = lv_label_create(lv_obj_1);
    lv_obj_set_x(lv_label_10, 658);
    lv_obj_set_y(lv_label_10, 518);
    lv_obj_set_width(lv_label_10, 64);
    lv_label_bind_text(lv_label_10, &flash_fuel, NULL);
    lv_obj_add_style(lv_label_10, &st_small_text, 0);
    
    lv_obj_t * lv_bar_0 = lv_bar_create(lv_obj_1);
    lv_obj_set_x(lv_bar_0, 302);
    lv_obj_set_y(lv_bar_0, 542);
    lv_obj_set_width(lv_bar_0, 420);
    lv_obj_set_height(lv_bar_0, 26);
    lv_bar_set_min_value(lv_bar_0, 0);
    lv_bar_set_max_value(lv_bar_0, 100);
    lv_bar_bind_value(lv_bar_0, &flash_fuel_bar);
    lv_obj_add_style(lv_bar_0, &st_fuel_track, 0);
    lv_obj_add_style(lv_bar_0, &st_fuel_indicator, LV_PART_INDICATOR);
    
    lv_obj_t * lv_obj_3 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_3, 122);
    lv_obj_set_y(lv_obj_3, 0);
    lv_obj_set_width(lv_obj_3, 780);
    lv_obj_set_height(lv_obj_3, 50);
    lv_obj_set_style_bg_opa(lv_obj_3, (255 * 0 / 100), 0);
    lv_obj_set_style_border_width(lv_obj_3, 0, 0);
    lv_obj_set_style_pad_all(lv_obj_3, 0, 0);
    lv_obj_t * lv_label_11 = lv_label_create(lv_obj_3);
    lv_obj_set_x(lv_label_11, 42);
    lv_obj_set_y(lv_label_11, 12);
    lv_label_set_text(lv_label_11, "");
    lv_obj_set_style_text_color(lv_label_11, lv_color_hex(0x4f6077), 0);
    lv_obj_set_style_text_font(lv_label_11, bs_icons_24, 0);
    
    lv_obj_t * lv_label_12 = lv_label_create(lv_obj_3);
    lv_obj_set_x(lv_label_12, 138);
    lv_obj_set_y(lv_label_12, 12);
    lv_label_set_text(lv_label_12, "");
    lv_obj_set_style_text_color(lv_label_12, lv_color_hex(0x4f6077), 0);
    lv_obj_set_style_text_font(lv_label_12, bs_icons_24, 0);
    
    lv_obj_t * lv_label_13 = lv_label_create(lv_obj_3);
    lv_obj_set_x(lv_label_13, 234);
    lv_obj_set_y(lv_label_13, 12);
    lv_label_set_text(lv_label_13, "");
    lv_obj_set_style_text_color(lv_label_13, lv_color_hex(0x4f6077), 0);
    lv_obj_set_style_text_font(lv_label_13, bs_icons_24, 0);
    
    lv_obj_t * lv_label_14 = lv_label_create(lv_obj_3);
    lv_obj_set_x(lv_label_14, 330);
    lv_obj_set_y(lv_label_14, 12);
    lv_label_set_text(lv_label_14, "");
    lv_obj_set_style_text_color(lv_label_14, lv_color_hex(0x4f6077), 0);
    lv_obj_set_style_text_font(lv_label_14, bs_icons_24, 0);
    
    lv_obj_t * lv_label_15 = lv_label_create(lv_obj_3);
    lv_obj_set_x(lv_label_15, 426);
    lv_obj_set_y(lv_label_15, 12);
    lv_label_set_text(lv_label_15, "");
    lv_obj_set_style_text_color(lv_label_15, lv_color_hex(0x4f6077), 0);
    lv_obj_set_style_text_font(lv_label_15, bs_icons_24, 0);
    
    lv_obj_t * lv_label_16 = lv_label_create(lv_obj_3);
    lv_obj_set_x(lv_label_16, 522);
    lv_obj_set_y(lv_label_16, 12);
    lv_label_set_text(lv_label_16, "");
    lv_obj_set_style_text_color(lv_label_16, lv_color_hex(0x4f6077), 0);
    lv_obj_set_style_text_font(lv_label_16, bs_icons_24, 0);
    
    lv_obj_t * lv_label_17 = lv_label_create(lv_obj_3);
    lv_obj_set_x(lv_label_17, 618);
    lv_obj_set_y(lv_label_17, 12);
    lv_label_set_text(lv_label_17, "");
    lv_obj_set_style_text_color(lv_label_17, lv_color_hex(0x4f6077), 0);
    lv_obj_set_style_text_font(lv_label_17, bs_icons_24, 0);
    
    lv_obj_t * lv_label_18 = lv_label_create(lv_obj_3);
    lv_obj_set_x(lv_label_18, 714);
    lv_obj_set_y(lv_label_18, 12);
    lv_label_set_text(lv_label_18, "");
    lv_obj_set_style_text_color(lv_label_18, lv_color_hex(0x4f6077), 0);
    lv_obj_set_style_text_font(lv_label_18, bs_icons_24, 0);
    
    lv_obj_t * lv_label_19 = lv_label_create(lv_obj_1);
    lv_obj_set_x(lv_label_19, 742);
    lv_obj_set_y(lv_label_19, 542);
    lv_label_set_text(lv_label_19, "SYSTEM CHECK");
    lv_obj_set_style_text_color(lv_label_19, lv_color_hex(0x3d5c78), 0);
    lv_obj_set_style_text_font(lv_label_19, font_16, 0);
    
    lv_obj_t * lv_label_20 = lv_label_create(lv_obj_1);
    lv_obj_set_x(lv_label_20, 742);
    lv_obj_set_y(lv_label_20, 564);
    lv_label_set_text(lv_label_20, "SEARCHING");
    lv_obj_set_style_text_color(lv_label_20, lv_color_hex(0x6f86a4), 0);
    lv_obj_set_style_text_font(lv_label_20, font_12, 0);

    LV_TRACE_OBJ_CREATE("finished");

    return lv_obj_0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

