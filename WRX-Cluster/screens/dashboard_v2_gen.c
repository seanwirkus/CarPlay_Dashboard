/**
 * @file dashboard_v2_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "dashboard_v2_gen.h"
#include "../WRX_Cluster.h"

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

lv_obj_t * dashboard_v2_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");


    static bool style_inited = false;

    if (!style_inited) {

        style_inited = true;
    }

    lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
    lv_obj_set_name_static(lv_obj_0, "dashboard_v2_#");
    lv_obj_set_width(lv_obj_0, 1024);
    lv_obj_set_height(lv_obj_0, 600);

    lv_obj_add_style(lv_obj_0, &st_screen, 0);
    lv_obj_t * lv_label_0 = lv_label_create(lv_obj_0);
    lv_label_set_text(lv_label_0, "CAM");
    lv_obj_set_align(lv_label_0, LV_ALIGN_TOP_MID);
    lv_obj_set_x(lv_label_0, 0);
    lv_obj_set_y(lv_label_0, 0);
    lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0xF2F4F8), 0);
    lv_obj_set_style_text_font(lv_label_0, font_16, 0);
    
    lv_obj_t * lv_label_1 = lv_label_create(lv_obj_0);
    lv_label_set_text(lv_label_1, "LTE");
    lv_obj_set_align(lv_label_1, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_x(lv_label_1, -190);
    lv_obj_set_y(lv_label_1, 0);
    lv_obj_set_style_text_color(lv_label_1, lv_color_hex(0xF2F4F8), 0);
    lv_obj_set_style_text_font(lv_label_1, font_12, 0);
    
    lv_obj_t * lv_label_2 = lv_label_create(lv_obj_0);
    lv_label_set_text(lv_label_2, "100%");
    lv_obj_set_align(lv_label_2, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_x(lv_label_2, -120);
    lv_obj_set_y(lv_label_2, 0);
    lv_obj_set_style_text_color(lv_label_2, lv_color_hex(0xF2F4F8), 0);
    lv_obj_set_style_text_font(lv_label_2, font_12, 0);
    
    lv_obj_t * lv_label_3 = lv_label_create(lv_obj_0);
    lv_label_bind_text(lv_label_3, &sp_speed, NULL);
    lv_obj_set_align(lv_label_3, LV_ALIGN_TOP_LEFT);
    lv_obj_set_x(lv_label_3, 74);
    lv_obj_set_y(lv_label_3, 80);
    lv_obj_set_style_text_color(lv_label_3, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(lv_label_3, font_48, 0);
    
    lv_obj_t * lv_label_4 = lv_label_create(lv_obj_0);
    lv_label_set_text(lv_label_4, "mph");
    lv_obj_set_align(lv_label_4, LV_ALIGN_TOP_LEFT);
    lv_obj_set_x(lv_label_4, 260);
    lv_obj_set_y(lv_label_4, 154);
    lv_obj_set_style_text_color(lv_label_4, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(lv_label_4, font_18, 0);
    
    lv_obj_t * lv_bar_0 = lv_bar_create(lv_obj_0);
    lv_obj_set_width(lv_bar_0, 250);
    lv_obj_set_height(lv_bar_0, 10);
    lv_obj_set_align(lv_bar_0, LV_ALIGN_TOP_LEFT);
    lv_obj_set_x(lv_bar_0, 74);
    lv_obj_set_y(lv_bar_0, 190);
    lv_bar_set_min_value(lv_bar_0, 0);
    lv_bar_set_max_value(lv_bar_0, 100);
    lv_bar_set_value(lv_bar_0, 45, false);
    lv_obj_add_style(lv_bar_0, &st_bar_track, 0);
    lv_obj_add_style(lv_bar_0, &st_bar_cool, LV_PART_INDICATOR);
    
    lv_obj_t * lv_label_5 = lv_label_create(lv_obj_0);
    lv_label_set_text(lv_label_5, "FUEL");
    lv_obj_set_align(lv_label_5, LV_ALIGN_TOP_LEFT);
    lv_obj_set_x(lv_label_5, 74);
    lv_obj_set_y(lv_label_5, 255);
    lv_obj_set_style_text_color(lv_label_5, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(lv_label_5, font_16, 0);
    
    lv_obj_t * lv_label_6 = lv_label_create(lv_obj_0);
    lv_label_bind_text(lv_label_6, &sp_range_mi, NULL);
    lv_obj_set_align(lv_label_6, LV_ALIGN_TOP_LEFT);
    lv_obj_set_x(lv_label_6, 248);
    lv_obj_set_y(lv_label_6, 252);
    lv_obj_set_style_text_color(lv_label_6, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(lv_label_6, font_22, 0);
    
    lv_obj_t * lv_label_7 = lv_label_create(lv_obj_0);
    lv_label_set_text(lv_label_7, "mi");
    lv_obj_set_align(lv_label_7, LV_ALIGN_TOP_LEFT);
    lv_obj_set_x(lv_label_7, 300);
    lv_obj_set_y(lv_label_7, 257);
    lv_obj_set_style_text_color(lv_label_7, lv_color_hex(0xA8ACB3), 0);
    lv_obj_set_style_text_font(lv_label_7, font_14, 0);
    
    lv_obj_t * lv_bar_1 = lv_bar_create(lv_obj_0);
    lv_obj_set_width(lv_bar_1, 250);
    lv_obj_set_height(lv_bar_1, 10);
    lv_obj_set_align(lv_bar_1, LV_ALIGN_TOP_LEFT);
    lv_obj_set_x(lv_bar_1, 74);
    lv_obj_set_y(lv_bar_1, 292);
    lv_bar_set_min_value(lv_bar_1, 0);
    lv_bar_set_max_value(lv_bar_1, 100);
    lv_bar_set_value(lv_bar_1, 62, false);
    lv_obj_add_style(lv_bar_1, &st_bar_track, 0);
    lv_obj_add_style(lv_bar_1, &st_bar_range, LV_PART_INDICATOR);
    
    lv_obj_t * lv_obj_1 = lv_obj_create(lv_obj_0);
    lv_obj_set_width(lv_obj_1, 1);
    lv_obj_set_height(lv_obj_1, 330);
    lv_obj_set_align(lv_obj_1, LV_ALIGN_TOP_LEFT);
    lv_obj_set_x(lv_obj_1, 354);
    lv_obj_set_y(lv_obj_1, 70);
    lv_obj_set_style_pad_all(lv_obj_1, 0, 0);
    lv_obj_set_style_border_width(lv_obj_1, 0, 0);
    lv_obj_set_style_bg_color(lv_obj_1, lv_color_hex(0x1A1D22), 0);
    lv_obj_set_style_bg_opa(lv_obj_1, 255, 0);
    
    lv_obj_t * freeway_3d_view_0 = freeway_3d_view_create(lv_obj_0);
    lv_obj_set_align(freeway_3d_view_0, LV_ALIGN_TOP_LEFT);
    lv_obj_set_x(freeway_3d_view_0, 362);
    lv_obj_set_y(freeway_3d_view_0, 58);
    
    lv_obj_t * lv_label_8 = lv_label_create(lv_obj_0);
    lv_label_set_text(lv_label_8, "HEAD");
    lv_obj_set_align(lv_label_8, LV_ALIGN_TOP_LEFT);
    lv_obj_set_x(lv_label_8, 454);
    lv_obj_set_y(lv_label_8, 374);
    lv_obj_set_style_text_color(lv_label_8, lv_color_hex(0x34C759), 0);
    lv_obj_set_style_text_font(lv_label_8, font_18, 0);
    
    lv_obj_t * lv_label_9 = lv_label_create(lv_obj_0);
    lv_label_bind_text(lv_label_9, &sp_heading, NULL);
    lv_obj_set_align(lv_label_9, LV_ALIGN_TOP_LEFT);
    lv_obj_set_x(lv_label_9, 592);
    lv_obj_set_y(lv_label_9, 376);
    lv_obj_set_style_text_color(lv_label_9, lv_color_hex(0xF2F4F8), 0);
    lv_obj_set_style_text_font(lv_label_9, font_16, 0);
    
    lv_obj_t * lv_obj_2 = lv_obj_create(lv_obj_0);
    lv_obj_set_width(lv_obj_2, 1);
    lv_obj_set_height(lv_obj_2, 330);
    lv_obj_set_align(lv_obj_2, LV_ALIGN_TOP_LEFT);
    lv_obj_set_x(lv_obj_2, 690);
    lv_obj_set_y(lv_obj_2, 70);
    lv_obj_set_style_pad_all(lv_obj_2, 0, 0);
    lv_obj_set_style_border_width(lv_obj_2, 0, 0);
    lv_obj_set_style_bg_color(lv_obj_2, lv_color_hex(0x1A1D22), 0);
    lv_obj_set_style_bg_opa(lv_obj_2, 255, 0);
    
    lv_obj_t * lv_obj_3 = lv_obj_create(lv_obj_0);
    lv_obj_set_width(lv_obj_3, 44);
    lv_obj_set_height(lv_obj_3, 44);
    lv_obj_set_align(lv_obj_3, LV_ALIGN_TOP_LEFT);
    lv_obj_set_x(lv_obj_3, 724);
    lv_obj_set_y(lv_obj_3, 136);
    lv_obj_add_style(lv_obj_3, &st_circle_chip, 0);
    lv_obj_t * lv_label_10 = lv_label_create(lv_obj_3);
    lv_label_set_text(lv_label_10, "3");
    lv_obj_set_align(lv_label_10, LV_ALIGN_CENTER);
    lv_obj_set_style_text_color(lv_label_10, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(lv_label_10, font_22, 0);
    
    lv_obj_t * lv_label_11 = lv_label_create(lv_obj_0);
    lv_label_set_text(lv_label_11, "Sport");
    lv_obj_set_align(lv_label_11, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_x(lv_label_11, -50);
    lv_obj_set_y(lv_label_11, 150);
    lv_obj_set_style_text_color(lv_label_11, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(lv_label_11, font_16, 0);
    
    lv_obj_t * lv_bar_2 = lv_bar_create(lv_obj_0);
    lv_obj_set_width(lv_bar_2, 250);
    lv_obj_set_height(lv_bar_2, 10);
    lv_obj_set_align(lv_bar_2, LV_ALIGN_TOP_LEFT);
    lv_obj_set_x(lv_bar_2, 724);
    lv_obj_set_y(lv_bar_2, 190);
    lv_bar_set_min_value(lv_bar_2, 0);
    lv_bar_set_max_value(lv_bar_2, 8000);
    lv_bar_bind_value(lv_bar_2, &sp_rpm);
    lv_obj_add_style(lv_bar_2, &st_bar_track, 0);
    lv_obj_add_style(lv_bar_2, &st_bar_warm, LV_PART_INDICATOR);
    
    lv_obj_t * lv_obj_4 = lv_obj_create(lv_obj_0);
    lv_obj_set_width(lv_obj_4, 31);
    lv_obj_set_height(lv_obj_4, 3);
    lv_obj_set_align(lv_obj_4, LV_ALIGN_TOP_LEFT);
    lv_obj_set_x(lv_obj_4, 940);
    lv_obj_set_y(lv_obj_4, 194);
    lv_obj_set_style_pad_all(lv_obj_4, 0, 0);
    lv_obj_set_style_border_width(lv_obj_4, 0, 0);
    lv_obj_set_style_bg_color(lv_obj_4, lv_color_hex(0xFF3B30), 0);
    lv_obj_set_style_bg_opa(lv_obj_4, 255, 0);
    lv_obj_set_style_radius(lv_obj_4, 2, 0);
    
    lv_obj_t * lv_label_12 = lv_label_create(lv_obj_0);
    lv_label_set_text(lv_label_12, "TEMP");
    lv_obj_set_align(lv_label_12, LV_ALIGN_TOP_LEFT);
    lv_obj_set_x(lv_label_12, 724);
    lv_obj_set_y(lv_label_12, 255);
    lv_obj_set_style_text_color(lv_label_12, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(lv_label_12, font_16, 0);
    
    lv_obj_t * lv_label_13 = lv_label_create(lv_obj_0);
    lv_label_set_text(lv_label_13, "195");
    lv_obj_set_align(lv_label_13, LV_ALIGN_TOP_LEFT);
    lv_obj_set_x(lv_label_13, 926);
    lv_obj_set_y(lv_label_13, 252);
    lv_obj_set_style_text_color(lv_label_13, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(lv_label_13, font_22, 0);
    
    lv_obj_t * lv_label_14 = lv_label_create(lv_obj_0);
    lv_label_set_text(lv_label_14, "F");
    lv_obj_set_align(lv_label_14, LV_ALIGN_TOP_LEFT);
    lv_obj_set_x(lv_label_14, 970);
    lv_obj_set_y(lv_label_14, 257);
    lv_obj_set_style_text_color(lv_label_14, lv_color_hex(0xA8ACB3), 0);
    lv_obj_set_style_text_font(lv_label_14, font_14, 0);
    
    lv_obj_t * lv_bar_3 = lv_bar_create(lv_obj_0);
    lv_obj_set_width(lv_bar_3, 250);
    lv_obj_set_height(lv_bar_3, 10);
    lv_obj_set_align(lv_bar_3, LV_ALIGN_TOP_LEFT);
    lv_obj_set_x(lv_bar_3, 724);
    lv_obj_set_y(lv_bar_3, 292);
    lv_bar_set_min_value(lv_bar_3, 0);
    lv_bar_set_max_value(lv_bar_3, 100);
    lv_bar_set_value(lv_bar_3, 52, false);
    lv_obj_add_style(lv_bar_3, &st_bar_track, 0);
    lv_obj_add_style(lv_bar_3, &st_bar_cool, LV_PART_INDICATOR);
    
    lv_obj_t * lv_obj_5 = lv_obj_create(lv_obj_0);
    lv_obj_set_width(lv_obj_5, 13);
    lv_obj_set_height(lv_obj_5, 13);
    lv_obj_set_align(lv_obj_5, LV_ALIGN_TOP_LEFT);
    lv_obj_set_x(lv_obj_5, 844);
    lv_obj_set_y(lv_obj_5, 291);
    lv_obj_set_style_pad_all(lv_obj_5, 0, 0);
    lv_obj_set_style_border_width(lv_obj_5, 0, 0);
    lv_obj_set_style_bg_color(lv_obj_5, lv_color_hex(0xC436FF), 0);
    lv_obj_set_style_bg_opa(lv_obj_5, 255, 0);
    lv_obj_set_style_radius(lv_obj_5, 7, 0);

    LV_TRACE_OBJ_CREATE("finished");

    return lv_obj_0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

