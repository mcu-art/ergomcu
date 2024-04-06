/**
 *
 */
#include "ergo_uart.h"

#include <stdbool.h>
#include <stdint.h>

#include "ergo_uart_settings.h"
#include "ergomcu.h"
#include "main.h"
#include "ringbuf32.h"
#include "usart.h"

/* ================ MACROS ================ */
#define MIN_VAL(x, y) ((x) < (y) ? (x) : (y))

#define CLEAR_UART_RX_ERROR_FLAGS(X) \
  __HAL_UART_CLEAR_PEFLAG(X);        \
  __HAL_UART_CLEAR_FEFLAG(X);        \
  __HAL_UART_CLEAR_NEFLAG(X);        \
  __HAL_UART_CLEAR_OREFLAG(X);

/* ================ VARIABLES ================ */

/* UART1 */
#if defined(UART1_USE_RX) || defined(UART1_USE_TX)
ergo_uart_t ergo_uart1;
ergo_uart_t *ergo_uart1_get(void) { return &ergo_uart1; }
#ifdef UART1_USE_RX
uint8_t uart1_prim_rx_buf[UART1_PRIMARY_RX_BUF_SIZE];
uint8_t uart1_sec_rx_buf[UART1_SECONDARY_RX_BUF_SIZE];
#endif
#ifdef UART1_USE_TX
uint8_t uart1_prim_tx_buf[UART1_PRIMARY_TX_BUF_SIZE];
uint8_t uart1_sec_tx_buf[UART1_SECONDARY_TX_BUF_SIZE];
#endif
#endif /* UART1 */

/* UART2 */
#if defined(UART2_USE_RX) || defined(UART2_USE_TX)
ergo_uart_t ergo_uart2;
ergo_uart_t *ergo_uart2_get(void) { return &ergo_uart2; }
#ifdef UART2_USE_RX
uint8_t uart2_prim_rx_buf[UART2_PRIMARY_RX_BUF_SIZE];
uint8_t uart2_sec_rx_buf[UART2_SECONDARY_RX_BUF_SIZE];
#endif
#ifdef UART2_USE_TX
uint8_t uart2_prim_tx_buf[UART2_PRIMARY_TX_BUF_SIZE];
uint8_t uart2_sec_tx_buf[UART2_SECONDARY_TX_BUF_SIZE];
#endif
#endif /* UART2 */

/* UART3 */
#if defined(UART3_USE_RX) || defined(UART3_USE_TX)
ergo_uart_t ergo_uart3;
ergo_uart_t *ergo_uart3_get(void) { return &ergo_uart3; }
#ifdef UART3_USE_RX
uint8_t uart3_prim_rx_buf[UART3_PRIMARY_RX_BUF_SIZE];
uint8_t uart3_sec_rx_buf[UART3_SECONDARY_RX_BUF_SIZE];
#endif
#ifdef UART3_USE_TX
uint8_t uart3_prim_tx_buf[UART3_PRIMARY_TX_BUF_SIZE];
uint8_t uart3_sec_tx_buf[UART3_SECONDARY_TX_BUF_SIZE];
#endif
#endif /* UART3 */

/* UART4 */
#if defined(UART4_USE_RX) || defined(UART4_USE_TX)
ergo_uart_t ergo_uart4;
ergo_uart_t *ergo_uart4_get(void) { return &ergo_uart4; }
#ifdef UART4_USE_RX
uint8_t uart4_prim_rx_buf[UART4_PRIMARY_RX_BUF_SIZE];
uint8_t uart4_sec_rx_buf[UART4_SECONDARY_RX_BUF_SIZE];
#endif
#ifdef UART4_USE_TX
uint8_t uart4_prim_tx_buf[UART4_PRIMARY_TX_BUF_SIZE];
uint8_t uart4_sec_tx_buf[UART4_SECONDARY_TX_BUF_SIZE];
#endif
#endif /* UART4 */

/* UART5 */
#if defined(UART5_USE_RX) || defined(UART5_USE_TX)
ergo_uart_t ergo_uart5;
ergo_uart_t *ergo_uart5_get(void) { return &ergo_uart5; }
#ifdef UART5_USE_RX
uint8_t uart5_prim_rx_buf[UART5_PRIMARY_RX_BUF_SIZE];
uint8_t uart5_sec_rx_buf[UART5_SECONDARY_RX_BUF_SIZE];
#endif
#ifdef UART5_USE_TX
uint8_t uart5_prim_tx_buf[UART5_PRIMARY_TX_BUF_SIZE];
uint8_t uart5_sec_tx_buf[UART5_SECONDARY_TX_BUF_SIZE];
#endif
#endif /* UART5 */

/* UART6 */
#if defined(UART6_USE_RX) || defined(UART6_USE_TX)
ergo_uart_t ergo_uart6;
ergo_uart_t *ergo_uart6_get(void) { return &ergo_uart6; }
#ifdef UART6_USE_RX
uint8_t uart6_prim_rx_buf[UART6_PRIMARY_RX_BUF_SIZE];
uint8_t uart6_sec_rx_buf[UART6_SECONDARY_RX_BUF_SIZE];
#endif
#ifdef UART6_USE_TX
uint8_t uart6_prim_tx_buf[UART6_PRIMARY_TX_BUF_SIZE];
uint8_t uart6_sec_tx_buf[UART6_SECONDARY_TX_BUF_SIZE];
#endif
#endif /* UART6 */

/* LPUART1 */
#if defined(LPUART1_USE_RX) || defined(LPUART1_USE_TX)
ergo_uart_t ergo_lpuart1;
ergo_uart_t *ergo_lpuart1_get(void) { return &ergo_lpuart1; }
#ifdef LPUART1_USE_RX
uint8_t lpuart1_prim_rx_buf[LPUART1_PRIMARY_RX_BUF_SIZE];
uint8_t lpuart1_sec_rx_buf[LPUART1_SECONDARY_RX_BUF_SIZE];
#endif
#ifdef LPUART1_USE_TX
uint8_t lpuart1_prim_tx_buf[LPUART1_PRIMARY_TX_BUF_SIZE];
uint8_t lpuart1_sec_tx_buf[LPUART1_SECONDARY_TX_BUF_SIZE];
#endif
#endif /* LPUART1 */

/* ================ FUNCTIONS ================ */

/**
 * \brief           Initialize all uarts that are configured to be used
 * in ergo_uart_settings.h
 * \return          `true` on success, `false` otherwise
 */
bool ergo_uart_init(void) {
/* UART1 INIT */
#if defined(UART1_USE_RX) || defined(UART1_USE_TX)
  // Set device id
  ergo_uart1._device_id = '1';
// Init ring buffers
#ifdef UART1_USE_RX
  ringbuf32_init(&ergo_uart1.rx_ring_buf, uart1_sec_rx_buf,
                 UART1_SECONDARY_RX_BUF_SIZE);
  ergo_uart1.rx_ring_buf_size = UART1_SECONDARY_RX_BUF_SIZE;
#endif
#ifdef UART1_USE_TX
  ringbuf32_init(&ergo_uart1.tx_ring_buf, uart1_sec_tx_buf,
                 UART1_SECONDARY_TX_BUF_SIZE);
  ergo_uart1.tx_ring_buf_size = UART1_SECONDARY_TX_BUF_SIZE;
#endif /* UART1_USE_TX */
#endif /* UART1 INIT */

/* UART2 INIT */
#if defined(UART2_USE_RX) || defined(UART2_USE_TX)
  // Set device id
  ergo_uart2._device_id = '2';
// Init ring buffers
#ifdef UART2_USE_RX
  ringbuf32_init(&ergo_uart2.rx_ring_buf, uart2_sec_rx_buf,
                 UART2_SECONDARY_RX_BUF_SIZE);
  ergo_uart2.rx_ring_buf_size = UART2_SECONDARY_RX_BUF_SIZE;
#endif
#ifdef UART2_USE_TX
  ringbuf32_init(&ergo_uart2.tx_ring_buf, uart2_sec_tx_buf,
                 UART2_SECONDARY_TX_BUF_SIZE);
  ergo_uart2.tx_ring_buf_size = UART2_SECONDARY_TX_BUF_SIZE;
#endif /* UART2_USE_TX */
#endif /* UART2 INIT */

/* UART3 INIT */
#if defined(UART3_USE_RX) || defined(UART3_USE_TX)
  // Set device id
  ergo_uart3._device_id = '3';
// Init ring buffers
#ifdef UART3_USE_RX
  ringbuf32_init(&ergo_uart3.rx_ring_buf, uart3_sec_rx_buf,
                 UART3_SECONDARY_RX_BUF_SIZE);
  ergo_uart3.rx_ring_buf_size = UART3_SECONDARY_RX_BUF_SIZE;
#endif
#ifdef UART3_USE_TX
  ringbuf32_init(&ergo_uart3.tx_ring_buf, uart3_sec_tx_buf,
                 UART3_SECONDARY_TX_BUF_SIZE);
  ergo_uart3.tx_ring_buf_size = UART3_SECONDARY_TX_BUF_SIZE;
#endif /* UART3_USE_TX */
#endif /* UART3 INIT */

/* UART4 INIT */
#if defined(UART4_USE_RX) || defined(UART4_USE_TX)
  // Set device id
  ergo_uart4._device_id = '4';
// Init ring buffers
#ifdef UART4_USE_RX
  ringbuf32_init(&ergo_uart4.rx_ring_buf, uart4_sec_rx_buf,
                 UART4_SECONDARY_RX_BUF_SIZE);
  ergo_uart4.rx_ring_buf_size = UART4_SECONDARY_RX_BUF_SIZE;
#endif
#ifdef UART4_USE_TX
  ringbuf32_init(&ergo_uart4.tx_ring_buf, uart4_sec_tx_buf,
                 UART4_SECONDARY_TX_BUF_SIZE);
  ergo_uart4.tx_ring_buf_size = UART4_SECONDARY_TX_BUF_SIZE;
#endif /* UART4_USE_TX */
#endif /* UART4 INIT */

/* UART5 INIT */
#if defined(UART5_USE_RX) || defined(UART5_USE_TX)
  // Set device id
  ergo_uart5._device_id = '5';
// Init ring buffers
#ifdef UART5_USE_RX
  ringbuf32_init(&ergo_uart5.rx_ring_buf, uart5_sec_rx_buf,
                 UART5_SECONDARY_RX_BUF_SIZE);
  ergo_uart5.rx_ring_buf_size = UART5_SECONDARY_RX_BUF_SIZE;
#endif
#ifdef UART5_USE_TX
  ringbuf32_init(&ergo_uart5.tx_ring_buf, uart5_sec_tx_buf,
                 UART5_SECONDARY_TX_BUF_SIZE);
  ergo_uart5.tx_ring_buf_size = UART5_SECONDARY_TX_BUF_SIZE;
#endif /* UART5_USE_TX */
#endif /* UART5 INIT */

/* UART6 INIT */
#if defined(UART6_USE_RX) || defined(UART6_USE_TX)
  // Set device id
  ergo_uart6._device_id = '6';
// Init ring buffers
#ifdef UART6_USE_RX
  ringbuf32_init(&ergo_uart6.rx_ring_buf, uart6_sec_rx_buf,
                 UART6_SECONDARY_RX_BUF_SIZE);
  ergo_uart6.rx_ring_buf_size = UART6_SECONDARY_RX_BUF_SIZE;
#endif
#ifdef UART6_USE_TX
  ringbuf32_init(&ergo_uart6.tx_ring_buf, uart6_sec_tx_buf,
                 UART6_SECONDARY_TX_BUF_SIZE);
  ergo_uart6.tx_ring_buf_size = UART6_SECONDARY_TX_BUF_SIZE;
#endif /* UART6_USE_TX */
#endif /* UART6 INIT */

/* LPUART1 INIT */
#if defined(LPUART1_USE_RX) || defined(LPUART1_USE_TX)
  // Set device id
  ergo_lpuart1._device_id = 'L';
// Init ring buffers
#ifdef LPUART1_USE_RX
  ringbuf32_init(&ergo_lpuart1.rx_ring_buf, lpuart1_sec_rx_buf,
                 LPUART1_SECONDARY_RX_BUF_SIZE);
  ergo_lpuart1.rx_ring_buf_size = LPUART1_SECONDARY_RX_BUF_SIZE;
#endif
#ifdef LPUART1_USE_TX
  ringbuf32_init(&ergo_lpuart1.tx_ring_buf, lpuart1_sec_tx_buf,
                 LPUART1_SECONDARY_TX_BUF_SIZE);
  ergo_lpuart1.tx_ring_buf_size = LPUART1_SECONDARY_TX_BUF_SIZE;
#endif /* LPUART1_USE_TX */
#endif /* LPUART1 INIT */

  return true;
}

/**
 * \brief           Read is thread-safe and lock-free in a SPSC
 * (Single Consumer, Single Producer) setup.
 * ergo_uart doesn't support multiple readers (consumers). In case you
 * need them, you should still read the data in a single task, and then share it
 * between other tasks on your own.
 * \param[in]       u: ergo_uart_t handle
 * \param[in]       dest: Pointer to memory into which to write data
 * \param[in]       max_size: Maximum number of bytes to be read
 *
 * \return          Number of bytes read
 */
uint32_t ergo_uart_read(ergo_uart_t *u, void *dest, uint32_t max_size) {
  return ringbuf32_read(&u->rx_ring_buf, dest, max_size);
}

uint32_t ergo_uart_read_to_ringbuf(ergo_uart_t *u, ringbuf32_t *dest,
                                   uint32_t max_size) {
  return ringbuf32_transfer(&u->rx_ring_buf, dest, max_size);
}

void _ergo_uart_trigger_pending_tx(void) {
  uint32_t dataSize = 0;

#ifdef UART1_USE_TX
  if (!ergo_uart1.tx_in_progress) {
    ergo_uart1.tx_in_progress = true;
    // Try transfer data from the tx_ring_buf into the primary transmission
    // buffer
    dataSize = ringbuf32_read(&ergo_uart1.tx_ring_buf, uart1_prim_tx_buf,
                              UART1_PRIMARY_TX_BUF_SIZE);
    if (dataSize > 0) {
#ifdef UART1_USE_TX_DMA
      // Transmit data using DMA
      if (HAL_OK ==
          HAL_UART_Transmit_DMA(&huart1, uart1_prim_tx_buf, dataSize)) {
        extern DMA_HandleTypeDef hdma_usart1_tx;
        // No need for the DMA half transfer interrupt
        __HAL_DMA_DISABLE_IT(&hdma_usart1_tx, DMA_IT_HT);
      } else {
        // TODO: check if further error processing required
        ergo_uart1.tx_in_progress = false;
      }
#else
      // Transmit data using interrupts
      if (HAL_OK ==
          HAL_UART_Transmit_IT(&huart1, uart1_prim_tx_buf, dataSize)) {
      } else {
        // TODO: check if further error processing required
        ergo_uart1.tx_in_progress = false;
      }
#endif
    } else {
      ergo_uart1.tx_in_progress = false;
    }
  }
#endif /* UART1_USE_TX */

#ifdef UART2_USE_TX
  if (!ergo_uart2.tx_in_progress) {
    ergo_uart2.tx_in_progress = true;
    // Try transfer data from the tx_ring_buf into the primary transmission
    // buffer
    dataSize = ringbuf32_read(&ergo_uart2.tx_ring_buf, uart2_prim_tx_buf,
                              UART2_PRIMARY_TX_BUF_SIZE);
    if (dataSize > 0) {
#ifdef UART2_USE_TX_DMA
      // Transmit data using DMA
      if (HAL_OK ==
          HAL_UART_Transmit_DMA(&huart2, uart2_prim_tx_buf, dataSize)) {
        extern DMA_HandleTypeDef hdma_usart2_tx;
        // No need for the DMA half transfer interrupt
        __HAL_DMA_DISABLE_IT(&hdma_usart2_tx, DMA_IT_HT);
      } else {
        // TODO: check if further error processing required
        ergo_uart2.tx_in_progress = false;
      }
#else
      // Transmit data using interrupts
      if (HAL_OK ==
          HAL_UART_Transmit_IT(&huart2, uart2_prim_tx_buf, dataSize)) {
      } else {
        // TODO: check if further error processing required
        ergo_uart2.tx_in_progress = false;
      }
#endif
    } else {
      ergo_uart2.tx_in_progress = false;
    }
  }
#endif /* UART2_USE_TX */

#ifdef UART3_USE_TX
  if (!ergo_uart3.tx_in_progress) {
    ergo_uart3.tx_in_progress = true;
    // Try transfer data from the tx_ring_buf into the primary transmission
    // buffer
    dataSize = ringbuf32_read(&ergo_uart3.tx_ring_buf, uart3_prim_tx_buf,
                              UART3_PRIMARY_TX_BUF_SIZE);
    if (dataSize > 0) {
#ifdef UART3_USE_TX_DMA
      // Transmit data using DMA
      if (HAL_OK ==
          HAL_UART_Transmit_DMA(&huart3, uart3_prim_tx_buf, dataSize)) {
        // No need for the DMA half transfer interrupt
        extern DMA_HandleTypeDef hdma_usart3_tx;
        __HAL_DMA_DISABLE_IT(&hdma_usart3_tx, DMA_IT_HT);
      } else {
        // TODO: check if further error processing required
        ergo_uart3.tx_in_progress = false;
      }
#else
      // Transmit data using interrupts
      if (HAL_OK ==
          HAL_UART_Transmit_IT(&huart3, uart3_prim_tx_buf, dataSize)) {
      } else {
        // TODO: check if further error processing required
        ergo_uart3.tx_in_progress = false;
      }
#endif
    } else {
      ergo_uart3.tx_in_progress = false;
    }
  }
#endif /* UART3_USE_TX */

#ifdef UART4_USE_TX
  if (!ergo_uart4.tx_in_progress) {
    ergo_uart4.tx_in_progress = true;
    // Try transfer data from the tx_ring_buf into the primary transmission
    // buffer
    dataSize = ringbuf32_read(&ergo_uart4.tx_ring_buf, uart4_prim_tx_buf,
                              UART4_PRIMARY_TX_BUF_SIZE);
    if (dataSize > 0) {
#ifdef UART4_USE_TX_DMA
      // Transmit data using DMA
      if (HAL_OK ==
          HAL_UART_Transmit_DMA(&huart4, uart4_prim_tx_buf, dataSize)) {
        // No need for the DMA half transfer interrupt
        extern DMA_HandleTypeDef hdma_usart4_tx;
        __HAL_DMA_DISABLE_IT(&hdma_usart4_tx, DMA_IT_HT);
      } else {
        // TODO: check if further error processing required
        ergo_uart4.tx_in_progress = false;
      }
#else
      // Transmit data using interrupts
      if (HAL_OK ==
          HAL_UART_Transmit_IT(&huart4, uart4_prim_tx_buf, dataSize)) {
      } else {
        // TODO: check if further error processing required
        ergo_uart4.tx_in_progress = false;
      }
#endif
    } else {
      ergo_uart4.tx_in_progress = false;
    }
  }
#endif /* UART4_USE_TX */

#ifdef UART5_USE_TX
  if (!ergo_uart5.tx_in_progress) {
    ergo_uart5.tx_in_progress = true;
    // Try transfer data from the tx_ring_buf into the primary transmission
    // buffer
    dataSize = ringbuf32_read(&ergo_uart5.tx_ring_buf, uart5_prim_tx_buf,
                              UART5_PRIMARY_TX_BUF_SIZE);
    if (dataSize > 0) {
#ifdef UART5_USE_TX_DMA
      // Transmit data using DMA
      if (HAL_OK ==
          HAL_UART_Transmit_DMA(&huart5, uart5_prim_tx_buf, dataSize)) {
        // No need for the DMA half transfer interrupt
        extern DMA_HandleTypeDef hdma_usart5_tx;
        __HAL_DMA_DISABLE_IT(&hdma_usart5_tx, DMA_IT_HT);
      } else {
        // TODO: check if further error processing required
        ergo_uart5.tx_in_progress = false;
      }
#else
      // Transmit data using interrupts
      if (HAL_OK ==
          HAL_UART_Transmit_IT(&huart5, uart5_prim_tx_buf, dataSize)) {
      } else {
        // TODO: check if further error processing required
        ergo_uart5.tx_in_progress = false;
      }
#endif
    } else {
      ergo_uart5.tx_in_progress = false;
    }
  }
#endif /* UART5_USE_TX */

#ifdef UART6_USE_TX
  if (!ergo_uart6.tx_in_progress) {
    ergo_uart6.tx_in_progress = true;
    // Try transfer data from the tx_ring_buf into the primary transmission
    // buffer
    dataSize = ringbuf32_read(&ergo_uart6.tx_ring_buf, uart6_prim_tx_buf,
                              UART6_PRIMARY_TX_BUF_SIZE);
    if (dataSize > 0) {
#ifdef UART6_USE_TX_DMA
      // Transmit data using DMA
      if (HAL_OK ==
          HAL_UART_Transmit_DMA(&huart6, uart6_prim_tx_buf, dataSize)) {
        // No need for the DMA half transfer interrupt
        extern DMA_HandleTypeDef hdma_usart6_tx;
        __HAL_DMA_DISABLE_IT(&hdma_usart6_tx, DMA_IT_HT);
      } else {
        // TODO: check if further error processing required
        ergo_uart6.tx_in_progress = false;
      }
#else
      // Transmit data using interrupts
      if (HAL_OK ==
          HAL_UART_Transmit_IT(&huart6, uart6_prim_tx_buf, dataSize)) {
      } else {
        // TODO: check if further error processing required
        ergo_uart6.tx_in_progress = false;
      }
#endif
    } else {
      ergo_uart6.tx_in_progress = false;
    }
  }
#endif /* UART6_USE_TX */

#ifdef LPUART1_USE_TX
  if (!ergo_lpuart1.tx_in_progress) {
    ergo_lpuart1.tx_in_progress = true;
    // Try transfer data from the tx_ring_buf into the primary transmission
    // buffer
    dataSize = ringbuf32_read(&ergo_lpuart1.tx_ring_buf, lpuart1_prim_tx_buf,
                              LPUART1_PRIMARY_TX_BUF_SIZE);
    if (dataSize > 0) {
#ifdef LPUART1_USE_TX_DMA
      // Transmit data using DMA
      if (HAL_OK ==
          HAL_UART_Transmit_DMA(&hlpuart1, lpuart1_prim_tx_buf, dataSize)) {
        // No need for the DMA half transfer interrupt
        extern DMA_HandleTypeDef hdma_lpuart1_tx;
        __HAL_DMA_DISABLE_IT(&hdma_lpuart1_tx, DMA_IT_HT);
      } else {
        // TODO: check if further error processing required
        ergo_lpuart1.tx_in_progress = false;
      }
#else
      // Transmit data using interrupts
      if (HAL_OK ==
          HAL_UART_Transmit_IT(&hlpuart1, lpuart1_prim_tx_buf, dataSize)) {
      } else {
        // TODO: check if further error processing required
        ergo_lpuart1.tx_in_progress = false;
      }
#endif
    } else {
      ergo_lpuart1.tx_in_progress = false;
    }
  }
#endif /* LPUART1_USE_TX */
}

uint32_t ergo_uart_retransmit(ergo_uart_t *src, ergo_uart_t *dest) {
  uint32_t bytesTransferred = ringbuf32_transfer(
      &src->rx_ring_buf, &dest->tx_ring_buf, dest->tx_ring_buf_size);
  _ergo_uart_trigger_pending_tx();
  return bytesTransferred;
}

/**
 * \brief           Write is thread-safe and lock-free
 * in a SPSC (Single Consumer, Single Producer) setup.
 * If there are multiple writers in different threads or tasks,
 * use **ergo_uart_writex()** or **ergo_uart_writex_large()** instead.
 * \param[in]       u: ergo_uart_t handle
 * \param[in]       src: Pointer to memory from which data to be read
 * \param[in]       size: Number of bytes to write; this value must be less that
 * the uart's tx large buffer size.
 *
 * \return          Number of bytes written
 */
uint32_t ergo_uart_write(ergo_uart_t *u, const void *src, uint32_t size) {
  uint32_t bytesWritten = ringbuf32_write(&u->tx_ring_buf, src, size);
  _ergo_uart_trigger_pending_tx();
  return bytesWritten;
}

/**
 * \brief           Write data into specified UART atomically,
 * and require it for exclusive usage. Safe for multi-task and concurrent usage.
 * Data size must NOT exceed the size of the uart's
 * tx ring (aka secondary) buffer.
 * \param[in]       u: ergo_uart_t handle
 * \param[in]       src: Pointer to memory from which data to be read
 * \param[in]       size: Number of bytes to write
 * \return          Number of bytes written
 */
ergo_uart_writex_result_t ergo_uart_writex_part(ergo_uart_t *u, const void *src,
                                                uint32_t size) {
  ergo_uart_writex_result_t result = {u->_last_writex_index, u->_writex_lock};
  const uint8_t *srcBytes = (const uint8_t *)src;
  // Check if there is an ongoing writex session
  if (result.bytesWritten > 0 && result.lockAcquired) {
    // Try writing a new portion of data
    uint32_t btw = ringbuf32_free_size(&u->tx_ring_buf);
    btw = MIN_VAL(btw, size - result.bytesWritten);
    result.bytesWritten +=
        ergo_uart_write(u, srcBytes + result.bytesWritten, btw);
    u->_last_writex_index = result.bytesWritten;
    return result;
  }

  // There is no ongoing writex session, try acquire lock
  // disable interrupts
  ERGO_DISABLE_INTERRUPTS;
  uint32_t btw;  // bytes to write
  if (u->_writex_lock != false) {
    goto writex_exit;
  }
  u->_writex_lock = true;
  result.lockAcquired = true;

  // Try writing a new portion of data
  btw = ringbuf32_free_size(&u->tx_ring_buf);
  btw = MIN_VAL(btw, size - result.bytesWritten);
  result.bytesWritten +=
      ergo_uart_write(u, srcBytes + result.bytesWritten, btw);
  u->_last_writex_index = result.bytesWritten;

  // Release lock if no data written
  if (result.bytesWritten == 0) {
    result.lockAcquired = false;
    u->_writex_lock = false;
  }

writex_exit:
  ERGO_RESTORE_INTERRUPTS;
  return result;
}

void ergo_uart_writex_reset(ergo_uart_t *u) { u->_last_writex_index = 0; }

void ergo_uart_writex_unlock(ergo_uart_t *u) { u->_writex_lock = false; }

/**
 * \brief           Writes data of any size into specified UART.
 * Synchronization is applied to synchronize writes between different tasks.
 * This function blocks until all data is written,
 * but when RTOS is configured in ergo_settings.h,
 * it calls osDelay each time when idle.
 * \param[in]       u: ergo_uart_t handle
 * \param[in]       src: Pointer to memory from which data to be read
 * \param[in]       size: Number of bytes to write
 * \return          Number of bytes written
 */
ergo_uart_writex_result_t ergo_uart_writex(ergo_uart_t *u, const void *src,
                                           uint32_t size) {
  ergo_uart_writex_result_t r;
  do {
    r = ergo_uart_writex_part(u, src, size);
    if (r.bytesWritten != size) {
      ERGO_DELAY(1);
    }
  } while (r.bytesWritten != size);
  ergo_uart_writex_reset(u);
  ergo_uart_writex_unlock(u);
  return r;
}

#ifdef UART1_USE_RX
bool _ergo_uart1_start_rx(void) {
#ifdef UART1_USE_RX_DMA
  if (HAL_OK == HAL_UARTEx_ReceiveToIdle_DMA(&huart1, uart1_prim_rx_buf,
                                             UART1_PRIMARY_RX_BUF_SIZE)) {
    ergo_uart1.rx_started = true;
    return true;
  }
#else
  if (HAL_OK == HAL_UARTEx_ReceiveToIdle_IT(&huart1, uart1_prim_rx_buf,
                                            UART1_PRIMARY_RX_BUF_SIZE)) {
    ergo_uart1.rx_started = true;
    return true;
  }
#endif
  ergo_uart1.rx_started = false;
  return false;
}

bool _ergo_uart1_stop_rx(void) {
  HAL_UART_AbortReceive(&huart1);
  ergo_uart1.rx_started = false;
  return true;
}
#endif

#ifdef UART2_USE_RX
bool _ergo_uart2_start_rx(void) {
#ifdef UART2_USE_RX_DMA
  if (HAL_OK == HAL_UARTEx_ReceiveToIdle_DMA(&huart2, uart2_prim_rx_buf,
                                             UART2_PRIMARY_RX_BUF_SIZE)) {
    ergo_uart2.rx_started = true;
    return true;
  }
#else
  if (HAL_OK == HAL_UARTEx_ReceiveToIdle_IT(&huart2, uart2_prim_rx_buf,
                                            UART2_PRIMARY_RX_BUF_SIZE)) {
    ergo_uart2.rx_started = true;
    return true;
  }
#endif
  ergo_uart2.rx_started = false;
  return false;
}

bool _ergo_uart2_stop_rx(void) {
  HAL_UART_AbortReceive(&huart2);
  ergo_uart2.rx_started = false;
  return true;
}
#endif

#ifdef UART3_USE_RX
bool _ergo_uart3_start_rx(void) {
#ifdef UART3_USE_RX_DMA
  if (HAL_OK == HAL_UARTEx_ReceiveToIdle_DMA(&huart3, uart3_prim_rx_buf,
                                             UART3_PRIMARY_RX_BUF_SIZE)) {
    ergo_uart3.rx_started = true;
    return true;
  }
#else
  if (HAL_OK == HAL_UARTEx_ReceiveToIdle_IT(&huart3, uart3_prim_rx_buf,
                                            UART3_PRIMARY_RX_BUF_SIZE)) {
    ergo_uart3.rx_started = true;
    return true;
  }
#endif
  ergo_uart3.rx_started = false;
  return false;
}

bool _ergo_uart3_stop_rx(void) {
  HAL_UART_AbortReceive(&huart3);
  ergo_uart3.rx_started = false;
  return true;
}
#endif

#ifdef UART4_USE_RX
bool _ergo_uart4_start_rx(void) {
#ifdef UART4_USE_RX_DMA
  if (HAL_OK == HAL_UARTEx_ReceiveToIdle_DMA(&huart4, uart4_prim_rx_buf,
                                             UART4_PRIMARY_RX_BUF_SIZE)) {
    ergo_uart4.rx_started = true;
    return true;
  }
#else
  if (HAL_OK == HAL_UARTEx_ReceiveToIdle_IT(&huart4, uart4_prim_rx_buf,
                                            UART4_PRIMARY_RX_BUF_SIZE)) {
    ergo_uart4.rx_started = true;
    return true;
  }
#endif
  ergo_uart4.rx_started = false;
  return false;
}

bool _ergo_uart4_stop_rx(void) {
  HAL_UART_AbortReceive(&huart4);
  ergo_uart4.rx_started = false;
  return true;
}
#endif

#ifdef UART5_USE_RX
bool _ergo_uart5_start_rx(void) {
#ifdef UART5_USE_RX_DMA
  if (HAL_OK == HAL_UARTEx_ReceiveToIdle_DMA(&huart5, uart5_prim_rx_buf,
                                             UART5_PRIMARY_RX_BUF_SIZE)) {
    ergo_uart5.rx_started = true;
    return true;
  }
#else
  if (HAL_OK == HAL_UARTEx_ReceiveToIdle_IT(&huart5, uart5_prim_rx_buf,
                                            UART5_PRIMARY_RX_BUF_SIZE)) {
    ergo_uart5.rx_started = true;
    return true;
  }
#endif
  ergo_uart5.rx_started = false;
  return false;
}

bool _ergo_uart5_stop_rx(void) {
  HAL_UART_AbortReceive(&huart5);
  ergo_uart5.rx_started = false;
  return true;
}
#endif

#ifdef UART6_USE_RX
bool _ergo_uart6_start_rx(void) {
#ifdef UART6_USE_RX_DMA
  if (HAL_OK == HAL_UARTEx_ReceiveToIdle_DMA(&huart6, uart6_prim_rx_buf,
                                             UART6_PRIMARY_RX_BUF_SIZE)) {
    ergo_uart6.rx_started = true;
    return true;
  }
#else
  if (HAL_OK == HAL_UARTEx_ReceiveToIdle_IT(&huart6, uart6_prim_rx_buf,
                                            UART6_PRIMARY_RX_BUF_SIZE)) {
    ergo_uart6.rx_started = true;
    return true;
  }
#endif
  ergo_uart6.rx_started = false;
  return false;
}

bool _ergo_uart6_stop_rx(void) {
  HAL_UART_AbortReceive(&huart6);
  ergo_uart6.rx_started = false;
  return true;
}
#endif

#ifdef LPUART1_USE_RX
bool _ergo_lpuart1_start_rx(void) {
#ifdef LPUART1_USE_RX_DMA
  if (HAL_OK == HAL_UARTEx_ReceiveToIdle_DMA(&hlpuart1, lpuart1_prim_rx_buf,
                                             LPUART1_PRIMARY_RX_BUF_SIZE)) {
    ergo_lpuart1.rx_started = true;
    return true;
  }
#else
  if (HAL_OK == HAL_UARTEx_ReceiveToIdle_IT(&hlpuart1, lpuart1_prim_rx_buf,
                                            LPUART1_PRIMARY_RX_BUF_SIZE)) {
    ergo_lpuart1.rx_started = true;
    return true;
  }
#endif
  ergo_lpuart1.rx_started = false;
  return false;
}

bool _ergo_lpuart1_stop_rx(void) {
  HAL_UART_AbortReceive(&hlpuart1);
  ergo_lpuart1.rx_started = false;
  return true;
}
#endif

bool ergo_uart_start_rx(ergo_uart_t *u) {
  if (u->rx_started) {
    return true;
  }
  u->_last_rx_index = 0;

#ifdef UART1_USE_RX
  if (u->_device_id == '1') {
    return _ergo_uart1_start_rx();
  }
#endif

#ifdef UART2_USE_RX
  if (u->_device_id == '2') {
    return _ergo_uart2_start_rx();
  }
#endif

#ifdef UART3_USE_RX
  if (u->_device_id == '3') {
    return _ergo_uart3_start_rx();
  }
#endif

#ifdef UART4_USE_RX
  if (u->_device_id == '4') {
    return _ergo_uart4_start_rx();
  }
#endif

#ifdef UART5_USE_RX
  if (u->_device_id == '5') {
    return _ergo_uart5_start_rx();
  }
#endif

#ifdef UART6_USE_RX
  if (u->_device_id == '6') {
    return _ergo_uart6_start_rx();
  }
#endif

#ifdef LPUART1_USE_RX
  if (u->_device_id == 'L') {
    return _ergo_lpuart1_start_rx();
  }
#endif
  return false;
}

bool ergo_uart_stop_rx(ergo_uart_t *u) {
#ifdef UART1_USE_RX
  if (u->_device_id == '1') {
    return _ergo_uart1_stop_rx();
  }
#endif

#ifdef UART2_USE_RX
  if (u->_device_id == '2') {
    return _ergo_uart2_stop_rx();
  }
#endif

#ifdef UART3_USE_RX
  if (u->_device_id == '3') {
    return _ergo_uart3_stop_rx();
  }
#endif

#ifdef UART4_USE_RX
  if (u->_device_id == '4') {
    return _ergo_uart4_stop_rx();
  }
#endif

#ifdef UART5_USE_RX
  if (u->_device_id == '5') {
    return _ergo_uart5_stop_rx();
  }
#endif

#ifdef UART6_USE_RX
  if (u->_device_id == '6') {
    return _ergo_uart6_stop_rx();
  }
#endif

#ifdef LPUART1_USE_RX
  if (u->_device_id == 'L') {
    return _ergo_lpuart1_stop_rx();
  }
#endif
  return false;
}

/* ===============CALLBACKS BEGIN=============== */

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
#ifdef UART1_USE_TX
  if (huart->Instance == USART1) {
    // Try to transmit more data from tx_ring_buf if available
    ergo_uart1.tx_in_progress = false;
    _ergo_uart_trigger_pending_tx();
  }
#endif

#ifdef UART2_USE_TX
  if (huart->Instance == USART2) {
    // Try to transmit more data from tx_ring_buf if available
    ergo_uart2.tx_in_progress = false;
    _ergo_uart_trigger_pending_tx();
  }
#endif

#ifdef UART3_USE_TX
  if (huart->Instance == USART3) {
    // Try to transmit more data from tx_ring_buf if available
    ergo_uart3.tx_in_progress = false;
    _ergo_uart_trigger_pending_tx();
  }
#endif

#ifdef UART4_USE_TX
  if (huart->Instance == USART4) {
    // Try to transmit more data from tx_ring_buf if available
    ergo_uart4.tx_in_progress = false;
    _ergo_uart_trigger_pending_tx();
  }
#endif

#ifdef UART5_USE_TX
  if (huart->Instance == USART5) {
    // Try to transmit more data from tx_ring_buf if available
    ergo_uart5.tx_in_progress = false;
    _ergo_uart_trigger_pending_tx();
  }
#endif

#ifdef UART6_USE_TX
  if (huart->Instance == USART6) {
    // Try to transmit more data from tx_ring_buf if available
    ergo_uart6.tx_in_progress = false;
    _ergo_uart_trigger_pending_tx();
  }
#endif

#ifdef LPUART1_USE_TX
  if (huart->Instance == LPUART1) {
    // Try to transmit more data from tx_ring_buf if available
    ergo_lpuart1.tx_in_progress = false;
    _ergo_uart_trigger_pending_tx();
  }
#endif
}

void _process_DMARxCallback(ergo_uart_t *e, const uint8_t *dmaRxBuf,
                            uint16_t Size) {
  uint16_t last_pos = e->_last_rx_index;
  if (last_pos == Size) {
    // No new data
    return;
  }

  if (Size > last_pos) {
    uint32_t bytesToWrite = Size - last_pos;
    ringbuf32_write(&e->rx_ring_buf, dmaRxBuf + last_pos, bytesToWrite);
    e->_last_rx_index = Size;
    return;
  }
  // This only happens
  // when _last_rx_index==UART1_DMA_RX_BUF_SIZE.
  // Write data chunk from buffer[0] to buffer[Size].
  ringbuf32_write(&e->rx_ring_buf, dmaRxBuf, Size);
  e->_last_rx_index = Size;
}

void _process_ITRxCallback(ergo_uart_t *e, const uint8_t *itRxBuf,
                           uint16_t Size) {
  // In case of interrupts, there is no need to store last_rx_index
  // as the data always starts from the beginning of the rx buffer.
  if (Size == 0) {
    return;
  }
  ringbuf32_write(&e->rx_ring_buf, itRxBuf, Size);
}

/* Parameter Size is the size of the data in DMA RX buffer (from the beginning),
 * and is equivalent to writerIndex of the DMA RX buffer.
 * It can have value from 1 to DMA RX buffer size.
 * Experiments show that it can't be 0 as there must be at least 1 received byte
 * in order this function to be called.
 * Size also retains after IDLE_LINE interrupt.
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
#ifdef UART1_USE_RX
  if (huart->Instance == USART1) {
#ifdef UART1_USE_RX_DMA
    _process_DMARxCallback(&ergo_uart1, uart1_prim_rx_buf, Size);
#else
    _process_ITRxCallback(&ergo_uart1, uart1_prim_rx_buf, Size);
    // Restart UART1 RX if it is not DMA
    HAL_UARTEx_ReceiveToIdle_IT(&huart1, uart1_prim_rx_buf,
                                UART1_PRIMARY_RX_BUF_SIZE);
#endif
  }
#endif

#ifdef UART2_USE_RX
  if (huart->Instance == USART2) {
#ifdef UART2_USE_RX_DMA
    _process_DMARxCallback(&ergo_uart2, uart2_prim_rx_buf, Size);
#else
    _process_ITRxCallback(&ergo_uart2, uart2_prim_rx_buf, Size);
    // Restart UART2 RX if it is not DMA
    HAL_UARTEx_ReceiveToIdle_IT(&huart2, uart2_prim_rx_buf,
                                UART2_PRIMARY_RX_BUF_SIZE);
#endif
  }
#endif

#ifdef UART3_USE_RX
  if (huart->Instance == USART3) {
#ifdef UART3_USE_RX_DMA
    _process_DMARxCallback(&ergo_uart3, uart3_prim_rx_buf, Size);
#else
    _process_ITRxCallback(&ergo_uart3, uart3_prim_rx_buf, Size);
    // Restart UART3 RX if it is not DMA
    HAL_UARTEx_ReceiveToIdle_IT(&huart3, uart3_prim_rx_buf,
                                UART3_PRIMARY_RX_BUF_SIZE);
#endif
  }
#endif

#ifdef UART4_USE_RX
  if (huart->Instance == USART4) {
#ifdef UART4_USE_RX_DMA
    _process_DMARxCallback(&ergo_uart4, uart4_prim_rx_buf, Size);
#else
    _process_ITRxCallback(&ergo_uart4, uart4_prim_rx_buf, Size);
    // Restart UART4 RX if it is not DMA
    HAL_UARTEx_ReceiveToIdle_IT(&huart4, uart4_prim_rx_buf,
                                UART4_PRIMARY_RX_BUF_SIZE);
#endif
  }
#endif

#ifdef UART5_USE_RX
  if (huart->Instance == USART5) {
#ifdef UART5_USE_RX_DMA
    _process_DMARxCallback(&ergo_uart5, uart5_prim_rx_buf, Size);
#else
    _process_ITRxCallback(&ergo_uart5, uart5_prim_rx_buf, Size);
    // Restart UART5 RX if it is not DMA
    HAL_UARTEx_ReceiveToIdle_IT(&huart5, uart5_prim_rx_buf,
                                UART5_PRIMARY_RX_BUF_SIZE);
#endif
  }
#endif

#ifdef UART6_USE_RX
  if (huart->Instance == USART6) {
#ifdef UART6_USE_RX_DMA
    _process_DMARxCallback(&ergo_uart6, uart6_prim_rx_buf, Size);
#else
    _process_ITRxCallback(&ergo_uart6, uart6_prim_rx_buf, Size);
    // Restart UART6 RX if it is not DMA
    HAL_UARTEx_ReceiveToIdle_IT(&huart6, uart6_prim_rx_buf,
                                UART6_PRIMARY_RX_BUF_SIZE);
#endif
  }
#endif

#ifdef LPUART1_USE_RX
  if (huart->Instance == LPUART1) {
#ifdef LPUART1_USE_RX_DMA
    _process_DMARxCallback(&ergo_lpuart1, lpuart1_prim_rx_buf, Size);
#else
    _process_ITRxCallback(&ergo_lpuart1, lpuart1_prim_rx_buf, Size);
    // Restart LPUART1 RX if it is not DMA
    HAL_UARTEx_ReceiveToIdle_IT(&hlpuart1, lpuart1_prim_rx_buf,
                                LPUART1_PRIMARY_RX_BUF_SIZE);
#endif
  }
#endif
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
  // Error handling is simple: just clear the error bits
  // and restore UART functionality.

#ifdef UART1_USE_RX
  if (huart->Instance == USART1) {
    ergo_uart_stop_rx(&ergo_uart1);
    CLEAR_UART_RX_ERROR_FLAGS(&huart1);
    ergo_uart_start_rx(&ergo_uart1);
  }
#endif

#ifdef UART2_USE_RX
  if (huart->Instance == USART2) {
    ergo_uart_stop_rx(&ergo_uart2);
    CLEAR_UART_RX_ERROR_FLAGS(&huart2);
    ergo_uart_start_rx(&ergo_uart2);
  }
#endif

#ifdef UART3_USE_RX
  if (huart->Instance == USART3) {
    ergo_uart_stop_rx(&ergo_uart3);
    CLEAR_UART_RX_ERROR_FLAGS(&huart3);
    ergo_uart_start_rx(&ergo_uart3);
  }
#endif

#ifdef UART4_USE_RX
  if (huart->Instance == USART4) {
    ergo_uart_stop_rx(&ergo_uart4);
    CLEAR_UART_RX_ERROR_FLAGS(&huart4);
    ergo_uart_start_rx(&ergo_uart4);
  }
#endif

#ifdef UART5_USE_RX
  if (huart->Instance == USART5) {
    ergo_uart_stop_rx(&ergo_uart5);
    CLEAR_UART_RX_ERROR_FLAGS(&huart5);
    ergo_uart_start_rx(&ergo_uart5);
  }
#endif

#ifdef UART6_USE_RX
  if (huart->Instance == USART6) {
    ergo_uart_stop_rx(&ergo_uart6);
    CLEAR_UART_RX_ERROR_FLAGS(&huart6);
    ergo_uart_start_rx(&ergo_uart6);
  }
#endif

#ifdef LPUART1_USE_RX
  if (huart->Instance == LPUART1) {
    ergo_uart_stop_rx(&ergo_lpuart1);
    CLEAR_UART_RX_ERROR_FLAGS(&hlpuart1);
    ergo_uart_start_rx(&ergo_lpuart1);
  }
#endif
}

/* ================CALLBACKS END================ */
