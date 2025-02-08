#ifndef JWC_FILE
#define JWC_FILE

#include <stdio.h>
#include <stdlib.h>

static inline char *file_read(const char *path, const char *mode) {
  FILE *file = fopen(path, mode);

  if (file == NULL)
    return NULL;

  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *content = (char *) malloc((size + 1) * sizeof(char));
  fread(content, sizeof(char), size, file);

  content[size] = '\0';
  fclose(file);

  return content;
}

#endif