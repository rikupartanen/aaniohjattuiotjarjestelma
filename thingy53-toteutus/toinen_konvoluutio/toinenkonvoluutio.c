#include <stdio.h>
#include "toinenkonvoluutio.h"

/*
Toisen konvoluution dokumentointi
Konvoluutio tarvitsee arvoiksi kuvien määrän (NUM_LAYERS) sekä koon (IMAGE_SIZE_X & IMAGE_SIZE_Y)
Nämä voidaan asettaa vakioiksi header-filessä sillä input-samplet taitavat olla aina samankokoisia
koska data tulee aina sekunnin mittaisena 16000 hZ tiedostona joka muutetaan tiettyyn muotoon.
Kernelit sekä bias täytyy pitää myös aina samana, niiden arvot täytyy päättää ennen kuin aletaan
opettamaan mallia.
Tällä hetkellä koodi printtaa konvoluution tulokset, sen voi estää kommentoimalla rivit 107-120

Toisen konvoluution kutsumisesimerkki:

int main() {
    printf("Aloitetaan konvoluutio\n");
    toinenkonvoluutio(image);
    return 0;
}

Tuossa käytetty image-float voisi näyttää esimerkiksi tältä:
    float image[NUM_LAYERS][IMAGE_SIZE_Y][IMAGE_SIZE_X] = {
    {
        {0, 1, 1, 0, 0},
        {1, 0, 1, 1, 0},
        {0, 1, 0, 1, 0},
        {1, 1, 0, 0, 0},
        {1, 0, 1, 1, 1}
    },
    {
        {0, 1, 1, 0, 1},
        {0, 0, 1, 1, 0},
        {1, 1, 0, 1, 1},
        {1, 1, 1, 0, 0},
        {1, 0, 0, 1, 1}
    },
    {
        {0, 1, 0, 1, 1},
        {0, 0, 0, 1, 1},
        {0, 1, 1, 0, 0},
        {1, 0, 1, 1, 0},
        {0, 0, 1, 0, 1}
    }
};

Ja tällöin NUM_LAYERS olisi 3, sekä IMAGE_SIZE_X ja IMAGE_SIZE_Y olisivat 5
Output tulee float toinenkonvoluutiooutput arrayna, koko riippuu inputeista

*/


// ReLU-funktio, jos arvo > 0 antaa saman arvon takaisin, jos arvo < 0 antaa tulokseksi 0
float relu(float x) {
    return (x > 0) ? x : 0;
}

//Bias-arvot, pidettävä samana
float bias[NUM_KERNELS] = {0.05, 0.02, -0.05, 0.06, 0.01, 0.03, 0.1};

//Kernel-arvot, pidettävä samana
float kernels[NUM_KERNELS][KERNEL_SIZE][KERNEL_SIZE] = {
    {{-0.1, 0.2, -0.1}, {0.2, 0.4, 0.2}, {-0.1, 0.2, -0.1}},
    {{-0.2, -0.3, -0.2}, {-0.3, 3, -0.3}, {-0.2, -0.3, -0.2}},
    {{0.1, 0.2, 0.1}, {0.2, 0.5, 0.2}, {0.1, 0.2, 0.1}},
    {{-0.2, 0.1, 0.2}, {-0.2, 0.3, 0.2}, {-0.2, 0.1, 0.2}},
    {{-0.2, -1, -0.2}, {-1, 5, -1}, {-0.2, -1, -0.2}},
    {{-0.67, 0.28, -0.91},{0.53, -0.75, 0.82},{-0.14, 0.91, -0.39}},
    {{0.25, -0.15, 0.60},{-0.40, 0.80, -0.35},{0.10, -0.20, 0.55}}
    };

// Konvoluutiofunktio
void toinenkonvoluutio(float image[NUM_LAYERS][IMAGE_SIZE_Y][IMAGE_SIZE_X]) {

    // Output array, alustetaan nollilla
    int output_layers = NUM_LAYERS * NUM_KERNELS;
    int oikealayer = 0;
    float toinenkonvoluutiooutput[output_layers][IMAGE_SIZE_Y - KERNEL_SIZE + 1][IMAGE_SIZE_X - KERNEL_SIZE + 1];
    for (int layer = 0; layer < output_layers; layer++) {
        for (int image_y = 0; image_y < IMAGE_SIZE_Y - KERNEL_SIZE + 1; image_y++) {
            for (int image_x = 0; image_x < IMAGE_SIZE_X - KERNEL_SIZE + 1; image_x++) {
                toinenkonvoluutiooutput[layer][image_y][image_x] = 0.0;
            }
        }
    }

    // Konvoluutioidaan jokaisen kernelin verran
    for (int kernel_num = 0; kernel_num < NUM_KERNELS; kernel_num++) {
        for (int layer = 0; layer < NUM_LAYERS; layer++, oikealayer++) {
            // Liikutetaan kerneliä kuvan päällä
            for (int image_y = 0; image_y <= IMAGE_SIZE_Y - KERNEL_SIZE; image_y++) {
                for (int image_x = 0; image_x <= IMAGE_SIZE_X - KERNEL_SIZE; image_x++) {
                    float sum = 0.0;
                    // Konvoluutio kaikilla layereilla
                    for (int kernel_x = 0; kernel_x < KERNEL_SIZE; kernel_x++) {
                        for (int kernel_y = 0; kernel_y < KERNEL_SIZE; kernel_y++) {
                            // Lasketaan kernel kuvaan
                            sum += image[layer][image_y + kernel_y][image_x + kernel_x] * kernels[kernel_num][kernel_y][kernel_x];
                    }
                }
                // Lisätään bias tulokseen ja käytetään arvot ReLU-funktion kautta
                toinenkonvoluutiooutput[oikealayer][image_y][image_x] = relu(sum + bias[kernel_num]);
                }
            }
        }
    }

    // Printataan jokaisen konvoluution tulos
    int layerprint = 0, kernelprint = 1;
    for (int i = 0; i < output_layers; ++i, ++layerprint){
        if (layerprint == NUM_LAYERS){layerprint = 0, kernelprint++;}
        printf("Output of convolution for layer %d", layerprint + 1);
        printf(" with kernel %d:\n", kernelprint);
        for (int j = 0; j <= IMAGE_SIZE_Y - KERNEL_SIZE; ++j) {
            for (int k = 0; k <= IMAGE_SIZE_X - KERNEL_SIZE; k++) {
                printf("%.2f\t", toinenkonvoluutiooutput[i][j][k]);
                }
            printf("\n");
        }
        printf("\n");
        }
}
