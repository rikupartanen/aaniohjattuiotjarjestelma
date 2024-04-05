#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 0  // 0 to 2

// Create shape array
#define SHAPE(...) \
    { __VA_ARGS__, 0 }
// Create vector array
#define VEC(...) \
    { __VA_ARGS__ }
// Noop, used for readability
#define DIM(...) __VA_ARGS__
// Helper for creating tensors
#define TENSOR(name, vec, shape) \
    struct tensor *name;           \
    do {                           \
        float v[] = vec;             \
        size_t s[] = shape;          \
        name = create_tensor(v, s);  \
    } while (0)

struct tensor {
    float *vec;
    size_t *shape;
};

// Return the lenght of shape list
static inline size_t get_shape_len(size_t *shape) {
    size_t len;
    for (len = 0; shape[len]; len++) {}
    return len;
}

/**
 * @brief Return geometric sum from start to len - end
 *
 * @param vec values to sum
 * @param len lenght of input
 * @param start start index
 * @param end end index as `len - end`
 */
static inline size_t geometric_sum(size_t *vec, size_t len, size_t start, size_t end) {
    size_t r_len = 1;
    for (size_t i = start; i < len - end; i++) {
        if (DEBUG>1) printf("geometric_sum: r_len:%zu vec[%zu]:%zu\n", r_len, i, vec[i]);
        r_len *= vec[i];
    }
    if (DEBUG>1) printf("final geometric_sum r_len: %zu\n", r_len);
    return r_len;
}

// Create tensor in heap memory
struct tensor *create_tensor(float *vec, size_t *shape) {
    if (DEBUG>1) printf("  creating tensor\n");
    struct tensor *l = malloc(sizeof(struct tensor));
    size_t shape_len = get_shape_len(shape);
    size_t vec_len = geometric_sum(shape, shape_len, 0, 0);

    shape_len++;
    l->vec = malloc(vec_len * sizeof(float));
    l->shape = malloc(shape_len * sizeof(size_t));
    memmove(l->vec, vec, vec_len * sizeof(float));
    memmove(l->shape, shape, shape_len * sizeof(size_t));
    if (DEBUG>1) printf("  done creating tensor\n");
    return l;
}

// Free heap tensor
void free_tensor(struct tensor *tensor) {
    if (tensor) {
        free(tensor->vec);
        free(tensor->shape);
        free(tensor);
        if (DEBUG>1) printf("  freed tensor\n");
    }
}

struct tensor *conv2d1(
    const struct tensor *input,
    const struct tensor *kernel,
    const struct tensor *bias
) 
{
 //   size_t ker_shape_len = get_shape_len(kernel->shape);  // length of shape list
    size_t in_shape_len = get_shape_len(input->shape);  // length of shape list

 //   size_t len_in = input->shape[0];  //  only taking one input in
    size_t len_iy = input->shape[1];
    size_t len_ix = input->shape[2];

    size_t len_kn = kernel->shape[0];  //  number of kernels
    size_t len_ky = kernel->shape[1];  //  number of kernel rows or dimension y
    size_t len_kx = kernel->shape[2];  //  number of kernel columns or dimension x

    size_t len_bias = bias->shape[2];

    if (len_bias != len_kn){
        printf("  Bias and kernel count mismatch, bias %zu, kernel %zu\n", len_bias, len_kn);
    }

    size_t size = (len_kn * (len_iy - len_ky + 1) * (len_ix - len_kx + 1));
    float ret[size];
    if (DEBUG>0) printf("Made temporary array with length %zu\n", size);

    float tempcount = 0.f;
    size_t position = 0;
    for(size_t kn = 0;kn<len_kn;kn++){
        for(size_t iy = 0;iy<(len_iy-len_ky+1);iy++){
            for(size_t ix = 0;ix<(len_ix-len_kx+1);ix++){
                for(size_t ky = 0;ky<len_ky;ky++){
                    for(size_t kx = 0;kx<len_kx;kx++){
                        float in_pos_content = input->vec[(ky * len_ix) + (iy * len_ix) + ix + kx];
                        float k_pos_content = kernel->vec[(kn * len_ky * len_kx) + (ky * len_kx) + kx];
                        tempcount += in_pos_content * k_pos_content;
                        if (DEBUG>1) printf("kn%zu iy%zu ix%zu ky%zu kx%zu IN%f K%f tc%f\n", kn, iy, ix, ky, kx, in_pos_content, k_pos_content, tempcount);
                    }
                }
                position = (kn * (len_iy - len_ky + 1) * (len_ix - len_kx + 1) + (iy * (len_ix - len_kx + 1)) + ix);
                if (bias){  // add bias if specified
                    tempcount += bias->vec[kn];
                    if (DEBUG>1) printf("ADDED BIAS: %f position %zu\n", bias->vec[kn], position);
                    }
                if (tempcount<0) tempcount = 0.0;  //  RELU, limit would-be less than 0 outputs to 0
                ret[position] = tempcount;
                if (DEBUG>0) printf("CONVO kn%zu iy%zu ix%zu %f to position %zu\n", kn, iy, ix, tempcount, position);
                tempcount = 0;
            }
        }
    }

    size_t out_shape[] = {len_kn, (len_iy - len_ky + 1), (len_ix - len_kx + 1), 0};
/*
    size_t out_shape[len_kn * (len_iy - len_ky + 1) * (len_ix - len_kx + 1)];
    for (size_t i = 0; i < in_shape_len - 1; i++){
        out_shape[i] = input->shape[i];
    }
    out_shape[in_shape_len - 1] = kernel->shape[ker_shape_len - 1];
    out_shape[in_shape_len] = 0;
*/
    if (DEBUG>0){
        printf("In function conv2d1: out_shape: ");
        for (size_t i = 0; i < in_shape_len + 1; i++) {
            if (i) printf(", ");
            printf("%zu", out_shape[i]);
        }
    }
    return create_tensor(ret, out_shape);
}

void print_tensor(struct tensor *tensor){
//    size_t len = get_shape_len(tensor->shape);
    size_t len_n = tensor->shape[0];
    size_t len_y = tensor->shape[1];
    size_t len_x = tensor->shape[2];
    for(size_t n = 0;n<len_n;n++){
        for(size_t y = 0;y<len_y;y++){
            for(size_t x = 0;x<len_x;x++){                        
                printf("%.6f  ", tensor->vec[(n * len_y * len_x) + (y * len_x) + x]);
            }
            printf("\n");
        }
        printf("\n");
    }
    printf("\n");
}

//input, kernel, bias ja out on kaikki struct tensor * -tyyppiä
int main() {
    // Manual tensor creation
    struct tensor *bias1;
    {
        float bias_vec[] = VEC(DIM(10000.f, 20000.f));
        size_t bias_shape[] = SHAPE(1, 1, 2);
        bias1 = create_tensor(bias_vec, bias_shape);
    }

    struct tensor *input1;
    {
        float tensor_vec[] = VEC(   DIM(1.f, 1.f, 1.f, 1.f, 1.f, 1.f),
                                    DIM(1.f, 1.f, 1.f, 1.f, 1.f, 1.f),
                                    DIM(1.f, 1.f, 1.f, 1.f, 1.f, 1.f),
                                    DIM(1.f, 1.f, 1.f, 1.f, 1.f, 1.f),
                                    DIM(1.f, 1.f, 1.f, 1.f, 1.f, 1.f),
                                    DIM(1.f, 1.f, 1.f, 1.f, 1.f, 1.f));
        size_t tensor_shape[] = SHAPE(1, 6, 6);
        input1 = create_tensor(tensor_vec, tensor_shape);
    }

    struct tensor *input2;
    {
        float tensor_vec[] = VEC(   DIM(1.f, 2.f, 3.f, 4.f, 5.f, 6.f),
                                    DIM(11.f, 12.f, 13.f, 14.f, 15.f, 16.f),
                                    DIM(21.f, 22.f, 23.f, 24.f, 25.f, 26.f),
                                    DIM(31.f, 32.f, 33.f, 34.f, 35.f, 36.f),
                                    DIM(41.f, 42.f, 43.f, 44.f, 45.f, 46.f),
                                    DIM(51.f, 52.f, 53.f, 54.f, 55.f, 56.f));
        size_t tensor_shape[] = SHAPE(1, 6, 6);
        input2 = create_tensor(tensor_vec, tensor_shape);
    }

    struct tensor *kernel1;
    {
        float ker_vec[] = VEC(DIM(1.f, 1.f, 1.f), 
                              DIM(1.f, 1.f, 1.f), 
                              DIM(1.f, 1.f, 1.f),
                              DIM(2.f, 2.f, 2.f),
                              DIM(2.f, 2.f, 2.f), 
                              DIM(2.f, 2.f, 2.f));
        size_t ker_shape[] = SHAPE(2, 3, 3);
        kernel1 = create_tensor(ker_vec, ker_shape);
    }

    struct tensor *kernel2;
    {
        float ker_vec[] = VEC(DIM(1.f, 2.f, 3.f), 
                              DIM(4.f, 5.f, 6.f), 
                              DIM(7.f, 8.f, 9.f),
                              DIM(11.f, 12.f, 13.f),
                              DIM(14.f, 15.f, 16.f), 
                              DIM(17.f, 18.f, 19.f));
        size_t ker_shape[] = SHAPE(2, 3, 3);
        kernel2 = create_tensor(ker_vec, ker_shape);
    }

    struct tensor *out = conv2d1(input2, kernel1, bias1);

    printf("INPUT\n");
    print_tensor(input2);

    printf("KERNEL\n");
    print_tensor(kernel1);

    printf("BIAS\n");
    print_tensor(bias1);

    printf("OUT\n");
    print_tensor(out);

    free_tensor(input1);
    free_tensor(input2);
    free_tensor(bias1);
    free_tensor(kernel1);
    free_tensor(kernel2);
    free_tensor(out);

    if (!out) return -1;
    return 0;
}