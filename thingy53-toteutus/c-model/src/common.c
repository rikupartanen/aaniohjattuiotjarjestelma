#include "common.h"

// Return the lenght of shape list
inline size_t get_shape_len(size_t *shape) {
  size_t len;
  for (len = 0; shape[len]; len++) {}
  return len;
}

/**
 * @brief Return geometric sum from start to len - end
 *
 * @param vec values to sum
 * @param len lenght of input
 * @param start start index
 * @param end end index as `len - end`
 */
inline size_t geometric_sum(size_t *vec, size_t len, size_t start, size_t end) {
  size_t r_len = 1;
  for (size_t i = start; i < len - end; i++) {
    r_len *= vec[i];
  }
  return r_len;
}

// Create tensor in heap memory
struct tensor *create_tensor(float *vec, size_t *shape) {
  struct tensor *l = malloc(sizeof(struct tensor));
  size_t shape_len = get_shape_len(shape);
  size_t vec_len = geometric_sum(shape, shape_len, 0, 0);

  shape_len++;
  l->_static = false;
  l->vec = malloc(vec_len * sizeof(float));
  l->shape = malloc(shape_len * sizeof(size_t));
  memmove(l->vec, vec, vec_len * sizeof(float));
  memmove(l->shape, shape, shape_len * sizeof(size_t));

  return l;
}

// Create static tensor in heap memory
const struct tensor *create_static_tensor(const float *vec, const size_t *shape) {
  struct tensor *l = malloc(sizeof(struct tensor));
  l->_static = true;
  l->vec = (float *)vec;
  l->shape = (size_t *)shape;

  return l;
}

// Free heap tensor
void free_tensor(const struct tensor *tensor) {
  if (tensor) {
    if (tensor->_static) {
      free((void *)tensor);
    } else {
      free((void *)tensor->vec);
      free((void *)tensor->shape);
      free((void *)tensor);
    }
  }
}

// Set a new shape for the tensor
extern inline void reshape(struct tensor *tensor, size_t *new_shape);

void print_shape(const struct tensor *out) {
  printf("Shape: ");
  for (size_t i = 0; out->shape[i]; i++) {
    if (i % 5 == 0 && i != 0) printf("\n");
    else if (i != 0) printf(" ");
    printf("%zu", out->shape[i]);
  }
  printf("\n");
}

void _print_tensor(const struct tensor *out) {
  size_t len = get_shape_len(out->shape);
  if (len < 1) return;
  size_t x = len > 2 ? out->shape[len - 2] : 1;
  size_t y = out->shape[len - 1];
  len = geometric_sum(out->shape, len, 0, 0);
  for (size_t i = 0; i < len; i++) {
    if (i != 0) {
      if (i % (y * x) == 0 && x != 1) printf("\n\n");
      else if (i % y == 0) printf("\n");
      else printf(" ");
    }
    printf("%f", out->vec[i]);
  }
  printf("\n");
}
