#include "normalisaatio.h"

#include <math.h>
#include <stdio.h>

#include "weights_map.h"

// Vaihda 1 jos haluat printattavan
int debug_level = 0;

// Normalisoidaan arrayn arvot käyttäen keskiarvoa ja varianssia
void normalisaatio(float input_array[ARRAY_SIZE_X][ARRAY_SIZE_Y]) {
  // Tensorflow:n esimerkissä käytetyt arvot. Pysyvät samana koulutuksen jälkeen.
  // Variance on jo laskettu neliöjuuren kautta.
  const _Float16 mean = normalization_kweights[0];
  const _Float16 inv_sqrt_variance = 1 / sqrt(normalization_kweights[1]);

  for (int i = 0; i < ARRAY_SIZE_X; i++) {
    for (int j = 0; j < ARRAY_SIZE_Y; j++) {
      input_array[i][j] = (input_array[i][j] - mean) * inv_sqrt_variance;
    }
  }

  if (debug_level == 1) {
    // Tulostetaan normalisoitu array
    printf("Normalisoitu array:\n");
    for (int i = 0; i < ARRAY_SIZE_X; i++) {
      for (int j = 0; j < ARRAY_SIZE_Y; j++) {
        printf("%.2f ", input_array[i][j]);
      }
      printf("\n");
    }
  }
}

// Esimerkissä oleva mean: 0.1258, variance: 0.5887, sqrt_variance: 0.7673
