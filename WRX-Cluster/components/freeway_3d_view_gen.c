/**
 * @file freeway_3d_view_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "freeway_3d_view_gen.h"
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

lv_obj_t * freeway_3d_view_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    lv_obj_t * lv_obj_0 = lv_obj_create(parent);
    lv_obj_set_name_static(lv_obj_0, "freeway_3d_view_#");
    lv_obj_set_width(lv_obj_0, 320);
    lv_obj_set_height(lv_obj_0, 306);

    lv_obj_add_style(lv_obj_0, &st_ff_scene, 0);
    lv_obj_t * lv_label_0 = lv_label_create(lv_obj_0);
    lv_label_set_text(lv_label_0, "RETROVISION");
    lv_obj_set_align(lv_label_0, LV_ALIGN_TOP_LEFT);
    lv_obj_set_x(lv_label_0, 18);
    lv_obj_set_y(lv_label_0, 12);
    lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0x8A8F99), 0);
    lv_obj_set_style_text_font(lv_label_0, font_12, 0);
    
    lv_obj_t * lv_label_1 = lv_label_create(lv_obj_0);
    lv_label_bind_text(lv_label_1, &ff_hud_mode, NULL);
    lv_obj_set_align(lv_label_1, LV_ALIGN_TOP_LEFT);
    lv_obj_set_x(lv_label_1, 112);
    lv_obj_set_y(lv_label_1, 12);
    lv_obj_set_style_text_color(lv_label_1, lv_color_hex(0x32D9FF), 0);
    lv_obj_set_style_text_font(lv_label_1, font_12, 0);
    
    lv_obj_t * lv_label_2 = lv_label_create(lv_obj_0);
    lv_label_bind_text(lv_label_2, &ff_status, NULL);
    lv_obj_set_align(lv_label_2, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_x(lv_label_2, -20);
    lv_obj_set_y(lv_label_2, 12);
    lv_obj_set_style_text_color(lv_label_2, lv_color_hex(0x32D9FF), 0);
    lv_obj_set_style_text_font(lv_label_2, font_12, 0);
    
    lv_obj_t * lv_obj_1 = lv_obj_create(lv_obj_0);
    lv_obj_set_width(lv_obj_1, 292);
    lv_obj_set_height(lv_obj_1, 228);
    lv_obj_set_align(lv_obj_1, LV_ALIGN_TOP_LEFT);
    lv_obj_set_x(lv_obj_1, 14);
    lv_obj_set_y(lv_obj_1, 34);
    lv_obj_add_style(lv_obj_1, &st_ff_sky, 0);
    lv_obj_t * lv_obj_2 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_2, 132);
    lv_obj_set_y(lv_obj_2, 42);
    lv_obj_set_width(lv_obj_2, 28);
    lv_obj_set_height(lv_obj_2, 24);
    lv_obj_add_style(lv_obj_2, &st_ff_road_far, 0);
    
    lv_obj_t * lv_obj_3 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_3, 112);
    lv_obj_set_y(lv_obj_3, 66);
    lv_obj_set_width(lv_obj_3, 68);
    lv_obj_set_height(lv_obj_3, 29);
    lv_obj_add_style(lv_obj_3, &st_ff_road_far, 0);
    
    lv_obj_t * lv_obj_4 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_4, 88);
    lv_obj_set_y(lv_obj_4, 94);
    lv_obj_set_width(lv_obj_4, 116);
    lv_obj_set_height(lv_obj_4, 35);
    lv_obj_add_style(lv_obj_4, &st_ff_road_mid, 0);
    
    lv_obj_t * lv_obj_5 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_5, 58);
    lv_obj_set_y(lv_obj_5, 128);
    lv_obj_set_width(lv_obj_5, 176);
    lv_obj_set_height(lv_obj_5, 42);
    lv_obj_add_style(lv_obj_5, &st_ff_road_mid, 0);
    
    lv_obj_t * lv_obj_6 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_6, 26);
    lv_obj_set_y(lv_obj_6, 166);
    lv_obj_set_width(lv_obj_6, 240);
    lv_obj_set_height(lv_obj_6, 48);
    lv_obj_add_style(lv_obj_6, &st_ff_road_near, 0);
    
    lv_obj_t * lv_obj_7 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_7, 114);
    lv_obj_set_y(lv_obj_7, 45);
    lv_obj_set_width(lv_obj_7, 4);
    lv_obj_set_height(lv_obj_7, 26);
    lv_obj_add_style(lv_obj_7, &st_ff_edge_yellow, 0);
    
    lv_obj_t * lv_obj_8 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_8, 94);
    lv_obj_set_y(lv_obj_8, 76);
    lv_obj_set_width(lv_obj_8, 5);
    lv_obj_set_height(lv_obj_8, 31);
    lv_obj_add_style(lv_obj_8, &st_ff_edge_yellow, 0);
    
    lv_obj_t * lv_obj_9 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_9, 68);
    lv_obj_set_y(lv_obj_9, 112);
    lv_obj_set_width(lv_obj_9, 5);
    lv_obj_set_height(lv_obj_9, 40);
    lv_obj_add_style(lv_obj_9, &st_ff_edge_yellow, 0);
    
    lv_obj_t * lv_obj_10 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_10, 34);
    lv_obj_set_y(lv_obj_10, 154);
    lv_obj_set_width(lv_obj_10, 6);
    lv_obj_set_height(lv_obj_10, 54);
    lv_obj_add_style(lv_obj_10, &st_ff_edge_yellow, 0);
    
    lv_obj_t * lv_obj_11 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_11, 174);
    lv_obj_set_y(lv_obj_11, 45);
    lv_obj_set_width(lv_obj_11, 4);
    lv_obj_set_height(lv_obj_11, 26);
    lv_obj_add_style(lv_obj_11, &st_ff_edge_white, 0);
    
    lv_obj_t * lv_obj_12 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_12, 193);
    lv_obj_set_y(lv_obj_12, 76);
    lv_obj_set_width(lv_obj_12, 5);
    lv_obj_set_height(lv_obj_12, 31);
    lv_obj_add_style(lv_obj_12, &st_ff_edge_white, 0);
    
    lv_obj_t * lv_obj_13 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_13, 219);
    lv_obj_set_y(lv_obj_13, 112);
    lv_obj_set_width(lv_obj_13, 5);
    lv_obj_set_height(lv_obj_13, 40);
    lv_obj_add_style(lv_obj_13, &st_ff_edge_white, 0);
    
    lv_obj_t * lv_obj_14 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_14, 252);
    lv_obj_set_y(lv_obj_14, 154);
    lv_obj_set_width(lv_obj_14, 6);
    lv_obj_set_height(lv_obj_14, 54);
    lv_obj_add_style(lv_obj_14, &st_ff_edge_white, 0);
    
    lv_obj_t * lv_obj_15 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_15, 144);
    lv_obj_set_y(lv_obj_15, 58);
    lv_obj_set_width(lv_obj_15, 3);
    lv_obj_set_height(lv_obj_15, 15);
    lv_obj_add_style(lv_obj_15, &st_ff_lane, 0);
    
    lv_obj_t * lv_obj_16 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_16, 143);
    lv_obj_set_y(lv_obj_16, 88);
    lv_obj_set_width(lv_obj_16, 4);
    lv_obj_set_height(lv_obj_16, 21);
    lv_obj_add_style(lv_obj_16, &st_ff_lane, 0);
    
    lv_obj_t * lv_obj_17 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_17, 142);
    lv_obj_set_y(lv_obj_17, 126);
    lv_obj_set_width(lv_obj_17, 5);
    lv_obj_set_height(lv_obj_17, 29);
    lv_obj_add_style(lv_obj_17, &st_ff_lane, 0);
    
    lv_obj_t * lv_obj_18 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_18, 141);
    lv_obj_set_y(lv_obj_18, 174);
    lv_obj_set_width(lv_obj_18, 7);
    lv_obj_set_height(lv_obj_18, 36);
    lv_obj_add_style(lv_obj_18, &st_ff_lane, 0);
    
    lv_obj_t * lv_obj_19 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_19, 78);
    lv_obj_set_y(lv_obj_19, 82);
    lv_obj_set_width(lv_obj_19, 136);
    lv_obj_set_height(lv_obj_19, 1);
    lv_obj_set_style_bg_color(lv_obj_19, lv_color_hex(0x32D9FF), 0);
    lv_obj_set_style_bg_opa(lv_obj_19, 42, 0);
    lv_obj_set_style_border_width(lv_obj_19, 0, 0);
    
    lv_obj_t * lv_obj_20 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_20, 30);
    lv_obj_set_y(lv_obj_20, 178);
    lv_obj_set_width(lv_obj_20, 232);
    lv_obj_set_height(lv_obj_20, 1);
    lv_obj_set_style_bg_color(lv_obj_20, lv_color_hex(0x32D9FF), 0);
    lv_obj_set_style_bg_opa(lv_obj_20, 70, 0);
    lv_obj_set_style_border_width(lv_obj_20, 0, 0);
    
    lv_obj_t * lv_label_3 = lv_label_create(lv_obj_1);
    lv_label_set_text(lv_label_3, "50 ft");
    lv_obj_set_x(lv_label_3, 155);
    lv_obj_set_y(lv_label_3, 70);
    lv_obj_set_style_text_color(lv_label_3, lv_color_hex(0x8A8F99), 0);
    lv_obj_set_style_text_font(lv_label_3, font_12, 0);
    
    lv_obj_t * lv_obj_21 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_21, 125);
    lv_obj_set_y(lv_obj_21, 56);
    lv_obj_set_width(lv_obj_21, 3);
    lv_obj_set_height(lv_obj_21, 148);
    lv_obj_add_style(lv_obj_21, &st_ff_sensor, 0);
    
    lv_obj_t * lv_obj_22 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_22, 165);
    lv_obj_set_y(lv_obj_22, 56);
    lv_obj_set_width(lv_obj_22, 3);
    lv_obj_set_height(lv_obj_22, 148);
    lv_obj_add_style(lv_obj_22, &st_ff_sensor, 0);
    
    lv_obj_t * lv_obj_23 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_23, 146);
    lv_obj_set_y(lv_obj_23, 47);
    lv_obj_set_width(lv_obj_23, 2);
    lv_obj_set_height(lv_obj_23, 162);
    lv_obj_add_style(lv_obj_23, &st_ff_lidar, 0);
    
    lv_obj_t * lv_obj_24 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_24, 140);
    lv_obj_set_y(lv_obj_24, 104);
    lv_obj_set_width(lv_obj_24, 13);
    lv_obj_set_height(lv_obj_24, 13);
    lv_obj_set_style_bg_color(lv_obj_24, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(lv_obj_24, 255, 0);
    lv_obj_set_style_border_width(lv_obj_24, 0, 0);
    lv_obj_set_style_radius(lv_obj_24, 6, 0);
    lv_obj_set_style_pad_all(lv_obj_24, 0, 0);
    
    lv_obj_t * lv_obj_25 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_25, 122);
    lv_obj_set_y(lv_obj_25, 66);
    lv_obj_set_width(lv_obj_25, 48);
    lv_obj_set_height(lv_obj_25, 4);
    lv_obj_add_style(lv_obj_25, &st_ff_car_shadow, 0);
    
    lv_obj_t * lv_obj_26 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_26, 118);
    lv_obj_set_y(lv_obj_26, 60);
    lv_obj_set_width(lv_obj_26, 56);
    lv_obj_set_height(lv_obj_26, 46);
    lv_obj_add_style(lv_obj_26, &st_ff_alert_body, 0);
    lv_obj_t * lv_obj_27 = lv_obj_create(lv_obj_26);
    lv_obj_set_x(lv_obj_27, 3);
    lv_obj_set_y(lv_obj_27, 16);
    lv_obj_set_width(lv_obj_27, 8);
    lv_obj_set_height(lv_obj_27, 8);
    lv_obj_add_style(lv_obj_27, &st_ff_car_wheel, 0);
    
    lv_obj_t * lv_obj_28 = lv_obj_create(lv_obj_26);
    lv_obj_set_x(lv_obj_28, 45);
    lv_obj_set_y(lv_obj_28, 16);
    lv_obj_set_width(lv_obj_28, 8);
    lv_obj_set_height(lv_obj_28, 8);
    lv_obj_add_style(lv_obj_28, &st_ff_car_wheel, 0);
    
    lv_obj_t * lv_obj_29 = lv_obj_create(lv_obj_26);
    lv_obj_set_x(lv_obj_29, 3);
    lv_obj_set_y(lv_obj_29, 31);
    lv_obj_set_width(lv_obj_29, 8);
    lv_obj_set_height(lv_obj_29, 8);
    lv_obj_add_style(lv_obj_29, &st_ff_car_wheel, 0);
    
    lv_obj_t * lv_obj_30 = lv_obj_create(lv_obj_26);
    lv_obj_set_x(lv_obj_30, 45);
    lv_obj_set_y(lv_obj_30, 31);
    lv_obj_set_width(lv_obj_30, 8);
    lv_obj_set_height(lv_obj_30, 8);
    lv_obj_add_style(lv_obj_30, &st_ff_car_wheel, 0);
    
    lv_obj_t * lv_obj_31 = lv_obj_create(lv_obj_26);
    lv_obj_set_x(lv_obj_31, 8);
    lv_obj_set_y(lv_obj_31, 10);
    lv_obj_set_width(lv_obj_31, 40);
    lv_obj_set_height(lv_obj_31, 18);
    lv_obj_add_style(lv_obj_31, &st_ff_bumper, 0);
    
    lv_obj_t * lv_obj_32 = lv_obj_create(lv_obj_26);
    lv_obj_set_x(lv_obj_32, 13);
    lv_obj_set_y(lv_obj_32, 5);
    lv_obj_set_width(lv_obj_32, 30);
    lv_obj_set_height(lv_obj_32, 15);
    lv_obj_add_style(lv_obj_32, &st_ff_glass, 0);
    
    lv_obj_t * lv_obj_33 = lv_obj_create(lv_obj_26);
    lv_obj_set_x(lv_obj_33, 18);
    lv_obj_set_y(lv_obj_33, 9);
    lv_obj_set_width(lv_obj_33, 9);
    lv_obj_set_height(lv_obj_33, 8);
    lv_obj_add_style(lv_obj_33, &st_ff_body_hi, 0);
    
    lv_obj_t * lv_obj_34 = lv_obj_create(lv_obj_26);
    lv_obj_set_x(lv_obj_34, 29);
    lv_obj_set_y(lv_obj_34, 9);
    lv_obj_set_width(lv_obj_34, 2);
    lv_obj_set_height(lv_obj_34, 8);
    lv_obj_add_style(lv_obj_34, &st_ff_body_lo, 0);
    
    lv_obj_t * lv_obj_35 = lv_obj_create(lv_obj_26);
    lv_obj_set_x(lv_obj_35, 10);
    lv_obj_set_y(lv_obj_35, 28);
    lv_obj_set_width(lv_obj_35, 10);
    lv_obj_set_height(lv_obj_35, 3);
    lv_obj_add_style(lv_obj_35, &st_ff_taillight, 0);
    
    lv_obj_t * lv_obj_36 = lv_obj_create(lv_obj_26);
    lv_obj_set_x(lv_obj_36, 36);
    lv_obj_set_y(lv_obj_36, 28);
    lv_obj_set_width(lv_obj_36, 10);
    lv_obj_set_height(lv_obj_36, 3);
    lv_obj_add_style(lv_obj_36, &st_ff_taillight, 0);
    
    lv_obj_t * lv_obj_37 = lv_obj_create(lv_obj_26);
    lv_obj_set_x(lv_obj_37, 23);
    lv_obj_set_y(lv_obj_37, 33);
    lv_obj_set_width(lv_obj_37, 10);
    lv_obj_set_height(lv_obj_37, 4);
    lv_obj_add_style(lv_obj_37, &st_ff_plate, 0);
    
    lv_obj_t * lv_obj_38 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_38, 113);
    lv_obj_set_y(lv_obj_38, 55);
    lv_obj_set_width(lv_obj_38, 66);
    lv_obj_set_height(lv_obj_38, 56);
    lv_obj_add_style(lv_obj_38, &st_ff_detect, 0);
    
    lv_obj_t * lv_obj_39 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_39, 113);
    lv_obj_set_y(lv_obj_39, 55);
    lv_obj_set_width(lv_obj_39, 14);
    lv_obj_set_height(lv_obj_39, 2);
    lv_obj_set_style_bg_color(lv_obj_39, lv_color_hex(0x32D9FF), 0);
    lv_obj_set_style_bg_opa(lv_obj_39, 255, 0);
    lv_obj_set_style_border_width(lv_obj_39, 0, 0);
    
    lv_obj_t * lv_obj_40 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_40, 113);
    lv_obj_set_y(lv_obj_40, 55);
    lv_obj_set_width(lv_obj_40, 2);
    lv_obj_set_height(lv_obj_40, 14);
    lv_obj_set_style_bg_color(lv_obj_40, lv_color_hex(0x32D9FF), 0);
    lv_obj_set_style_bg_opa(lv_obj_40, 255, 0);
    lv_obj_set_style_border_width(lv_obj_40, 0, 0);
    
    lv_obj_t * lv_obj_41 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_41, 165);
    lv_obj_set_y(lv_obj_41, 55);
    lv_obj_set_width(lv_obj_41, 14);
    lv_obj_set_height(lv_obj_41, 2);
    lv_obj_set_style_bg_color(lv_obj_41, lv_color_hex(0x32D9FF), 0);
    lv_obj_set_style_bg_opa(lv_obj_41, 255, 0);
    lv_obj_set_style_border_width(lv_obj_41, 0, 0);
    
    lv_obj_t * lv_obj_42 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_42, 177);
    lv_obj_set_y(lv_obj_42, 55);
    lv_obj_set_width(lv_obj_42, 2);
    lv_obj_set_height(lv_obj_42, 14);
    lv_obj_set_style_bg_color(lv_obj_42, lv_color_hex(0x32D9FF), 0);
    lv_obj_set_style_bg_opa(lv_obj_42, 255, 0);
    lv_obj_set_style_border_width(lv_obj_42, 0, 0);
    
    lv_obj_t * lv_obj_43 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_43, 202);
    lv_obj_set_y(lv_obj_43, 126);
    lv_obj_set_width(lv_obj_43, 43);
    lv_obj_set_height(lv_obj_43, 33);
    lv_obj_set_style_bg_color(lv_obj_43, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(lv_obj_43, 80, 0);
    lv_obj_set_style_border_width(lv_obj_43, 0, 0);
    lv_obj_set_style_radius(lv_obj_43, 3, 0);
    lv_obj_set_style_pad_all(lv_obj_43, 0, 0);
    
    lv_obj_t * lv_obj_44 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_44, 199);
    lv_obj_set_y(lv_obj_44, 116);
    lv_obj_set_width(lv_obj_44, 48);
    lv_obj_set_height(lv_obj_44, 45);
    lv_obj_add_style(lv_obj_44, &st_ff_target_body, 0);
    lv_obj_t * lv_obj_45 = lv_obj_create(lv_obj_44);
    lv_obj_set_x(lv_obj_45, 3);
    lv_obj_set_y(lv_obj_45, 12);
    lv_obj_set_width(lv_obj_45, 7);
    lv_obj_set_height(lv_obj_45, 8);
    lv_obj_add_style(lv_obj_45, &st_ff_car_wheel, 0);
    
    lv_obj_t * lv_obj_46 = lv_obj_create(lv_obj_44);
    lv_obj_set_x(lv_obj_46, 38);
    lv_obj_set_y(lv_obj_46, 12);
    lv_obj_set_width(lv_obj_46, 7);
    lv_obj_set_height(lv_obj_46, 8);
    lv_obj_add_style(lv_obj_46, &st_ff_car_wheel, 0);
    
    lv_obj_t * lv_obj_47 = lv_obj_create(lv_obj_44);
    lv_obj_set_x(lv_obj_47, 3);
    lv_obj_set_y(lv_obj_47, 29);
    lv_obj_set_width(lv_obj_47, 7);
    lv_obj_set_height(lv_obj_47, 8);
    lv_obj_add_style(lv_obj_47, &st_ff_car_wheel, 0);
    
    lv_obj_t * lv_obj_48 = lv_obj_create(lv_obj_44);
    lv_obj_set_x(lv_obj_48, 38);
    lv_obj_set_y(lv_obj_48, 29);
    lv_obj_set_width(lv_obj_48, 7);
    lv_obj_set_height(lv_obj_48, 8);
    lv_obj_add_style(lv_obj_48, &st_ff_car_wheel, 0);
    
    lv_obj_t * lv_obj_49 = lv_obj_create(lv_obj_44);
    lv_obj_set_x(lv_obj_49, 8);
    lv_obj_set_y(lv_obj_49, 8);
    lv_obj_set_width(lv_obj_49, 32);
    lv_obj_set_height(lv_obj_49, 16);
    lv_obj_add_style(lv_obj_49, &st_ff_glass, 0);
    
    lv_obj_t * lv_obj_50 = lv_obj_create(lv_obj_44);
    lv_obj_set_x(lv_obj_50, 12);
    lv_obj_set_y(lv_obj_50, 11);
    lv_obj_set_width(lv_obj_50, 8);
    lv_obj_set_height(lv_obj_50, 9);
    lv_obj_add_style(lv_obj_50, &st_ff_body_hi, 0);
    
    lv_obj_t * lv_obj_51 = lv_obj_create(lv_obj_44);
    lv_obj_set_x(lv_obj_51, 23);
    lv_obj_set_y(lv_obj_51, 11);
    lv_obj_set_width(lv_obj_51, 2);
    lv_obj_set_height(lv_obj_51, 9);
    lv_obj_add_style(lv_obj_51, &st_ff_body_lo, 0);
    
    lv_obj_t * lv_obj_52 = lv_obj_create(lv_obj_44);
    lv_obj_set_x(lv_obj_52, 8);
    lv_obj_set_y(lv_obj_52, 29);
    lv_obj_set_width(lv_obj_52, 10);
    lv_obj_set_height(lv_obj_52, 3);
    lv_obj_add_style(lv_obj_52, &st_ff_taillight, 0);
    
    lv_obj_t * lv_obj_53 = lv_obj_create(lv_obj_44);
    lv_obj_set_x(lv_obj_53, 30);
    lv_obj_set_y(lv_obj_53, 29);
    lv_obj_set_width(lv_obj_53, 10);
    lv_obj_set_height(lv_obj_53, 3);
    lv_obj_add_style(lv_obj_53, &st_ff_taillight, 0);
    
    lv_obj_t * lv_obj_54 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_54, 92);
    lv_obj_set_y(lv_obj_54, 155);
    lv_obj_set_width(lv_obj_54, 108);
    lv_obj_set_height(lv_obj_54, 55);
    lv_obj_add_style(lv_obj_54, &st_ff_car_shadow, 0);
    
    lv_obj_t * lv_obj_55 = lv_obj_create(lv_obj_1);
    lv_obj_set_x(lv_obj_55, 91);
    lv_obj_set_y(lv_obj_55, 137);
    lv_obj_set_width(lv_obj_55, 110);
    lv_obj_set_height(lv_obj_55, 78);
    lv_obj_add_style(lv_obj_55, &st_ff_ego_body, 0);
    lv_obj_t * lv_obj_56 = lv_obj_create(lv_obj_55);
    lv_obj_set_x(lv_obj_56, 6);
    lv_obj_set_y(lv_obj_56, 16);
    lv_obj_set_width(lv_obj_56, 15);
    lv_obj_set_height(lv_obj_56, 15);
    lv_obj_add_style(lv_obj_56, &st_ff_car_wheel, 0);
    
    lv_obj_t * lv_obj_57 = lv_obj_create(lv_obj_55);
    lv_obj_set_x(lv_obj_57, 89);
    lv_obj_set_y(lv_obj_57, 16);
    lv_obj_set_width(lv_obj_57, 15);
    lv_obj_set_height(lv_obj_57, 15);
    lv_obj_add_style(lv_obj_57, &st_ff_car_wheel, 0);
    
    lv_obj_t * lv_obj_58 = lv_obj_create(lv_obj_55);
    lv_obj_set_x(lv_obj_58, 6);
    lv_obj_set_y(lv_obj_58, 52);
    lv_obj_set_width(lv_obj_58, 15);
    lv_obj_set_height(lv_obj_58, 15);
    lv_obj_add_style(lv_obj_58, &st_ff_car_wheel, 0);
    
    lv_obj_t * lv_obj_59 = lv_obj_create(lv_obj_55);
    lv_obj_set_x(lv_obj_59, 89);
    lv_obj_set_y(lv_obj_59, 52);
    lv_obj_set_width(lv_obj_59, 15);
    lv_obj_set_height(lv_obj_59, 15);
    lv_obj_add_style(lv_obj_59, &st_ff_car_wheel, 0);
    
    lv_obj_t * lv_obj_60 = lv_obj_create(lv_obj_55);
    lv_obj_set_x(lv_obj_60, 12);
    lv_obj_set_y(lv_obj_60, 3);
    lv_obj_set_width(lv_obj_60, 86);
    lv_obj_set_height(lv_obj_60, 17);
    lv_obj_add_style(lv_obj_60, &st_ff_ego_roof, 0);
    
    lv_obj_t * lv_obj_61 = lv_obj_create(lv_obj_55);
    lv_obj_set_x(lv_obj_61, 20);
    lv_obj_set_y(lv_obj_61, 8);
    lv_obj_set_width(lv_obj_61, 70);
    lv_obj_set_height(lv_obj_61, 9);
    lv_obj_add_style(lv_obj_61, &st_ff_glass, 0);
    
    lv_obj_t * lv_obj_62 = lv_obj_create(lv_obj_55);
    lv_obj_set_x(lv_obj_62, 18);
    lv_obj_set_y(lv_obj_62, 28);
    lv_obj_set_width(lv_obj_62, 74);
    lv_obj_set_height(lv_obj_62, 26);
    lv_obj_add_style(lv_obj_62, &st_ff_bumper, 0);
    
    lv_obj_t * lv_obj_63 = lv_obj_create(lv_obj_55);
    lv_obj_set_x(lv_obj_63, 24);
    lv_obj_set_y(lv_obj_63, 34);
    lv_obj_set_width(lv_obj_63, 62);
    lv_obj_set_height(lv_obj_63, 10);
    lv_obj_add_style(lv_obj_63, &st_ff_glass, 0);
    
    lv_obj_t * lv_obj_64 = lv_obj_create(lv_obj_55);
    lv_obj_set_x(lv_obj_64, 51);
    lv_obj_set_y(lv_obj_64, 34);
    lv_obj_set_width(lv_obj_64, 2);
    lv_obj_set_height(lv_obj_64, 10);
    lv_obj_add_style(lv_obj_64, &st_ff_body_lo, 0);
    
    lv_obj_t * lv_obj_65 = lv_obj_create(lv_obj_55);
    lv_obj_set_x(lv_obj_65, 12);
    lv_obj_set_y(lv_obj_65, 56);
    lv_obj_set_width(lv_obj_65, 24);
    lv_obj_set_height(lv_obj_65, 3);
    lv_obj_add_style(lv_obj_65, &st_ff_taillight, 0);
    
    lv_obj_t * lv_obj_66 = lv_obj_create(lv_obj_55);
    lv_obj_set_x(lv_obj_66, 74);
    lv_obj_set_y(lv_obj_66, 56);
    lv_obj_set_width(lv_obj_66, 24);
    lv_obj_set_height(lv_obj_66, 3);
    lv_obj_add_style(lv_obj_66, &st_ff_taillight, 0);
    
    lv_obj_t * lv_obj_67 = lv_obj_create(lv_obj_55);
    lv_obj_set_x(lv_obj_67, 42);
    lv_obj_set_y(lv_obj_67, 61);
    lv_obj_set_width(lv_obj_67, 26);
    lv_obj_set_height(lv_obj_67, 7);
    lv_obj_add_style(lv_obj_67, &st_ff_plate, 0);
    
    lv_obj_t * lv_obj_68 = lv_obj_create(lv_obj_55);
    lv_obj_set_x(lv_obj_68, 13);
    lv_obj_set_y(lv_obj_68, 4);
    lv_obj_set_width(lv_obj_68, 24);
    lv_obj_set_height(lv_obj_68, 2);
    lv_obj_add_style(lv_obj_68, &st_ff_headlight, 0);
    
    lv_obj_t * lv_obj_69 = lv_obj_create(lv_obj_55);
    lv_obj_set_x(lv_obj_69, 73);
    lv_obj_set_y(lv_obj_69, 4);
    lv_obj_set_width(lv_obj_69, 24);
    lv_obj_set_height(lv_obj_69, 2);
    lv_obj_add_style(lv_obj_69, &st_ff_headlight, 0);
    
    lv_obj_t * lv_label_4 = lv_label_create(lv_obj_0);
    lv_label_bind_text(lv_label_4, &ff_lidar, NULL);
    lv_obj_set_x(lv_label_4, 18);
    lv_obj_set_y(lv_label_4, 274);
    lv_obj_set_style_text_color(lv_label_4, lv_color_hex(0xFF3B30), 0);
    lv_obj_set_style_text_font(lv_label_4, font_12, 0);
    
    lv_obj_t * lv_label_5 = lv_label_create(lv_obj_0);
    lv_label_bind_text(lv_label_5, &ff_target_range, NULL);
    lv_obj_set_x(lv_label_5, 216);
    lv_obj_set_y(lv_label_5, 274);
    lv_obj_set_style_text_color(lv_label_5, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(lv_label_5, font_12, 0);

    LV_TRACE_OBJ_CREATE("finished");

    return lv_obj_0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

