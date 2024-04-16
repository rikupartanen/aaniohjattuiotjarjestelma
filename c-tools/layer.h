#ifndef LAYER_H__
#define LAYER_H__

struct layer_map{
    const _Float16 *kernel;
    const size_t klen;
    const _Float16 *bias;
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
