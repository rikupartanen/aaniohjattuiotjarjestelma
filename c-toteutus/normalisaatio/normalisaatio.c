#include <stdio.h>
#include <math.h>
#include "normalisaatio.h"

//Tensorflow:n esimerkissä käytetyt arvot. Pysyvät samana koulutuksen jälkeen.
// Variance on jo laskettu neliöjuuren kautta.
const float mean = 0.1258;
const float variance = 0.7673;
// Vaihda 1 jos haluat printattavan
int debug_level = 1;

// Normalisoidaan arrayn arvot käyttäen keskiarvoa ja varianssia
void normalisaatio(float input_array[ARRAY_SIZE_X][ARRAY_SIZE_Y]) {
    for (int i = 0; i < ARRAY_SIZE_X; i++) {
        for (int j = 0; j < ARRAY_SIZE_Y; j++) {
            input_array[i][j] = (input_array[i][j] - mean) / variance;
        }
    }

if (debug_level == 1){
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

//Esimerkissä oleva mean: 0.1258, variance: 0.5887, sqrt_variance: 0.7673