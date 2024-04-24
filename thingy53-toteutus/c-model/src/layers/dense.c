#include "../common.h"

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
  printf("Vec: %zu, Ker: %zu\n", vec_len, kernel->shape[0]);
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