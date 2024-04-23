#ifndef  FFTR_H
#define FFTR_H

extern int window_size;
extern int window_step; 

void FFTr(float *signal, float (*spectrogram)[window_size / 2 + 1]);

#endif