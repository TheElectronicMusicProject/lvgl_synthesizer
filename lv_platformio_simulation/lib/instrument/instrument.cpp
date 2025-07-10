#include "instrument.h"
#include "lvgl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static void on_button_cb(lv_event_t * p_event);
static void on_knob_cb(lv_event_t * p_event);
static void on_drop_cb(lv_event_t * p_event);

static uint8_t * gp_volume = NULL;
static uint8_t * gp_q_key_press = NULL;
static waveform_list_t * gp_sel_wave = NULL;
static const char g_waveform_names[] = "Sine\n" "Triangle\n" "Square";

static void
on_button_cb (lv_event_t * p_event)
{
    if (NULL != p_event)
    {
        lv_obj_t * p_btn = lv_event_get_target_obj(p_event);
        key_number_t * p_active_key =
                            (key_number_t *) lv_event_get_user_data(p_event);
        char cmd_array[80] = {0};
        char cmd_wave[10] = {0};
        double key_freq = pow(2.0, ((double) p_active_key->num - 9.0) / 12.0)
                                   * 440.0;   /* Align to middle C */
        
        switch (p_event->code)
        {
            case LV_EVENT_PRESSED:
            {
                lv_log("PRESSED %f\n", key_freq);

                switch (*gp_sel_wave)
                {
                    default:
                        /* Fall throug */
                    case SINE_WAVE:
                    {
                        strncpy(cmd_wave, "sin", 10);
                    }
                    break;

                    case TRIANGLE_WAVE:
                    {
                        strncpy(cmd_wave, "triangle", 10);
                    }
                    break;

                    case SQUARE_WAVE:
                    {
                        strncpy(cmd_wave, "square", 10);
                    }
                    break;
                }
                
                sprintf(cmd_array,
                        "play -V1 -r 48000 -n synth %s %f trim 0 0.5 vol %f&",
                        cmd_wave, key_freq,
                        (double) *gp_volume / 100.0 /
                        (double) (*gp_q_key_press + 1));
                lv_log("## cmd: %s", cmd_array);
                system(cmd_array);
                fflush(NULL);
                ++(*gp_q_key_press);
            }
            break;

            case LV_EVENT_RELEASED:
            {
                lv_log("RELEASED\n");
                system("pkill play");
                fflush(NULL);
                --(*gp_q_key_press);
            }
            break;

            default:
            break;
        }
    }    
}   /* on_button_cb() */

static void
on_knob_cb (lv_event_t * p_event)
{
    if (NULL != p_event)
    {
        switch (p_event->code)
        {
            case LV_EVENT_VALUE_CHANGED:
            {
                lv_obj_t * p_knob = lv_event_get_target_obj(p_event);
                lv_obj_t * p_knob_label =
                                (lv_obj_t *) lv_event_get_user_data(p_event);

                *gp_volume = lv_arc_get_value(p_knob);

                lv_label_set_text_fmt(p_knob_label, "%d%%", *gp_volume);
            }
            break;

            default:
            break;
        }
    }
}   /* on_knob_cb() */

static void
on_drop_cb (lv_event_t * p_event)
{
    if (NULL != p_event)
    {
        switch (p_event->code)
        {
            case LV_EVENT_VALUE_CHANGED:
            {
                lv_obj_t * p_select = lv_event_get_target_obj(p_event);
                char result[10] = {0};

                lv_dropdown_get_selected_str(p_select, result, sizeof(result));

                if (0 == strncmp(result, "Sine", 4))
                {
                    *gp_sel_wave = SINE_WAVE;
                }
                else if (0 == strncmp(result, "Triangle", 8))
                {
                    *gp_sel_wave = TRIANGLE_WAVE;
                }
                else if (0 == strncmp(result, "Square", 6))
                {
                    *gp_sel_wave = SQUARE_WAVE;
                }
                else
                {
                    *gp_sel_wave = SINE_WAVE;
                }

                lv_log("Selected %s and variable is %d\n", result,
                       *gp_sel_wave);
                fflush(NULL);
            }
            break;

            default:
            break;
        }
    }
}   /* on_drop_cb() */

uint8_t
init_instrument (instrument_t * p_instr)
{
    uint8_t ret = 0;

    if (NULL != p_instr)
    {
        p_instr->prop.volume = 100;
        p_instr->prop.waveform = SINE_WAVE;
        p_instr->q_key_press = 0;
        ret = 1;
    }
    else
    {
        ret = 0;
    }

    return (ret);
}   /* init_instrument() */

void
create_instrument (instrument_t * p_instr)
{
    if (NULL != p_instr)
    {
        int32_t idx = 0;
        static key_number_t * p_key_num = NULL;
        static lv_coord_t col_dsc[INSTR_NUM_KEY] = {0};
        static lv_style_t main_style{0};
        static lv_style_t upper_style{0};
        static lv_style_t white_key_style{0};
        static lv_style_t black_key_style{0};
        const char key_name_list[INSTR_NUM_KEY][3] = {"C", "C#", "D", "D#",
                                                      "E", "F", "F#", "G",
                                                      "G#", "A", "A#", "B",
                                                      "C"};

        gp_volume = &p_instr->prop.volume;
        gp_q_key_press = &p_instr->q_key_press;
        gp_sel_wave = &p_instr->prop.waveform;

        // Workaround to make the first played note emitting sound.
        //
        system("play");

        for (idx = 0; idx < INSTR_NUM_KEY; ++idx)
        {
            col_dsc[idx] = LV_GRID_FR(1);
            p_key_num = &p_instr->key[idx];
            strncpy(p_key_num->key_name, key_name_list[idx], 3);
        }

        col_dsc[INSTR_NUM_KEY] = LV_GRID_TEMPLATE_LAST;

        static lv_coord_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1),
                                       LV_GRID_FR(1), LV_GRID_FR(1),
                                       LV_GRID_TEMPLATE_LAST};

        lv_obj_t * p_screen(lv_screen_active());
        lv_obj_set_grid_dsc_array(p_screen, col_dsc, row_dsc);
        lv_obj_set_grid_align(p_screen, LV_GRID_ALIGN_CENTER,
                              LV_GRID_ALIGN_CENTER);

        // ROW 0
        //
        lv_obj_t * p_waveform_ctrl = lv_obj_create(p_screen);
        lv_obj_set_grid_cell(p_waveform_ctrl, LV_GRID_ALIGN_STRETCH, 0,
                             INSTR_NUM_KEY / 2,
                             LV_GRID_ALIGN_STRETCH, 0, 2);

        lv_obj_t * p_volume_ctrl = lv_obj_create(p_screen);
        lv_obj_set_grid_cell(p_volume_ctrl, LV_GRID_ALIGN_STRETCH,
                             INSTR_NUM_KEY / 2, INSTR_NUM_KEY / 2 + 1,
                             LV_GRID_ALIGN_STRETCH, 0, 2);

        // Style for Row 0.
        //
        lv_style_init(&main_style);
        lv_style_set_bg_color(&main_style,
                              lv_palette_main(LV_PALETTE_LIGHT_GREEN));
        lv_obj_add_style(p_screen, &main_style, LV_PART_MAIN);

        lv_style_init(&upper_style);
        lv_style_set_bg_color(&upper_style, lv_palette_main(LV_PALETTE_GREY));
        lv_obj_add_style(p_waveform_ctrl, &upper_style, LV_PART_MAIN);
        lv_obj_add_style(p_volume_ctrl, &upper_style, LV_PART_MAIN);

        // Waveform selector inside Row 0.
        //
        lv_obj_t * p_waveform_list = lv_dropdown_create(p_waveform_ctrl);

        if ((NULL != p_waveform_list) && (NULL != p_waveform_ctrl))
        {
            lv_dropdown_set_options(p_waveform_list, g_waveform_names);
            lv_obj_add_event_cb(p_waveform_list, on_drop_cb,
                                LV_EVENT_VALUE_CHANGED,
                                &p_instr->prop.waveform);
        }

        // Knob inside Row 0.
        //
        lv_obj_t * p_knob_label = lv_label_create(p_volume_ctrl);

        if ((NULL != p_knob_label) && (NULL != p_volume_ctrl))
        {
            lv_label_set_text(p_knob_label, "100%");
            lv_obj_set_align(p_knob_label, LV_ALIGN_CENTER);
            
            lv_obj_t * p_knob = lv_arc_create(p_volume_ctrl);

            if (NULL != p_knob)
            {
                lv_obj_center(p_knob);
                lv_arc_set_range(p_knob, 0, 100);
                lv_arc_set_value(p_knob, 100);
                lv_obj_add_event_cb(p_knob, on_knob_cb, LV_EVENT_VALUE_CHANGED,
                                    p_knob_label);
            }
        }

        // ROW 1
        //
        lv_obj_t * p_btn = NULL;

        lv_style_init(&white_key_style);
        lv_style_set_bg_color(&white_key_style, {0xFF, 0xFF, 0xFF});

        lv_style_init(&black_key_style);
        lv_style_set_bg_color(&black_key_style,
                              lv_palette_main(LV_PALETTE_NONE));

        for (idx = 0; idx < INSTR_NUM_KEY; ++idx)
        {
            // Keyboard made of buttons.
            //
            p_btn = lv_button_create(p_screen);

            p_instr->key[idx].num = idx;
            lv_obj_t * p_key_label = lv_label_create(p_btn);
            p_key_num = &p_instr->key[idx];
            lv_label_set_text(p_key_label, p_key_num->key_name);
            lv_obj_set_align(p_key_label, LV_ALIGN_CENTER);

            lv_obj_add_event_cb(p_btn, on_button_cb, LV_EVENT_PRESSED,
                                &p_instr->key[idx]);
            lv_obj_add_event_cb(p_btn, on_button_cb, LV_EVENT_RELEASED,
                                &p_instr->key[idx]);
            
            // Style for Row 1.
            //
            if ((1 == idx) || (3 == idx) || (6 == idx) || (8 == idx) ||
                (10 == idx))
            {
                lv_obj_add_style(p_btn, &black_key_style, LV_PART_MAIN);
                lv_obj_set_style_text_color(p_key_label, {0xFF, 0xFF, 0xFF}, 0);
                lv_obj_set_grid_cell(p_btn, LV_GRID_ALIGN_STRETCH, idx, 1,
                                     LV_GRID_ALIGN_STRETCH, 2, 1);
            }
            else
            {
                lv_obj_add_style(p_btn, &white_key_style, LV_PART_MAIN);
                lv_obj_set_style_text_color(p_key_label, {0x0, 0x0, 0x0}, 0);
                lv_obj_set_grid_cell(p_btn, LV_GRID_ALIGN_STRETCH, idx, 1,
                                     LV_GRID_ALIGN_STRETCH, 2, 2);
            }
        }
    }
}   /* create_instrument() */


/*** End of file ***/
