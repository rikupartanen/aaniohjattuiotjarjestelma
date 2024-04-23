#include "common.h"
#include "layers/conv1.h"
#include "layers/dense.h"
#include "layers/toinenkonvoluutio.h"
#include "weights_map.h"

struct tensor *model(struct tensor *input) {
  // clang-format off
  // TENSOR(conv2d_ker, VEC(
  //     DIM(1.f, 1.f, 1.f),
  //     DIM(2.f, 0.f, 2.f),
  //     DIM(3.f, 3.f, 3.f),
  // ), SHAPE(1, 3, 3));
  // TENSOR(conv2d_bias, VEC(DIM(0.f)), SHAPE(1, 1, 1));
  // clang-format on

  const struct tensor *conv2d_ker = create_static_tensor(conv2d_kweights, conv2d_kshape);
  const struct tensor *conv2d_bias;
  if (!conv2d_bweights) {
    float v[] = VEC(DIM(0.f));
    size_t s[] = SHAPE(1, 1, 1);
    conv2d_bias = create_tensor(v, s);
  } else {
    conv2d_bias = create_static_tensor(conv2d_bweights, conv2d_bshape);
  }

  struct tensor *out = conv2d1(input, conv2d_ker, conv2d_bias);
  printf("OUT\n");
  print_tensor(out);
  print_shape(out);

  float ret[OUT_IMAGES][OUT_SIZE_Y][OUT_SIZE_X];
  toinenkonvoluutio((float(*)[4][4])out->vec, ret);
  free_tensor(out);

  out = create_tensor((float *)ret, (size_t[])SHAPE(1, 2, 2));
  _print_tensor(out);
  print_shape(out);

  // Initialize dense kernel
  // clang-format off
  // TENSOR(dense_kernel, VEC(
  //     DIM(1, 1, 2, 2),
  //     DIM(1, 1, .5, .5),
  //     DIM(1, 1, 2, 2),
  //     DIM(1, 1, .5, .5)
  // ), SHAPE(2 * 2, 4));
  // clang-format on
  const struct tensor *dense_kernel = create_static_tensor(dense_kweights, dense_kshape);
  puts("Dense kernel:");
  _print_tensor(dense_kernel);
  print_shape(dense_kernel);

  size_t new_shape[] = SHAPE(1, 2 * 2);
  reshape(out, new_shape);
  puts("Reshaped out:");
  print_shape(out);

  struct tensor *tmp = dense(out, dense_kernel, NULL);
  free_tensor(out);
  out = tmp;
  if (out) {
    puts("\nModel fin:");
    _print_tensor(out);
    print_shape(out);
  }

  free_tensor(conv2d_bias);
  free_tensor(conv2d_ker);
  free_tensor(dense_kernel);

  return out;
}
