#include <stdio.h>
#include "toinenkonvoluutio.h"

// ReLU funktio
float relu(float x) {
    return (x > 0) ? x : 0;
}

// Konvoluutiofunktio
void toinenkonvoluutio(float image[NUM_LAYERS][IMAGE_SIZE_Y][IMAGE_SIZE_X], float kernels[NUM_KERNELS][KERNEL_SIZE][KERNEL_SIZE], float bias[NUM_KERNELS]) {

    // Output array, alustetaan nollilla 
    float output[OUTPUT_LAYERS][IMAGE_SIZE_Y - KERNEL_SIZE + 1][IMAGE_SIZE_X - KERNEL_SIZE + 1];
    for (int layer = 0; layer < OUTPUT_LAYERS; layer++) {
        for (int image_y = 0; image_y < IMAGE_SIZE_Y - KERNEL_SIZE + 1; image_y++) {
            for (int image_x = 0; image_x < IMAGE_SIZE_X - KERNEL_SIZE + 1; image_x++) {
                output[layer][image_y][image_x] = 0.0;
        }
    }
}

    int image_x, image_y, kernel_num, kernel_x, kernel_y, layer;
    // Konvoluutioidaan jokaisen kernelin verran
    for (kernel_num = 0; kernel_num < NUM_KERNELS; kernel_num++) {
        for (layer = 0; layer < NUM_LAYERS; layer++) {
            // Liikutetaan kerneliä kuvan päällä
            for (image_y = 0; image_y <= IMAGE_SIZE_Y - KERNEL_SIZE; image_y++) {
                for (image_x = 0; image_x <= IMAGE_SIZE_X - KERNEL_SIZE; image_x++) {
                    float sum = 0.0;
                    // Konvoluutio kaikilla layereilla
                    for (kernel_x = 0; kernel_x < KERNEL_SIZE; kernel_x++) {
                        for (kernel_y = 0; kernel_y < KERNEL_SIZE; kernel_y++) {
                            // Lasketaan kernel kuvaan
                            sum += image[layer][image_y + kernel_y][image_x + kernel_x] * kernels[kernel_num][kernel_y][kernel_x];
                    }
                }
                // Lisätään bias tulokseen ja käytetään arvot ReLU-funktion läpi
                output[layer][image_y][image_x] = relu(sum + bias[layer]);
            }
        }
    }
        // Printataan jokaisen konvoluution tulos
    for (int i = 0; i < NUM_LAYERS; ++i){
        printf("Output of convolution for kernel %d:\n", i + 1);
        for (int j = 0; j <= IMAGE_SIZE_Y - KERNEL_SIZE; ++j) {
            for (int k = 0; k <= IMAGE_SIZE_X - KERNEL_SIZE; k++) {
                printf("%.2f\t", output[i][j][k]);
                }
            printf("\n");
        }
        printf("\n");
        }
    }
}
