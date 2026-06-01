/**
 * @file lvgl_editor_gen.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "lvgl_editor_gen.h"

#if LV_USE_XML
#endif /* LV_USE_XML */

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/*----------------
 * Translations
 *----------------*/

/**********************
 *  GLOBAL VARIABLES
 **********************/

/*--------------------
 *  Permanent screens
 *-------------------*/

lv_obj_t * lvgl_studio_flash = NULL;

/*----------------
 * Fonts
 *----------------*/

lv_font_t * font_12;
extern lv_font_t font_12_data;
lv_font_t * font_16;
extern lv_font_t font_16_data;
lv_font_t * font_18;
extern lv_font_t font_18_data;
lv_font_t * font_20;
extern lv_font_t font_20_data;
lv_font_t * font_24;
extern lv_font_t font_24_data;
lv_font_t * font_32;
extern lv_font_t font_32_data;
lv_font_t * font_48;
extern lv_font_t font_48_data;
lv_font_t * font_64;
extern lv_font_t font_64_data;
lv_font_t * font_96;
extern lv_font_t font_96_data;
lv_font_t * bs_icons_24;
extern lv_font_t bs_icons_24_data;
lv_font_t * bs_icons_48;
extern lv_font_t bs_icons_48_data;

/*----------------
 * Images
 *----------------*/

/*----------------
 * Global styles
 *----------------*/

lv_style_t st_black;
lv_style_t st_arc_track_rpm;
lv_style_t st_arc_fill_rpm;
lv_style_t st_arc_track_mph;
lv_style_t st_arc_fill_mph;
lv_style_t st_arc_knob_hidden;
lv_style_t st_panel;
lv_style_t st_panel_soft;
lv_style_t st_text_primary;
lv_style_t st_text_secondary;
lv_style_t st_text_muted;
lv_style_t st_text_dim;
lv_style_t st_speed_text;
lv_style_t st_value_text;
lv_style_t st_small_text;
lv_style_t st_tiny_text;
lv_style_t st_bar_track;
lv_style_t st_speed_indicator;
lv_style_t st_fuel_track;
lv_style_t st_fuel_indicator;
lv_style_t st_rpm_track;
lv_style_t st_rpm_indicator;
lv_style_t st_temp_track;
lv_style_t st_temp_indicator;
lv_style_t st_blue_indicator;
lv_style_t st_road;
lv_style_t st_road_line;
lv_style_t st_lane_shadow;
lv_style_t st_car_body;
lv_style_t st_car_glass;
lv_style_t st_chip;
lv_style_t st_ff_scene;
lv_style_t st_ff_sky;
lv_style_t st_ff_road_far;
lv_style_t st_ff_road_mid;
lv_style_t st_ff_road_near;
lv_style_t st_ff_lane;
lv_style_t st_ff_edge_yellow;
lv_style_t st_ff_edge_white;
lv_style_t st_ff_sensor;
lv_style_t st_ff_lidar;
lv_style_t st_ff_detect;
lv_style_t st_ff_ego_body;
lv_style_t st_ff_ego_roof;
lv_style_t st_ff_glass;
lv_style_t st_ff_target_body;
lv_style_t st_ff_alert_body;

/*----------------
 * Subjects
 *----------------*/

lv_subject_t sp_speed;
lv_subject_t sp_speed_bar;
lv_subject_t sp_fuel_range;
lv_subject_t sp_fuel_bar;
lv_subject_t sp_drive_mode;
lv_subject_t sp_gear;
lv_subject_t sp_rpm_bar;
lv_subject_t sp_temp;
lv_subject_t sp_temp_bar;
lv_subject_t sp_odo;
lv_subject_t ff_hud_mode;
lv_subject_t ff_status;
lv_subject_t ff_target_range;
lv_subject_t ff_lidar;
lv_subject_t ff_left_sensor;
lv_subject_t ff_right_sensor;
lv_subject_t flash_rpm;
lv_subject_t flash_mph;
lv_subject_t flash_fuel;
lv_subject_t flash_fuel_bar;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lvgl_editor_init_gen(const char * asset_path)
{
    char buf[256];


    /*----------------
     * Fonts
     *----------------*/

    /* get font 'font_12' from a C array */
    font_12 = &font_12_data;
    /* get font 'font_16' from a C array */
    font_16 = &font_16_data;
    /* get font 'font_18' from a C array */
    font_18 = &font_18_data;
    /* get font 'font_20' from a C array */
    font_20 = &font_20_data;
    /* get font 'font_24' from a C array */
    font_24 = &font_24_data;
    /* get font 'font_32' from a C array */
    font_32 = &font_32_data;
    /* get font 'font_48' from a C array */
    font_48 = &font_48_data;
    /* get font 'font_64' from a C array */
    font_64 = &font_64_data;
    /* get font 'font_96' from a C array */
    font_96 = &font_96_data;
    /* get font 'bs_icons_24' from a C array */
    bs_icons_24 = &bs_icons_24_data;
    /* get font 'bs_icons_48' from a C array */
    bs_icons_48 = &bs_icons_48_data;


    /*----------------
     * Images
     *----------------*/
    /*----------------
     * Global styles
     *----------------*/

    static bool style_inited = false;

    if (!style_inited) {
        lv_style_init(&st_black);
        lv_style_set_bg_color(&st_black, lv_color_hex(0x020305));
        lv_style_set_bg_opa(&st_black, (255 * 100 / 100));
        lv_style_set_border_width(&st_black, 0);
        lv_style_set_radius(&st_black, 0);
        lv_style_set_pad_all(&st_black, 0);

        lv_style_init(&st_arc_track_rpm);
        lv_style_set_arc_color(&st_arc_track_rpm, lv_color_hex(0x3a3108));
        lv_style_set_arc_width(&st_arc_track_rpm, 22);
        lv_style_set_arc_rounded(&st_arc_track_rpm, true);

        lv_style_init(&st_arc_fill_rpm);
        lv_style_set_arc_color(&st_arc_fill_rpm, lv_color_hex(0xffd21a));
        lv_style_set_arc_width(&st_arc_fill_rpm, 22);
        lv_style_set_arc_rounded(&st_arc_fill_rpm, true);

        lv_style_init(&st_arc_track_mph);
        lv_style_set_arc_color(&st_arc_track_mph, lv_color_hex(0x12224a));
        lv_style_set_arc_width(&st_arc_track_mph, 22);
        lv_style_set_arc_rounded(&st_arc_track_mph, true);

        lv_style_init(&st_arc_fill_mph);
        lv_style_set_arc_color(&st_arc_fill_mph, lv_color_hex(0x0a84ff));
        lv_style_set_arc_width(&st_arc_fill_mph, 22);
        lv_style_set_arc_rounded(&st_arc_fill_mph, true);

        lv_style_init(&st_arc_knob_hidden);
        lv_style_set_bg_opa(&st_arc_knob_hidden, (255 * 0 / 100));
        lv_style_set_border_width(&st_arc_knob_hidden, 0);
        lv_style_set_pad_all(&st_arc_knob_hidden, -20);

        lv_style_init(&st_panel);
        lv_style_set_bg_color(&st_panel, lv_color_hex(0x101113));
        lv_style_set_bg_opa(&st_panel, (255 * 100 / 100));
        lv_style_set_border_width(&st_panel, 0);
        lv_style_set_radius(&st_panel, 28);
        lv_style_set_pad_all(&st_panel, 0);

        lv_style_init(&st_panel_soft);
        lv_style_set_bg_color(&st_panel_soft, lv_color_hex(0x191a1d));
        lv_style_set_bg_opa(&st_panel_soft, (255 * 70 / 100));
        lv_style_set_border_width(&st_panel_soft, 0);
        lv_style_set_radius(&st_panel_soft, 18);
        lv_style_set_pad_all(&st_panel_soft, 0);

        lv_style_init(&st_text_primary);
        lv_style_set_text_color(&st_text_primary, lv_color_hex(0xf5f7fb));

        lv_style_init(&st_text_secondary);
        lv_style_set_text_color(&st_text_secondary, lv_color_hex(0xd8dbe2));

        lv_style_init(&st_text_muted);
        lv_style_set_text_color(&st_text_muted, lv_color_hex(0x8c919b));

        lv_style_init(&st_text_dim);
        lv_style_set_text_color(&st_text_dim, lv_color_hex(0x444951));

        lv_style_init(&st_speed_text);
        lv_style_set_text_color(&st_speed_text, lv_color_hex(0xf5f7fb));
        lv_style_set_text_font(&st_speed_text, font_96);

        lv_style_init(&st_value_text);
        lv_style_set_text_color(&st_value_text, lv_color_hex(0xf5f7fb));
        lv_style_set_text_font(&st_value_text, font_24);

        lv_style_init(&st_small_text);
        lv_style_set_text_color(&st_small_text, lv_color_hex(0xd8dbe2));
        lv_style_set_text_font(&st_small_text, font_16);

        lv_style_init(&st_tiny_text);
        lv_style_set_text_color(&st_tiny_text, lv_color_hex(0x8c919b));
        lv_style_set_text_font(&st_tiny_text, font_12);

        lv_style_init(&st_bar_track);
        lv_style_set_bg_color(&st_bar_track, lv_color_hex(0x35101d));
        lv_style_set_bg_opa(&st_bar_track, (255 * 100 / 100));
        lv_style_set_radius(&st_bar_track, 13);
        lv_style_set_border_width(&st_bar_track, 0);
        lv_style_set_pad_all(&st_bar_track, 0);

        lv_style_init(&st_speed_indicator);
        lv_style_set_bg_color(&st_speed_indicator, lv_color_hex(0xff1162));
        lv_style_set_bg_opa(&st_speed_indicator, (255 * 100 / 100));
        lv_style_set_radius(&st_speed_indicator, 13);

        lv_style_init(&st_fuel_track);
        lv_style_set_bg_color(&st_fuel_track, lv_color_hex(0x09330f));
        lv_style_set_bg_opa(&st_fuel_track, (255 * 100 / 100));
        lv_style_set_radius(&st_fuel_track, 13);
        lv_style_set_border_width(&st_fuel_track, 0);
        lv_style_set_pad_all(&st_fuel_track, 0);

        lv_style_init(&st_fuel_indicator);
        lv_style_set_bg_color(&st_fuel_indicator, lv_color_hex(0x12d31f));
        lv_style_set_bg_opa(&st_fuel_indicator, (255 * 100 / 100));
        lv_style_set_radius(&st_fuel_indicator, 13);

        lv_style_init(&st_rpm_track);
        lv_style_set_bg_color(&st_rpm_track, lv_color_hex(0x3a3108));
        lv_style_set_bg_opa(&st_rpm_track, (255 * 100 / 100));
        lv_style_set_radius(&st_rpm_track, 6);
        lv_style_set_border_width(&st_rpm_track, 0);
        lv_style_set_pad_all(&st_rpm_track, 0);

        lv_style_init(&st_rpm_indicator);
        lv_style_set_bg_color(&st_rpm_indicator, lv_color_hex(0xffd21a));
        lv_style_set_bg_opa(&st_rpm_indicator, (255 * 100 / 100));
        lv_style_set_radius(&st_rpm_indicator, 6);

        lv_style_init(&st_temp_track);
        lv_style_set_bg_color(&st_temp_track, lv_color_hex(0x32102b));
        lv_style_set_bg_opa(&st_temp_track, (255 * 100 / 100));
        lv_style_set_radius(&st_temp_track, 6);
        lv_style_set_border_width(&st_temp_track, 0);
        lv_style_set_pad_all(&st_temp_track, 0);

        lv_style_init(&st_temp_indicator);
        lv_style_set_bg_color(&st_temp_indicator, lv_color_hex(0xd719b9));
        lv_style_set_bg_opa(&st_temp_indicator, (255 * 100 / 100));
        lv_style_set_radius(&st_temp_indicator, 6);

        lv_style_init(&st_blue_indicator);
        lv_style_set_bg_color(&st_blue_indicator, lv_color_hex(0x0a84ff));
        lv_style_set_bg_opa(&st_blue_indicator, (255 * 100 / 100));
        lv_style_set_radius(&st_blue_indicator, 5);

        lv_style_init(&st_road);
        lv_style_set_bg_color(&st_road, lv_color_hex(0x222326));
        lv_style_set_bg_opa(&st_road, (255 * 100 / 100));
        lv_style_set_border_width(&st_road, 0);
        lv_style_set_radius(&st_road, 4);
        lv_style_set_pad_all(&st_road, 0);

        lv_style_init(&st_road_line);
        lv_style_set_bg_color(&st_road_line, lv_color_hex(0xe9edf4));
        lv_style_set_bg_opa(&st_road_line, (255 * 100 / 100));
        lv_style_set_border_width(&st_road_line, 0);
        lv_style_set_radius(&st_road_line, 2);
        lv_style_set_pad_all(&st_road_line, 0);

        lv_style_init(&st_lane_shadow);
        lv_style_set_bg_color(&st_lane_shadow, lv_color_hex(0x5b5f66));
        lv_style_set_bg_opa(&st_lane_shadow, (255 * 70 / 100));
        lv_style_set_border_width(&st_lane_shadow, 0);
        lv_style_set_radius(&st_lane_shadow, 2);
        lv_style_set_pad_all(&st_lane_shadow, 0);

        lv_style_init(&st_car_body);
        lv_style_set_bg_color(&st_car_body, lv_color_hex(0xeceff3));
        lv_style_set_bg_opa(&st_car_body, (255 * 100 / 100));
        lv_style_set_border_width(&st_car_body, 1);
        lv_style_set_border_color(&st_car_body, lv_color_hex(0xa7abb2));
        lv_style_set_radius(&st_car_body, 14);
        lv_style_set_pad_all(&st_car_body, 0);

        lv_style_init(&st_car_glass);
        lv_style_set_bg_color(&st_car_glass, lv_color_hex(0x8c929c));
        lv_style_set_bg_opa(&st_car_glass, (255 * 100 / 100));
        lv_style_set_border_width(&st_car_glass, 0);
        lv_style_set_radius(&st_car_glass, 8);
        lv_style_set_pad_all(&st_car_glass, 0);

        lv_style_init(&st_chip);
        lv_style_set_bg_color(&st_chip, lv_color_hex(0x5c6067));
        lv_style_set_bg_opa(&st_chip, (255 * 100 / 100));
        lv_style_set_border_width(&st_chip, 0);
        lv_style_set_radius(&st_chip, 20);
        lv_style_set_pad_all(&st_chip, 0);

        lv_style_init(&st_ff_scene);
        lv_style_set_bg_color(&st_ff_scene, lv_color_hex(0x07080a));
        lv_style_set_bg_opa(&st_ff_scene, (255 * 100 / 100));
        lv_style_set_border_width(&st_ff_scene, 0);
        lv_style_set_radius(&st_ff_scene, 26);
        lv_style_set_pad_all(&st_ff_scene, 0);

        lv_style_init(&st_ff_sky);
        lv_style_set_bg_color(&st_ff_sky, lv_color_hex(0x0d1118));
        lv_style_set_bg_opa(&st_ff_sky, (255 * 100 / 100));
        lv_style_set_border_width(&st_ff_sky, 0);
        lv_style_set_radius(&st_ff_sky, 18);
        lv_style_set_pad_all(&st_ff_sky, 0);

        lv_style_init(&st_ff_road_far);
        lv_style_set_bg_color(&st_ff_road_far, lv_color_hex(0x191d24));
        lv_style_set_bg_opa(&st_ff_road_far, (255 * 100 / 100));
        lv_style_set_border_width(&st_ff_road_far, 0);
        lv_style_set_radius(&st_ff_road_far, 4);
        lv_style_set_pad_all(&st_ff_road_far, 0);

        lv_style_init(&st_ff_road_mid);
        lv_style_set_bg_color(&st_ff_road_mid, lv_color_hex(0x222832));
        lv_style_set_bg_opa(&st_ff_road_mid, (255 * 100 / 100));
        lv_style_set_border_width(&st_ff_road_mid, 0);
        lv_style_set_radius(&st_ff_road_mid, 5);
        lv_style_set_pad_all(&st_ff_road_mid, 0);

        lv_style_init(&st_ff_road_near);
        lv_style_set_bg_color(&st_ff_road_near, lv_color_hex(0x303846));
        lv_style_set_bg_opa(&st_ff_road_near, (255 * 100 / 100));
        lv_style_set_border_width(&st_ff_road_near, 0);
        lv_style_set_radius(&st_ff_road_near, 6);
        lv_style_set_pad_all(&st_ff_road_near, 0);

        lv_style_init(&st_ff_lane);
        lv_style_set_bg_color(&st_ff_lane, lv_color_hex(0xedf3fb));
        lv_style_set_bg_opa(&st_ff_lane, (255 * 92 / 100));
        lv_style_set_border_width(&st_ff_lane, 0);
        lv_style_set_radius(&st_ff_lane, 2);
        lv_style_set_pad_all(&st_ff_lane, 0);

        lv_style_init(&st_ff_edge_yellow);
        lv_style_set_bg_color(&st_ff_edge_yellow, lv_color_hex(0xeab308));
        lv_style_set_bg_opa(&st_ff_edge_yellow, (255 * 100 / 100));
        lv_style_set_border_width(&st_ff_edge_yellow, 0);
        lv_style_set_radius(&st_ff_edge_yellow, 2);
        lv_style_set_pad_all(&st_ff_edge_yellow, 0);

        lv_style_init(&st_ff_edge_white);
        lv_style_set_bg_color(&st_ff_edge_white, lv_color_hex(0xf8fafc));
        lv_style_set_bg_opa(&st_ff_edge_white, (255 * 100 / 100));
        lv_style_set_border_width(&st_ff_edge_white, 0);
        lv_style_set_radius(&st_ff_edge_white, 2);
        lv_style_set_pad_all(&st_ff_edge_white, 0);

        lv_style_init(&st_ff_sensor);
        lv_style_set_bg_color(&st_ff_sensor, lv_color_hex(0x22d3ee));
        lv_style_set_bg_opa(&st_ff_sensor, (255 * 70 / 100));
        lv_style_set_border_width(&st_ff_sensor, 0);
        lv_style_set_radius(&st_ff_sensor, 2);
        lv_style_set_pad_all(&st_ff_sensor, 0);

        lv_style_init(&st_ff_lidar);
        lv_style_set_bg_color(&st_ff_lidar, lv_color_hex(0xf43f5e));
        lv_style_set_bg_opa(&st_ff_lidar, (255 * 85 / 100));
        lv_style_set_border_width(&st_ff_lidar, 0);
        lv_style_set_radius(&st_ff_lidar, 2);
        lv_style_set_pad_all(&st_ff_lidar, 0);

        lv_style_init(&st_ff_detect);
        lv_style_set_bg_opa(&st_ff_detect, 0);
        lv_style_set_border_width(&st_ff_detect, 1);
        lv_style_set_border_color(&st_ff_detect, lv_color_hex(0x22d3ee));
        lv_style_set_radius(&st_ff_detect, 6);
        lv_style_set_pad_all(&st_ff_detect, 0);

        lv_style_init(&st_ff_ego_body);
        lv_style_set_bg_color(&st_ff_ego_body, lv_color_hex(0xf1f5f9));
        lv_style_set_bg_opa(&st_ff_ego_body, (255 * 100 / 100));
        lv_style_set_border_width(&st_ff_ego_body, 1);
        lv_style_set_border_color(&st_ff_ego_body, lv_color_hex(0xffffff));
        lv_style_set_radius(&st_ff_ego_body, 14);
        lv_style_set_pad_all(&st_ff_ego_body, 0);

        lv_style_init(&st_ff_ego_roof);
        lv_style_set_bg_color(&st_ff_ego_roof, lv_color_hex(0xcbd5e1));
        lv_style_set_bg_opa(&st_ff_ego_roof, (255 * 100 / 100));
        lv_style_set_border_width(&st_ff_ego_roof, 0);
        lv_style_set_radius(&st_ff_ego_roof, 10);
        lv_style_set_pad_all(&st_ff_ego_roof, 0);

        lv_style_init(&st_ff_glass);
        lv_style_set_bg_color(&st_ff_glass, lv_color_hex(0x64748b));
        lv_style_set_bg_opa(&st_ff_glass, (255 * 100 / 100));
        lv_style_set_border_width(&st_ff_glass, 0);
        lv_style_set_radius(&st_ff_glass, 7);
        lv_style_set_pad_all(&st_ff_glass, 0);

        lv_style_init(&st_ff_target_body);
        lv_style_set_bg_color(&st_ff_target_body, lv_color_hex(0x64748b));
        lv_style_set_bg_opa(&st_ff_target_body, (255 * 100 / 100));
        lv_style_set_border_width(&st_ff_target_body, 1);
        lv_style_set_border_color(&st_ff_target_body, lv_color_hex(0xcbd5e1));
        lv_style_set_radius(&st_ff_target_body, 9);
        lv_style_set_pad_all(&st_ff_target_body, 0);

        lv_style_init(&st_ff_alert_body);
        lv_style_set_bg_color(&st_ff_alert_body, lv_color_hex(0xb91c1c));
        lv_style_set_bg_opa(&st_ff_alert_body, (255 * 100 / 100));
        lv_style_set_border_width(&st_ff_alert_body, 1);
        lv_style_set_border_color(&st_ff_alert_body, lv_color_hex(0xfecaca));
        lv_style_set_radius(&st_ff_alert_body, 9);
        lv_style_set_pad_all(&st_ff_alert_body, 0);

        style_inited = true;
    }

    /*----------------
     * Subjects
     *----------------*/
    lv_subject_init_int(&sp_speed, 60);
    lv_subject_init_int(&sp_speed_bar, 34);
    lv_subject_init_int(&sp_fuel_range, 200);
    lv_subject_init_int(&sp_fuel_bar, 50);
    static char sp_drive_mode_buf[UI_SUBJECT_STRING_LENGTH];
    static char sp_drive_mode_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&sp_drive_mode,
                           sp_drive_mode_buf,
                           sp_drive_mode_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "Sport"
                          );
    static char sp_gear_buf[UI_SUBJECT_STRING_LENGTH];
    static char sp_gear_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&sp_gear,
                           sp_gear_buf,
                           sp_gear_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "3"
                          );
    lv_subject_init_int(&sp_rpm_bar, 58);
    lv_subject_init_int(&sp_temp, 195);
    lv_subject_init_int(&sp_temp_bar, 52);
    static char sp_odo_buf[UI_SUBJECT_STRING_LENGTH];
    static char sp_odo_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&sp_odo,
                           sp_odo_buf,
                           sp_odo_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "1250 mi"
                          );
    static char ff_hud_mode_buf[UI_SUBJECT_STRING_LENGTH];
    static char ff_hud_mode_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&ff_hud_mode,
                           ff_hud_mode_buf,
                           ff_hud_mode_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "3D"
                          );
    static char ff_status_buf[UI_SUBJECT_STRING_LENGTH];
    static char ff_status_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&ff_status,
                           ff_status_buf,
                           ff_status_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "TRACK"
                          );
    static char ff_target_range_buf[UI_SUBJECT_STRING_LENGTH];
    static char ff_target_range_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&ff_target_range,
                           ff_target_range_buf,
                           ff_target_range_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "42 ft"
                          );
    static char ff_lidar_buf[UI_SUBJECT_STRING_LENGTH];
    static char ff_lidar_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&ff_lidar,
                           ff_lidar_buf,
                           ff_lidar_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "LiDAR 42ft"
                          );
    static char ff_left_sensor_buf[UI_SUBJECT_STRING_LENGTH];
    static char ff_left_sensor_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&ff_left_sensor,
                           ff_left_sensor_buf,
                           ff_left_sensor_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "L OK"
                          );
    static char ff_right_sensor_buf[UI_SUBJECT_STRING_LENGTH];
    static char ff_right_sensor_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&ff_right_sensor,
                           ff_right_sensor_buf,
                           ff_right_sensor_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "R OK"
                          );
    static char flash_rpm_buf[UI_SUBJECT_STRING_LENGTH];
    static char flash_rpm_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&flash_rpm,
                           flash_rpm_buf,
                           flash_rpm_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "0"
                          );
    static char flash_mph_buf[UI_SUBJECT_STRING_LENGTH];
    static char flash_mph_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&flash_mph,
                           flash_mph_buf,
                           flash_mph_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "0"
                          );
    static char flash_fuel_buf[UI_SUBJECT_STRING_LENGTH];
    static char flash_fuel_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&flash_fuel,
                           flash_fuel_buf,
                           flash_fuel_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "-- %"
                          );
    lv_subject_init_int(&flash_fuel_bar, 0);

    /*----------------
     * Translations
     *----------------*/

#if LV_USE_XML
    /* Register widgets */

    /* Register fonts */
    lv_xml_register_font(NULL, "font_12", font_12);
    lv_xml_register_font(NULL, "font_16", font_16);
    lv_xml_register_font(NULL, "font_18", font_18);
    lv_xml_register_font(NULL, "font_20", font_20);
    lv_xml_register_font(NULL, "font_24", font_24);
    lv_xml_register_font(NULL, "font_32", font_32);
    lv_xml_register_font(NULL, "font_48", font_48);
    lv_xml_register_font(NULL, "font_64", font_64);
    lv_xml_register_font(NULL, "font_96", font_96);
    lv_xml_register_font(NULL, "bs_icons_24", bs_icons_24);
    lv_xml_register_font(NULL, "bs_icons_48", bs_icons_48);

    /* Register subjects */
    lv_xml_register_subject(NULL, "sp_speed", &sp_speed);
    lv_xml_register_subject(NULL, "sp_speed_bar", &sp_speed_bar);
    lv_xml_register_subject(NULL, "sp_fuel_range", &sp_fuel_range);
    lv_xml_register_subject(NULL, "sp_fuel_bar", &sp_fuel_bar);
    lv_xml_register_subject(NULL, "sp_drive_mode", &sp_drive_mode);
    lv_xml_register_subject(NULL, "sp_gear", &sp_gear);
    lv_xml_register_subject(NULL, "sp_rpm_bar", &sp_rpm_bar);
    lv_xml_register_subject(NULL, "sp_temp", &sp_temp);
    lv_xml_register_subject(NULL, "sp_temp_bar", &sp_temp_bar);
    lv_xml_register_subject(NULL, "sp_odo", &sp_odo);
    lv_xml_register_subject(NULL, "ff_hud_mode", &ff_hud_mode);
    lv_xml_register_subject(NULL, "ff_status", &ff_status);
    lv_xml_register_subject(NULL, "ff_target_range", &ff_target_range);
    lv_xml_register_subject(NULL, "ff_lidar", &ff_lidar);
    lv_xml_register_subject(NULL, "ff_left_sensor", &ff_left_sensor);
    lv_xml_register_subject(NULL, "ff_right_sensor", &ff_right_sensor);
    lv_xml_register_subject(NULL, "flash_rpm", &flash_rpm);
    lv_xml_register_subject(NULL, "flash_mph", &flash_mph);
    lv_xml_register_subject(NULL, "flash_fuel", &flash_fuel);
    lv_xml_register_subject(NULL, "flash_fuel_bar", &flash_fuel_bar);

    /* Register callbacks */
#endif

    /* Register all the global assets so that they won't be created again when globals.xml is parsed.
     * While running in the editor skip this step to update the preview when the XML changes */
#if LV_USE_XML && !defined(LV_EDITOR_PREVIEW)
    /* Register images */
#endif

#if LV_USE_XML == 0
    /*--------------------
     *  Permanent screens
     *-------------------*/
    /* If XML is enabled it's assumed that the permanent screens are created
     * manaully from XML using lv_xml_create() */
    /* To allow screens to reference each other, create them all before calling the sceen create functions */
    lvgl_studio_flash = lv_obj_create(NULL);

    lvgl_studio_flash_create();
#endif
}

/* Callbacks */

/**********************
 *   STATIC FUNCTIONS
 **********************/