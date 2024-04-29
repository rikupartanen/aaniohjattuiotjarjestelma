#include "weight_tensors.h"

#include "weights_map.h"

const struct tensor conv2d_kernel = {
    .vec = (float *)conv2d_kweights,
    .shape = (size_t *)conv2d_kshape
};
const struct tensor conv2d_bias = {
    .vec = (float *)conv2d_bweights,
    .shape = (size_t *)conv2d_bshape
};

const struct tensor conv2d_1_kernel = {
    .vec = (float *)conv2d_1_kweights,
    .shape = (size_t *)conv2d_1_kshape
};
const struct tensor conv2d_1_bias = {
    .vec = (float *)conv2d_1_bweights,
    .shape = (size_t *)conv2d_1_bshape
};

const struct tensor dense_kernel = {
    .vec = (float *)dense_kweights,
    .shape = (size_t *)dense_kshape
};
const struct tensor dense_bias = {
    .vec = (float *)dense_bweights,
    .shape = (size_t *)dense_bshape
};

const struct tensor dense_1_kernel = {
    .vec = (float *)dense_1_kweights,
    .shape = (size_t *)dense_1_kshape
};
const struct tensor dense_1_bias = {
    .vec = (float *)dense_1_bweights,
    .shape = (size_t *)dense_1_bshape
};
