/**
 *
 */

#include <ringbuf32.h>

#ifdef WITH_STDIO_TRACE
#include <stdio.h> /* for trace only */
#endif

#define MIN_VAL(x, y) ((x) < (y) ? (x) : (y))
#define MAX_VAL(x, y) ((x) > (y) ? (x) : (y))
#define ARRAY_LEN(array) (sizeof(array) / sizeof((array)[0]))

static void _ringbuf32_memzero(void *data, uint32_t size) {
  uint8_t *ptr = (uint8_t *)data;
  for (uint32_t i = 0; i < size; i++) {
    *ptr++ = 0;
  }
}

/**
 * \brief           Initialize buffer handle to default values with size and
 * buffer data array \param[in]       b: Buffer handle \param[in]       databuf:
 * Pointer to memory to use as buffer data \param[in]       size: Size of
 * `databuf` in units of bytes Maximum number of bytes buffer can hold is `size
 * - 1` \return          `1` on success, `0` otherwise
 */
uint8_t ringbuf32_init(ringbuf32_t *b, void *databuf, uint32_t size) {
  _ringbuf32_memzero(b, sizeof(*b));
  b->size = size;
  b->data = databuf;
  return 1;
}

/**
 * \brief           Reset readerIndex and writerIndex of the buffer,
   discarding all unread data. peakUnread field is reset as well.
 * \param[in]       b: Buffer handle
 * \return          none
 */
void ringbuf32_reset(ringbuf32_t *b) {
  b->readerIndex = 0;
  b->writerIndex = 0;
  b->peakUnread = 0;
}

/* _free_size operates on snapshots of the readerIndex and writerIndex
to avoid race conditions. */
static uint32_t _free_size(uint32_t size, uint32_t r, uint32_t w) {
  // Because writerIndex and readerIndex are constantly incremented,
  // free size equals buffer size minus unread data.
  return size - (w - r);
}

/**
 * \brief           Return available space for write operation.
 * The maximum available size is always one byte less then the backing buffer
 * capacity due to the implementation limitation that forces readerIndex to
 * never reach writerIndex when there is unread data. Otherwise, it'd be
 * impossible to distinguish between empty and full buffer without introducing
 * an additional variable, which may require more advanced synchronization
 * between threads. \param[in]       b: Buffer handle \return          Number of
 * free bytes in memory
 */
uint32_t ringbuf32_free_size(ringbuf32_t *b) {
  return _free_size(b->size, b->readerIndex, b->writerIndex);
}

/* _unread_size operates on snapshots of the readerIndex and writerIndex
to avoid race conditions. */
static uint32_t _unread_size(uint32_t r, uint32_t w) { return w - r; }

/**
 * \brief           Get unread data size.
 * \param[in]       b: Buffer handle
 * \return          Number of bytes of data that can be read.
 */
uint32_t ringbuf32_unread_size(ringbuf32_t *b) {
  return _unread_size(b->readerIndex, b->writerIndex);
}

/**
 * \brief           Writes a single byte to buffer.
 * If there is not enough free space, no data is copied and 0 is returned.
 *
 * \param[in]       b: Buffer handle
 * \param[in]       data: data byte
 * \return          True if operation successful.
 */
bool ringbuf32_write_one(ringbuf32_t *b, const uint8_t data) {
  uint32_t r = b->readerIndex;
  uint32_t w = b->writerIndex;
  if (_free_size(b->size, r, w) < 1) {
    return 0;
  }
  b->data[w % b->size] = data;
  b->writerIndex++;
  return true;
}

/**
 * \brief           Write data to buffer.
 * Copies data from `data` array to buffer.
 * If there is not enough free space, no data is copied and 0 is returned.
 *
 * \param[in]       b: Buffer handle
 * \param[in]       src: Pointer to data to write into buffer
 * \param[in]       btw: Number of bytes to write
 * \return          Number of bytes written to buffer.
 *                      When returned value is less than `btw`, there was no
 * enough memory available to copy full data array.
 */
uint32_t ringbuf32_write(ringbuf32_t *b, const void *src, uint32_t btw) {
  uint32_t r = b->readerIndex;
  uint32_t w = b->writerIndex;
  uint32_t freeSize = _free_size(b->size, r, w);
  if (freeSize < btw || btw == 0) {
    return 0;
  }
  const uint8_t *dataBytes = (const uint8_t *)src;
  for (uint32_t i = 0; i < btw; i++) {
    b->data[w++ % b->size] = *dataBytes++;
  }
  b->writerIndex = w;
  return btw;
}

/**
 * \brief           Reads a single byte from buffer.
 * If there is not enough free space, no data is copied and 0 is returned.
 *
 * \param[in]       b: Buffer handle
 * \param[in]       data: data byte
 * \return          True if operation successful.
 */
/*
bool ringbuf32_read_one(ringbuf32_t *b, const uint8_t data)
{	uint32_t r = b->readerIndex;
    uint32_t w = b->writerIndex;
    if (_free_size(b->size, r, w) < 1) {
        return 0;
    }
    b->data[w % b->size] = data;
    b->writerIndex++;
    return true;
}
*/

/**
 * \brief           Read data from buffer.
 * Copies data from buffer to `data` array and marks buffer as free for maximum
 * `btr` number of bytes
 *
 * \param[in]       b: Buffer handle
 * \param[out]      dest: Pointer to output memory to copy buffer data to
 * \param[in]       btr: Maximum number of bytes to read
 * \return          Number of bytes read and copied to data array
 */
uint32_t ringbuf32_read(ringbuf32_t *b, void *dest, uint32_t btr) {
  uint32_t r = b->readerIndex;
  uint32_t w = b->writerIndex;
  uint32_t unreadSize = _unread_size(r, w);
  if (unreadSize == 0 || btr == 0) {
    return 0;
  }

  btr = MIN_VAL(btr, unreadSize);
  uint8_t *dataBytes = (uint8_t *)dest;
  for (uint32_t i = 0; i < btr; i++) {
    *dataBytes++ = b->data[r++ % b->size];
  }
  b->readerIndex = r;
  return btr;
}

/**
 * \brief           Transfer data between the ring buffers:
 * read data from src and write into dest.
 * This function tries to transfer all unread data from src,
 * actual number of the transferred bytes is returned.
 * \param[in]       src: Source ring buffer
 * \param[out]      dest: Destination ring buffer
 * \param[in]       max_size: Maximum number of bytes to transfer
 * \return          Number of bytes transferred.
 */
uint32_t ringbuf32_transfer(ringbuf32_t *src, ringbuf32_t *dest,
                            uint32_t max_size) {
  uint32_t rSrc = src->readerIndex;
  uint32_t wSrc = src->writerIndex;
  uint32_t rDest = dest->readerIndex;
  uint32_t wDest = dest->writerIndex;
  uint32_t freeSpace = _free_size(dest->size, rDest, wDest);
  uint32_t btw = _unread_size(rSrc, wSrc);
  btw = MIN_VAL(btw, freeSpace);
  btw = MIN_VAL(btw, max_size);
  if (btw == 0) {
    return 0;
  }
  for (uint32_t i = 0; i < btw; i++) {
    dest->data[wDest++ % dest->size] = src->data[rSrc++ % src->size];
  }
  dest->writerIndex += btw;
  src->readerIndex += btw;
  return btw;
}

/* Optional debug and trace functions */
#ifdef WITH_STDIO_TRACE
void ringbuf32_trace(ringbuf32_t *b) {
  uint32_t free = ringbuf32_free_size(b);
  printf("\n  size: %d, free: %d, writerIndex: %d, readerIndex: %d\n", b->size,
         free, b->writerIndex, b->readerIndex);
  const uint32_t maxTrace = 128;
  const uint32_t size = MIN_VAL(maxTrace, b->size);
  printf("  .data (first %d bytes in hex):\n  ", size);
  for (uint32_t i = 0; i < size; i++) {
    printf("%02x ", b->data[i]);
  }
  printf("\n");
}
#endif
