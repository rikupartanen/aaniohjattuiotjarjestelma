//  based on dense implementation
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "common.h"
#include "layers/conv1.h"
#include "layers/dense.h"
#include "layers/toinenkonvoluutio.h"
#include "FFTi.h"
#include "model.h"
#include "sample.h"
#include "qspi/qspi.h"

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

#define SPEC_SIZE 124 * 129
int main() {
  printf("Hello World\n");
  qspi_init();
  // PRINT_ARR(sample, 16000, "Sample:");
  float *spec = malloc(16000 * sizeof(float));
  FFTi(sample, (float(*)[])spec);

  // float *spec_f = malloc(16000 * sizeof(float));
  // ARR_CAST(spec_f, spec, float, SPEC_SIZE);
  // free(spec);

  // clang-format off
  // TENSOR(input, VEC(
  //     DIM(.1f, .2f, .3f, .4f, .5f, .6f),
  //     DIM(.6f, .5f, .4f, .3f, .2f, .1f),
  //     DIM(.1f, .2f, .3f, .4f, .5f, .6f),
  //     DIM(.6f, .5f, .4f, .3f, .2f, .1f),
  //     DIM(.1f, .2f, .3f, .4f, .5f, .6f),
  //     DIM(.6f, .5f, .4f, .3f, .2f, .1f)
  // ), SHAPE(1, 6, 6, 1));
  struct tensor *input;
  {
    input = malloc(sizeof(struct tensor));
    input->vec = spec;
    input->shape = malloc(3 * sizeof(size_t));

    input->shape[0] = 129;
    input->shape[1] = 124;
    input->shape[2] = 0;
  }
  // clang-format on

  struct tensor *out = model(input);
  if (!out) return -1;
  
  printf("OUT\n");
  _print_tensor(out);
  print_shape(out);

  free_tensor(out);

  if (!out) return -1;
  return 0;
}
