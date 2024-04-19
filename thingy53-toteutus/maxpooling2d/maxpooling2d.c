#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define BATCH_SIZE 1

void maxpooling2d(float* input_data, int batch_size, int input_height, int input_width, int num_channels,
                  int pool_height, int pool_width, int stride_height, int stride_width,
                  float* output_data) {

    int output_height = (input_height - pool_height) / stride_height + 1;
    int output_width = (input_width - pool_width) / stride_width + 1;

    for (int b = 0; b < batch_size; ++b) {
        for (int h = 0; h < output_height; ++h) {
            for (int w = 0; w < output_width; ++w) {
                for (int c = 0; c < num_channels; ++c) {
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
    const int batch_size = 1;
    const int input_height = 14;
    const int input_width = 14;
    const int num_channels = 1;
    const int pool_height = 2;
    const int pool_width = 2;
    const int stride_height = 2;
    const int stride_width = 2;

    float input_data[batch_size * input_height * input_width * num_channels];
    float output_data[batch_size * ((input_height - pool_height) / stride_height + 1) *
                      ((input_width - pool_width) / stride_width + 1) * num_channels];

    // Fill input_data array here...
    int arr[] = {  };

    // Perform maxpooling
    maxpooling2d(input_data, batch_size, input_height, input_width, num_channels,
                 pool_height, pool_width, stride_height, stride_width,
                 output_data);

    // Print output data
    printf("\nOutput after max pooling:\n");
    for (int i = 0; i < batch_size; ++i) {
        for (int j = 0; j < ((input_height - pool_height) / stride_height + 1); ++j) {
            for (int k = 0; k < ((input_width - pool_width) / stride_width + 1); ++k) {
                for (int l = 0; l < num_channels; ++l) {
                    printf("%.2f ", output_data[(i * ((input_height - pool_height) / stride_height + 1) *
                                                 ((input_width - pool_width) / stride_width + 1) * num_channels) +
                                                (j * ((input_width - pool_width) / stride_width + 1) * num_channels) +
                                                (k * num_channels) + l]);
                }
                printf("\n");
            }
        }
    }

    return 0;
}
