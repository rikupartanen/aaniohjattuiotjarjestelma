#ifndef F27BD3AE_D9EC_44F5_90AB_79D0BAB9DBAD
#define F27BD3AE_D9EC_44F5_90AB_79D0BAB9DBAD

#include <stddef.h>

int mat_mul(float *vec, size_t vec_len, const struct tensor *kernel, float *out);
struct tensor *dense(
    const struct tensor *input,
    const struct tensor *kernel,
    const struct tensor *bias
);

#endif /* F27BD3AE_D9EC_44F5_90AB_79D0BAB9DBAD */
