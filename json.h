#ifndef JWC_JSON
#define JWC_JSON

#include <stdint.h>
#include <string.h>

#ifndef JWC_LIST
#define JWC_LIST

#define list_define(TYPENAME, TYPE) \
typedef struct TYPENAME { \
  TYPE *data; \
  size_t size; \
  size_t capacity; \
} TYPENAME;

#define list_create(TYPENAME, IDENTIFIER) TYPENAME IDENTIFIER = {0}
#define list_delete(LIST) do { free((LIST).data); (LIST).data = null; (LIST).size = (LIST).capacity = 0; } while (0)
#define list_clear(LIST) list_delete(LIST)

#define list_get(LIST, INDEX) (LIST).data[INDEX]
#define list_append(LIST, ELEMENT) \
do { \
  if ((LIST).capacity <= (LIST).size) { \
    (LIST).capacity = (size_t) (((LIST).capacity + 1) * 1.5F); \
    (LIST).data = realloc((LIST).data, (LIST).capacity); \
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

typedef struct json_t *json;

list_define(str_list, const char *);
list_define(json_list, json);

json json_create(const char *src);
void json_delete(json obj);

#ifdef JWC_IMPLEMENTATION

struct json_t {
  str_list properties;
  json_list elements;
  const char *value;
};

static const char *json_parse(const char *src, json dst);
static const char *json_parse_object(const char *src, json dst);
static const char *json_parse_array(const char *src, json dst);
static const char *json_parse_value(const char *src, json dst);

json json_create(const char *src) {
  json dst = calloc(1, sizeof(*dst));
  json_parse(src, dst);
  return dst;
}

void json_delete(json obj) {
  printf("unimplemented\n");
  exit(1);
}

const char *json_parse(const char *src, json dst) {
  printf("unimplemented\n");
  exit(1);
}

const char *json_parse_object(const char *src, json dst) {
  printf("unimplemented\n");
  exit(1);
}

const char *json_parse_array(const char *src, json dst) {
  printf("unimplemented\n");
  exit(1);
}

const char *json_parse_value(const char *src, json dst) {
  printf("unimplemented\n");
  exit(1);
}


#endif

#endif