#include "common.h"
#include "layers/conv2d.h"
#include "layers/dense.h"
#include "layers/maxpooling2d.h"
#include "layers/normalisaatio.h"
#include "layers/resizing.h"
#include "weights/weight_tensors.h"

#define STRIDE_SIZE 2
#define POOL_SIZE 2

void swap(struct tensor **a, struct tensor **b) {
  struct tensor *tmp = *a;
  *a = *b;
  *b = tmp;
}

static void relu(struct tensor *tensor) {
  size_t len = get_shape_len(tensor->shape);
  len = geometric_sum(tensor->shape, len, 0, 0);
  for (size_t i = 0; i < len; i++) {
    tensor->vec[i] = tensor->vec[i] < 0 ? 0 : tensor->vec[i];
  }
}

void _free(struct tensor *in) {
  if (!in) return;
  free(in->vec);
  free(in->shape);
}

struct tensor *model(struct tensor *input) {
  struct tensor *tmp = malloc(sizeof(*tmp));
  struct tensor *out = malloc(sizeof(*out));

  float *ret = malloc(32 * 32 * sizeof(float));
  resizing((float(*)[])input->vec, (float(*)[])ret);
  free_tensor(input);
  normalisaatio((float(*)[])ret);
  {
    out->vec = ret;
    out->shape = malloc(4 * sizeof(size_t));
    create_shape(out->shape, 4, 32, 32, 1);
  }

  puts("Conv2d");
  swap(&out, &tmp);
  conv2d1_fixed(tmp, &conv2d_kernel, &conv2d_bias, out);
  _free(tmp);

  puts("Conv2d 2");
  swap(&out, &tmp);
  conv2d1_fixed(tmp, &conv2d_1_kernel, &conv2d_1_bias, out);
  _free(tmp);

  puts("Maxpooling");
  swap(&out, &tmp);
  {
    out->vec = calloc(12544, sizeof(float));
    out->shape = malloc(3 * sizeof(size_t));
    create_shape(out->shape, 3, 1, 12544);
  }
  maxpooling2d(
      tmp->vec,
      1,              // Batch size
      tmp->shape[2],  // Input channel count
      tmp->shape[0],  // Input height
      tmp->shape[1],  // Input width
      POOL_SIZE,
      POOL_SIZE,
      STRIDE_SIZE,
      STRIDE_SIZE,
      out->vec
  );
  _free(tmp);

  puts("Dense");
  swap(&out, &tmp);
  relu(dense(tmp, &dense_kernel, &dense_bias, out));
  _free(tmp);

  puts("Dense 2");
  swap(&out, &tmp);
  dense(tmp, &dense_1_kernel, &dense_1_bias, out);
  free_tensor(tmp);

  puts("\nModel fin:");
  return out;
}
