#ifndef JWC_JSON
#define JWC_JSON

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

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

#ifndef JWC_LIST
#define JWC_LIST

#define list_define(TYPENAME, TYPE) \
typedef struct TYPENAME { \
  TYPE *data; \
  size_t size; \
  size_t capacity; \
} typename

#endif

typedef struct json_t *json;

list_define(str_list, const char *);
list_define(json_list, json);

json json_create(const char *src);
void json_delete(json);

void json_print(FILE *file, json);

bool json_contains(json, const char *propname);

json json_property(json, const char *propname);
json json_element(json, size_t elementidx);
const char *json_value(json);

str_list json_properties(json);
json_list json_elements(json);
size_t json_size(json);


#ifdef JWC_IMPLEMENTATION

#ifndef JWC_LIST
#define JWC_LIST

#define list_delete(LIST) do { free((LIST).data); (LIST).data = null; (LIST).size = (LIST).capacity = 0; } while (0)

#define list_append(LIST, ELEMENT) \
do { \
  if ((LIST).capacity <= (LIST).size) { \
    (LIST).capacity = (size_t) (((LIST).capacity + 1) * 1.5F); \
    (LIST).data = realloc((LIST).data, (LIST).capacity * sizeof(*(LIST).data)); \
  } \
  \
  (LIST).data[(LIST).size++] = (ELEMENT); \
} while (0)

#endif

struct json_t {
  str_list properties;
  json_list elements;
  const char *value;
};

static size_t json_indexof(json json, const char *propname) { for (size_t i = 0; i < json->properties.size; i++) if (strcmp(propname, json->properties.data[i]) == 0) return i; return SIZE_MAX; }
bool json_contains(json json, const char *propname) { return json_indexof(json, propname) != SIZE_MAX; }
json json_property(json json, const char *propname) { return json->elements.data[json_indexof(json, propname)]; }
json json_element(json json, size_t elementidx) { return json->elements.data[elementidx]; }
const char *json_value(json json) { return json->value; }

str_list json_properties(json json) { return json->properties; }
json_list json_elements(json json) { return json->elements; }
size_t json_size(json json) { return json->elements.size; }

static const char *json_parse(const char *src, json dst);
static void json_write(FILE *file, json obj, size_t indent);

json json_create(const char *src) {
  json dst = calloc(1, sizeof(*dst));
  json_parse(src, dst);
  return dst;
}

void json_delete(json obj) {
  if (obj->value)
    free((void *) obj->value);

  for (size_t i = 0; i < obj->properties.size; i++)
    free((void *) obj->properties.data[i]);
  list_delete(obj->properties);

  for (size_t i = 0; i < obj->elements.size; i++)
    json_delete(obj->elements.data[i]);
  list_delete(obj->properties);

  free((void *) obj);
}

void json_print(FILE *file, json obj) {
  if (obj->properties.size)
    fprintf(file, "{\n");
  else if (obj->elements.size)
    fprintf(file, "[\n");

  json_write(file, obj, 1);

  if (obj->properties.size)
    fprintf(file, "}\n");
  else if (obj->elements.size)
    fprintf(file, "]\n");
}

void json_write(FILE *file, json obj, size_t indent) {
#define __INDENT__(extra) for (size_t __ = 0; __ < indent; __++) fprintf(file, "  ");

  if (obj->properties.size) {
    for (size_t i = 0; i < obj->properties.size; i++) {
      __INDENT__(1);
      fprintf(file, "%s: ", obj->properties.data[i]);

      json value = obj->elements.data[i];

      if (value->properties.size) {
        fprintf(file, "{\n");
        json_write(file, value, indent + 1);
        __INDENT__(1);
        fprintf(file, "}");
      } else if (value->elements.size) {
        fprintf(file, "[\n");
        json_write(file, value, indent + 1);
        __INDENT__(1);
        fprintf(file, "]");
      } else if (value->value) {
        fprintf(file, "%s", value->value);
      }

      if (i != obj->properties.size - 1)
        fprintf(file, ",");
      fprintf(file, "\n");
    }

    return;
  } else if (obj->elements.size) {
    for (size_t i = 0; i < obj->elements.size; i++) {
      json value = obj->elements.data[i];

      if (value->properties.size) {
        __INDENT__(1);
        fprintf(file, "{\n");
        json_write(file, value, indent + 1);
        __INDENT__(1);
        fprintf(file, "}");
      } else if (value->elements.size) {
        __INDENT__(1);
        fprintf(file, "[\n");
        json_write(file, value, indent + 1);
        __INDENT__(1);
        fprintf(file, "]");
      } else if (value->value) {
        __INDENT__(1);
        fprintf(file, "%s", value->value);
      }

      if (i != obj->elements.size - 1)
        fprintf(file, ",");
      fprintf(file, "\n");
    }
    return;
  } else if (obj->value) {
    __INDENT__(0);
    fprintf(file, obj->value);
    __INDENT__(0);
    return;
  }

#undef __INDENT__
}

static const char *json_parse_whitespace(const char *src) { while (isspace(*src)) src++; return src; }
static const char *json_parse_object(const char *src, json dst);
static const char *json_parse_array(const char *src, json dst);
static const char *json_parse_value(const char *src, json dst);

const char *json_parse(const char *src, json dst) {
  src = json_parse_whitespace(src);

  switch (*src) {
  case '{': return json_parse_object(src, dst);
  case '[': return json_parse_array(src, dst);
  default: return json_parse_value(src, dst);
  }
}

static const char *json_expect(const char *src, char c, const char *verb) {
  src = json_parse_whitespace(src);
  require(*(src++) == c, "json@jwc", "error while %s, expected '%c' but found '%c'", verb, c, *(--src));
  return json_parse_whitespace(src);
}

const char *json_parse_object(const char *src, json dst) {
  src = json_expect(src, '{', "parsing object");

  while (*src != '}') {
    src = json_expect(src, '"', "parsing object property name");

    const char *start = src;
    while (*src != '"') {
      if (*src == '\\')
        src++;
      src++;
    }

    char *property = calloc((src - start) + 1, sizeof(char));
    memcpy(property, start, src - start);

    src = json_expect(src, '"', "parsing object property name");
    src = json_expect(src, ':', "parsing object property");

    json element = calloc(1, sizeof(*element));
    src = json_parse(src, element);

    list_append(dst->properties, property);
    list_append(dst->elements, element);

    if (*src == ',')
      src = json_expect(src, ',', "parsing object properties (this is likely a bug in the parser -- please report!)");
  }

  return json_expect(src, '}', "parsing object");
}

const char *json_parse_array(const char *src, json dst) {
  src = json_expect(src, '[', "parsing array");

  while (*src != ']') {
    json element = calloc(1, sizeof(*element));
    src = json_parse(src, element);

    list_append(dst->elements, element);

    if (*src == ',')
      src = json_expect(src, ',', "parsing array elements (this is likely a bug in the parser -- please report!)");
  }

  return json_expect(src, ']', "parsing array");
}

const char *json_parse_value(const char *src, json dst) {
  src = json_parse_whitespace(src);

  if (*src == '"') {
    src = json_expect(src, '"', "parsing string literal");

    const char *start = src;

    while (*src != '"') {
      if (*src == '\\')
        src++;
      src++;
    }

    char *value = calloc((src - start) + 1, sizeof(char));
    memcpy(value, start, src - start);

    src = json_expect(src, '"', "parsing string literal");

    dst->value = value;
  } else {
    const char *start = src;

    while (!isspace(*src) && *src != '}' && *src != ']' && *src != ',') {
      src++;
    }

    char *value = calloc((src - start) + 1, sizeof(char));
    memcpy(value, start, src - start);

    dst->value = value;
  }

  return json_parse_whitespace(src);
}

#endif

#endif