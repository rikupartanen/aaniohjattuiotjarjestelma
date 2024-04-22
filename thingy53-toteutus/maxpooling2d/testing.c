#ifdef _WIN32
#include <conio.h>
#else
#include <stdio.h>
#define clrscr() printf("\e[1;1H\e[2J")
#endif
#include <stdlib.h>
#include <math.h>


#define BATCH_SIZE 1

void maxpooling2d(float* input_data, int batch_size, int input_height, int input_width, int num_channels,
                  int pool_height, int pool_width, int stride_height, int stride_width,
                  float* output_data) {

    int output_height = (input_height - pool_height) / stride_height + 1;
    int output_width = (input_width - pool_width) / stride_width + 1;

    for (int b = 0; b < batch_size; ++b) {
        for (int c = 0; c < num_channels; ++c) {
            for (int h = 0; h < output_height; ++h) {
                for (int w = 0; w < output_width; ++w) {
                    int h_start = h * stride_height;
                    int h_end = h_start + pool_height;
                    int w_start = w * stride_width;
                    int w_end = w_start + pool_width;

                    float max_val = -INFINITY;
                    for (int i = h_start; i < h_end; ++i) {
                        for (int j = w_start; j < w_end; ++j) {
                            float val = input_data[(b * input_height * input_width * num_channels) +
                                                   (i * input_width * num_channels) +
                                                   (j * num_channels) + c];
                            if (val > max_val) {
                                max_val = val;
                            }
                        }
                    }
                    output_data[(b * output_height * output_width * num_channels) +
                                (h * output_width * num_channels) +
                                (w * num_channels) + c] = max_val;
                }
            }
        }
    }
}




int main() {
    const int batch_size = 2;
    const int input_height = 4;
    const int input_width = 4;
    const int num_channels = 2;
    const int pool_height = 2;
    const int pool_width = 2;
    const int stride_height = 2;
    const int stride_width = 2;

    int printcounter = 0;

    int input_data_length = (batch_size * input_height * input_width * num_channels);

    float input_data[input_data_length];

    for(int i = 0;i < input_data_length;i++){
        input_data[i] = (float)i;
    }

    int output_data_length = (batch_size * ((input_height - pool_height) / stride_height + 1) *
                      ((input_width - pool_width) / stride_width + 1) * num_channels);

    float output_data[output_data_length];


    for(int i = 0;i < output_data_length;i++){
        output_data[i] = i;
    }

    clrscr();

    printf("Input: length:%d\n", input_data_length);
    for (int b = 0; b < batch_size; b++) {
        printf("Batch:%d\n",b);
        for (int c = 0; c < num_channels; c++) {
            printf("Channel:%d\n",c);
            for (int h = 0; h < input_height; h++) {
                for (int w = 0; w < input_width; w++) {
                    //printf("b%02d c%02d h%02d w%02d=%02.2f\t", b, c, h, w, input_data[(b*num_channels*input_height*input_width) + (c*input_height*input_width) + (h * input_width) + w]);
                    printf("%02.2f\t", input_data[(b*num_channels*input_height*input_width) + (c*input_height*input_width) + (h * input_width) + w]);
                }
                printf("\n");
            }
            printf("\n");
        }
    }


    printf("Output before max pooling: length:%d\n", output_data_length);
    printcounter = 0;
    for (int b = 0; b < batch_size; ++b) {
        printf("Batch:%d\n",b);
        for (int c = 0; c < num_channels; ++c) {
            printf("Channel:%d\n",c);
            for (int h = 0; h < ((input_height - pool_height) / stride_height + 1); ++h) {
                for (int w = 0; w < ((input_width - pool_width) / stride_width + 1); ++w) {
                    /*
                    printf("i%d j%d k%d l%d =%.2f ", i, j, k, l, output_data[(i * ((input_height - pool_height) / stride_height + 1) *
                                                 ((input_width - pool_width) / stride_width + 1) * num_channels) +
                                                (j * ((input_width - pool_width) / stride_width + 1) * num_channels) +
                                                (k * num_channels) + l]);
                    */
                    printf("%.2f\t",output_data[printcounter]);
                    printcounter++;
                }
                printf("\n");
            }
            printf("\n");
        }
    }


    // Perform maxpooling
    maxpooling2d(input_data, batch_size, input_height, input_width, num_channels,
                 pool_height, pool_width, stride_height, stride_width,
                 output_data);


    printf("Output after max pooling: length:%d\n", output_data_length);
    printcounter = 0;
    for (int b = 0; b < batch_size; ++b) {
        printf("Batch:%d\n",b);
        for (int c = 0; c < num_channels; ++c) {
            printf("Channel:%d\n",c);
            for (int h = 0; h < ((input_height - pool_height) / stride_height + 1); ++h) {
                for (int w = 0; w < ((input_width - pool_width) / stride_width + 1); ++w) {
                    /*
                    printf("i%d j%d k%d l%d =%.2f ", i, j, k, l, output_data[(i * ((input_height - pool_height) / stride_height + 1) *
                                                 ((input_width - pool_width) / stride_width + 1) * num_channels) +
                                                (j * ((input_width - pool_width) / stride_width + 1) * num_channels) +
                                                (k * num_channels) + l]);
                    */
                    printf("%.2f\t",output_data[printcounter]);
                    printcounter++;
                }
                printf("\n");
            }
            printf("\n");
        }
    }



    printf("Output after max pooling (flat):\n");
    for (int i = 0; i < output_data_length; ++i) {
        printf("%02.2f ", output_data[i]);
    }


    return 0;
}
