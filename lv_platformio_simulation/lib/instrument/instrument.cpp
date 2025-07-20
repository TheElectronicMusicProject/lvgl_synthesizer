/**
 * @file    instrument.c
 * 
 * @brief   Instrument creation and use
 * 
 * @author  Filippo Graziani (circuitry.passion@gmail.com)
 * 
 * @date    15/07/2025
 * 
 * @par     Description
 * Instrument management with init function and widgets creation. The callbacks
 * are managed in this source code.
 */

/******************************************************************************
 *                              INCLUDE STATEMENS                             *
 * The headers for each grouping should be sorted alphabetically:             *
 * 1 - The paired header file                                                 *
 * 2 - Other headers from your project                                        *
 * 3 - 3rd party library headers                                              *
 * 4 - Standard library headers                                               *
 ******************************************************************************/
#include "instrument.h"
#include "utilities.h"
#include "lvgl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/******************************************************************************
 *                        DATA TYPES, CONSTANTS, MACROS                       *
 ******************************************************************************/
static const char g_waveform_names[] = "Sine\n" "Triangle\n" "Square";

/******************************************************************************
 *                              STATIC VARIABLES                              *
 ******************************************************************************/
static uint8_t * gp_volume = NULL;
static uint8_t * gp_q_key_press = NULL;
static waveform_list_t * gp_sel_wave = NULL;
#if KEY_SIMULATION
static uint8_t g_active_key_num = 0;
#endif   /* KEY_SIMULATION */

/******************************************************************************
 *                        PRIVATE FUNCTIONS PROTOTYPES                        *
 ******************************************************************************/
static void on_button_cb(lv_event_t * p_event);
static void on_knob_cb(lv_event_t * p_event);
static void on_drop_cb(lv_event_t * p_event);

#if KEY_SIMULATION
static void sim_on_key_press_cb(lv_indev_t * p_indev, lv_indev_data_t * p_data);
static void sim_on_button_pressed_cb(lv_event_t * p_event);
#endif   /* KEY_SIMULATION */

/******************************************************************************
 *                           PUBLIC FUNCTION BODIES                           *
 ******************************************************************************/
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

#ifdef __linux__
        // Workaround to make the first played note emitting sound.
        //
        system("play");
#endif /* __linux__ */

        for (idx = 0; idx < INSTR_NUM_KEY; ++idx)
        {
            col_dsc[idx] = LV_GRID_FR(1);
            p_key_num = &p_instr->key[idx];
            p_key_num->num = idx;
            strncpy(p_key_num->key_name, key_name_list[idx], 3);
        }

        col_dsc[INSTR_NUM_KEY] = LV_GRID_TEMPLATE_LAST;

        static lv_coord_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1),
                                       LV_GRID_FR(1), LV_GRID_FR(1),
                                       LV_GRID_TEMPLATE_LAST};

        lv_obj_t * p_screen = lv_screen_active();

        lv_obj_t * p_cont = lv_obj_create(p_screen);
        lv_obj_set_size(p_cont, lv_pct(100), lv_pct(100));
        lv_obj_set_style_pad_gap(p_cont, 0, 0);

        lv_obj_set_grid_dsc_array(p_cont, col_dsc, row_dsc);
        lv_obj_set_grid_align(p_cont, LV_GRID_ALIGN_CENTER,
                              LV_GRID_ALIGN_CENTER);

        // ROW 0
        //
        lv_obj_t * p_waveform_ctrl = lv_obj_create(p_cont);
        lv_obj_set_grid_cell(p_waveform_ctrl, LV_GRID_ALIGN_STRETCH, 0,
                             INSTR_NUM_KEY / 2,
                             LV_GRID_ALIGN_STRETCH, 0, 2);

        lv_obj_t * p_volume_ctrl = lv_obj_create(p_cont);
        lv_obj_set_grid_cell(p_volume_ctrl, LV_GRID_ALIGN_STRETCH,
                             INSTR_NUM_KEY / 2, INSTR_NUM_KEY / 2 + 1,
                             LV_GRID_ALIGN_STRETCH, 0, 2);

        // Style for Row 0.
        //
        lv_style_init(&main_style);
        lv_style_set_bg_color(&main_style,
                              lv_palette_main(LV_PALETTE_LIGHT_GREEN));
        lv_obj_add_style(p_cont, &main_style, LV_PART_MAIN);

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

#if KEY_SIMULATION
        lv_indev_t * p_key_input = lv_indev_create();
        lv_indev_set_type(p_key_input, LV_INDEV_TYPE_KEYPAD);
        lv_indev_set_read_cb(p_key_input, sim_on_key_press_cb);
        lv_group_t * p_grp = lv_group_create();
        lv_group_set_default(p_grp);
#endif  /* KEY_SIMULATION */

        for (idx = 0; idx < INSTR_NUM_KEY; ++idx)
        {
            // Keyboard made of buttons.
            //
            p_btn = lv_button_create(p_cont);

            lv_obj_t * p_key_label = lv_label_create(p_btn);
            p_key_num = &p_instr->key[idx];
            lv_label_set_text(p_key_label, p_key_num->key_name);
            lv_obj_set_align(p_key_label, LV_ALIGN_CENTER);

            lv_obj_add_event_cb(p_btn, on_button_cb, LV_EVENT_PRESSED,
                                &p_instr->key[idx]);
            lv_obj_add_event_cb(p_btn, on_button_cb, LV_EVENT_RELEASED,
                                &p_instr->key[idx]);
#if KEY_SIMULATION
            lv_group_add_obj(p_grp, p_btn);
            lv_indev_set_group(p_key_input, p_grp);
            lv_obj_add_event_cb(p_btn, sim_on_button_pressed_cb,
                                LV_EVENT_CLICKED, &p_instr->key[idx]);
#endif  /* KEY_SIMULATION */
            
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


/******************************************************************************
 *                           PRIVATE FUNCTION BODIES                          *
 ******************************************************************************/
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
#ifdef __linux__
                switch (*gp_sel_wave)
                {
                    default:
                        /* Fall through */
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
#endif /* __linux__ */

                fflush(NULL);
                ++(*gp_q_key_press);
            }
            break;

            case LV_EVENT_RELEASED:
            {
                lv_log("RELEASED\n");
#ifdef __linux__
                system("pkill play");
#endif /* __linux__ */

                fflush(NULL);
                --(*gp_q_key_press);
            }
            break;

            case LV_EVENT_CLICKED:
            {
                
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

#if KEY_SIMULATION

static void
sim_on_key_press_cb (lv_indev_t * p_indev, lv_indev_data_t * p_data)
{
#ifdef __linux__ 
    // Check if pressing a button.
    //
    int32_t key_code = get_key();

    if (0 != key_code)
    {
        lv_log("Key code pressed is %d\n", key_code);
        fflush(stdout);

        p_data->state = LV_INDEV_STATE_PRESSED;

        // We want to check if we're pressing ENTER.
        //
        switch (key_code)
        {
            // A key.
            //
            case 97:
            {
                g_active_key_num = 0;
            }
            break;

            // W key.
            //
            case 119:
            {
                g_active_key_num = 1;
            }
            break;

            // S key.
            //
            case 115:
            {
                g_active_key_num = 2;
            }
            break;

            // E key.
            //
            case 101:
            {
                g_active_key_num = 3;
            }
            break;

            // D key.
            //
            case 100:
            {
                g_active_key_num = 4;
            }
            break;

            // F key.
            //
            case 102:
            {
                g_active_key_num = 5;
            }
            break;

            // T key.
            //
            case 116:
            {
                g_active_key_num = 6;
            }
            break;

            // G key.
            //
            case 103:
            {
                g_active_key_num = 7;
            }
            break;

            // Y key.
            //
            case 121:
            {
                g_active_key_num = 8;
            }
            break;

            // H key.
            //
            case 104:
            {
                g_active_key_num = 9;
            }
            break;

            // U key.
            //
            case 117:
            {
                g_active_key_num = 10;
            }
            break;

            // J key.
            //
            case 106:
            {
                g_active_key_num = 11;
            }
            break;

            // K key.
            //
            case 107:
            {
                g_active_key_num = 12;
            }
            break;

            default:
            {
                p_data->state = LV_INDEV_STATE_RELEASED;
            }
            break;
        }
    }
    else
    {
        // Key hasn't been pressed.
        //
        p_data->state = LV_INDEV_STATE_RELEASED;
    }
#endif
}   /* sim_on_key_press_cb() */

static void
sim_on_button_pressed_cb (lv_event_t * p_event)
{
    if (NULL != p_event)
    {
        lv_obj_t * p_btn = lv_event_get_target_obj(p_event);
        key_number_t * p_active_key =
                            (key_number_t *) lv_event_get_user_data(p_event);
        char cmd_array[80] = {0};
        char cmd_wave[10] = {0};
#if KEY_SIMULATION
        double key_freq = pow(2.0, ((double) g_active_key_num - 9.0) / 12.0)
                                   * 440.0;   /* Align to middle C */
#else
        double key_freq = pow(2.0, ((double) p_active_key->num - 9.0) / 12.0)
                                   * 440.0;   /* Align to middle C */
#endif  /* KEY_SIMULATION */
        
        switch (p_event->code)
        {
            case LV_EVENT_CLICKED:
                /* Fall through */
            case LV_EVENT_PRESSED:
            {
                lv_log("PRESSED %f\n", key_freq);
#ifdef __linux__
                switch (*gp_sel_wave)
                {
                    default:
                        /* Fall through */
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
#endif /* __linux__ */

                fflush(NULL);
                ++(*gp_q_key_press);
            }
            break;

            case LV_EVENT_RELEASED:
            {
                lv_log("RELEASED\n");
#ifdef __linux__
                system("pkill play");
#endif /* __linux__ */

                fflush(NULL);
                --(*gp_q_key_press);
            }
            break;

            default:
            break;
        }
    }    
}   /* sim_on_button_pressed_cb() */

#endif  /* KEY_SIMULATION */


/*** End of file ***/
