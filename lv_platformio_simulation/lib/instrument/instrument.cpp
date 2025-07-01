#include "instrument.h"
#include "lvgl.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static void on_button_cb(lv_event_t * p_event);

static void
on_button_cb (lv_event_t * p_event)
{
    lv_obj_t * p_btn = lv_event_get_target_obj(p_event);
    key_number_t * p_active_key = (key_number_t *) lv_event_get_user_data(p_event);
    char cmd_array[60] = {0};
    double key_num = pow(2.0, ((double) p_active_key->num) / 12.0) * 440.0;   /* Align to middle C */
    
    switch (p_event->code)
    {
        case LV_EVENT_PRESSED:
        {
            lv_log("PRESSED %d\n", key_num);
            sprintf(cmd_array, "play -V1 -r 48000 -n synth sin %f trim 0 0.5 &", key_num);
            system(cmd_array);
            fflush(NULL);
        }
        break;

        case LV_EVENT_RELEASED:
        {
            lv_log("RELEASED\n");
            system("pkill play");
            fflush(NULL);
        }
        break;

        default:
        break;
    }
    
}   /* on_button_cb() */

void
create_instrument (void)
{
    int32_t idx = 0;
    static key_number_t piano_key[14] = {0};
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
        piano_key[idx].num = idx;

        lv_obj_add_event_cb(p_btn, on_button_cb, LV_EVENT_PRESSED, &piano_key[idx]);
        lv_obj_add_event_cb(p_btn, on_button_cb, LV_EVENT_RELEASED, &piano_key[idx]);
    }

}   /* create_instrument() */