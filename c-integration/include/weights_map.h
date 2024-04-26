#ifndef REALWEIGHTS_H__
#define REALWEIGHTS_H__
#include <stddef.h>
#define FLASH_BASE 0x10000000

extern const float normalization_kweights[];
extern const size_t normalization_kshape[];

extern const float conv2d_kweights[];
extern const float conv2d_bweights[];
extern const size_t conv2d_kshape[];
extern const size_t conv2d_bshape[];

extern const float conv2d_1_kweights[];
extern const float conv2d_1_bweights[];
extern const size_t conv2d_1_kshape[];
extern const size_t conv2d_1_bshape[];

extern const float dense_kweights[];
extern const float dense_bweights[];
extern const size_t dense_kshape[];
extern const size_t dense_bshape[];

extern const float dense_1_kweights[];
extern const float dense_1_bweights[];
extern const size_t dense_1_kshape[];
extern const size_t dense_1_bshape[];

#endif /* REALWEIGHTS_H__ */
