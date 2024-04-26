//  based on dense implementation
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "common.h"
#include "layers/conv1.h"
#include "layers/conv2d.h"
#include "libFFTi/FFTi.h"
#include "model.h"
#include "sample.h"

#define PRINT_ARR(arr, size, header)             \
  puts(header);                                  \
  for (size_t i = 0; i < size; i++) {            \
    if (i % 5 == 0 && i != 0) printf("\n");      \
    else if (i != 0) printf(" ");                \
    char str[8];                                 \
    snprintf(str, 8, "% .4f", (double)(arr)[i]); \
    printf("%s", str);                           \
  }                                              \
  printf("\n")

#define TENSOR_P(name, vec, shape) struct tensor *name = create_tensor(vec, shape);
#define ARR_CAST(dst, src, to, len)  \
  for (size_t i = 0; i < len; i++) { \
    (dst)[i] = (to)(src)[i];         \
  }

#define SPEC_SIZE 124 * (256 / 2 + 1)
int main() {
  // PRINT_ARR(sample, 16000, "Sample:");
  size_t spec_shape[] = SHAPE(124, 129);
  float spec[SPEC_SIZE] = {0};
  FFTi(sample, (float(*)[])spec);
  // PRINT_ARR(spec, SPEC_SIZE, "Out after FFT");

  // float spec_f[SPEC_SIZE];
  // ARR_CAST(spec_f, spec, float, SPEC_SIZE);
  struct tensor *input = create_tensor(spec, spec_shape);

  // clang-format off
  // TENSOR(input1, VEC(
  //     DIM(.1f, .2f, .3f, .4f, .5f, .6f),
  //     DIM(.6f, .5f, .4f, .3f, .2f, .1f),
  //     DIM(.1f, .2f, .3f, .4f, .5f, .6f),
  //     DIM(.6f, .5f, .4f, .3f, .2f, .1f),
  //     DIM(.1f, .2f, .3f, .4f, .5f, .6f),
  //     DIM(.6f, .5f, .4f, .3f, .2f, .1f)
  // ), SHAPE(6, 6, 1));
  // TENSOR_P(input2, spec_f, (size_t[])SHAPE(1, 124, 124, 1));
  // TENSOR(ker, VEC(
  //   DIM(
  //     DIM(0.1, 0.2),
  //     DIM(0.3, 0.4)
  //   ),
  //   DIM(
  //     DIM(0.4, 0.3),
  //     DIM(0.2, 0.1)
  //   )
  // ), SHAPE(2, 2, 1, 2));
  // TENSOR(ker, VEC(
  //   DIM(
  //     DIM(0.1, 0.2),
  //     DIM(0.3, 0.4)
  //   ),
  // ), SHAPE(2, 2, 1, 1));
  // clang-format on

  struct tensor *out;
  // out = conv2d1_fixed(input1, ker, NULL);
  out = model(input);
  if (!out) return -1;

  printf("OUT\n");
  _print_tensor(out);
  print_shape(out);

  // free_tensor(out);
  return 0;
}
