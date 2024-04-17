#include <math.h>     //provides cos(), sin() and cabs(), log2() functions, used in the algorithm
#include <complex.h> //provides complex calculations
#include <stdio.h> 
#include "FFTi.h"

//Parameters for the FFT: 
int window_size = 256; //how much values in one window
int window_step = 128; //how much overlap per window 
int numberOfWindows = 124; //how much windows 

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
void hanningFunction(int window_size, double (*windows)[window_size]){
  for (int i = 0; i < numberOfWindows; i++) {
   for (int j = 0; j < window_size; j++){
     windows[i][j] *= 0.5 - 0.5 * cos(2.0 * M_PI * ((float)j / (float)window_size));
   }
 }
}




//Part of FFT, reverses bits to shuffle original array for iterarive FFT
unsigned short int reversebits(int arraySizePos){
  unsigned short int x = 0; 
  for(int i = 0; i < 8; i++){
   x <<= 1; 
   x |= (1 & arraySizePos); 
   arraySizePos >>= 1; 
  }
          
  return x; 
}



//Iterative FFT function
void FFT(complex double *window, int arraySize){
     
  //bit-reversal permutation
  for(int i = 1; i < arraySize; i++){
     short int bitrev = reversebits(i); 
     if(i > bitrev){   //ensure that it doesn't dublicate
        complex double temp = window[i];  //then swap numbers
        window[i] = window[bitrev];
        window[bitrev] = temp;

     }
  }
     
    for(int length = 2; length <= arraySize; length *= 2){ //FFT stages, 2x 
      for(int prediv = 0; prediv < arraySize; prediv += length){//Segments
        for(int oddev = 0; oddev < length / 2; oddev++){ //Butterfly calculation
          
          complex double twiddle_factor = cos(2.0 * M_PI * (double)oddev / (double)length) - I * sin(2.0 * M_PI * (double)oddev / (double)length);  //Twiddle factor for the complex part
          
          complex double w1 = window[prediv + oddev];  //temp values for butterfly calculations
          complex double w2 = window[prediv + oddev + length / 2];

          window[oddev + prediv] = w1 + twiddle_factor * w2;
          window[oddev + prediv + length / 2] = w1 - twiddle_factor * w2;

        } 
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




void FFTi(float *signal, double (*spectrogram)[window_size / 2]){

double windows[numberOfWindows][window_size]; //should be 124 windows, and 256 values in each window array, it holds the windowed data

windowingFunction(signal, windows); //splits the signal data into windows 
hanningFunction(window_size, windows); //reduces spectral leakage from windowing
spectrogramFunction(windows, spectrogram); //calls the FFT function and gets the spectrogram later
}
