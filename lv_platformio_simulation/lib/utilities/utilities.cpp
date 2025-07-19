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

#ifdef __linux__

/******************************************************************************
 *                              INCLUDE STATEMENS                             *
 * The headers for each grouping should be sorted alphabetically:             *
 * 1 - The paired header file                                                 *
 * 2 - Other headers from your project                                        *
 * 3 - 3rd party library headers                                              *
 * 4 - Standard library headers                                               *
 ******************************************************************************/
#   include "utilities.h"
#   include <termios.h>
#   include <unistd.h>
#   include <stdio.h>
#   include <fcntl.h>

/******************************************************************************
 *                        DATA TYPES, CONSTANTS, MACROS                       *
 ******************************************************************************/

/******************************************************************************
 *                              STATIC VARIABLES                              *
 ******************************************************************************/

/******************************************************************************
 *                        PRIVATE FUNCTIONS PROTOTYPES                        *
 ******************************************************************************/

/******************************************************************************
 *                           PUBLIC FUNCTION BODIES                           *
 ******************************************************************************/

 /******************************************************************************
 *                           PRIVATE FUNCTION BODIES                          *
 ******************************************************************************/
int32_t
get_key (void)
{
    int32_t ret = 0;

    // Hold the current terminal configuration.
    //
    struct termios oldterm = {0};
    struct termios newterm = {0};

    // We want to get the current terminal settings.
    //
    tcgetattr(STDIN_FILENO, &oldterm);
    newterm = oldterm;

    // Set the terminal to disable two flags: we don't care about the new line
    // and we don't want the echo. So it doesn't block.
    //
    newterm.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newterm);

    // Obtain the file handler.
    //
    int32_t h_oldfile = fcntl(STDIN_FILENO, F_GETFL, 0);

    // Set it so it doesn't block.
    //
    fcntl(STDIN_FILENO, F_SETFL, h_oldfile | O_NONBLOCK);

    // After gettin the char, we set back the original configuration of the
    // file handler and terminal.
    //
    int32_t in_ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldterm);
    fcntl(STDIN_FILENO, F_SETFL, h_oldfile);

    // Before return, we return the char and we clear the terminal.
    //
    if (EOF != in_ch)
    {
        ret = in_ch;
    }

    return ret;
}   /* get_key() */

#endif /* __linux__ */