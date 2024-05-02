#include "toinenkonvoluutio.h"

#include <stdio.h>

#include "weights_map.h"

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
Output tulee float toinen_konvoluutio_output arrayna, koko riippuu inputeista

*/

// ReLU-funktio, jos arvo > 0 antaa saman arvon takaisin, jos arvo < 0 antaa tulokseksi 0
float relu(float x) {
  return (x > 0) ? x : 0;
}



// Bias-arvot, pidettävä samana koulutuksen jälkeen
// float bias[NUM_KERNELS] = {0};
float *bias = conv2d_1_bweights;

// Kernel-arvot, pidettävä samana koulutuksen jälkeen
// clang-format off
// float kernels[NUM_KERNELS][KERNEL_SIZE][KERNEL_SIZE] = {{
//     {0.5, 0.5, 0.5},
//     {1.0, 1.0, 1.0},
//     {0.0, 0.0, 0.0}
// }};
float (*kernels)[KERNEL_SIZE][KERNEL_SIZE] = (float(*)[KERNEL_SIZE][KERNEL_SIZE])conv2d_1_kweights;
// clang-format on

// Konvoluutiofunktio
void toinenkonvoluutio(
    float image[NUM_LAYERS][IMAGE_SIZE_Y][IMAGE_SIZE_X],
    float out[OUT_IMAGES][OUT_SIZE_Y][OUT_SIZE_X]
) {
  float(*toinen_konvoluutio_output)[OUT_SIZE_Y][OUT_SIZE_X] = out;
  // int output_layers = OUT_IMAGES;
  int oikea_output_layer = 0;
  int bias_lisays = 1;
  int layer = 1;


  // Output array, alustetaan nollilla
  // float toinen_konvoluutio_output[output_layers][IMAGE_SIZE_Y - KERNEL_SIZE + 1][IMAGE_SIZE_X - KERNEL_SIZE + 1];
  // for (int layer_alustus = 0; layer_alustus < output_layers; layer_alustus++) {
  //   for (int alustus_y = 0; alustus_y < IMAGE_SIZE_Y - KERNEL_SIZE + 1; alustus_y++) {
  //     for (int alustus_x = 0; alustus_x < IMAGE_SIZE_X - KERNEL_SIZE + 1; alustus_x++) {
  //       toinen_konvoluutio_output[layer_alustus][alustus_y][alustus_x] = 0.0;
  //     }
  //   }
  // }

  // Konvoluutioidaan jokaisen kernelin verran
  printf("Aloitetaan konvoluutio\n");
  for (int kernel_num = 0; kernel_num < NUM_KERNELS; kernel_num++, layer++) {
    // Liikutetaan kerneliä kuvan päällä
    for (int image_y = 0; image_y <= IMAGE_SIZE_Y - KERNEL_SIZE; image_y++) {
      for (int image_x = 0; image_x <= IMAGE_SIZE_X - KERNEL_SIZE; image_x++) {
        float sum = 0.0;
        for (int kernel_x = 0; kernel_x < KERNEL_SIZE; kernel_x++) {
          for (int kernel_y = 0; kernel_y < KERNEL_SIZE; kernel_y++) {
            // Lasketaan kernelin joka kohta kuvan päällä
            sum += image[layer - 1][image_y + kernel_y][image_x + kernel_x] * kernels[kernel_num][kernel_y][kernel_x];
          }
        }
        // Käytetään arvot ReLU-funktion kautta, lisätään bias viimeisellä layerilla
        if (bias_lisays == NUM_LAYERS) {
          // printf("Lisataan tulos %.2f kohtaan %d , %d , %d , lisataan bias\n", sum, oikea_output_layer, image_y, image_x);
          toinen_konvoluutio_output[oikea_output_layer][image_y][image_x] += relu(sum + bias[oikea_output_layer]);
          if (image_y == IMAGE_SIZE_Y - KERNEL_SIZE && image_x == IMAGE_SIZE_X - KERNEL_SIZE) {
            bias_lisays = 1;
          }
        } else if (bias_lisays < NUM_LAYERS) {
          // printf("Lisataan tulos %.2f kohtaan %d , %d , %d , ilman biasta\n", sum, oikea_output_layer, image_y, image_x);
          toinen_konvoluutio_output[oikea_output_layer][image_y][image_x] += relu(sum);
          if (image_y == IMAGE_SIZE_Y - KERNEL_SIZE && image_x == IMAGE_SIZE_X - KERNEL_SIZE) {
            bias_lisays++;
          }
        }
      }
    }
    if (layer == NUM_LAYERS) {
      layer = 0;
      oikea_output_layer++;
    }
  }

  // Printataan jokaisen konvoluution tulos
  // printf("Aloitetaan tulosten tulostus\n");
  // for (int i = 0; i < output_layers; ++i) {
  //   printf("Output of convolution # %d:\n", i + 1);
  //   for (int j = 0; j <= IMAGE_SIZE_Y - KERNEL_SIZE; ++j) {
  //     for (int k = 0; k <= IMAGE_SIZE_X - KERNEL_SIZE; k++) {
  //       printf("%.2f\t", toinen_konvoluutio_output[i][j][k]);
  //     }
  //     printf("\n");
  //   }
  //   printf("\n");
  // }
}
