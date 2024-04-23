#ifndef WEIGHTS_MAP_H__
#define WEIGHTS_MAP_H__
#include <stddef.h>

#define FLASH_BASE 0x10000000

const float *conv2d_kweights = (float*)(FLASH_BASE + 0);
const float *conv2d_bweights = NULL;
const size_t conv2d_kshape[] = {  1, 3, 3, 0  };
const size_t conv2d_bshape[] = {  0  };

#define conv2d_1_kweights (float*)(FLASH_BASE + 0x24)
#define conv2d_1_bweights (float*)(FLASH_BASE + 0x48)
const size_t conv2d_1_kshape[] = {  1, 3, 3, 0  };
const size_t conv2d_1_bshape[] = {  1, 0  };

const float *dense_kweights = (float*)(FLASH_BASE + 0x4c);
const float *dense_bweights = NULL;
const size_t dense_kshape[] = {  4, 4, 0  };
const size_t dense_bshape[] = {  0  };

#endif /* WEIGHTS_MAP_H__ */