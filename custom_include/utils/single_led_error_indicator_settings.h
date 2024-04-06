#ifndef SINGLE_LED_ERROR_INDICATOR_SETTINGS_H
#define SINGLE_LED_ERROR_INDICATOR_SETTINGS_H

/* CubeMX usually generates GPIO port and pin definitions in main.h.
If you don't use those definitions in your project, comment this line out. */
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Define macros that switch the LED on and off.
Example (in main.h):
#define LedUser1_ON HAL_GPIO_WritePin(LedUser1_GPIO_Port, LedUser1_Pin,
GPIO_PIN_SET) #define LedUser1_OFF HAL_GPIO_WritePin(LedUser1_GPIO_Port,
LedUser1_Pin, GPIO_PIN_RESET)

In this file:
#define SL_INDICATOR_LED_ON LedUser1_ON
#define SL_INDICATOR_LED_OFF LedUser1_OFF

*/

/* Modify no-error indication if required
by uncommenting one of the following lines.
The default style is blinking with 1 second period. */
/* #define SL_INDICATOR_NO_ERROR_PERMANENTLY_ON */
/* #define SL_INDICATOR_NO_ERROR_PERMANENTLY_OFF */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SINGLE_LED_ERROR_INDICATOR_SETTINGS_H */