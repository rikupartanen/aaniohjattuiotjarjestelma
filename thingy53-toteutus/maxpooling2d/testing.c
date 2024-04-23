#ifdef _WIN32
    #include <conio.h>
#else
    #include <stdio.h>
    #define clrscr() printf("\e[1;1H\e[2J")
#endif
#include <stdlib.h>
#include <math.h>

#define DEBUG 0  // debug output 

void maxpooling2d(  float* input_data, 
                    int batch_size, int num_channels, 
                    int input_height, int input_width, 
                    int pool_height, int pool_width, 
                    int stride_height, int stride_width,
                    float* output_data) {

    int output_height = (input_height - pool_height) / stride_height + 1;
    int output_width = (input_width - pool_width) / stride_width + 1;

    if (DEBUG>0) printf("output_height %d\n", output_height);
    if (DEBUG>0) printf("output_width %d\n", output_width);

    int startindex = 0;
    int indexnumber = 0;
    int poolindex = 0;
    int outputindex = 0;
    for (int b = 0; b < batch_size; b++) {
        for (int c = 0; c < num_channels; c++) {
            for (int h = 0; h < input_height; h = h + stride_height) {
                for (int w = 0; w < input_width; w = w + stride_width) {
                    //int h_start = h * stride_height;
                    //int h_end = h_start + pool_height;
                    //int w_start = w * stride_width;
                    //int w_end = w_start + pool_width;
                    float max_val = -INFINITY;
                    startindex =    (b * num_channels * input_height * input_width ) +
                                    (c * input_height * input_width) +
                                    (h * input_width) + w;
                    //printf("h_start%d h_end%d w_start%d w_end%d found:   ", h_start, h_end, w_start,  w_end);
                    for (int i = 0; i < pool_height; i++) {
                        for (int j = 0; j < pool_width; j++) {
                            poolindex =     i * input_width + j;
                            indexnumber =   startindex + poolindex;
                            
                            float val = input_data[indexnumber];
                            if (DEBUG>1) printf("b%d c%d h%d w%d i%d j%d si%02d pi%02d val%02.0f   ", b, c, h, w, i, j, startindex, poolindex, val);
                            //printf("%02.0f ", val);
                            if (val > max_val) {
                                max_val = val;
                            }
                        }
                    }
                    if (DEBUG>1) printf("max_val=%05.2f\n", max_val);
                    output_data[outputindex] = max_val;
                    outputindex++;
                }
            }
        }
    }
}




int main() {
    const int batch_size = 2;
    const int num_channels = 2;
    const int input_height = 6;
    const int input_width = 6;
    const int pool_height = 2;
    const int pool_width = 2;
    const int stride_height = 2;
    const int stride_width = 2;

    int printcounter = 0;

    int input_data_length = (batch_size * num_channels * input_height * input_width);

    float input_data[input_data_length];

    for(int i = 0;i < input_data_length;i++){
        input_data[i] = (float)i;
    }

    int output_data_length = (batch_size * num_channels * 
                            ((input_height - pool_height) / stride_height + 1) *
                            ((input_width - pool_width) / stride_width + 1));

    float output_data[output_data_length];


    for(int i = 0;i < output_data_length;i++){
        output_data[i] = i/100.;
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

/*
    printf("Output before max pooling: length:%d\n", output_data_length);
    printcounter = 0;
    for (int b = 0; b < batch_size; ++b) {
        printf("Batch:%d\n",b);
        for (int c = 0; c < num_channels; ++c) {
            printf("Channel:%d\n",c);
            for (int h = 0; h < ((input_height - pool_height) / stride_height + 1); ++h) {
                for (int w = 0; w < ((input_width - pool_width) / stride_width + 1); ++w) {
                    // printf("i%d j%d k%d l%d =%.2f ", i, j, k, l, output_data[(i * ((input_height - pool_height) / stride_height + 1) *
                    //                              ((input_width - pool_width) / stride_width + 1) * num_channels) +
                    //                             (j * ((input_width - pool_width) / stride_width + 1) * num_channels) +
                    //                             (k * num_channels) + l]);
                    //
                    printf("%.2f\t",output_data[printcounter]);
                    printcounter++;
                }
                printf("\n");
            }
            printf("\n");
        }
    }
*/

    // Perform maxpooling
    maxpooling2d(   input_data, 
                    batch_size,  num_channels,
                    input_height, input_width,
                    pool_height, pool_width,
                    stride_height, stride_width,
                    output_data);

/*
    printf("Output after max pooling: length:%d\n", output_data_length);
    printcounter = 0;
    for (int b = 0; b < batch_size; ++b) {
        printf("Batch:%d\n",b);
        for (int c = 0; c < num_channels; ++c) {
            printf("Channel:%d\n",c);
            for (int h = 0; h < ((input_height - pool_height) / stride_height + 1); ++h) {
                for (int w = 0; w < ((input_width - pool_width) / stride_width + 1); ++w) {
                    // printf("i%d j%d k%d l%d =%.2f ", i, j, k, l, output_data[(i * ((input_height - pool_height) / stride_height + 1) *
                    //                              ((input_width - pool_width) / stride_width + 1) * num_channels) +
                    //                             (j * ((input_width - pool_width) / stride_width + 1) * num_channels) +
                    //                             (k * num_channels) + l]);
                    printf("%.2f\t",output_data[printcounter]);
                    printcounter++;
                }
                printf("\n");
            }
            printf("\n");
        }
    }
*/

    printf("After max pooling (flat):\n");
    printf("  Output: ");
    for (int i = 0; i < output_data_length; ++i) {
        printf("%05.2f ", output_data[i]);
    }
    
    //printf("\nExpected: 05.00 07.00 13.00 15.00 21.00 23.00 29.00 31.00 37.00 39.00 45.00 47.00 53.00 55.00 61.00 63.00");

    return 0;
}
