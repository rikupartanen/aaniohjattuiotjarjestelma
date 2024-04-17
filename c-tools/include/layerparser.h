#ifndef LAYERPARSER_H__
#define LAYERPARSER_H__
#include "layer.h"


size_t get_layers(const char *filename, struct layer ***layers);
void free_layers(struct layer **layers, size_t n);
void print_layers(struct layer **layers, size_t n);
void get_weights(struct layer *l);

#endif /* LAYERPARSER_H__ */
