// toinenkonvoluutio.h

#ifndef toinenkonvoluutio_H
#define toinenkonvoluutio_H

#define NUM_LAYERS 1    // Layereiden määrä
#define IMAGE_SIZE_Y 4  // Layereiden koko Y-akselilla
#define IMAGE_SIZE_X 4  // Layereiden koko Y-akselilla
#define KERNEL_SIZE 3   // Kernelien koko, esim 3 = 3x3
#define NUM_KERNELS 1   // Kernelien lukumäärä

#define OUT_IMAGES ((NUM_KERNELS / 2) ? (NUM_KERNELS / 2) : 1)
#define OUT_SIZE_Y (IMAGE_SIZE_Y - KERNEL_SIZE + 1)
#define OUT_SIZE_X (IMAGE_SIZE_X - KERNEL_SIZE + 1)

void toinenkonvoluutio(
    float image[NUM_LAYERS][IMAGE_SIZE_Y][IMAGE_SIZE_X],
    float out[OUT_IMAGES][OUT_SIZE_Y][OUT_SIZE_X]
);

#endif  // toinenkonvoluutio_H
