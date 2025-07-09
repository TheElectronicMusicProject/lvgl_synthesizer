#ifndef INSTRUMENT_H

#   define INSTRUMENT_H
#   include <stdint.h>

#   define INSTR_NUM_KEY    (13)
#   define SCREEN_WIDTH     (320)
#   define SCREEN_HEIGHT    (240)

typedef struct key_number_t
{
    uint8_t num;
    char key_name[3];
} key_number_t;

typedef struct properties_t
{
    uint8_t volume;
    uint8_t waveform;
} properties_t;

typedef struct instrument_t
{
    key_number_t key[INSTR_NUM_KEY];
    properties_t prop;
    uint8_t q_key_press;
} instrument_t;

void create_instrument(instrument_t * p_instr);
uint8_t init_instrument(instrument_t * p_instr);

#endif /* INSTRUMENT_H */