/**
 * @file    utilities.h
 * 
 * @brief   Additional function to help the simulator
 * 
 * @author  Filippo Graziani (circuitry.passion@gmail.com)
 * 
 * @date    19/07/2025
 * 
 * @par     Description
 * - get_key() detect the input key from the physical keyboard.
 */

#ifndef UTILITIES_H
#   define  UTILITIES_H
#   ifdef __linux__

/******************************************************************************
 *                              INCLUDE STATEMENS                             *
 * The headers for each grouping should be sorted alphabetically:             *
 * 1 - The paired header file                                                 *
 * 2 - Other headers from your project                                        *
 * 3 - 3rd party library headers                                              *
 * 4 - Standard library headers                                               *
 ******************************************************************************/
#       include <stdint.h>

/******************************************************************************
 *                            PROTOTYPES OR MACROS                            *
 ******************************************************************************/

/******************************************************************************
 *                                   #DEFINE                                  *
 ******************************************************************************/

 /******************************************************************************
 *                      TYPEDEFS (STRUCTS, ENUMS, UNIONS)                     *
 ******************************************************************************/

 /******************************************************************************
 *                         PUBLIC FUNCTIONS PROTOTYPES                        *
 ******************************************************************************/
int32_t get_key(void);

#   endif /* __linux__ */

#endif /* UTILITIES_H */


/*** End of file ***/
