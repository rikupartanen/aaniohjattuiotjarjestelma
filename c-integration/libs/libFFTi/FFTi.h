#ifndef  FFTI_H
#define FFTI_H

/*
Parameters for the FFT: 
window_size = 256 - values in one window
window_step = 128 - overlap per window
numberOfWindows = 124; - windows
*/

extern int window_size;
extern int window_step;

void FFTi(float *signal, float (*spectrogram)[129]); //pass float signal array of 16000 values and double float spectrogram[124][129] array to store the result

#endif
