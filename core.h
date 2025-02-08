#ifndef JWC_CORE
#define JWC_CORE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifndef JWC_INT
#define JWC_INT

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

#endif

#ifndef JWC_FLOAT
#define JWC_FLOAT

typedef float f32;
typedef double f64;

#endif

#ifndef JWC_NULL
#define JWC_NULL

#ifndef null
#define null NULL
#endif

#endif

#ifndef JWC_BOOL
#define JWC_BOOL

#ifndef bool
#define bool int
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#endif

#ifndef JWC_REQUIRE
#define JWC_REQUIRE

#define require(CONDITION, DOMAIN, ...) do { \
  if (!(CONDITION)) { \
    fprintf(stderr, "[%s] ", DOMAIN); \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n"); \
    exit(1); \
  } \
} while (0)

#endif

#ifndef JWC_POINTERS
#define JWC_POINTERS

/*
  Allocates (size - n) bytes and copies the data:
    (ptr) ------> (ptr + pos)
                             [ n ]
                                  (ptr + pos + n) ------> (ptr + size)
*/
void *ptrslice(const void *ptr, size_t size, size_t pos, size_t n) {
  char *result = malloc(size - n);
  memcpy(result, (char *) ptr, pos);
  memcpy(result + pos, (char *) ptr + pos + n, size - pos - n);
  return result;
}

#endif

#ifndef JWC_VIEW
#define JWC_VIEW

typedef struct view {
  const void *data;
  size_t size;
} view;

#endif

#ifndef JWC_LIST
#define JWC_LIST

#define list_define(TYPENAME, TYPE) \
typedef struct TYPENAME { \
  TYPE *data; \
  size_t size; \
  size_t capacity; \
} TYPENAME

#define list_create(TYPENAME, IDENTIFIER) TYPENAME IDENTIFIER = {0}
#define list_delete(LIST) do { free((LIST).data); (LIST).data = null; (LIST).size = (LIST).capacity = 0; } while (0)
#define list_clear(LIST) list_delete(LIST)

#define list_get(LIST, INDEX) (LIST).data[INDEX]
#define list_append(LIST, ELEMENT) \
do { \
  if ((LIST).capacity <= (LIST).size) { \
    (LIST).capacity = (size_t) (((LIST).capacity + 1) * 1.5F); \
    (LIST).data = realloc((LIST).data, (LIST).capacity * sizeof(*(LIST).data)); \
  } \
  \
  (LIST).data[(LIST).size++] = (ELEMENT); \
} while (0)
#define list_erase(LIST, INDEX) \
do { \
  void *_FREE_ = (LIST).data; \
  (LIST).data = ptrslice((LIST).data, (LIST).size, INDEX * sizeof(*(LIST).data), sizeof(*(LIST).data)); \
  free(_FREE_); \
} while (0)

#endif

#endif
