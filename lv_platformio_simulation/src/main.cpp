/**
	******************************************************************************
	* @file    main.c
	* @author  Ac6
	* @version V1.0
	* @date    01-December-2013
	* @brief   Default main function.
	******************************************************************************
*/


#include "lvgl.h"
#include "app_hal.h"
#include <stdio.h>
#include "instrument.h"

#include "demos/lv_demos.h"

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
