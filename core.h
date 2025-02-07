#ifndef JWC_CORE
#define JWC_CORE

#include <stdio.h>
#include <stdlib.h>

#ifndef bool
#define bool int
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#define require(CONDITION, DOMAIN, ...) do { \
  if (!(CONDITION)) { \
    fprintf(stderr, "[%s] ", DOMAIN); \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n"); \
    exit(1); \
  } \
} while (0)

#endif
