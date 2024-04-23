#ifndef  FFTR_H
#define FFTR_H

/*
Parameters for the FFT: 
window_size = 256 - values in one window
window_step = 128 - overlap per window
numberOfWindows = 124; - windows
*/

extern int window_size;
extern int window_step; 

void FFTr(float *signal, double (*spectrogram)[window_size / 2 + 1]); //pass float signal array of 16000 values and double float spectrogram array to store the result

#endif
