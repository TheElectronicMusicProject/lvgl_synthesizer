/**
 * @file    main.c
 * 
 * @brief   Main function with instrument instantiation.
 * 
 * @author  Filippo Graziani (circuitry.passion@gmail.com)
 * 
 * @date    15/07/2025
 * 
 * @par     Description
 * Init of LVGL and HAL, then init of instrument and creation of its graphics.
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
 #include "lvgl.h"
#include "app_hal.h"
#include <stdio.h>

/******************************************************************************
 *                           PUBLIC FUNCTION BODIES                           *
 ******************************************************************************/
int
main (void)
{
	lv_init();

	hal_setup();

	instrument_t my_piano = {0};
	uint8_t ret = init_instrument(&my_piano);

	if (0 == ret)
	{
		exit(EXIT_FAILURE);
	}

	create_instrument(&my_piano);

	lv_log("Hello %s\n", "World");
	fflush(NULL);

	hal_loop();
}	/* main() */


/*** End of file ***/
