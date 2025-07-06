#include "instrument.h"
#include "lvgl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define INSTR_NUM_KEY   (13)

static void on_button_cb(lv_event_t * p_event);
static void on_knob_cb(lv_event_t * p_event);

static int32_t g_volume = 100;

static void
on_button_cb (lv_event_t * p_event)
{
    lv_obj_t * p_btn = lv_event_get_target_obj(p_event);
    key_number_t * p_active_key =
                            (key_number_t *) lv_event_get_user_data(p_event);
    char cmd_array[70] = {0};
    double key_freq = pow(2.0, ((double) p_active_key->num - 9.0) / 12.0)
                      * 440.0;   /* Align to middle C */
    
    switch (p_event->code)
    {
        case LV_EVENT_PRESSED:
        {
            lv_log("PRESSED %f\n", key_freq);
            sprintf(cmd_array, "play -V1 -r 48000 -n synth sin %f trim 0 0.5 vol %f&",
                    key_freq, (double) g_volume / 1000);
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

static void
on_knob_cb (lv_event_t * p_event)
{
    switch (p_event->code)
    {
        case LV_EVENT_VALUE_CHANGED:
        {
            lv_obj_t * p_knob = lv_event_get_target_obj(p_event);
            lv_obj_t * p_knob_label =
                                (lv_obj_t *) lv_event_get_user_data(p_event);

            g_volume = lv_arc_get_value(p_knob);

            lv_label_set_text_fmt(p_knob_label, "%d%%", g_volume);
        }
        break;

        default:
        break;
    }
    
}   /* on_knob_cb() */

void
create_instrument (void)
{
    int32_t idx = 0;
    key_number_t * p_key_num = NULL;
    static key_number_t piano_key[INSTR_NUM_KEY] = {0};
    static lv_coord_t col_dsc[INSTR_NUM_KEY] = {0};
    static lv_style_t main_style{0};
    static lv_style_t upper_style{0};
    static lv_style_t white_key_style{0};
    static lv_style_t black_key_style{0};
    const char key_name_list[INSTR_NUM_KEY][3] = {"C", "C#", "D", "D#", "E",
                                                  "F", "F#", "G", "G#", "A",
                                                  "A#", "B", "C"};

    for (idx = 0; idx < INSTR_NUM_KEY; ++idx)
    {
        col_dsc[idx] = LV_GRID_FR(1);
        p_key_num = &piano_key[idx];
        strncpy(p_key_num->key_name, key_name_list[idx], 3);
    }

    col_dsc[INSTR_NUM_KEY] = LV_GRID_TEMPLATE_LAST;

    static lv_coord_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1),
                                   LV_GRID_TEMPLATE_LAST};

    lv_obj_t * p_screen(lv_screen_active());
    lv_obj_set_grid_dsc_array(p_screen, col_dsc, row_dsc);
    lv_obj_set_grid_align(p_screen, LV_GRID_ALIGN_CENTER, LV_GRID_ALIGN_CENTER);

    // ROW 0
    //
    lv_obj_t * p_first_row = lv_obj_create(p_screen);
    lv_obj_set_grid_cell(p_first_row, LV_GRID_ALIGN_STRETCH, 0, INSTR_NUM_KEY,
                         LV_GRID_ALIGN_STRETCH, 0, 1);

    // Style for Row 0.
    //
    lv_style_init(&main_style);
    lv_style_set_bg_color(&main_style, lv_palette_main(LV_PALETTE_LIGHT_GREEN));
    lv_obj_add_style(p_screen, &main_style, LV_PART_MAIN);

    lv_style_init(&upper_style);
    lv_style_set_bg_color(&upper_style, lv_palette_main(LV_PALETTE_GREY));
    lv_obj_add_style(p_first_row, &upper_style, LV_PART_MAIN);

    lv_style_init(&white_key_style);
    lv_style_set_bg_color(&white_key_style, {0xFF, 0xFF, 0xFF});

    lv_style_init(&black_key_style);
    lv_style_set_bg_color(&black_key_style, lv_palette_main(LV_PALETTE_NONE));

    // Knob inside Row 0.
    //
    lv_obj_t * p_knob_label = lv_label_create(p_first_row);
    lv_label_set_text(p_knob_label, "0%");
    
    lv_obj_t * p_knob = lv_arc_create(p_first_row);
    lv_obj_center(p_knob);
    lv_arc_set_range(p_knob, 0, 100);
    lv_arc_set_value(p_knob, 100);
    lv_obj_add_event_cb(p_knob, on_knob_cb, LV_EVENT_VALUE_CHANGED,
                        p_knob_label);

    // ROW 1
    //
    lv_obj_t * p_btn = NULL;

    for (idx = 0; idx < INSTR_NUM_KEY; ++idx)
    {
        // Keyboard made of buttons.
        //
        p_btn = lv_button_create(p_screen);
        lv_obj_set_grid_cell(p_btn, LV_GRID_ALIGN_STRETCH, idx, 1,
                             LV_GRID_ALIGN_STRETCH, 1, 1);
        piano_key[idx].num = idx;
        lv_obj_t * p_key_label = lv_label_create(p_btn);
        p_key_num = &piano_key[idx];
        lv_label_set_text(p_key_label, p_key_num->key_name);

        lv_obj_add_event_cb(p_btn, on_button_cb, LV_EVENT_PRESSED,
                            &piano_key[idx]);
        lv_obj_add_event_cb(p_btn, on_button_cb, LV_EVENT_RELEASED,
                            &piano_key[idx]);
        
        // Style for Row 1.
        //
        if ((1 == idx) || (3 == idx) || (6 == idx) || (8 == idx) || (10 == idx))
        {
            lv_obj_add_style(p_btn, &black_key_style, LV_PART_MAIN);
            lv_obj_set_style_text_color(p_key_label, {0xFF, 0xFF, 0xFF}, 0);
        }
        else
        {
            lv_obj_add_style(p_btn, &white_key_style, LV_PART_MAIN);
            lv_obj_set_style_text_color(p_key_label, {0x0, 0x0, 0x0}, 0);
        }
    }

}   /* create_instrument() */