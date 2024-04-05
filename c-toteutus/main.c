//  based on dense implementation
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "conv1.h"
#include "common.h"

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

// Return the lenght of shape list
static inline size_t get_shape_len(size_t *shape) {
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
static inline size_t geometric_sum(size_t *vec, size_t len, size_t start, size_t end) {
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
  l->vec = malloc(vec_len * sizeof(float));
  l->shape = malloc(shape_len * sizeof(size_t));
  memmove(l->vec, vec, vec_len * sizeof(float));
  memmove(l->shape, shape, shape_len * sizeof(size_t));

  return l;
}

// Free heap tensor
void free_tensor(struct tensor *tensor) {
  if (tensor) {
    free(tensor->vec);
    free(tensor->shape);
    free(tensor);
  }
}

/**
 * @brief Calculate matrix multiplication between vector and kernel.
 * @note Vector lenght and kernel's second dimension lenght have to match.
 *
 * @param vec input
 * @param vec_len lenght of input
 * @param kernel two dimensional matrix
 * @param out output array with same lenght as kernel's second dimension
 * @return -1 on error, otherwise 0
 */
int mat_mul(float *vec, size_t vec_len, const struct tensor *kernel, float *out) {
  size_t len = get_shape_len(kernel->shape);
  if (vec_len != kernel->shape[0])
    return -1;

  // In-place transpose kernel
  for (size_t j = 0; j < kernel->shape[len - 1]; j++) {
    float sum = 0.0f;
    for (size_t i = 0; i < kernel->shape[0]; i++) {
      sum += vec[i] * kernel->vec[j + i * kernel->shape[len - 1]];
    }
    out[j] = sum;
  }

  return 0;
}

// Calculate dense layer
struct tensor *dense(
    const struct tensor *input,
    const struct tensor *kernel,
    const struct tensor *bias
) {
  size_t ker_shape_len = get_shape_len(kernel->shape);
  size_t in_shape_len = get_shape_len(input->shape);

  size_t batches = input->shape[0];
  size_t mid = geometric_sum(input->shape, in_shape_len, 1, 1);
  size_t vec_len = input->shape[in_shape_len - 1];  // Convert len to 0 index

  size_t size = batches * mid * kernel->shape[ker_shape_len - 1];
  float ret[size];

  for (size_t b = 0; b < batches; b++) {
    for (size_t m = 0; m < mid; m++) {
      size_t ret_pos = b * (mid * kernel->shape[ker_shape_len - 1])
                     + m * kernel->shape[ker_shape_len - 1];
      float *p = &ret[ret_pos];

      size_t in_pos = b * mid * vec_len + m * vec_len;

      // Matrix multiply input vector with kernel
      int err = mat_mul(
          &input->vec[in_pos],  // Start position in input
          vec_len,              // Vectors lenght
          kernel,               //
          p                     // Start position in return array
      );

      if (bias) {
        for (size_t i = 0; i < kernel->shape[ker_shape_len - 1]; i++) {
          p[i] += bias->vec[i];
        }
      }
      if (err) {
        printf("ERROR in dense\n");
        return NULL;
      }
    }
  }

  size_t out_shape[in_shape_len + 1];
  for (size_t i = 0; i < in_shape_len - 1; i++)
    out_shape[i] = input->shape[i];
  out_shape[in_shape_len - 1] = kernel->shape[ker_shape_len - 1];
  out_shape[in_shape_len] = 0;

#if false
  for (size_t i = 0; i < in_shape_len + 1; i++) {
    if (i) printf(", ");
    printf("%lu", out_shape[i]);
  }
  printf("\n");
#endif

  return create_tensor(ret, out_shape);
}

int main() {

    struct tensor *bias1;
    {
        float bias_vec[] = VEC(DIM(10000.f, 20000.f));
        size_t bias_shape[] = SHAPE(1, 1, 2);
        bias1 = create_tensor(bias_vec, bias_shape);
    }

    struct tensor *input1;
    {
        float tensor_vec[] = VEC(   DIM(1.f, 1.f, 1.f, 1.f, 1.f, 1.f),
                                    DIM(1.f, 1.f, 1.f, 1.f, 1.f, 1.f),
                                    DIM(1.f, 1.f, 1.f, 1.f, 1.f, 1.f),
                                    DIM(1.f, 1.f, 1.f, 1.f, 1.f, 1.f),
                                    DIM(1.f, 1.f, 1.f, 1.f, 1.f, 1.f),
                                    DIM(1.f, 1.f, 1.f, 1.f, 1.f, 1.f));
        size_t tensor_shape[] = SHAPE(1, 6, 6);
        input1 = create_tensor(tensor_vec, tensor_shape);
    }

    struct tensor *input2;
    {
        float tensor_vec[] = VEC(   DIM(1.f, 2.f, 3.f, 4.f, 5.f, 6.f),
                                    DIM(11.f, 12.f, 13.f, 14.f, 15.f, 16.f),
                                    DIM(21.f, 22.f, 23.f, 24.f, 25.f, 26.f),
                                    DIM(31.f, 32.f, 33.f, 34.f, 35.f, 36.f),
                                    DIM(41.f, 42.f, 43.f, 44.f, 45.f, 46.f),
                                    DIM(51.f, 52.f, 53.f, 54.f, 55.f, 56.f));
        size_t tensor_shape[] = SHAPE(1, 6, 6);
        input2 = create_tensor(tensor_vec, tensor_shape);
    }

    struct tensor *kernel1;
    {
        float ker_vec[] = VEC(DIM(1.f, 1.f, 1.f), 
                              DIM(1.f, 1.f, 1.f), 
                              DIM(1.f, 1.f, 1.f),
                              DIM(2.f, 2.f, 2.f),
                              DIM(2.f, 2.f, 2.f), 
                              DIM(2.f, 2.f, 2.f));
        size_t ker_shape[] = SHAPE(2, 3, 3);
        kernel1 = create_tensor(ker_vec, ker_shape);
    }

    struct tensor *kernel2;
    {
        float ker_vec[] = VEC(DIM(1.f, 2.f, 3.f), 
                              DIM(4.f, 5.f, 6.f), 
                              DIM(7.f, 8.f, 9.f),
                              DIM(11.f, 12.f, 13.f),
                              DIM(14.f, 15.f, 16.f), 
                              DIM(17.f, 18.f, 19.f));
        size_t ker_shape[] = SHAPE(2, 3, 3);
        kernel2 = create_tensor(ker_vec, ker_shape);
    }

    struct tensor *out = conv2d1(input2, kernel1, bias1);
//    struct tensor *out = dense(input1, kernel1, NULL);

   printf("INPUT\n");
    print_tensor(input2);

    printf("KERNEL\n");
    print_tensor(kernel1);

    printf("BIAS\n");
    print_tensor(bias1);

    printf("OUT\n");
    print_tensor(out);


  if (out != NULL) {
    size_t len = get_shape_len(out->shape);
    size_t data_len = 1;
    for (size_t i = 0; i < len; i++) {
      data_len *= out->shape[i];
    }
    for (size_t i = 0; i < data_len; i++) {
      printf("%f\n", out->vec[i]);
    }
  }

    free_tensor(input1);
    free_tensor(input2);
    free_tensor(bias1);
    free_tensor(kernel1);
    free_tensor(kernel2);
    free_tensor(out);


    if (!out) return -1;
    return 0;
}
