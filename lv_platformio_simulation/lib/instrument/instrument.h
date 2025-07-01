#ifndef INSTRUMENT_H

#   define INSTRUMENT_H
#   include <stdint.h>
typedef struct key_number_t
{
    uint8_t num;
} key_number_t;


void create_instrument(void);

#endif /* INSTRUMENT_H */