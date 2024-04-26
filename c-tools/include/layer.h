#ifndef LAYER_H__
#define LAYER_H__

#include "floattype.h"

struct layer_map{
    const FLOAT_T *kernel;
    const size_t klen;
    const FLOAT_T *bias;
    const size_t blen;
};

struct layer_offset{
    size_t kernel;
    size_t bias;
};

struct layer {
    char *name;
    size_t *kshape;
    size_t kshape_len;
    size_t *bshape;
    size_t bshape_len;
    struct layer_map *weights;
    struct layer_offset *offsets;
};


#endif /* LAYER_H__ */
