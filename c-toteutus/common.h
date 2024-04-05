#ifndef CONV1_H
#define CONV1_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

struct tensor *create_tensor(float *vec, size_t *shape);
#endif