#include <stdio.h>

#define IMAGE_SIZE_Y 7
#define IMAGE_SIZE_X 7
//Kernelien koko, esim 3 = 3x3
#define KERNEL_SIZE 3
// Kernelien lukumäärä
#define NUM_KERNELS 6

// Konvoluutiofunktio
void toinenkonvoluutio(float image[IMAGE_SIZE_Y][IMAGE_SIZE_X], float kernel[NUM_KERNELS][KERNEL_SIZE][KERNEL_SIZE], float bias[NUM_KERNELS], float output[IMAGE_SIZE_Y - KERNEL_SIZE + 1][IMAGE_SIZE_X - KERNEL_SIZE + 1]) {
    int image_x, image_y, kernel_num, kernel_x, kernel_y;

    // Konvoluutioidaan jokaisen kernelin verran
    for (kernel_num = 0; kernel_num < NUM_KERNELS; kernel_num++) {
        // Liikutetaan kerneliä kuvan päällä
        for (image_y = 0; image_y <= IMAGE_SIZE_Y - KERNEL_SIZE; image_y++) {
            for (image_x = 0; image_x <= IMAGE_SIZE_X - KERNEL_SIZE; image_x++) {
                float sum = 0.0;

                // Konvoluutio
                for (kernel_x = 0; kernel_x < KERNEL_SIZE; kernel_x++) {
                    for (kernel_y = 0; kernel_y < KERNEL_SIZE; kernel_y++) {
                        // Lasketaan kernel kuvaan
                        sum += image[image_y + kernel_y][image_x + kernel_x] * kernel[kernel_num][kernel_y][kernel_x];
                    }
                }

                // Lisätään bias tulokseen
                output[image_y][image_x] = sum + bias[kernel_num];
            }
        }

        // Printataan jokaisen konvoluution tulos kernelien jälkeen
        printf("Output of convolution for kernel %d:\n", kernel_num + 1);
        for (image_y = 0; image_y <= IMAGE_SIZE_Y - KERNEL_SIZE; image_y++) {
            for (image_x = 0; image_x <= IMAGE_SIZE_X - KERNEL_SIZE; image_x++) {
                printf("%.2f\t", output[image_y][image_x]);
            }
            printf("\n");
        }
        printf("\n");
    }
}

int main() {

    // Esimerkkikuva
    float image[IMAGE_SIZE_Y][IMAGE_SIZE_X] = {
        {1, 2, 3, 4, 5, 6, 7},
        {8, 9, 10, 11, 12, 13, 14},
        {15, 16, 17, 18, 19, 20, 21},
        {22, 23, 24, 25, 26, 27, 28},
        {29, 30, 31, 32, 33, 34, 35},
        {36, 37, 38, 39, 40, 41, 42},
        {43, 44, 45, 46, 47, 48, 49}
    };

/*
    // Testausta erikokoisella kuvalla
    float image[IMAGE_SIZE_Y][IMAGE_SIZE_X] = {
        {1, 2, 3, 4, 5, 6, 7},
        {8, 9, 10, 11, 12, 13, 14},
        {15, 16, 17, 18, 19, 20, 21},
        {22, 23, 24, 25, 26, 27, 28},
        {29, 30, 31, 32, 33, 34, 35}
    };
*/

    // Esimerkkikernelit
    float kernels[NUM_KERNELS][KERNEL_SIZE][KERNEL_SIZE] = {
        {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}}, // Vertical sobel filter
        {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}}, // Horizontal sobel filter
        {{-1, 0, 1}, {-1, 0, 1}, {-1, 0, 1}}, // Vertical prewitt filter
        {{-1, -1, -1}, {0, 0, 0}, {1, 1, 1}}, // Horizontal prewitt filter
        {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}}, // Gaussian filter
        {{0, 1, 0}, {1, -4, 1}, {0, 1, 0}} // Laplacian filter
    };

    // Esimerkki bias
    float biases[NUM_KERNELS] = {0.05, 0.05, 0.05, 0.05, 0.01, 0.03};

    // Output array 
    float output[IMAGE_SIZE_Y - KERNEL_SIZE + 1][IMAGE_SIZE_X - KERNEL_SIZE + 1];

    // Alustetaan output-array nollilla
    for (int image_y = 0; image_y < IMAGE_SIZE_Y - KERNEL_SIZE + 1; image_y++) {
        for (int image_x = 0; image_x < IMAGE_SIZE_X - KERNEL_SIZE + 1; image_x++) {
            output[image_y][image_x] = 0.0;
        }
    }

    // Kutsutaan konvoluutio
    printf("Aloitetaan konvoluutio\n");
    toinenkonvoluutio(image, kernels, biases, output);

    return 0;
}