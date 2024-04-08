#include <cstddef>
#include <cstring>
#include <iostream>
#include <ostream>
#include <vector>

// Create shape array
#define SHAPE(...) \
  { __VA_ARGS__, 0 }
// Create vector array
#define VEC(...) \
  { __VA_ARGS__ }
// Noop, used for readability
#define DIM(...) __VA_ARGS__

class tensor {
  using VecType = std::vector<float>;

  std::vector<float> _vec;
  std::vector<size_t> _shape;

public:
  tensor(std::vector<float> vec, std::vector<size_t> shape) : _vec(vec), _shape(shape){};

  VecType::iterator begin() { return _vec.begin(); }
  VecType::iterator end() { return _vec.end(); }
  float &operator[](size_t i) { return _vec[i]; }
  float operator[](size_t i) const { return (*const_cast<tensor *>(this))[i]; }

  std::vector<float> &vec = _vec;
  std::vector<size_t> &shape = _shape;
};

/**
 * @brief Return geometric sum from start to len - end
 *
 * @param vec values to sum
 * @param len lenght of input
 * @param start start index
 * @param end end index as `len - end`
 */
static inline size_t geometric_sum(
    std::vector<size_t> &vec,
    size_t start,
    size_t end
) {
  size_t r_len = 1;
  for (size_t i = start; i < vec.size() - end; i++) {
    r_len *= vec[i];
  }
  return r_len;
}

/**
 * @brief Calculate matrix multiplication between vector and kernel.
 * @note Vector lenght and kernel's second dimension lenght have to match.
 *
 * @param vec input
 * @param vec_len lenght of input
 * @param kernel two dimensional matrix
 * @param out output array with same lenght as kernel's second dimension
 * @return -1 on error, otherwise 0
 */
int mat_mul(float *vec, size_t vec_len, const tensor &kernel, float *out) {
  size_t first = kernel.shape.front();
  if (vec_len != first) return -1;
  size_t last = kernel.shape.back();

  // In-place transpose kernel
  for (size_t j = 0; j < last; j++) {
    float sum = 0.0f;
    for (size_t i = 0; i < first; i++) {
      sum += vec[i] * kernel.vec[j + i * last];
    }
    out[j] = sum;
  }

  return 0;
}

// Calculate dense layer
tensor *dense(
    const tensor *input,
    const tensor &kernel,
    const tensor *bias
) {
  size_t ker_shape_len = kernel.shape.size();
  size_t in_shape_len = input->shape.size();

  size_t batches = input->shape.front();
  size_t mid = geometric_sum(input->shape, 1, 1);
  size_t vec_len = input->shape.back();

  size_t size = batches * mid * kernel.shape.back();
  std::vector<float> ret(size);

  for (size_t b = 0; b < batches; b++) {
    for (size_t m = 0; m < mid; m++) {
      size_t ret_pos = b * (mid * kernel.shape.back())
                     + m * kernel.shape.back();
      float *p = &ret[ret_pos];

      size_t in_pos = b * mid * vec_len + m * vec_len;

      // Matrix multiply input vector with kernel
      int err = mat_mul(
          const_cast<float *>(&input->vec[in_pos]),  // Start position in input
          vec_len,                                   // Vectors lenght
          kernel,                                    //
          p                                          // Start position in return array
      );

      if (bias) {
        for (size_t i = 0; i < kernel.shape.back(); i++) {
          p[i] += (*bias)[i];
        }
      }
      if (err) return nullptr;
    }
  }

  std::vector<size_t> out_shape(in_shape_len);
  for (size_t i = 0; i < in_shape_len - 1; i++)
    out_shape[i] = input->shape[i];
  out_shape[in_shape_len - 1] = kernel.shape.back();

#if false
  for (size_t i = 0; i < in_shape_len + 1; i++) {
    if (i) printf(", ");
    printf("%lu", out_shape[i]);
  }
  printf("\n");
#endif

  return new tensor(ret, out_shape);
}

int main() {
  tensor *input = new tensor({DIM(1.f, 2.f), DIM(1.f, 2.f)}, {2, 2});
  tensor *kernel = new tensor({DIM(1.f, 2.f, 3.f), DIM(1.f, 2.f, 3.f)}, {2, 3});
  tensor *out = dense(input, *kernel, nullptr);

  if (out) {
    for (auto f : out->vec) {
      std::cout << f << '\n';
    }
  }

  if (out) {
    size_t len = out->shape.size();
    size_t data_len = 1;
    for (size_t i = 0; i < len; i++) {
      data_len *= out->shape[i];
    }
    for (size_t i = 0; i < data_len; i++) {
      std::cout << out->vec[i];
    }
    std::cout << std::endl;
  }

  delete input;
  delete kernel;
  delete out;

  if (!out) return -1;
  return 0;
}
