#include "common.h"
#include "layers/conv1.h"
#include "layers/dense.h"
#include "layers/maxpooling2d.h"
#include "layers/normalisaatio.h"
#include "layers/resizing.h"
#include "layers/toinenkonvoluutio.h"
#include "weights_map.h"

struct tensor *model(struct tensor *input) {
  struct tensor *tmp;
  struct tensor *out;

  float *ret = malloc(32 * 32 * sizeof(float));
  resizing((float(*)[])input->vec, (float(*)[])ret);
  free_tensor(input);
  normalisaatio((float(*)[])ret);
  {
    out = malloc(sizeof(struct tensor));
    out->vec = ret;
    out->shape = malloc(4 * sizeof(size_t));

    out->shape[0] = 1;
    out->shape[1] = 32;
    out->shape[2] = 32;
    out->shape[3] = 0;
  }

  // clang-format off
  // TENSOR(conv2d_kernel, VEC(
  //     DIM(1.f, 1.f, 1.f),
  //     DIM(2.f, 0.f, 2.f),
  //     DIM(3.f, 3.f, 3.f),
  // ), SHAPE(1, 3, 3));
  // TENSOR(conv2d_bias, VEC(DIM(0.f)), SHAPE(1, 1, 1));
  // clang-format on

  puts("Conv2d");
  // size_t shape[] = SHAPE(32, 3, 3);
  const struct tensor *conv2d_kernel = create_static_tensor(conv2d_kweights, conv2d_kshape);
  const struct tensor *conv2d_bias = create_static_tensor(conv2d_bweights, conv2d_bshape);
  tmp = conv2d1(out, conv2d_kernel, conv2d_bias);
  free_tensor(out);
  out = tmp;
  free_tensor(conv2d_kernel);
  free_tensor(conv2d_bias);

  puts("Conv2d 2");
  ret = calloc(OUT_IMAGES * OUT_SIZE_Y * OUT_SIZE_X, sizeof(float));
  toinenkonvoluutio((float(*)[IMAGE_SIZE_Y][IMAGE_SIZE_X])out->vec, (float(*)[OUT_SIZE_Y][OUT_SIZE_X])ret);
  free_tensor(out);

  // const struct tensor *conv2d_1_kernel = create_static_tensor(conv2d_1_kweights, conv2d_1_kshape);
  // const struct tensor *conv2d_1_bias = create_static_tensor(conv2d_1_bweights, conv2d_1_bshape);
  // tmp = conv2d1(out, conv2d_1_kernel, conv2d_1_bias);
  // free_tensor(out);
  // out = tmp;
  // free_tensor(conv2d_kernel);
  // free_tensor(conv2d_bias);

  // Initialize dense kernel
  // clang-format off
  // TENSOR(dense_kernel, VEC(
  //     DIM(1, 1, 2, 2),
  //     DIM(1, 1, .5, .5),
  //     DIM(1, 1, 2, 2),
  //     DIM(1, 1, .5, .5)
  // ), SHAPE(2 * 2, 4));
  // clang-format on

  puts("Maxpooling");
  {
    out = malloc(sizeof(struct tensor));
    out->vec = calloc(12544, sizeof(float));
    out->shape = malloc(3 * sizeof(size_t));

    out->shape[0] = 1;
    out->shape[1] = 12544;
    out->shape[2] = 0;
  }
  maxpooling2d(ret, 1, 64, 28, 28, 2, 2, 2, 2, out->vec);

  // size_t new_shape[] = SHAPE(1, 12544);
  // reshape(out, new_shape);
  // puts("Reshaped out:");
  // print_shape(out);

  puts("Dense");
  const struct tensor *dense_kernel = create_static_tensor(dense_kweights, dense_kshape);
  const struct tensor *dense_bias = create_static_tensor(dense_bweights, dense_bshape);
  tmp = dense(out, dense_kernel, dense_bias);
  free_tensor(out);
  out = tmp;

  free_tensor(dense_kernel);
  free_tensor(dense_bias);

  puts("Dense 2");
  const struct tensor *dense_1_kernel = create_static_tensor(dense_1_kweights, dense_1_kshape);
  const struct tensor *dense_1_bias = create_static_tensor(dense_1_bweights, dense_1_bshape);
  tmp = dense(out, dense_1_kernel, dense_1_bias);
  free_tensor(out);
  out = tmp;
  free_tensor(dense_1_kernel);
  free_tensor(dense_1_bias);

  puts("\nModel fin:");
  return out;
}
