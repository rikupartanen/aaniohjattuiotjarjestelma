// toinenkonvoluutio.h

#ifndef toinenkonvoluutio_H
#define toinenkonvoluutio_H

#define IMAGE_SIZE 7
#define KERNEL_SIZE 3
#define NUM_KERNELS 3

void toinenkonvoluutio(float image[2][IMAGE_SIZE][IMAGE_SIZE], float kernels[NUM_KERNELS][2][KERNEL_SIZE][KERNEL_SIZE], float biases[NUM_KERNELS], float output[IMAGE_SIZE - KERNEL_SIZE + 1][IMAGE_SIZE - KERNEL_SIZE + 1]);

#endif // toinenkonvoluutio_H