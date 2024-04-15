#include <stdio.h>
#include <stdlib.h>

#include "layerparser.h"

int main(){
    struct layer **layers = NULL;

    size_t n_layers = get_layers(&layers);

    print_layers(layers, n_layers);

    free_layers(layers, n_layers);
    free(layers);
}
