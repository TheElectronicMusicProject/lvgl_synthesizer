/**
 * @file    instrument.h
 * 
 * @brief   Functions to use the instrument and structures to define the
 *          instrument's properties.
 * 
 * @author  Filippo Graziani (circuitry.passion@gmail.com)
 * 
 * @date    15/07/2025
 * 
 * @par     Description
 * - init_instrument() is the first function which must be called to initialise
 *   the instrument.
 * - create_instrument() is the main function that creates and instantiaes the
 *   LVGL objects to create the graphics and the callbacks.
 */

#ifndef INSTRUMENT_H

#   define INSTRUMENT_H

/******************************************************************************
 *                              INCLUDE STATEMENS                             *
 * The headers for each grouping should be sorted alphabetically:             *
 * 1 - The paired header file                                                 *
 * 2 - Other headers from your project                                        *
 * 3 - 3rd party library headers                                              *
 * 4 - Standard library headers                                               *
 ******************************************************************************/
#   include <stdint.h>

/******************************************************************************
 *                            PROTOTYPES OR MACROS                            *
 ******************************************************************************/

/******************************************************************************
 *                                   #DEFINE                                  *
 ******************************************************************************/

// To be customised in order to define the number of keys to be drawn (it is an
// entire octave).
// ATTENTION: changing this function modifies the number of elements in the
// string array key_name_list[], at the moment it isn't able to show more than
// 13 notes.
//
#   define INSTR_NUM_KEY    (13)

/******************************************************************************
 *                      TYPEDEFS (STRUCTS, ENUMS, UNIONS)                     *
 ******************************************************************************/

// List of available input waveforms.
//
typedef enum waveform_list_t
{
    SINE_WAVE = 0,              /* Sine wave. */
    TRIANGLE_WAVE,              /* Triangle wave. */
    SQUARE_WAVE,                /* Square wave. */
} waveform_list_t;

// Manages the single key and its note name.
//
typedef struct key_number_t
{
    uint8_t num;                /* Number from 0 to INSTR_NUM_KEY. */
    char key_name[3];           /* String containing the note name. */
} key_number_t;

// Manages the properties of the instrument.
//
typedef struct properties_t
{
    uint8_t volume;             /* Volume from 0 to 1. */
    waveform_list_t waveform;   /* Selected waveform form the enum. */
} properties_t;

// The usable instrument structure.
//
typedef struct instrument_t
{
    key_number_t key[INSTR_NUM_KEY];    /* The available keys. */
    properties_t prop;                  /* Instrument properties. */
    uint8_t q_key_press;                /* Number of simultaneous pressed
                                         * keys. */
} instrument_t;

/******************************************************************************
 *                         PUBLIC FUNCTIONS PROTOTYPES                        *
 ******************************************************************************/
uint8_t init_instrument(instrument_t * p_instr);
void create_instrument(instrument_t * p_instr);

#endif /* INSTRUMENT_H */


/*** End of file ***/
