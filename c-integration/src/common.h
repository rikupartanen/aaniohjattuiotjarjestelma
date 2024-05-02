#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

// Create shape array
#define SHAPE(...) \
  { __VA_ARGS__, 0 }
// Create vector array
#define VEC(...) \
  { __VA_ARGS__ }
// Noop, used for readability
#define DIM(...) __VA_ARGS__
// Helper for creating tensors
#define TENSOR(name, vec, shape) \
  struct tensor *name;           \
  do {                           \
    float v[] = vec;             \
    size_t s[] = shape;          \
    name = create_tensor(v, s);  \
  } while (0)

struct tensor {
  float *vec;
  size_t *shape;
};

size_t get_shape_len(size_t *shape);
size_t geometric_sum(size_t *vec, size_t len, size_t start, size_t end);
struct tensor *create_tensor(float *vec, size_t *shape);
void free_tensor(struct tensor *tensor);

inline void reshape(struct tensor *tensor, size_t *new_shape) {
  const size_t len = get_shape_len(new_shape);
  free(tensor->shape);
  tensor->shape = malloc(len * sizeof(size_t));
  memmove(tensor->shape, new_shape, len * sizeof(size_t));
}

inline void create_shape(size_t *shape, size_t len, ...) {
  va_list args;
  va_start(args, len);
  
  size_t i;
  for (i = 0; i < len - 1; i++) {
    shape[i] = va_arg(args, size_t);
  }
  shape[i] = 0;

  va_end(args);
} 

void print_shape(struct tensor *out);
void _print_tensor(struct tensor *out);

#endif
