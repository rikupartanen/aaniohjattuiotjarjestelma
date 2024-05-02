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
double spectrogram[124][256 / 2];

FFTr(signal, spectrogram);

//testing the output
printf("Position 0,0:  C code: %.15f, python code: 0.04090776\n", spectrogram[0][0]);
printf("Position 0,128:  C code: %.15f, python code: 0.00120234\n\n", spectrogram[0][127]);

printf("Position 124,0:  C code: %.15f, python code: 5.44809579e-01\n", spectrogram[123][0]);
printf("Position 124,128:  C code: %.15f, python code: 1.72394136e-03\n\n", spectrogram[123][127]);

    return 0; 
}
