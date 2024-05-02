#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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
  bool _static;
  float *vec;
  size_t *shape;
};

size_t get_shape_len(size_t *shape);
size_t geometric_sum(size_t *vec, size_t len, size_t start, size_t end);
struct tensor *create_tensor(float *vec, size_t *shape);
const struct tensor *create_static_tensor(const float *vec, const size_t *shape);
void free_tensor(const struct tensor *tensor);

inline void reshape(struct tensor *tensor, size_t *new_shape) {
  const size_t len = get_shape_len(new_shape);
  free(tensor->shape);
  tensor->shape = malloc((len + 1) * sizeof(size_t));
  memmove(tensor->shape, new_shape, (len + 1) * sizeof(size_t));
}

void print_shape(const struct tensor *out);
void _print_tensor(const struct tensor *out);

#endif
