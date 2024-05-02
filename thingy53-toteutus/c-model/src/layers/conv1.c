#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../common.h"
#include "conv1.h"

#define DEBUG 0  // Debug message level 0 to 2

struct tensor *conv2d1(
    const struct tensor *input,
    const struct tensor *kernel,
    const struct tensor *bias
) 
{
 //   size_t ker_shape_len = get_shape_len(kernel->shape);  // length of shape list
 //   size_t in_shape_len = get_shape_len(input->shape);  // length of shape list

 //   size_t len_in = input->shape[0];  //  only taking one input in
    size_t len_iy = input->shape[1];
    size_t len_ix = input->shape[2];

    size_t len_kn = kernel->shape[0];  //  number of kernels
    size_t len_ky = kernel->shape[1];  //  number of kernel rows or dimension y
    size_t len_kx = kernel->shape[2];  //  number of kernel columns or dimension x

    size_t len_bias = bias->shape[0];

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
