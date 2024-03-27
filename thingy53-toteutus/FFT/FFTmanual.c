#include <math.h>     //provides cos() and sin() functions
#include <complex.h> //provides complex calculations
#include <stdio.h> //for printing


float signal[16000];
int window_size = 256; //parameters for the FFT-algorithm
int window_step = 128; 


float windowingFunction(float signal[], float windows[][window_size]);

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
            return 1;
        }
 }
  
fclose(raw);

int numberOfWindows = (sizeof(signal) / sizeof(signal[0])-window_size) / window_step + 1; //should be 128
printf("numberOfWindows: %d \n", numberOfWindows);

float windows[numberOfWindows][window_size] = {}; //should be about 255 values and 124 windows

windowingFunction(signal, windows);

printf("Test of returning the value: %.15f", windows[0][255]);

return 0; 
}

//function to split data into windows
float windowingFunction(float signal[], float windows[][window_size]){
  for(int i = 0; i < 124; i++){
    int start = i * window_step; 
    int end = start + window_size;   
    //printf("The start value is: %d, the iteration value is:%d\n\n", start, i); 
    //printf("The end value is: %d, the iteration value is:%d\n\n", end, i); 

    float window[window_size]; 
    
    for (int j = start; j < end; j++){
       int x = 256 - (end - j);
       window[x] = signal[j];             //split signal data into 1 window 
    }
    
    for (int j = 0; j < window_size; j++) {         //populate windows with window
    windows[i][j] = window[j];
    //printf("The 'windows' value is: %.10f, current window value: %.10f, the j val is: %d \n", windows[i][j],window[j], j);
    
  }
  }
  return windows[123][window_size]; 
}

//function for the Hamming window
float HammingFunction(int length){
 float hammingWindow[length];
 for(int i = 0; i < length; i++){
  hammingWindow[i] = 0.54 - 0.46 * cos(2 * M_PI * (i / length) );
 }
 return *hammingWindow;
}

//FFT function developed via the Cooley-Tukey algorithm
int FFT(){
    
 
 return 0; 
}

