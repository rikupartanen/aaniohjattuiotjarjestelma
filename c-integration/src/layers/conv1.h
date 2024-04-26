#ifndef CONV1_H
#define CONV1_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct tensor *conv2d1(const struct tensor *input, const struct tensor *kernel, const struct tensor *bias);
void print_tensor(struct tensor *tensor);

#endif
