#include <stdio.h>
#include <math.h>
#include "normalisaatio.h"

/*
Normalisaatiofunktio. Normalisoi sisääntulevan arrayn. Headerfilessä voi asettaa arrayn koon.
Kutsutaan:
    normalisaatio(input_array);
jossa input_array on 2D array.
*/

//Tensorflow:n esimerkissä käytetyt arvot. Pysyvät samana koulutuksen jälkeen.
const float const_mean = 0.1258;
const float const_sqrt_variance = 0.7673;

// debug_level arvoksi 0, 1 tai 2
// 0 käyttää valmiita const mean ja variance arvoja eikä printtaa tuloksia
// 1 laskee datan omat mean ja variance ja käyttää niitä normalisointiin 
// 2  on sama kuin 1 mutta lisäksi printtaa tulokset
int debug_level = 1;

// Keskiarvon laskemisfunktio
float calculate_mean(float data[ARRAY_SIZE_X][ARRAY_SIZE_Y]) {
    float sum = 0;
    for (int i = 0; i < ARRAY_SIZE_X; i++) {
        for (int j = 0; j < ARRAY_SIZE_Y; j++) {
            sum += data[i][j];
        }
    }
    return sum / (ARRAY_SIZE_X * ARRAY_SIZE_Y);
}

// Varianssin laskemisfunktio
float calculate_variance(float data[ARRAY_SIZE_X][ARRAY_SIZE_Y], float mean) {
    float sum_of_variance = 0;
    for (int i = 0; i < ARRAY_SIZE_X; i++) {
        for (int j = 0; j < ARRAY_SIZE_Y; j++) {
            sum_of_variance += pow(data[i][j] - mean, 2);
        }
    }
    return sum_of_variance / (ARRAY_SIZE_X * ARRAY_SIZE_Y);
}

// Normalisoidaan arrayn arvot käyttäen keskiarvoa ja varianssia
void normalisaatio(float input_array[ARRAY_SIZE_X][ARRAY_SIZE_Y]) {
    if(debug_level == 0){
        for (int i = 0; i < ARRAY_SIZE_X; i++) {
            for (int j = 0; j < ARRAY_SIZE_Y; j++) {
                input_array[i][j] = (input_array[i][j] - const_mean) / const_sqrt_variance;
            }
        }
    }
    else if(debug_level > 0){
        // Lasketaan mean ja variancen neliöjuuri
        float mean = calculate_mean(input_array);
        float variance = calculate_variance(input_array, mean);
        float sqrt_variance = sqrt(variance);

        // Normalisoidaan jokainen luku arrayssa
        for (int i = 0; i < ARRAY_SIZE_X; i++) {
            for (int j = 0; j < ARRAY_SIZE_Y; j++) {
                input_array[i][j] = (input_array[i][j] - mean) / sqrt_variance;
            }
        }
    }

if (debug_level == 2){
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
