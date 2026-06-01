/**
 * @file freeway_3d_view_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "freeway_3d_view_gen.h"
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

lv_obj_t * freeway_3d_view_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    lv_obj_t * lv_obj_0 = lv_obj_create(parent);
    lv_obj_set_name_static(lv_obj_0, "freeway_3d_view_#");
    lv_obj_set_width(lv_obj_0, 300);
    lv_obj_set_height(lv_obj_0, 306);

    lv_obj_add_style(lv_obj_0, &st_ff_scene, 0);
    lv_obj_t * lv_label_0 = lv_label_create(lv_obj_0);
    lv_obj_set_x(lv_label_0, 18);
    lv_obj_set_y(lv_label_0, 12);
    lv_label_set_text(lv_label_0, "FREEWAY");
    lv_obj_add_style(lv_label_0, &st_tiny_text, 0);
    
    lv_obj_t * lv_label_1 = lv_label_create(lv_obj_0);
    lv_obj_set_x(lv_label_1, 84);
    lv_obj_set_y(lv_label_1, 12);
    lv_label_bind_text(lv_label_1, &ff_hud_mode, NULL);
    lv_obj_set_style_text_color(lv_label_1, lv_color_hex(0x22d3ee), 0);
    lv_obj_set_style_text_font(lv_label_1, font_12, 0);
    
    lv_obj_t * lv_label_2 = lv_label_create(lv_obj_0);
    lv_obj_set_x(lv_label_2, 223);
    lv_obj_set_y(lv_label_2, 12);
    lv_label_bind_text(lv_label_2, &ff_status, NULL);
    lv_obj_set_style_text_color(lv_label_2, lv_color_hex(0x22d3ee), 0);
    lv_obj_set_style_text_font(lv_label_2, font_12, 0);
    
    lv_obj_t * lv_obj_1 = lv_obj_create(lv_obj_0);
    lv_obj_set_x(lv_obj_1, 14);
    lv_obj_set_y(lv_obj_1, 36);
    lv_obj_set_width(lv_obj_1, 272);
    lv_obj_set_height(lv_obj_1, 224);
    lv_obj_add_style(lv_obj_1, &st_ff_sky, 0);
    lv_obj_t * lv_obj_2 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_2, 111);
    lv_obj_set_y(lv_obj_2, 40);
    lv_obj_set_width(lv_obj_2, 50);
    lv_obj_set_height(lv_obj_2, 30);
    lv_obj_add_style(lv_obj_2, &st_ff_road_far, 0);
    
    lv_obj_t * lv_obj_3 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_3, 96);
    lv_obj_set_y(lv_obj_3, 66);
    lv_obj_set_width(lv_obj_3, 80);
    lv_obj_set_height(lv_obj_3, 31);
    lv_obj_add_style(lv_obj_3, &st_ff_road_far, 0);
    
    lv_obj_t * lv_obj_4 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_4, 78);
    lv_obj_set_y(lv_obj_4, 94);
    lv_obj_set_width(lv_obj_4, 116);
    lv_obj_set_height(lv_obj_4, 34);
    lv_obj_add_style(lv_obj_4, &st_ff_road_mid, 0);
    
    lv_obj_t * lv_obj_5 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_5, 55);
    lv_obj_set_y(lv_obj_5, 124);
    lv_obj_set_width(lv_obj_5, 162);
    lv_obj_set_height(lv_obj_5, 39);
    lv_obj_add_style(lv_obj_5, &st_ff_road_mid, 0);
    
    lv_obj_t * lv_obj_6 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_6, 29);
    lv_obj_set_y(lv_obj_6, 158);
    lv_obj_set_width(lv_obj_6, 214);
    lv_obj_set_height(lv_obj_6, 52);
    lv_obj_add_style(lv_obj_6, &st_ff_road_near, 0);
    
    lv_obj_t * lv_obj_7 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_7, 101);
    lv_obj_set_y(lv_obj_7, 46);
    lv_obj_set_width(lv_obj_7, 4);
    lv_obj_set_height(lv_obj_7, 27);
    lv_obj_add_style(lv_obj_7, &st_ff_edge_yellow, 0);
    
    lv_obj_t * lv_obj_8 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_8, 84);
    lv_obj_set_y(lv_obj_8, 75);
    lv_obj_set_width(lv_obj_8, 5);
    lv_obj_set_height(lv_obj_8, 30);
    lv_obj_add_style(lv_obj_8, &st_ff_edge_yellow, 0);
    
    lv_obj_t * lv_obj_9 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_9, 63);
    lv_obj_set_y(lv_obj_9, 108);
    lv_obj_set_width(lv_obj_9, 5);
    lv_obj_set_height(lv_obj_9, 36);
    lv_obj_add_style(lv_obj_9, &st_ff_edge_yellow, 0);
    
    lv_obj_t * lv_obj_10 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_10, 34);
    lv_obj_set_y(lv_obj_10, 148);
    lv_obj_set_width(lv_obj_10, 6);
    lv_obj_set_height(lv_obj_10, 52);
    lv_obj_add_style(lv_obj_10, &st_ff_edge_yellow, 0);
    
    lv_obj_t * lv_obj_11 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_11, 169);
    lv_obj_set_y(lv_obj_11, 46);
    lv_obj_set_width(lv_obj_11, 4);
    lv_obj_set_height(lv_obj_11, 27);
    lv_obj_add_style(lv_obj_11, &st_ff_edge_white, 0);
    
    lv_obj_t * lv_obj_12 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_12, 184);
    lv_obj_set_y(lv_obj_12, 75);
    lv_obj_set_width(lv_obj_12, 5);
    lv_obj_set_height(lv_obj_12, 30);
    lv_obj_add_style(lv_obj_12, &st_ff_edge_white, 0);
    
    lv_obj_t * lv_obj_13 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_13, 205);
    lv_obj_set_y(lv_obj_13, 108);
    lv_obj_set_width(lv_obj_13, 5);
    lv_obj_set_height(lv_obj_13, 36);
    lv_obj_add_style(lv_obj_13, &st_ff_edge_white, 0);
    
    lv_obj_t * lv_obj_14 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_14, 233);
    lv_obj_set_y(lv_obj_14, 148);
    lv_obj_set_width(lv_obj_14, 6);
    lv_obj_set_height(lv_obj_14, 52);
    lv_obj_add_style(lv_obj_14, &st_ff_edge_white, 0);
    
    lv_obj_t * lv_obj_15 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_15, 135);
    lv_obj_set_y(lv_obj_15, 52);
    lv_obj_set_width(lv_obj_15, 3);
    lv_obj_set_height(lv_obj_15, 14);
    lv_obj_add_style(lv_obj_15, &st_ff_lane, 0);
    
    lv_obj_t * lv_obj_16 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_16, 134);
    lv_obj_set_y(lv_obj_16, 82);
    lv_obj_set_width(lv_obj_16, 4);
    lv_obj_set_height(lv_obj_16, 20);
    lv_obj_add_style(lv_obj_16, &st_ff_lane, 0);
    
    lv_obj_t * lv_obj_17 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_17, 133);
    lv_obj_set_y(lv_obj_17, 118);
    lv_obj_set_width(lv_obj_17, 5);
    lv_obj_set_height(lv_obj_17, 27);
    lv_obj_add_style(lv_obj_17, &st_ff_lane, 0);
    
    lv_obj_t * lv_obj_18 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_18, 132);
    lv_obj_set_y(lv_obj_18, 163);
    lv_obj_set_width(lv_obj_18, 7);
    lv_obj_set_height(lv_obj_18, 37);
    lv_obj_add_style(lv_obj_18, &st_ff_lane, 0);
    
    lv_obj_t * lv_obj_19 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_19, 73);
    lv_obj_set_y(lv_obj_19, 75);
    lv_obj_set_width(lv_obj_19, 126);
    lv_obj_set_height(lv_obj_19, 1);
    lv_obj_set_style_bg_color(lv_obj_19, lv_color_hex(0x22d3ee), 0);
    lv_obj_set_style_bg_opa(lv_obj_19, (255 * 22 / 100), 0);
    lv_obj_set_style_border_width(lv_obj_19, 0, 0);
    
    lv_obj_t * lv_obj_20 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_20, 50);
    lv_obj_set_y(lv_obj_20, 118);
    lv_obj_set_width(lv_obj_20, 172);
    lv_obj_set_height(lv_obj_20, 1);
    lv_obj_set_style_bg_color(lv_obj_20, lv_color_hex(0x22d3ee), 0);
    lv_obj_set_style_bg_opa(lv_obj_20, (255 * 26 / 100), 0);
    lv_obj_set_style_border_width(lv_obj_20, 0, 0);
    
    lv_obj_t * lv_obj_21 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_21, 26);
    lv_obj_set_y(lv_obj_21, 168);
    lv_obj_set_width(lv_obj_21, 220);
    lv_obj_set_height(lv_obj_21, 1);
    lv_obj_set_style_bg_color(lv_obj_21, lv_color_hex(0x22d3ee), 0);
    lv_obj_set_style_bg_opa(lv_obj_21, (255 * 30 / 100), 0);
    lv_obj_set_style_border_width(lv_obj_21, 0, 0);
    
    lv_obj_t * lv_label_3 = lv_label_create(lv_obj_1);
    lv_obj_set_x(lv_label_3, 143);
    lv_obj_set_y(lv_label_3, 62);
    lv_label_set_text(lv_label_3, "60 ft");
    lv_obj_add_style(lv_label_3, &st_tiny_text, 0);
    
    lv_obj_t * lv_label_4 = lv_label_create(lv_obj_1);
    lv_obj_set_x(lv_label_4, 151);
    lv_obj_set_y(lv_label_4, 105);
    lv_label_set_text(lv_label_4, "35 ft");
    lv_obj_add_style(lv_label_4, &st_tiny_text, 0);
    
    lv_obj_t * lv_label_5 = lv_label_create(lv_obj_1);
    lv_obj_set_x(lv_label_5, 161);
    lv_obj_set_y(lv_label_5, 154);
    lv_label_set_text(lv_label_5, "12 ft");
    lv_obj_add_style(lv_label_5, &st_tiny_text, 0);
    
    lv_obj_t * lv_obj_22 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_22, 116);
    lv_obj_set_y(lv_obj_22, 57);
    lv_obj_set_width(lv_obj_22, 3);
    lv_obj_set_height(lv_obj_22, 140);
    lv_obj_add_style(lv_obj_22, &st_ff_sensor, 0);
    
    lv_obj_t * lv_obj_23 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_23, 154);
    lv_obj_set_y(lv_obj_23, 57);
    lv_obj_set_width(lv_obj_23, 3);
    lv_obj_set_height(lv_obj_23, 140);
    lv_obj_add_style(lv_obj_23, &st_ff_sensor, 0);
    
    lv_obj_t * lv_obj_24 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_24, 136);
    lv_obj_set_y(lv_obj_24, 47);
    lv_obj_set_width(lv_obj_24, 2);
    lv_obj_set_height(lv_obj_24, 157);
    lv_obj_add_style(lv_obj_24, &st_ff_lidar, 0);
    
    lv_obj_t * lv_obj_25 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_25, 129);
    lv_obj_set_y(lv_obj_25, 88);
    lv_obj_set_width(lv_obj_25, 16);
    lv_obj_set_height(lv_obj_25, 16);
    lv_obj_set_style_bg_color(lv_obj_25, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_bg_opa(lv_obj_25, (255 * 100 / 100), 0);
    lv_obj_set_style_border_width(lv_obj_25, 0, 0);
    lv_obj_set_style_radius(lv_obj_25, 8, 0);
    lv_obj_set_style_pad_all(lv_obj_25, 0, 0);
    
    lv_obj_t * lv_obj_26 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_26, 119);
    lv_obj_set_y(lv_obj_26, 70);
    lv_obj_set_width(lv_obj_26, 35);
    lv_obj_set_height(lv_obj_26, 28);
    lv_obj_add_style(lv_obj_26, &st_ff_alert_body, 0);
    lv_obj_t * lv_obj_27 = lv_obj_create(lv_obj_26);
    lv_obj_set_x(lv_obj_27, 8);
    lv_obj_set_y(lv_obj_27, 5);
    lv_obj_set_width(lv_obj_27, 19);
    lv_obj_set_height(lv_obj_27, 9);
    lv_obj_add_style(lv_obj_27, &st_ff_glass, 0);
    
    lv_obj_t * lv_obj_28 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_28, 113);
    lv_obj_set_y(lv_obj_28, 65);
    lv_obj_set_width(lv_obj_28, 47);
    lv_obj_set_height(lv_obj_28, 39);
    lv_obj_add_style(lv_obj_28, &st_ff_detect, 0);
    
    lv_obj_t * lv_obj_29 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_29, 58);
    lv_obj_set_y(lv_obj_29, 116);
    lv_obj_set_width(lv_obj_29, 39);
    lv_obj_set_height(lv_obj_29, 31);
    lv_obj_add_style(lv_obj_29, &st_ff_target_body, 0);
    lv_obj_t * lv_obj_30 = lv_obj_create(lv_obj_29);
    lv_obj_set_x(lv_obj_30, 8);
    lv_obj_set_y(lv_obj_30, 6);
    lv_obj_set_width(lv_obj_30, 23);
    lv_obj_set_height(lv_obj_30, 9);
    lv_obj_add_style(lv_obj_30, &st_ff_glass, 0);
    
    lv_obj_t * lv_obj_31 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_31, 185);
    lv_obj_set_y(lv_obj_31, 129);
    lv_obj_set_width(lv_obj_31, 47);
    lv_obj_set_height(lv_obj_31, 36);
    lv_obj_add_style(lv_obj_31, &st_ff_target_body, 0);
    lv_obj_t * lv_obj_32 = lv_obj_create(lv_obj_31);
    lv_obj_set_x(lv_obj_32, 9);
    lv_obj_set_y(lv_obj_32, 6);
    lv_obj_set_width(lv_obj_32, 28);
    lv_obj_set_height(lv_obj_32, 11);
    lv_obj_add_style(lv_obj_32, &st_ff_glass, 0);
    
    lv_obj_t * lv_obj_33 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_33, 89);
    lv_obj_set_y(lv_obj_33, 159);
    lv_obj_set_width(lv_obj_33, 94);
    lv_obj_set_height(lv_obj_33, 54);
    lv_obj_set_style_bg_color(lv_obj_33, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(lv_obj_33, (255 * 55 / 100), 0);
    lv_obj_set_style_border_width(lv_obj_33, 0, 0);
    lv_obj_set_style_radius(lv_obj_33, 18, 0);
    lv_obj_set_style_pad_all(lv_obj_33, 0, 0);
    
    lv_obj_t * lv_obj_34 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_34, 92);
    lv_obj_set_y(lv_obj_34, 145);
    lv_obj_set_width(lv_obj_34, 88);
    lv_obj_set_height(lv_obj_34, 68);
    lv_obj_add_style(lv_obj_34, &st_ff_ego_body, 0);
    lv_obj_t * lv_obj_35 = lv_obj_create(lv_obj_34);
    lv_obj_set_x(lv_obj_35, 19);
    lv_obj_set_y(lv_obj_35, 7);
    lv_obj_set_width(lv_obj_35, 50);
    lv_obj_set_height(lv_obj_35, 23);
    lv_obj_add_style(lv_obj_35, &st_ff_ego_roof, 0);
    
    lv_obj_t * lv_obj_36 = lv_obj_create(lv_obj_34);
    lv_obj_set_x(lv_obj_36, 26);
    lv_obj_set_y(lv_obj_36, 13);
    lv_obj_set_width(lv_obj_36, 36);
    lv_obj_set_height(lv_obj_36, 12);
    lv_obj_add_style(lv_obj_36, &st_ff_glass, 0);
    
    lv_obj_t * lv_obj_37 = lv_obj_create(lv_obj_34);
    lv_obj_set_x(lv_obj_37, 13);
    lv_obj_set_y(lv_obj_37, 39);
    lv_obj_set_width(lv_obj_37, 13);
    lv_obj_set_height(lv_obj_37, 8);
    lv_obj_set_style_bg_color(lv_obj_37, lv_color_hex(0xef4444), 0);
    lv_obj_set_style_bg_opa(lv_obj_37, (255 * 100 / 100), 0);
    lv_obj_set_style_border_width(lv_obj_37, 0, 0);
    lv_obj_set_style_radius(lv_obj_37, 3, 0);
    
    lv_obj_t * lv_obj_38 = lv_obj_create(lv_obj_34);
    lv_obj_set_x(lv_obj_38, 62);
    lv_obj_set_y(lv_obj_38, 39);
    lv_obj_set_width(lv_obj_38, 13);
    lv_obj_set_height(lv_obj_38, 8);
    lv_obj_set_style_bg_color(lv_obj_38, lv_color_hex(0xef4444), 0);
    lv_obj_set_style_bg_opa(lv_obj_38, (255 * 100 / 100), 0);
    lv_obj_set_style_border_width(lv_obj_38, 0, 0);
    lv_obj_set_style_radius(lv_obj_38, 3, 0);
    
    lv_obj_t * lv_obj_39 = lv_obj_create(lv_obj_34);
    lv_obj_set_x(lv_obj_39, 35);
    lv_obj_set_y(lv_obj_39, 48);
    lv_obj_set_width(lv_obj_39, 18);
    lv_obj_set_height(lv_obj_39, 8);
    lv_obj_set_style_bg_color(lv_obj_39, lv_color_hex(0xe2e8f0), 0);
    lv_obj_set_style_bg_opa(lv_obj_39, (255 * 100 / 100), 0);
    lv_obj_set_style_border_width(lv_obj_39, 0, 0);
    lv_obj_set_style_radius(lv_obj_39, 3, 0);
    
    lv_obj_t * lv_label_6 = lv_label_create(lv_obj_0);
    lv_obj_set_x(lv_label_6, 18);
    lv_obj_set_y(lv_label_6, 268);
    lv_label_bind_text(lv_label_6, &ff_lidar, NULL);
    lv_obj_set_style_text_color(lv_label_6, lv_color_hex(0xf43f5e), 0);
    lv_obj_set_style_text_font(lv_label_6, font_12, 0);
    
    lv_obj_t * lv_label_7 = lv_label_create(lv_obj_0);
    lv_obj_set_x(lv_label_7, 123);
    lv_obj_set_y(lv_label_7, 268);
    lv_label_bind_text(lv_label_7, &ff_target_range, NULL);
    lv_obj_set_style_text_color(lv_label_7, lv_color_hex(0xf8fafc), 0);
    lv_obj_set_style_text_font(lv_label_7, font_12, 0);
    
    lv_obj_t * lv_label_8 = lv_label_create(lv_obj_0);
    lv_obj_set_x(lv_label_8, 218);
    lv_obj_set_y(lv_label_8, 268);
    lv_label_bind_text(lv_label_8, &ff_left_sensor, NULL);
    lv_obj_set_style_text_color(lv_label_8, lv_color_hex(0x22d3ee), 0);
    lv_obj_set_style_text_font(lv_label_8, font_12, 0);
    
    lv_obj_t * lv_label_9 = lv_label_create(lv_obj_0);
    lv_obj_set_x(lv_label_9, 251);
    lv_obj_set_y(lv_label_9, 268);
    lv_label_bind_text(lv_label_9, &ff_right_sensor, NULL);
    lv_obj_set_style_text_color(lv_label_9, lv_color_hex(0x22d3ee), 0);
    lv_obj_set_style_text_font(lv_label_9, font_12, 0);

    LV_TRACE_OBJ_CREATE("finished");

    return lv_obj_0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

