#include <stdio.h>
#include <stdlib.h>

#include "layerparser.h"
#include "serial.h"

#define WORDSIZE 4
#define IS_ALIGNED(x) (!(x % WORDSIZE))
#define ALIGN(x) (x = (x + (WORDSIZE -1)) & -WORDSIZE)

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

void calculate_offsets(struct layer **layers, size_t n){
    for(size_t i = 0; i < n; ++i){
        layers[i]->offsets = malloc(sizeof (struct layer_offset));

        if(i == 0){
            layers[i]->offsets->kernel = 0x0;
            layers[i]->offsets->bias = layers[i]->weights->klen * sizeof(_Float16);
            ALIGN(layers[i]->offsets->kernel);
            ALIGN(layers[i]->offsets->bias);
            continue;
        }

        layers[i]->offsets->kernel = layers[i - 1]->offsets->bias + (layers[i - 1]->weights->blen * sizeof(_Float16));
        layers[i]->offsets->bias = layers[i]->offsets->kernel + (layers[i]->weights->klen * sizeof(_Float16));

        ALIGN(layers[i]->offsets->kernel);
        ALIGN(layers[i]->offsets->bias);
    }

}

void print_offsets(struct layer *l){
    printf("Layer %s\n", l->name);
    printf("\tKernel offset: %#x\n", l->offsets->kernel);
    printf("\tKernel len: %#x\n", (l->weights->klen) * sizeof(_Float16));
    printf("\tBias offset: %#x\n", l->offsets->bias);
    printf("\tBias len: %#x\n", (l->weights->blen) * sizeof(_Float16));
    putchar('\n');
}

int main(){

    struct layer **layers = NULL;
    size_t n_layers = get_layers(&layers);


    for(size_t i = 0; i < n_layers; ++i){
        get_weights(layers[i]);
    }

    calculate_offsets(layers, n_layers);
    for(size_t i = 0; i < n_layers; ++i){
        print_offsets(layers[i]);
    }



    print_layers(layers, n_layers);
    free_layers(layers, n_layers);
    free(layers);


}
