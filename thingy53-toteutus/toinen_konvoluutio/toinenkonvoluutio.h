// toinenkonvoluutio.h

#ifndef toinenkonvoluutio_H
#define toinenkonvoluutio_H

#define IMAGE_SIZE_Y 14
#define IMAGE_SIZE_X 14
#define KERNEL_SIZE 3 //Kernelien koko, esim 3 = 3x3
#define NUM_KERNELS 7 // Kernelien lukumäärä
#define NUM_LAYERS 3 // Layereiden määrä
#define OUTPUT_LAYERS 21 // kernelien määrä * input layerien määrä

void toinenkonvoluutio(float image[NUM_LAYERS][IMAGE_SIZE_Y][IMAGE_SIZE_X], float kernels[NUM_KERNELS][KERNEL_SIZE][KERNEL_SIZE], float bias[NUM_KERNELS]);

#endif // toinenkonvoluutio_H
