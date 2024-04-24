#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "resizing.h"

void resizing(float input_array[INPUT_SIZE_Y][INPUT_SIZE_X], float output_array[OUTPUT_SIZE_Y][OUTPUT_SIZE_X]) {
  // Skaalaa ensin x-suunta. Tehdään (in_x * in_y) taulukosta (out_x * in_y) kokoinen.
  float step = 1.0 * INPUT_SIZE_X / OUTPUT_SIZE_X;
  float start = ((INPUT_SIZE_X - 1) - (1.0 * step * (OUTPUT_SIZE_X - 1))) / 2.0;

  float** temp_array = malloc(INPUT_SIZE_Y * sizeof(float*));

  for(int row = 0; row < INPUT_SIZE_Y; row++) {
    temp_array[row] = malloc(OUTPUT_SIZE_X * sizeof(float*));
    for(int col = 0; col < OUTPUT_SIZE_X; col++) {
      float location = start + col * step;
      float low_proportion = (1 - (location - (int)location));

      temp_array[row][col] = low_proportion * input_array[row][(int)location];
      if(low_proportion != 1) { // Avoid crashing when OUTPUT_SIZE_X equals INPUT_SIZE_X
        temp_array[row][col] += (1-low_proportion) * input_array[row][(int)location + 1];
      }
      // printf("temp_array[%d][%d] = %.4f\n", row, col, temp_array[row][col]);
    }
  }

  // Skaalataan y-suunta. Tehdään (out_x * in_y) taulukosta (out_x * out_y) kokoinen.
  step = 1.0 * INPUT_SIZE_Y / OUTPUT_SIZE_Y;
  start = ((INPUT_SIZE_Y - 1) - (step * (OUTPUT_SIZE_Y - 1))) / 2.0;

  for(int col = 0; col < OUTPUT_SIZE_X; col++) {
    for(int row = 0; row < OUTPUT_SIZE_Y; row++){
      float location = start + row * step;
      float low_proportion = (1 - (location - (int)location));

      output_array[row][col] = low_proportion * temp_array[(int)location][col];
      if(low_proportion != 1) { // Avoid crashing when OUTPUT_SIZE_Y equals INPUT_SIZE_Y
        output_array[row][col] += (1-low_proportion) * temp_array[(int)location + 1][col];
      }
      // printf("output_array[%d][%d] = %.4f\n", row, col, output_array[row][col]);
    }
  }

  for(int row=0; row<INPUT_SIZE_Y; row++) {
    free(temp_array[row]);
  }
  free(temp_array);

  //printf("\n");
}

/*
gcc -o resizing.exe resizing.c resizing_test.c -I.
*/