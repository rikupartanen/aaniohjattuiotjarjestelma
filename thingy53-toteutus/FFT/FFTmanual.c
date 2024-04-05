#include <math.h>     //provides cos() and sin() functions
#include <complex.h> //provides complex calculations
#include <stdio.h> //for printing


float signal[16000]; 
int window_size = 256; //parameters for the FFT-algorithm
int window_step = 128; 
int numberOfWindows = 0; 


void windowingFunction(float *signal, float (*windows)[window_size]);
void hammingFunction(int window_size, float (*windows)[window_size]);
void spectrogramFunction(float (*windows)[window_size]);
complex FFT(float *window, int arraySize); //it takes only a single window array from windows arrays, have to specify the size because of the decay

int main(){
 //Load sample data from the file to process and check
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

numberOfWindows = (sizeof(signal) / sizeof(signal[0])-window_size) / window_step + 1; //should be 128

//printf("numberOfWindows: %d \n", numberOfWindows);

float windows[numberOfWindows][window_size]; //should be about 124 windows(123 from 0), and 256 values(255 from 0)

windowingFunction(signal, windows); //splits the signal data into windows 
//printf("Windoweed: %.18f\n", windows[123][1]);
hammingFunction(window_size, windows); //reduces spectral leakage from windowing
//printf("After Hamming window: %.18f\n", windows[123][1]);

spectrogramFunction(windows);



return 0; 
}


//function to split data into windows
void windowingFunction(float *signal, float (*windows)[window_size]){
  for(int i = 0; i < numberOfWindows; i++){
    int start = i * window_step; 
    int end = start + window_size;   

    float window[window_size]; 
    
    for (int j = start; j < end; j++){
       int x = window_size - (end - j);
       window[x] = signal[j];             //split signal data into 1 window 
    }
    
    for (int j = 0; j < window_size; j++) {         //populate windows with window
    windows[i][j] = window[j];
  }
  }
}

//function for the Hamming window
void hammingFunction(int window_size, float (*windows)[window_size]){
  for (int i = 0; i < numberOfWindows; i++) {
   for (int j = 0; j < window_size; j++){
     windows[i][j] *= 0.54 - 0.46 * cos(2.0 * M_PI * ((float)j / (float)window_size));
   }
 }
}

//function to go through FFT and calculate a spectrogram
void spectrogramFunction(float (*windows)[window_size]){
  float FFT_result[numberOfWindows][window_size];
  for (int i = 0; i < numberOfWindows; i++){
      FFT(windows[i], window_size);
  }
   
}

//FFT function developed via the Cooley-Tukey algorithm
complex FFT(float *window, int arraySize){
  if (arraySize <= 1){
     //printf("Got to the recursive loop...\n");
     return 0;
  }
  float even[arraySize/2];
  float odd[arraySize/2];
  
  for(int i = 0; i < arraySize; i++){ // split into even and odd positions of the windows
    if(i % 2 == 0){
      even[i / 2] = window[i];
    }
    else{
      odd[i / 2] = window[i];
    }
  }

  //printf("The array size is: %d\n", arraySize);
  
  FFT(even, sizeof(even) / sizeof(even[0]));
  FFT(odd, sizeof(odd) / sizeof(odd[0]));
 double abs_val; 
  
  complex merged[arraySize];
  for (int i = 0; i < arraySize / 2; i++){
   complex twiddle_factor = cos(2.0 * M_PI * (float)i / (float)arraySize) - 1I * sin(2.0 * M_PI * (float)i / (float)arraySize);
   merged[i] = even[i] + twiddle_factor * odd[i];
   merged[i + arraySize / 2] = even[i] - twiddle_factor * odd[i];
  // abs_val = cabs(merged[i]);
   //printf("The abs val sample is: %0.15f\n", abs_val);
   printf("twiddle factor is: %.30lf %.2lfi, the i is %d, and array size is %d \n", creal(twiddle_factor), cimag(twiddle_factor), i, arraySize);
   
  }
  
  //printf("merged: %.15f\n", merged[0]);

  return 0; //placeholder
}
