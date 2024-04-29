#include "conv2d.h"

#include <stdio.h>
#include <stdlib.h>

#include "../common.h"

struct tensor *conv2d1_fixed(
    const struct tensor *input,
    const struct tensor *kernel,
    const struct tensor *bias,
    struct tensor *out
) {
  size_t kernel_shape_len = get_shape_len(kernel->shape);
  if (kernel_shape_len != 4) return NULL;

  size_t len_iy = input->shape[0];  // in height
  size_t len_ix = input->shape[1];  // in width
  size_t len_in = input->shape[2];  // in channels

  size_t len_ky = kernel->shape[0];  // number of kernel rows or dimension y
  size_t len_kx = kernel->shape[1];  // number of kernel columns or dimension x
  size_t len_kf = kernel->shape[2];  // number of kernel filters
  size_t len_kn = kernel->shape[3];  // nunber of kernels

  if (bias) {
    size_t len_bias = bias->shape[0];
    if (len_bias != len_kn) {
      printf("  Bias and kernel count mismatch, bias %zu, kernel %zu\n", len_bias, len_kn);
    }
  }

  size_t len_oy = (len_iy - len_ky + 1);  // Output y length
  size_t len_ox = (len_ix - len_kx + 1);  // Output x length

  size_t size = ((len_oy) * (len_ox)*len_kn);
  out->vec = malloc(size * sizeof(float));
  float *ret = out->vec;

  size_t position = 0;
  for (size_t kn = 0; kn < len_kn; kn++) {
    for (size_t iy = 0; iy < len_oy; iy++) {
      for (size_t ix = 0; ix < len_ox; ix++) {
        float sum = 0;
        for (size_t ky = 0; ky < len_ky; ky++) {
          for (size_t kx = 0; kx < len_kx; kx++) {
            for (size_t k = 0; k < len_kf; k++) {
              size_t i = (iy + ky) * len_ix * len_in;
              size_t j = (ix + kx) * len_in;
              float in_pos_content = input->vec[i + j + k];

              size_t ker_pos = (ky * len_kx * len_kf * len_kn)
                             + (kx * len_kf * len_kn)
                             + (k * len_kn) + kn;
              float k_pos_content = kernel->vec[ker_pos];

              sum += in_pos_content * k_pos_content;
            }
          }
        }
        position = (iy * len_ox * len_kn) + (ix * len_kn) + kn;
        if (bias) {  // add bias if specified
          sum += bias->vec[kn];
        }
        if (sum < 0) sum = 0.0;  //  RELU, limit would-be less than 0 outputs to 0
        ret[position] = sum;
      }
    }
  }

  out->shape = malloc(4 * sizeof(size_t));
  create_shape(out->shape, 4, len_oy, len_ox, len_kn);
  return out;
}
