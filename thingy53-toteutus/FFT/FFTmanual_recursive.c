#include <math.h>     //provides cos(), sin() and cabs() functions, used in the algorithm
#include <complex.h> //provides complex calculations
#include <stdio.h> 
#include <stdlib.h> //malloc

float signal[16000]; 
int window_size = 256; //parameters for the FFT-algorithm
int window_step = 128; 
int numberOfWindows = 0; // will be recalculated in the main


void windowingFunction(float *signal, double (*windows)[window_size]);
void hammingFunction(int window_size, double (*windows)[window_size]);
void spectrogramFunction(double (*windows)[window_size], double (*spectrogram)[window_size]);
void FFT(complex double *window, int arraySize); //it takes only a single window array from windows arrays, have to specify the size because of the decay
void fft2(complex double *x , int L);



int main(){
 //Load sample data from the file to process and check, saves it so "signal" float array
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

numberOfWindows = (sizeof(signal) / sizeof(signal[0])-window_size) / window_step + 1; //should be 124 in this case
if(numberOfWindows == 124 && window_size % 2 == 0){
  //printf("Correct number of windows and correct windows size, continuing...\n");

}
else if(numberOfWindows != 124){
 printf("Incorrect number of windows, shutting down...\n"); 
 return 0; 
}
else if(!(window_size % 2)){
 printf("Incorrect window size,  shutting down...\n"); 
 return 0; 
}

//printf("numberOfWindows: %d \n", numberOfWindows);

double windows[numberOfWindows][window_size]; //should be 124 windows, and 256 values in each window array
windowingFunction(signal, windows); //splits the signal data into windows 
hammingFunction(window_size, windows); //reduces spectral leakage from windowing
double spectrogram[numberOfWindows][window_size / 2]; // contains the final result after the spectorgramFunction call 
spectrogramFunction(windows, spectrogram); //calls the FFT function and gets the spectrogram later




return 0; 
}


//function to split data into windows, in this case signal float array should have 16000 values from audio sample
void windowingFunction(float *signal, double (*windows)[window_size]){
  for(int i = 0; i < numberOfWindows; i++){
    int start = i * window_step; 
    int end = start + window_size;   

    double window[window_size]; 
    
    for (int j = start; j < end; j++){
       int x = window_size - (end - j);
       window[x] = signal[j];             //split signal data into 1 window
    }
    
    for (int j = 0; j < window_size; j++) {         //populate windows array of arrays with window arrays
    windows[i][j] = window[j];
  }
  }
}

//function for the Hamming window, to split data
void hammingFunction(int window_size, double (*windows)[window_size]){
  for (int i = 0; i < numberOfWindows; i++) {
   for (int j = 0; j < window_size; j++){
     windows[i][j] *= 0.54 - 0.46 * cos(2.0 * M_PI * ((float)j / (float)window_size));
   }
 }
}

//function to go through FFT and calculate the spectrogram
void spectrogramFunction(double (*windows)[window_size], double (*spectrogram)[window_size / 2]){
  complex double window[window_size]; //buffer to store values from windows as complex, needed to perfrom calculations correctly
  complex double fftrest[numberOfWindows][window_size]; // array containing the complex result of FFT

  //saves data from windows double array to window complex double array and get the FFT result
  for (int i = 0; i < numberOfWindows; i++){
    for(int j = 0; j < window_size; j++){
      window[j] =(complex double)windows[i][j]; //saves windows to complex array
    }

     FFT(window, window_size); 
   
    for(int j = 0; j < window_size; j++){
     fftrest[i][j] = window[j];
    }
  }
  
  //Calculates the absolute value to get the spectrogram for half of the values(in this case 128) 
  
  for(int i = 0; i < numberOfWindows; i++){
    for(int j = 0; j < window_size / 2; j++){
     spectrogram[i][j] = cabs(fftrest[i][j]); 
    }
  }
}

//Cooley-Tukey radix 2 FFT algorithm 
void FFT(complex double *window, int arraySize){
  if (arraySize <= 1){
      return;
  }
  complex double even[arraySize / 2];
  complex double odd[arraySize / 2];

 for(int i = 0; i < arraySize; i++){ // split into even and odd positions of the windows
    if(i % 2 == 0){
      even[i / 2] = window[i];
          }
    else{
      odd[i / 2] = window[i];
          }
      }
    
  FFT(even, arraySize / 2);
  FFT(odd,  arraySize / 2); 
  
  
  for (int i = 0; i < (arraySize / 2); i++){
   
   complex double twiddle_factor = cos(2.0 * M_PI * (double)i / (double)arraySize) - I * sin(2.0 * M_PI * (double)i / (double)arraySize); // calculates the twiddle factor
   window[i] = even[i] + twiddle_factor * odd[i];   //perform butterfly calculations to combine results
   window[i + arraySize / 2] = even[i] - twiddle_factor * odd[i]; 
  }
  
}
