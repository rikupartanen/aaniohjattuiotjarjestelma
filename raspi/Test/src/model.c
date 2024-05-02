#include "common.h"
#include "layers/conv1.h"
#include "layers/dense.h"
#include "layers/maxpooling2d.h"
#include "layers/normalisaatio.h"
#include "layers/resizing.h"
#include "layers/toinenkonvoluutio.h"
#include "weights_map.h"
#include "layers/conv2d.h"

struct tensor *model(struct tensor *input) {
  struct tensor *tmp;
  struct tensor *out;

  float *ret = malloc(32 * 32 * sizeof(float));
  resizing((float(*)[])input->vec, (float(*)[])ret);
  free_tensor(input);
  normalisaatio((float(*)[])ret);
  {
    out = malloc(sizeof(struct tensor));
    out->_static = false;
    out->vec = ret;
    out->shape = malloc(4 * sizeof(size_t));

    out->shape[0] = 32;
    out->shape[1] = 32;
    out->shape[2] = 1;
    out->shape[3] = 0;
  }

  puts("Conv2d");
  // size_t shape[] = SHAPE(32, 3, 3);
  // const struct tensor *conv2d_kernel = create_static_tensor(conv2d_kweights, shape);
  const struct tensor *conv2d_kernel = create_static_tensor(conv2d_kweights, conv2d_kshape);
  const struct tensor *conv2d_bias = create_static_tensor(conv2d_bweights, conv2d_bshape);
  tmp = conv2d1_fixed(out, conv2d_kernel, conv2d_bias);
  free_tensor(out);
  out = tmp;
  free_tensor(conv2d_kernel);
  free_tensor(conv2d_bias);

  return out;


  puts("Conv2d 2");
  conv2d_kernel = create_static_tensor(conv2d_1_kweights, conv2d_1_kshape);
  conv2d_bias = create_static_tensor(conv2d_1_bweights, conv2d_1_bshape);
  tmp = conv2d1_fixed(out, conv2d_kernel, conv2d_bias);
  free_tensor(out);
  free_tensor(conv2d_kernel);
  free_tensor(conv2d_bias);

  // ret = calloc(OUT_IMAGES * OUT_SIZE_Y * OUT_SIZE_X, sizeof(float));
  // toinenkonvoluutio((float(*)[IMAGE_SIZE_Y][IMAGE_SIZE_X])out->vec, (float(*)[OUT_SIZE_Y][OUT_SIZE_X])ret);
  // free_tensor(out);

  puts("Maxpooling");
  {
    out = malloc(sizeof(struct tensor));
    out->_static = false;
    out->vec = calloc(12544, sizeof(float));
    out->shape = malloc(3 * sizeof(size_t));

    out->shape[0] = 1;
    out->shape[1] = 12544;
    out->shape[2] = 0;
  }
  maxpooling2d(tmp->vec, 1, 64, 28, 28, 2, 2, 2, 2, out->vec);

  puts("Dense");
  const struct tensor *dense_kernel = create_static_tensor(dense_kweights, dense_kshape);
  const struct tensor *dense_bias = create_static_tensor(dense_bweights, dense_bshape);
  tmp = dense(out, dense_kernel, dense_bias);
  free_tensor(out);
  out = tmp;
  free_tensor(dense_kernel);
  free_tensor(dense_bias);

  size_t len = get_shape_len(out->shape);
  len = geometric_sum(out->shape, len, 0, 0);
  for (size_t i = 0; i < len; i++) {
    out->vec[i] = out->vec[i] < 0 ? 0 : out->vec[i];
  }

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
