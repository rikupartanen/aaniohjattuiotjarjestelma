#include <stdio.h>
#include "FFTr.h"

float signal[16000]; //load the data from the mic here, 1 track  - float values samples. 


int main(){
//sample data from .txt file:
FILE *raw = fopen("preprocessed_track.txt", "r");
 if (!raw) {
  printf("Failed to open the file...\n");
  return 0; 
 }
 else if (raw){
  printf("File opened...\n");
 }
 int lines = 16000;
 for (int i = 0; i < lines; i++) {
        if (fscanf(raw, "%f", &signal[i]) != 1) {
            printf("Error reading value at line %d.\n", i + 1);
            printf("The latest readout value is: %f\n", signal[i]);
            fclose(raw);
            return 0;
        }
 }
  
fclose(raw);


//array to store the spectrogram data, the output will contain 124 arrays filled with 256/2 values. 
float spectrogram[124][256 / 2 + 1];

FFTr(signal, spectrogram);

//testing the output
printf("Position 0,0:  C code: %.15f, python code: 0.04093553\n", spectrogram[0][0]);
printf("Position 0,129:  C code: %.15f, python code: 0.00014467\n\n", spectrogram[0][128]);

printf("Position 124,0:  C code: %.15f, python code: 6.74616284e-01\n", spectrogram[123][0]);
printf("Position 124,129:  C code: %.15f, python code: 1.62829080e-04\n\n", spectrogram[123][128]);

    return 0; 
}
