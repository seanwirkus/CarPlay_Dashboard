// LVGL 9.3 → 9.4 compatibility shim implementations.

#include "lvgl_compat.h"
#include "lvgl.h"

void lv_arc_set_min_value(lv_obj_t *obj, int32_t value)
{
    lv_arc_set_range(obj, value, lv_arc_get_max_value(obj));
}

void lv_arc_set_max_value(lv_obj_t *obj, int32_t value)
{
    lv_arc_set_range(obj, lv_arc_get_min_value(obj), value);
}

void lv_bar_set_min_value(lv_obj_t *obj, int32_t value)
{
    lv_bar_set_range(obj, value, lv_bar_get_max_value(obj));
}

void lv_bar_set_max_value(lv_obj_t *obj, int32_t value)
{
    lv_bar_set_range(obj, lv_bar_get_min_value(obj), value);
}

static void bar_subject_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
    lv_obj_t *bar = (lv_obj_t *)lv_observer_get_target(observer);
    if (!bar) return;
    const int32_t v = lv_subject_get_int(subject);
    lv_bar_set_value(bar, v, LV_ANIM_OFF);
}

lv_observer_t *lv_bar_bind_value(lv_obj_t *obj, lv_subject_t *subject)
{
    return lv_subject_add_observer_obj(subject, bar_subject_observer_cb, obj, NULL);
}
