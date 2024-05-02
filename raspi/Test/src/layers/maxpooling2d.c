#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG 0  // debug output

void maxpooling2d(
    float *input_data,
    int batch_size,
    int num_channels,
    int input_height,
    int input_width,
    int pool_height,
    int pool_width,
    int stride_height,
    int stride_width,
    float *output_data
) {
  int output_height = (input_height - pool_height) / stride_height + 1;
  int output_width = (input_width - pool_width) / stride_width + 1;

  if (DEBUG > 0) printf("output_height %d\n", output_height);
  if (DEBUG > 0) printf("output_width %d\n", output_width);

  int startindex = 0;
  int indexnumber = 0;
  int poolindex = 0;
  int outputindex = 0;
  for (int b = 0; b < batch_size; b++) {
    for (int c = 0; c < num_channels; c++) {
      for (int h = 0; h < input_height; h = h + stride_height) {
        for (int w = 0; w < input_width; w = w + stride_width) {
          // int h_start = h * stride_height;
          // int h_end = h_start + pool_height;
          // int w_start = w * stride_width;
          // int w_end = w_start + pool_width;
          float max_val = -INFINITY;
          startindex = (b * num_channels * input_height * input_width)
                     + (c * input_height * input_width)
                     + (h * input_width) + w;
          // printf("h_start%d h_end%d w_start%d w_end%d found:   ", h_start, h_end, w_start,  w_end);
          for (int i = 0; i < pool_height; i++) {
            for (int j = 0; j < pool_width; j++) {
              poolindex = i * input_width + j;
              indexnumber = startindex + poolindex;

              float val = input_data[indexnumber];
              if (DEBUG > 1) printf("b%d c%d h%d w%d i%d j%d si%02d pi%02d val%02.0f   ", b, c, h, w, i, j, startindex, poolindex, val);
              // printf("%02.0f ", val);
              if (val > max_val) {
                max_val = val;
              }
            }
          }
          if (DEBUG > 1) printf("max_val=%05.2f\n", max_val);
          output_data[outputindex] = max_val;
          outputindex++;
        }
      }
    }
  }
}
