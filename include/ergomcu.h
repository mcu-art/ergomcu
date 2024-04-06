#ifndef ERGOMCU_H
#define ERGOMCU_H

#include <stdbool.h>
#include <stdint.h>

#include "ergo_settings.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef ERGO_USE_RTOS
#define ERGO_DELAY(X) osDelay(X)
#else
#define ERGO_DELAY(X) HAL_Delay(X)
#endif

/* INTERRUPTS SECTION */
#ifdef ERGO_USE_RTOS
#define ERGO_DISABLE_INTERRUPTS taskENTER_CRITICAL()
#define ERGO_RESTORE_INTERRUPTS taskEXIT_CRITICAL()
#else /* bare metal */
#define ERGO_DISABLE_INTERRUPTS                     \
  bool interrupts_enabled = (__get_PRIMASK() == 0); \
  __disable_irq();

#define ERGO_RESTORE_INTERRUPTS \
  if (interrupts_enabled) {     \
    __enable_irq();             \
  }
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ERGOMCU_H */
