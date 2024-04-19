#include <stdio.h>
#include <math.h>
#include "normalisaatio.h"

// Keskiarvon laskemisfunktio
float calculate_mean(float data[][32]) {
    float sum = 0;
    for (int i = 0; i < ARRAY_SIZE_X; i++) {
        for (int j = 0; j < ARRAY_SIZE_Y; j++) {
            sum += data[i][j];
        }
    }
    return sum / (ARRAY_SIZE_X * ARRAY_SIZE_Y);
}

// Varianssin laskemisfunktio
float calculate_variance(float data[][32], float mean) {
    float sum_of_variance = 0;
    for (int i = 0; i < ARRAY_SIZE_X; i++) {
        for (int j = 0; j < ARRAY_SIZE_Y; j++) {
            sum_of_variance += pow(data[i][j] - mean, 2);
        }
    }
    return sum_of_variance / (ARRAY_SIZE_X * ARRAY_SIZE_Y);
}

// Lasketaan arrayn arvot 0 ja 1 välille käyttäen keskiarvoa ja varianssia
void normalisaatio(float input_array[ARRAY_SIZE_X][ARRAY_SIZE_Y]) {
    // Lasketaan keskiarvo ja varianssi
    float mean = calculate_mean(input_array);
    float variance = calculate_variance(input_array, mean);
    float sqrt_variance = sqrt(variance);

    // Normalisoidaan jokainen luku arrayssa
    for (int i = 0; i < ARRAY_SIZE_X; i++) {
        for (int j = 0; j < ARRAY_SIZE_Y; j++) {
            input_array[i][j] = (input_array[i][j] - mean) / sqrt_variance;
        }
    }

    // Tulostetaan normalisoitu array
    printf("Mean: %.2f\n", mean);
    printf("Variance: %.2f\n", variance);
    printf("Sqrt_variance: %f\n", sqrt_variance);
    printf("Normalisoitu array:\n");
    for (int i = 0; i < ARRAY_SIZE_X; i++) {
        for (int j = 0; j < ARRAY_SIZE_Y; j++) {
            printf("%.2f ", input_array[i][j]);
        }
        printf("\n");
    }
}
