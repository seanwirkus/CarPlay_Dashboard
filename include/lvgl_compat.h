// LVGL 9.3 → 9.4 compatibility shim. Provides functions used by the
// Editor-generated code that don't exist in the local 9.3 LVGL tree.
//
// Force-included via build_flags `-include lvgl_compat.h` so every
// generated .c sees the declarations before its first call.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

void lv_arc_set_min_value(lv_obj_t *obj, int32_t value);
void lv_arc_set_max_value(lv_obj_t *obj, int32_t value);
void lv_bar_set_min_value(lv_obj_t *obj, int32_t value);
void lv_bar_set_max_value(lv_obj_t *obj, int32_t value);
lv_observer_t *lv_bar_bind_value(lv_obj_t *obj, lv_subject_t *subject);

/* Name is debug/lookup metadata in 9.4. Local 9.3 lacks the API — no-op. */
static inline void lv_obj_set_name_static(lv_obj_t *obj, const char *name) {
    (void)obj; (void)name;
}

#ifdef __cplusplus
}
#endif
