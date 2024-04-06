#ifndef RINGBUF32_H
#define RINGBUF32_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * ringbuf32_t is a light-weight ring buffer designed to work
 * with 32-bit micro controllers, e.g. to transfer data
 * between ISRs and tasks. Its design guarantees that the read
 * and write functions are safe to be run concurrently from
 * different threads given that 32-bit read and write operations are atomic
 * (as they are in STM32 MCU family), and there is only one reader and one
 * writer. Under these conditions, it is lock-free and no further
 * synchronization is needed.
 */
typedef struct ringbuf32_s {
  volatile uint8_t *data;
  uint32_t size;
  volatile uint32_t readerIndex;
  volatile uint32_t writerIndex;
  volatile uint32_t peakUnread; /* stores peak unread data size for statistics
                                   and debug purposes */

} ringbuf32_t;

uint8_t ringbuf32_init(ringbuf32_t *b, void *databuf, uint32_t size);
void ringbuf32_reset(ringbuf32_t *b);
uint32_t ringbuf32_free_size(ringbuf32_t *b);
uint32_t ringbuf32_unread_size(ringbuf32_t *b);
uint32_t ringbuf32_write(ringbuf32_t *b, const void *src, uint32_t btw);
uint32_t ringbuf32_read(ringbuf32_t *b, void *dest, uint32_t btr);
uint32_t ringbuf32_transfer(ringbuf32_t *src, ringbuf32_t *dest,
                            uint32_t max_size);

/* Optional debug and trace functions */
#ifdef WITH_STDIO_TRACE
void ringbuf32_trace(ringbuf32_t *b);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RINGBUF32_H */
