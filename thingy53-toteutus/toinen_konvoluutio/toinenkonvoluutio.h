// toinenkonvoluutio.h

#ifndef toinenkonvoluutio_H
#define toinenkonvoluutio_H

#define NUM_LAYERS 3 // Layereiden määrä
#define IMAGE_SIZE_Y 14 // Layereiden koko Y-akselilla
#define IMAGE_SIZE_X 14 // Layereiden koko Y-akselilla
#define KERNEL_SIZE 3 //Kernelien koko, esim 3 = 3x3
#define NUM_KERNELS 7 // Kernelien lukumäärä

void toinenkonvoluutio(float image[NUM_LAYERS][IMAGE_SIZE_Y][IMAGE_SIZE_X]);

#endif // toinenkonvoluutio_H
