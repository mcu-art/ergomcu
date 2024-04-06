#include "ringbuf32.h"

#include <stdio.h>

#include "minunit.h"

#define MIN_VAL(x, y) ((x) < (y) ? (x) : (y))

// Returns 1 if success or zero otherwise
uint8_t initbuf(ringbuf32_t *rb, void *databuf, uint32_t size) {
  return ringbuf32_init(rb, databuf, size);
}

static void tracebuf(ringbuf32_t *b, int line, const char *msg) {
  if (msg != NULL) {
    if (line > 0) {
      printf("\n(tracing %s at line %d):", msg, line);
    } else {
      printf("\n(tracing %s):", msg);
    }
  } else {
    if (line > 0) {
      printf("\n(tracing unnamed buffer at line %d):", line);
    } else {
      printf("\n(tracing unnamed buffer):");
    }
  }
#ifdef WITH_STDIO_TRACE
  ringbuf32_trace(b);
#endif
}

/* ==============TESTS BEGIN============== */

static char *TestCreation() {
  uint8_t bdata[4];
  ringbuf32_t b;
  mu_assert("ring buffer must be properly initialized",
            initbuf(&b, bdata, 4) == 1);
  mu_assert("data buffer must be properly assigned", b.data == bdata);
  mu_assert("readerIndex and writerIndex must be zero",
            b.readerIndex == 0 && b.writerIndex == 0);
  mu_assert("size field be properly initialized", b.size == 4);
  mu_assert("newly created RB must have free size equal to its size",
            4 == ringbuf32_free_size(&b));
  mu_assert("newly created RB must have zero unread data",
            0 == ringbuf32_unread_size(&b));
  return 0;
}

static char *TestSizeCalculation() {
  uint8_t bdata[4];
  ringbuf32_t b;
  initbuf(&b, bdata, 4);

  mu_assert("when readIndex=0 and writeIndex=0, free size must be 4",
            4 == ringbuf32_free_size(&b));
  mu_assert("when readIndex=0 and writeIndex=0, unread data size must be 0",
            0 == ringbuf32_unread_size(&b));
  b.writerIndex = 1;
  mu_assert("when readIndex=0 and writeIndex=1, free size must be 3",
            3 == ringbuf32_free_size(&b));
  mu_assert("when readIndex=0 and writeIndex=1, unread data size must be 1",
            1 == ringbuf32_unread_size(&b));

  b.readerIndex = 1;
  mu_assert("when readIndex=1 and writeIndex=1, free size must be 4",
            4 == ringbuf32_free_size(&b));
  mu_assert("when readIndex=1 and writeIndex=1, unread data size must be 0",
            0 == ringbuf32_unread_size(&b));

  return 0;
}

static char *TestReadWrite() {
  uint8_t bdata[4];
  ringbuf32_t b;
  initbuf(&b, bdata, 4);
  const uint8_t src1[3] = {1, 2, 3};
  mu_assert("one byte must be successfully written",
            1 == ringbuf32_write(&b, src1, 1));

  uint32_t freeSize = ringbuf32_free_size(&b);
  mu_assert("after 1 byte written, free space must be 3", 3 == freeSize);

  mu_assert("after 1 byte written, unread data must be 1",
            1 == ringbuf32_unread_size(&b));

  mu_assert(
      "after attempt to write more bytes than free space, operation must fail",
      0 == ringbuf32_write(&b, src1, 4));

  mu_assert("2 more bytes must be successfully written",
            2 == ringbuf32_write(&b, src1 + 1, 2));

  mu_assert("after 3 bytes written, free space must be 1",
            1 == ringbuf32_free_size(&b));

  mu_assert("after 3 bytes written, unread data must be 3",
            3 == ringbuf32_unread_size(&b));

  uint8_t readBuf[4] = {0};
  mu_assert("1 byte read must be OK", 1 == ringbuf32_read(&b, readBuf, 1));

  mu_assert("after 1 byte read, free space must be 2",
            2 == ringbuf32_free_size(&b));

  mu_assert("at the same time unread data must be 2",
            2 == ringbuf32_unread_size(&b));

  mu_assert("readBuf[0] must contain 1 (the first read byte)", 1 == readBuf[0]);

  mu_assert(
      "specifying more bytes to read than are available must read all "
      "available data",
      2 == ringbuf32_read(&b, readBuf, 4));

  mu_assert("must contain: readBuf[0]==2 and readBuf[1]==3",
            (readBuf[0] == 2) && (readBuf[1] == 3));

  /*
    mu_assert("after reading all data, free space must be 3",
              3 == ringbuf32_free_size(&b));

    mu_assert("3 new bytes must be successfully written",
              3 == ringbuf32_write(&b, src1, 3));

    mu_assert("3 bytes must be successfully read",
              3 == ringbuf32_read(&b, readBuf, 4));

    mu_assert("readBuf must contain: {1, 2, 3, ...}",
              (readBuf[0] == 1) && (readBuf[1] == 2) && (readBuf[2] == 3));
  */

  /* Testing basic usage */

  // Write and read data many times
  uint8_t src[3] = {1, 2, 3};
  uint8_t dest[3] = {0};

  ringbuf32_reset(&b);

  mu_assert("dest must contain: {0, 0, 0}",
            (dest[0] == 0) && (dest[1] == 0) && (dest[2] == 0));

  uint32_t bytesWritten = 0;
  for (uint32_t i = 0; i < 100; i++) {
    ringbuf32_write(&b, src, 3);
    dest[0] = 0;
    dest[1] = 0;
    dest[2] = 0;
    uint32_t bytesRead = ringbuf32_read(&b, dest, 3);
    mu_assert("3 bytes read OK", bytesRead == 3);
    mu_assert("dest must contain: {1, 2, 3}",
              (dest[0] == 1) && (dest[1] == 2) && (dest[2] == 3));
    bytesWritten += 3;
  }

  mu_assert("writerIndex=bytesWritten", b.writerIndex == bytesWritten);

  mu_assert("readerIndex=bytesWritten", b.readerIndex == bytesWritten);

  /* Testing corner cases */

  return 0;
}

/* ==============TESTS END============== */

static char *all_tests() {
  mu_run_test(TestCreation);
  mu_run_test(TestSizeCalculation);
  mu_run_test(TestReadWrite);
  return 0;
}

int main(void) {
  char *result = all_tests();
  if (result != 0) {
    printf("%s\n", result);
  } else {
    printf("\nALL TESTS PASSED\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
