//  based on dense implementation
#include <stdio.h>

#include "common.h"
#include "libFFTr/FFTr.h"
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

#define SPEC_SIZE 124 * 256 / 2
int main() {
  PRINT_ARR(sample, 16000, "Sample:");
  double spec[SPEC_SIZE] = {0};
  FFTr(sample, (double(*)[])spec);
  PRINT_ARR(spec, SPEC_SIZE, "Out after FFT");

  float spec_f[SPEC_SIZE];
  ARR_CAST(spec_f, spec, float, SPEC_SIZE);

  // clang-format off
  TENSOR(input1, VEC(
      DIM(.1f, .2f, .3f, .4f, .5f, .6f),
      DIM(.6f, .5f, .4f, .3f, .2f, .1f),
      DIM(.1f, .2f, .3f, .4f, .5f, .6f),
      DIM(.6f, .5f, .4f, .3f, .2f, .1f),
      DIM(.1f, .2f, .3f, .4f, .5f, .6f),
      DIM(.6f, .5f, .4f, .3f, .2f, .1f)
  ), SHAPE(1, 6, 6, 1));
  TENSOR_P(input2, spec_f, (size_t[])SHAPE(1, 124, 124, 1));
  // clang-format on

  struct tensor *out = model(input1);
  if (!out) return -1;

  printf("OUT\n");
  _print_tensor(out);
  print_shape(out);

  free_tensor(out);
  free_tensor(input1);
  free_tensor(input2);

  if (!out) return -1;
  return 0;
}
