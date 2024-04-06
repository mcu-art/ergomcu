#ifndef SINGLE_LED_ERROR_INDICATOR_H
#define SINGLE_LED_ERROR_INDICATOR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define SLEI_OFF 0xFF
#define SLEI_NO_ERROR 0

    void DisplayError(uint8_t err);
    void SingleLedErrorIndicator_OnOneMillisecondTimer(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SINGLE_LED_ERROR_INDICATOR_H */
