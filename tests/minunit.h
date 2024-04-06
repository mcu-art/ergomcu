/*
 * File:   minunit.h
 * Author: Zed. A. Shaw, Sam
 *
 * @see http://c.learncodethehardway.org/book/ex30.html
 *
 * Modified by iotanbo.
 */

#ifndef MINUNIT_H
#define MINUNIT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>

#define log_err(message) printf("\tError: %s\n", message)

#define mu_suite_start() char *message = NULL

#define mu_assert(message, test) \
  do {                           \
    if (!(test)) {               \
      log_err(message);          \
      return message;            \
    }                            \
  } while (0)

#define mu_run_test(test)           \
  do {                              \
    printf("\n-----%s", " " #test); \
    char *message = test();         \
    tests_run++;                    \
    if (message) {                  \
      return message;               \
    }                               \
  } while (0)

int tests_run;

#ifdef __cplusplus
}
#endif

#endif /* MINUNIT_H */
