#include "instrument.h"
#include "lvgl.h"
#include <stdint.h>
#include <stdio.h>

static void on_button_cb(lv_event_t * p_event);

static void
on_button_cb (lv_event_t * p_event)
{
    lv_log("PRESSED\n");
	fflush(NULL);
}   /* on_button_cb() */

void
create_instrument (void)
{
    int32_t idx = 0;
    static lv_coord_t col_dsc[14] = {0};

    for (idx = 0; idx < 13; ++idx)
    {
        col_dsc[idx] = LV_GRID_FR(1);
    }

    col_dsc[13] = LV_GRID_TEMPLATE_LAST;

    static lv_coord_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1),
                                   LV_GRID_TEMPLATE_LAST};

    lv_obj_t * p_screen(lv_screen_active());
    lv_obj_set_grid_dsc_array(p_screen, col_dsc, row_dsc);
    lv_obj_set_grid_align(p_screen, LV_GRID_ALIGN_CENTER, LV_GRID_ALIGN_CENTER);

    // ROW 0
    //
    lv_obj_t * p_obj = lv_obj_create(p_screen);
    lv_obj_set_grid_cell(p_obj, LV_GRID_ALIGN_STRETCH, 0, 13, LV_GRID_ALIGN_STRETCH, 0, 1);

    // ROW 1
    //
    lv_obj_t * p_btn = NULL;

    for (idx = 0; idx < 13; ++idx)
    {
        p_btn = lv_button_create(p_screen);
        lv_obj_set_grid_cell(p_btn, LV_GRID_ALIGN_STRETCH, idx, 1, LV_GRID_ALIGN_STRETCH, 1, 1);

        lv_obj_add_event_cb(p_btn, on_button_cb, LV_EVENT_PRESSED, NULL);
    }

}   /* create_instrument() */