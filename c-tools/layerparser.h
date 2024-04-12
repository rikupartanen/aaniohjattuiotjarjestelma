#ifndef LAYERPARSER_H__
#define LAYERPARSER_H__
struct layer {
    char *name;
    size_t *kshape;
    size_t kshape_len;
    size_t *bshape;
    size_t bshape_len;
};


size_t get_layers(struct layer ***layers);
void free_layers(struct layer **layers, size_t n);
void print_layers(struct layer **layers, size_t n);

#endif /* LAYERPARSER_H__ */
