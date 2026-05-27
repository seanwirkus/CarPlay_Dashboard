/**
 * @file WRX_Cluster_gen.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "WRX_Cluster_gen.h"

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

/*----------------
 * Fonts
 *----------------*/

lv_font_t * font_12;
extern lv_font_t font_12_data;
lv_font_t * font_14;
extern lv_font_t font_14_data;
lv_font_t * font_16;
extern lv_font_t font_16_data;
lv_font_t * font_18;
extern lv_font_t font_18_data;
lv_font_t * font_22;
extern lv_font_t font_22_data;
lv_font_t * font_48;
extern lv_font_t font_48_data;

/*----------------
 * Images
 *----------------*/

/*----------------
 * Global styles
 *----------------*/

lv_style_t st_screen;
lv_style_t st_bar_track;
lv_style_t st_bar_cool;
lv_style_t st_bar_warm;
lv_style_t st_bar_nav;
lv_style_t st_bar_range;
lv_style_t st_circle_chip;
lv_style_t st_ghost;
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
lv_style_t st_ff_car_shadow;
lv_style_t st_ff_car_wheel;
lv_style_t st_ff_body_hi;
lv_style_t st_ff_body_lo;
lv_style_t st_ff_bumper;
lv_style_t st_ff_headlight;
lv_style_t st_ff_taillight;
lv_style_t st_ff_plate;

/*----------------
 * Subjects
 *----------------*/

lv_subject_t sp_speed;
lv_subject_t sp_rpm;
lv_subject_t sp_range_mi;
lv_subject_t sp_heading;
lv_subject_t sp_street;
lv_subject_t sp_dist;
lv_subject_t sp_eta;
lv_subject_t ff_hud_mode;
lv_subject_t ff_status;
lv_subject_t ff_target_range;
lv_subject_t ff_lidar;
lv_subject_t ff_left_sensor;
lv_subject_t ff_right_sensor;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void WRX_Cluster_init_gen(const char * asset_path)
{
    char buf[256];


    /*----------------
     * Fonts
     *----------------*/

    /* get font 'font_12' from a C array */
    font_12 = &font_12_data;
    /* get font 'font_14' from a C array */
    font_14 = &font_14_data;
    /* get font 'font_16' from a C array */
    font_16 = &font_16_data;
    /* get font 'font_18' from a C array */
    font_18 = &font_18_data;
    /* get font 'font_22' from a C array */
    font_22 = &font_22_data;
    /* get font 'font_48' from a C array */
    font_48 = &font_48_data;


    /*----------------
     * Images
     *----------------*/
    /*----------------
     * Global styles
     *----------------*/

    static bool style_inited = false;

    if (!style_inited) {
        lv_style_init(&st_screen);
        lv_style_set_bg_color(&st_screen, COL_BG);
        lv_style_set_bg_opa(&st_screen, 255);
        lv_style_set_border_width(&st_screen, 0);
        lv_style_set_pad_all(&st_screen, 0);
        lv_style_set_text_color(&st_screen, COL_WHITE);

        lv_style_init(&st_bar_track);
        lv_style_set_bg_color(&st_bar_track, COL_TRACK);
        lv_style_set_bg_opa(&st_bar_track, 255);
        lv_style_set_radius(&st_bar_track, 6);
        lv_style_set_border_width(&st_bar_track, 0);
        lv_style_set_pad_all(&st_bar_track, 0);

        lv_style_init(&st_bar_cool);
        lv_style_set_bg_color(&st_bar_cool, COL_CYAN);
        lv_style_set_bg_opa(&st_bar_cool, 255);
        lv_style_set_bg_grad_color(&st_bar_cool, COL_MAGENTA);
        lv_style_set_bg_grad_dir(&st_bar_cool, LV_GRAD_DIR_HOR);
        lv_style_set_radius(&st_bar_cool, 6);

        lv_style_init(&st_bar_warm);
        lv_style_set_bg_color(&st_bar_warm, COL_YELLOW);
        lv_style_set_bg_opa(&st_bar_warm, 255);
        lv_style_set_bg_grad_color(&st_bar_warm, COL_RED);
        lv_style_set_bg_grad_dir(&st_bar_warm, LV_GRAD_DIR_HOR);
        lv_style_set_radius(&st_bar_warm, 6);

        lv_style_init(&st_bar_nav);
        lv_style_set_bg_color(&st_bar_nav, COL_BLUE);
        lv_style_set_bg_opa(&st_bar_nav, 255);
        lv_style_set_radius(&st_bar_nav, 6);

        lv_style_init(&st_bar_range);
        lv_style_set_bg_color(&st_bar_range, COL_GREEN);
        lv_style_set_bg_opa(&st_bar_range, 255);
        lv_style_set_radius(&st_bar_range, 6);

        lv_style_init(&st_circle_chip);
        lv_style_set_bg_color(&st_circle_chip, COL_CHIP);
        lv_style_set_bg_opa(&st_circle_chip, 255);
        lv_style_set_radius(&st_circle_chip, 9999);
        lv_style_set_border_width(&st_circle_chip, 0);
        lv_style_set_text_color(&st_circle_chip, COL_WHITE);

        lv_style_init(&st_ghost);
        lv_style_set_pad_all(&st_ghost, 0);
        lv_style_set_border_width(&st_ghost, 0);
        lv_style_set_bg_opa(&st_ghost, 0);

        lv_style_init(&st_ff_scene);
        lv_style_set_bg_color(&st_ff_scene, lv_color_hex(0x06080B));
        lv_style_set_bg_opa(&st_ff_scene, 255);
        lv_style_set_border_width(&st_ff_scene, 0);
        lv_style_set_radius(&st_ff_scene, 26);
        lv_style_set_pad_all(&st_ff_scene, 0);

        lv_style_init(&st_ff_sky);
        lv_style_set_bg_color(&st_ff_sky, lv_color_hex(0x0C1118));
        lv_style_set_bg_opa(&st_ff_sky, 255);
        lv_style_set_border_width(&st_ff_sky, 0);
        lv_style_set_radius(&st_ff_sky, 20);
        lv_style_set_pad_all(&st_ff_sky, 0);

        lv_style_init(&st_ff_road_far);
        lv_style_set_bg_color(&st_ff_road_far, lv_color_hex(0x171C24));
        lv_style_set_bg_opa(&st_ff_road_far, 255);
        lv_style_set_border_width(&st_ff_road_far, 0);
        lv_style_set_radius(&st_ff_road_far, 4);
        lv_style_set_pad_all(&st_ff_road_far, 0);

        lv_style_init(&st_ff_road_mid);
        lv_style_set_bg_color(&st_ff_road_mid, lv_color_hex(0x222A35));
        lv_style_set_bg_opa(&st_ff_road_mid, 255);
        lv_style_set_border_width(&st_ff_road_mid, 0);
        lv_style_set_radius(&st_ff_road_mid, 5);
        lv_style_set_pad_all(&st_ff_road_mid, 0);

        lv_style_init(&st_ff_road_near);
        lv_style_set_bg_color(&st_ff_road_near, lv_color_hex(0x323C4C));
        lv_style_set_bg_opa(&st_ff_road_near, 255);
        lv_style_set_border_width(&st_ff_road_near, 0);
        lv_style_set_radius(&st_ff_road_near, 6);
        lv_style_set_pad_all(&st_ff_road_near, 0);

        lv_style_init(&st_ff_lane);
        lv_style_set_bg_color(&st_ff_lane, lv_color_hex(0xEEF4FF));
        lv_style_set_bg_opa(&st_ff_lane, 235);
        lv_style_set_border_width(&st_ff_lane, 0);
        lv_style_set_radius(&st_ff_lane, 2);
        lv_style_set_pad_all(&st_ff_lane, 0);

        lv_style_init(&st_ff_edge_yellow);
        lv_style_set_bg_color(&st_ff_edge_yellow, COL_YELLOW);
        lv_style_set_bg_opa(&st_ff_edge_yellow, 255);
        lv_style_set_border_width(&st_ff_edge_yellow, 0);
        lv_style_set_radius(&st_ff_edge_yellow, 2);
        lv_style_set_pad_all(&st_ff_edge_yellow, 0);

        lv_style_init(&st_ff_edge_white);
        lv_style_set_bg_color(&st_ff_edge_white, COL_WHITE);
        lv_style_set_bg_opa(&st_ff_edge_white, 255);
        lv_style_set_border_width(&st_ff_edge_white, 0);
        lv_style_set_radius(&st_ff_edge_white, 2);
        lv_style_set_pad_all(&st_ff_edge_white, 0);

        lv_style_init(&st_ff_sensor);
        lv_style_set_bg_color(&st_ff_sensor, COL_CYAN);
        lv_style_set_bg_opa(&st_ff_sensor, 150);
        lv_style_set_border_width(&st_ff_sensor, 0);
        lv_style_set_radius(&st_ff_sensor, 2);
        lv_style_set_pad_all(&st_ff_sensor, 0);

        lv_style_init(&st_ff_lidar);
        lv_style_set_bg_color(&st_ff_lidar, COL_RED);
        lv_style_set_bg_opa(&st_ff_lidar, 215);
        lv_style_set_border_width(&st_ff_lidar, 0);
        lv_style_set_radius(&st_ff_lidar, 2);
        lv_style_set_pad_all(&st_ff_lidar, 0);

        lv_style_init(&st_ff_detect);
        lv_style_set_bg_opa(&st_ff_detect, 0);
        lv_style_set_border_width(&st_ff_detect, 1);
        lv_style_set_border_color(&st_ff_detect, COL_CYAN);
        lv_style_set_radius(&st_ff_detect, 6);
        lv_style_set_pad_all(&st_ff_detect, 0);

        lv_style_init(&st_ff_ego_body);
        lv_style_set_bg_color(&st_ff_ego_body, lv_color_hex(0xDCE6F2));
        lv_style_set_bg_opa(&st_ff_ego_body, 255);
        lv_style_set_border_width(&st_ff_ego_body, 1);
        lv_style_set_border_color(&st_ff_ego_body, COL_WHITE);
        lv_style_set_radius(&st_ff_ego_body, 5);
        lv_style_set_pad_all(&st_ff_ego_body, 0);

        lv_style_init(&st_ff_ego_roof);
        lv_style_set_bg_color(&st_ff_ego_roof, lv_color_hex(0x94A3B8));
        lv_style_set_bg_opa(&st_ff_ego_roof, 255);
        lv_style_set_border_width(&st_ff_ego_roof, 0);
        lv_style_set_radius(&st_ff_ego_roof, 3);
        lv_style_set_pad_all(&st_ff_ego_roof, 0);

        lv_style_init(&st_ff_glass);
        lv_style_set_bg_color(&st_ff_glass, lv_color_hex(0x161920));
        lv_style_set_bg_opa(&st_ff_glass, 255);
        lv_style_set_border_width(&st_ff_glass, 0);
        lv_style_set_radius(&st_ff_glass, 2);
        lv_style_set_pad_all(&st_ff_glass, 0);

        lv_style_init(&st_ff_target_body);
        lv_style_set_bg_color(&st_ff_target_body, lv_color_hex(0x586474));
        lv_style_set_bg_opa(&st_ff_target_body, 255);
        lv_style_set_border_width(&st_ff_target_body, 1);
        lv_style_set_border_color(&st_ff_target_body, lv_color_hex(0xCBD5E1));
        lv_style_set_radius(&st_ff_target_body, 3);
        lv_style_set_pad_all(&st_ff_target_body, 0);

        lv_style_init(&st_ff_alert_body);
        lv_style_set_bg_color(&st_ff_alert_body, lv_color_hex(0xB91C1C));
        lv_style_set_bg_opa(&st_ff_alert_body, 255);
        lv_style_set_border_width(&st_ff_alert_body, 1);
        lv_style_set_border_color(&st_ff_alert_body, lv_color_hex(0xFECACA));
        lv_style_set_radius(&st_ff_alert_body, 3);
        lv_style_set_pad_all(&st_ff_alert_body, 0);

        lv_style_init(&st_ff_car_shadow);
        lv_style_set_bg_color(&st_ff_car_shadow, lv_color_hex(0x000000));
        lv_style_set_bg_opa(&st_ff_car_shadow, 130);
        lv_style_set_border_width(&st_ff_car_shadow, 0);
        lv_style_set_radius(&st_ff_car_shadow, 4);
        lv_style_set_pad_all(&st_ff_car_shadow, 0);

        lv_style_init(&st_ff_car_wheel);
        lv_style_set_bg_color(&st_ff_car_wheel, lv_color_hex(0x030305));
        lv_style_set_bg_opa(&st_ff_car_wheel, 255);
        lv_style_set_border_width(&st_ff_car_wheel, 1);
        lv_style_set_border_color(&st_ff_car_wheel, lv_color_hex(0x27272A));
        lv_style_set_radius(&st_ff_car_wheel, 2);
        lv_style_set_pad_all(&st_ff_car_wheel, 0);

        lv_style_init(&st_ff_body_hi);
        lv_style_set_bg_color(&st_ff_body_hi, lv_color_hex(0xF8FAFC));
        lv_style_set_bg_opa(&st_ff_body_hi, 90);
        lv_style_set_border_width(&st_ff_body_hi, 0);
        lv_style_set_radius(&st_ff_body_hi, 1);
        lv_style_set_pad_all(&st_ff_body_hi, 0);

        lv_style_init(&st_ff_body_lo);
        lv_style_set_bg_color(&st_ff_body_lo, lv_color_hex(0x020617));
        lv_style_set_bg_opa(&st_ff_body_lo, 85);
        lv_style_set_border_width(&st_ff_body_lo, 0);
        lv_style_set_radius(&st_ff_body_lo, 1);
        lv_style_set_pad_all(&st_ff_body_lo, 0);

        lv_style_init(&st_ff_bumper);
        lv_style_set_bg_color(&st_ff_bumper, lv_color_hex(0x111827));
        lv_style_set_bg_opa(&st_ff_bumper, 255);
        lv_style_set_border_width(&st_ff_bumper, 0);
        lv_style_set_radius(&st_ff_bumper, 2);
        lv_style_set_pad_all(&st_ff_bumper, 0);

        lv_style_init(&st_ff_headlight);
        lv_style_set_bg_color(&st_ff_headlight, lv_color_hex(0xCFFAFE));
        lv_style_set_bg_opa(&st_ff_headlight, 230);
        lv_style_set_border_width(&st_ff_headlight, 0);
        lv_style_set_radius(&st_ff_headlight, 1);
        lv_style_set_pad_all(&st_ff_headlight, 0);

        lv_style_init(&st_ff_taillight);
        lv_style_set_bg_color(&st_ff_taillight, lv_color_hex(0xEF4444));
        lv_style_set_bg_opa(&st_ff_taillight, 255);
        lv_style_set_border_width(&st_ff_taillight, 0);
        lv_style_set_radius(&st_ff_taillight, 1);
        lv_style_set_pad_all(&st_ff_taillight, 0);

        lv_style_init(&st_ff_plate);
        lv_style_set_bg_color(&st_ff_plate, lv_color_hex(0xE2E8F0));
        lv_style_set_bg_opa(&st_ff_plate, 235);
        lv_style_set_border_width(&st_ff_plate, 0);
        lv_style_set_radius(&st_ff_plate, 1);
        lv_style_set_pad_all(&st_ff_plate, 0);

        style_inited = true;
    }

    /*----------------
     * Subjects
     *----------------*/
    lv_subject_init_int(&sp_speed, 45);
    lv_subject_init_int(&sp_rpm, 2880);
    lv_subject_init_int(&sp_range_mi, 207);
    lv_subject_init_int(&sp_heading, 342);
    static char sp_street_buf[UI_SUBJECT_STRING_LENGTH];
    static char sp_street_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&sp_street,
                           sp_street_buf,
                           sp_street_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "24th Street"
                          );
    static char sp_dist_buf[UI_SUBJECT_STRING_LENGTH];
    static char sp_dist_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&sp_dist,
                           sp_dist_buf,
                           sp_dist_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "0.2 mi"
                          );
    static char sp_eta_buf[UI_SUBJECT_STRING_LENGTH];
    static char sp_eta_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&sp_eta,
                           sp_eta_buf,
                           sp_eta_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "10:12 PM"
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

    /*----------------
     * Translations
     *----------------*/

#if LV_USE_XML
    /* Register widgets */

    /* Register fonts */
    lv_xml_register_font(NULL, "font_12", font_12);
    lv_xml_register_font(NULL, "font_14", font_14);
    lv_xml_register_font(NULL, "font_16", font_16);
    lv_xml_register_font(NULL, "font_18", font_18);
    lv_xml_register_font(NULL, "font_22", font_22);
    lv_xml_register_font(NULL, "font_48", font_48);

    /* Register subjects */
    lv_xml_register_subject(NULL, "sp_speed", &sp_speed);
    lv_xml_register_subject(NULL, "sp_rpm", &sp_rpm);
    lv_xml_register_subject(NULL, "sp_range_mi", &sp_range_mi);
    lv_xml_register_subject(NULL, "sp_heading", &sp_heading);
    lv_xml_register_subject(NULL, "sp_street", &sp_street);
    lv_xml_register_subject(NULL, "sp_dist", &sp_dist);
    lv_xml_register_subject(NULL, "sp_eta", &sp_eta);
    lv_xml_register_subject(NULL, "ff_hud_mode", &ff_hud_mode);
    lv_xml_register_subject(NULL, "ff_status", &ff_status);
    lv_xml_register_subject(NULL, "ff_target_range", &ff_target_range);
    lv_xml_register_subject(NULL, "ff_lidar", &ff_lidar);
    lv_xml_register_subject(NULL, "ff_left_sensor", &ff_left_sensor);
    lv_xml_register_subject(NULL, "ff_right_sensor", &ff_right_sensor);

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
#endif
}

/* Callbacks */

/**********************
 *   STATIC FUNCTIONS
 **********************/