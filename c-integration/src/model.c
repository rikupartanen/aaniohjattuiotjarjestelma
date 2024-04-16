#include "common.h"
#include "layers/conv1.h"
#include "layers/dense.h"
#include "layers/toinenkonvoluutio.h"

struct tensor *model(struct tensor *input) {
  // clang-format off
  TENSOR(conv2d_ker, VEC(
      DIM(1.f, 1.f, 1.f),
      DIM(2.f, 0.f, 2.f),
      DIM(3.f, 3.f, 3.f),
  ), SHAPE(1, 3, 3));
  TENSOR(conv2d_bias, VEC(DIM(0.f)), SHAPE(1, 1, 1));
  // clang-format on

  struct tensor *out = conv2d1(input, conv2d_ker, conv2d_bias);
  printf("OUT\n");
  print_tensor(out);
  print_shape(out);

  float ret[OUT_IMAGES][OUT_SIZE_Y][OUT_SIZE_X];
  toinenkonvoluutio((float(*)[4][4])out->vec, ret);
  free_tensor(out);

  out = create_tensor(ret, (size_t[])SHAPE(1, 2, 2));
  _print_tensor(out);
  print_shape(out);

  float vec[2 * 2 * 4] = VEC(
      DIM(1, 1, 2, 2),
      DIM(1, 1, .5, .5),
      DIM(1, 1, 2, 2),
      DIM(1, 1, .5, .5)
  );
  size_t shape[] = SHAPE(2 * 2, 4);
  struct tensor *dense_kernl = create_tensor(vec, shape);
  _print_tensor(dense_kernl);

  print_shape(out);
  size_t new_shape[] = SHAPE(1, 2 * 2);
  reshape(out, new_shape);
  print_shape(out);
  print_shape(dense_kernl);

  struct tensor *tmp = dense(out, dense_kernl, NULL);
  free_tensor(out);
  out = tmp;
  if (out) {
    puts("\nFinal OUT:");
    _print_tensor(out);
    printf("Shape: ");
    print_shape(out);
  }

  free_tensor(conv2d_bias);
  free_tensor(conv2d_ker);

  return out;
}
