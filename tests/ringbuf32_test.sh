#!/bin/bash
gcc -DWITH_STDIO_TRACE -o test-runner ringbuf32.c ringbuf32_test.c
./test-runner
