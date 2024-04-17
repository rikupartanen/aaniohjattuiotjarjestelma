#ifndef HEADERGEN_H__
#define HEADERGEN_H__

#include <stddef.h>

#include "layer.h"

int write_header(const char *name, struct layer **layers, size_t n);

#endif /* HEADERGEN_H__ */
