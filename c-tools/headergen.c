#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>

#include "headergen.h"
#include "layer.h"

int write_header(const char *name, struct layer **layers, size_t n){
    size_t len = sizeof(char) * (strlen(name) + strlen(".h") + 1);
    char *fullname = malloc(len);

    strncpy(fullname, name, (strlen(name) + 1) * sizeof(char));
    strcat(fullname, ".h");

    FILE *fp = fopen(fullname, "wb");

    if(fp == NULL){
        perror("Error opening file");
        return errno;
    }

    char *header_guard = strdup(name);

    for(char *p = header_guard; *p; p++){
        *p = toupper(*p);
    }

    fprintf(fp, "#ifndef %s_H__\n", header_guard);
    fprintf(fp, "#define %s_H__\n", header_guard);

    char *includes =    "#include <stddef.h>\n";
    char *defines =     "#define FLASH_BASE 0x10000000\n\n";

    for(size_t i = 0; i < n; ++i){
        /* Kernel weights */
        fprintf(fp, "const _Float16 *%s_kweights = (_Float16*)(FLASH_BASE + %#x);\n", layers[i]->name, layers[i]->offsets->kernel);

        /* Check for NULL pointer */
        if(layers[i]->bshape[0] == 0){
            fprintf(fp, "const _Float16 *%s_bweights = NULL;\n", layers[i]->name);
        }else{
            fprintf(fp, "const _Float16 *%s_bweights = (_Float16*)(FLASH_BASE + %#x);\n", layers[i]->name, layers[i]->offsets->bias);
        }

        /* Kernel Shape */
        fprintf(fp, "const size_t %s_kshape[] = { ", layers[i]->name);
        for(size_t j = 0; j < layers[i]->kshape_len; ++j){
            if(j != 0){ fputc(',', fp); }
            fprintf(fp, " %zu", layers[i]->kshape[j]);
        }
        fprintf(fp, "  };\n");

        /* Bias shape */
        fprintf(fp, "const size_t %s_bshape[] = { ", layers[i]->name);
        for(size_t j = 0; j < layers[i]->bshape_len; ++j){
            if(j != 0){ fputc(',', fp); }
            fprintf(fp, " %zu", layers[i]->bshape[j]);
        }
        fprintf(fp, "  };\n");

        fprintf(fp,"\n");
    }


    fprintf(fp, "#endif /* %s_H__ */", header_guard);
    free(header_guard);
    free(fullname);
    fclose(fp);

    return 0;
}
