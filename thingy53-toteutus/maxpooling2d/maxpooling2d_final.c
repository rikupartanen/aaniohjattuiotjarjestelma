#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define clrscr() printf("\e[1;1H\e[2J")  // make debug output nicer to read
#define DEBUG 0  // debug output level 0 - 2

// input_data as pointer to float array, output_data as pointer to float array
void maxpooling2d(  float* input_data, 
                    int batch_size, int num_channels, 
                    int input_height, int input_width, 
                    int pool_height, int pool_width, 
                    int stride_height, int stride_width,
                    float* output_data) {
    int startindex = 0;   // position of pooling operation from input dimensions
    int poolindex = 0;    // position of pooling operation from pool and stride dimensions
    int indexnumber = 0;  // startindex + poolindex
    int outputindex = 0;  // position of output array
    for (int b = 0; b < batch_size; b++) {  
        for (int c = 0; c < num_channels; c++) {
            for (int h = 0; h < input_height; h = h + stride_height) {   // iterate over input array by stride
                for (int w = 0; w < input_width; w = w + stride_width) {
                    float max_val = -INFINITY;
                    startindex =    (b * num_channels * input_height * input_width ) +  // set the base position for pooling
                                    (c * input_height * input_width) +
                                    (h * input_width) + w;
                    for (int i = 0; i < pool_height; i++) {   // iterate over pool
                        for (int j = 0; j < pool_width; j++) {
                            poolindex =     i * input_width + j;
                            indexnumber =   startindex + poolindex;
                            float val = input_data[indexnumber];
                            if (DEBUG>=2) printf("b%d c%d h%d w%d i%d j%d si%02d pi%02d val%02.0f   ", b, c, h, w, i, j, startindex, poolindex, val);
                            if (val > max_val) {  // find the largest value
                                max_val = val;  
                            }
                        }
                    }
                    if (DEBUG>=2) printf("max_val=%05.2f\n", max_val);
                    output_data[outputindex] = max_val;  // write found maximum to output array
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
    const int pool_height = 3;
    const int pool_width = 3;
    const int stride_height = pool_height;  // typically stride size == pool size
    const int stride_width = pool_width;
    int printcounter = 0;
    int input_data_length = batch_size * num_channels * input_height * input_width;
    float input_data[input_data_length];

    for(int i = 0;i < input_data_length;i++){  // populate input array
        input_data[i] = (float)i;
    }

    int output_data_length = (batch_size * num_channels * 
                            ((input_height - pool_height) / stride_height + 1) *
                            ((input_width - pool_width) / stride_width + 1));

    float output_data[output_data_length];  // create output array of proper size

    for(int i = 0;i < output_data_length;i++){  // populate output array with low values
        output_data[i] = i/100.;                // so we can easily spot missed writes
    }                                           // during testing

    clrscr();  // clear output terminal

    printf("Input: length:%d\n", input_data_length);  // print the input array
    for (int b = 0; b < batch_size; b++) {
        printf("Batch:%d\n",b);
        for (int c = 0; c < num_channels; c++) {
            printf("Channel:%d\n",c);
            for (int h = 0; h < input_height; h++) {
                for (int w = 0; w < input_width; w++) {
                    if (DEBUG>=1) printf("b%02d c%02d h%02d w%02d=%02.2f\t", b, c, h, w, input_data[(b*num_channels*input_height*input_width) + (c*input_height*input_width) + (h * input_width) + w]);
                    if (DEBUG==0) printf("%02.2f\t", input_data[(b*num_channels*input_height*input_width) + (c*input_height*input_width) + (h * input_width) + w]);
                }
                printf("\n");
            }
            printf("\n");
        }
    }

/*    // for testing
    printf("Output before max pooling: length:%d\n", output_data_length);
    printcounter = 0;
    for (int b = 0; b < batch_size; ++b) {
        printf("Batch:%d\n",b);
        for (int c = 0; c < num_channels; ++c) {
            printf("Channel:%d\n",c);
            for (int h = 0; h < ((input_height - pool_height) / stride_height + 1); ++h) {
                for (int w = 0; w < ((input_width - pool_width) / stride_width + 1); ++w) {
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

    // print output array
    printf("Output after max pooling: length:%d\n", output_data_length);
    printcounter = 0;
    for (int b = 0; b < batch_size; ++b) {
        printf("Batch:%d\n",b);
        for (int c = 0; c < num_channels; ++c) {
            printf("Channel:%d\n",c);
            for (int h = 0; h < ((input_height - pool_height) / stride_height + 1); ++h) {
                for (int w = 0; w < ((input_width - pool_width) / stride_width + 1); ++w) {
                    printf("%.2f\t",output_data[printcounter]);
                    printcounter++;
                }
                printf("\n");
            }
            printf("\n");
        }
    }


    printf("After max pooling (flat):\n");
    printf("  Output: ");
    for (int i = 0; i < output_data_length; ++i) {
        printf("%05.2f ", output_data[i]);
    }

    // for 2 x 2 x 4 x 4 input with 2 x 2 pool     
    //printf("\nExpected: 05.00 07.00 13.00 15.00 21.00 23.00 29.00 31.00 37.00 39.00 45.00 47.00 53.00 55.00 61.00 63.00");

    return 0;
}
