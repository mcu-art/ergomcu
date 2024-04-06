#ifndef ERGO_UART_H
#define ERGO_UART_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "ergo_uart_settings.h"
#include "ringbuf32.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 *
 */
typedef struct ergo_uart_s {
  ringbuf32_t rx_ring_buf;
  uint32_t rx_ring_buf_size;
  ringbuf32_t tx_ring_buf;
  uint32_t tx_ring_buf_size;
  volatile uint32_t _last_writex_index;
  /* Stores last data byte index in the rx primary buffer.
It is used to detect if there is new data	in the rx buffer
  in the rx callback function. */
  volatile uint16_t _last_rx_index;
  volatile bool rx_started;
  volatile bool tx_in_progress;
  volatile bool _writex_lock;
  uint8_t _device_id;
} ergo_uart_t;

/**
 * @brief
 * @param
 * @return
 */
bool ergo_uart_init(void);
bool ergo_uart_start_rx(ergo_uart_t *u);
bool ergo_uart_stop_rx(ergo_uart_t *u);

/* Read is thread-safe and lock-free in a SPSC (Single Consumer, Single
Producer) setup. ergo_uart doesn't support multiple readers (consumers). In case
you need them, you shoul still read the data in a single task, and then share it
between other tasks on your own. */

/**
 * @brief
 * @param u
 * @param data
 * @param max_size
 * @return
 */
uint32_t ergo_uart_read(ergo_uart_t *u, void *data, uint32_t max_size);
uint32_t ergo_uart_read_to_ringbuf(ergo_uart_t *u, ringbuf32_t *dest,
                                   uint32_t max_size);
uint32_t ergo_uart_retransmit(ergo_uart_t *src, ergo_uart_t *dest);

/* This function is only thread-safe and lock-free in a SPSC
(Single Consumer, Single Producer) setup.
If there are multiple writers in different threads or tasks,
use **ergo_uart_writex()** instead. */
uint32_t ergo_uart_write(ergo_uart_t *u, const void *src, uint32_t size);

/* Thread-safe exclusive write */
typedef struct ergo_uart_writex_result_s {
  uint32_t bytesWritten;
  bool lockAcquired;
} ergo_uart_writex_result_t;

ergo_uart_writex_result_t ergo_uart_writex(ergo_uart_t *u, const void *src,
                                           uint32_t size);
ergo_uart_writex_result_t ergo_uart_writex_part(ergo_uart_t *u, const void *src,
                                                uint32_t size);
void ergo_uart_writex_reset(ergo_uart_t *u);
void ergo_uart_writex_unlock(ergo_uart_t *u);

#if defined(UART1_USE_RX) || defined(UART1_USE_TX)
ergo_uart_t *ergo_uart1_get(void);
#endif

#if defined(UART2_USE_RX) || defined(UART2_USE_TX)
ergo_uart_t *ergo_uart2_get(void);
#endif

#if defined(UART3_USE_RX) || defined(UART3_USE_TX)
ergo_uart_t *ergo_uart3_get(void);
#endif

#if defined(UART4_USE_RX) || defined(UART4_USE_TX)
ergo_uart_t *ergo_uart4_get(void);
#endif

#if defined(UART5_USE_RX) || defined(UART5_USE_TX)
ergo_uart_t *ergo_uart5_get(void);
#endif

#if defined(UART6_USE_RX) || defined(UART6_USE_TX)
ergo_uart_t *ergo_uart6_get(void);
#endif

#if defined(LPUART1_USE_RX) || defined(LPUART1_USE_TX)
ergo_uart_t *ergo_lpuart1_get(void);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ERGO_UART_H */
