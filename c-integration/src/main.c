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

  struct tensor *out;
  out = model(input);
  if (!out) return -1;

  printf("OUT\n");
  _print_tensor(out);
  print_shape(out);

  free_tensor(out);
  return 0;
}
