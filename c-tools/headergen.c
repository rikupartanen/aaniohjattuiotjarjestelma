#include <stddef.h>
#include <stdio.h>

#include "headergen.h"
#include "layer.h"

void write_header(const char *name, struct layer **layers, size_t n){
    char buff[128];
    FILE *fp = fopen(name, "wb");

    char *header_guard =    "#ifndef HEADER_H__\n"
                            "#define HEADER_H__\n"
                            "#include <stddef.h>\n"
                            "#define FLASH_BASE 0x10000000\n\n";

    fputs(header_guard, fp);


    for(size_t i = 0; i < n; ++i){
        fprintf(fp, "const _Float16 *%s_kweights = (_Float16*)(FLASH_BASE + %#x);\n", layers[i]->name, layers[i]->offsets->kernel);

        if(layers[i]->bshape[0] == 0){
            fprintf(fp, "const _Float16 *%s_bweights = NULL;\n", layers[i]->name);
        }else{
            fprintf(fp, "const _Float16 *%s_bweights = (_Float16*)(FLASH_BASE + %#x);\n", layers[i]->name, layers[i]->offsets->bias);
        }

        fprintf(fp, "const size_t %s_kshape[] = { ", layers[i]->name);
        for(size_t j = 0; j < layers[i]->kshape_len; ++j){
            if(j != 0){ fputc(',', fp); }
            fprintf(fp, " %zu", layers[i]->kshape[j]);
        }
        fprintf(fp, "  };\n");

        fprintf(fp, "const size_t %s_bshape[] = { ", layers[i]->name);
        for(size_t j = 0; j < layers[i]->bshape_len; ++j){
            if(j != 0){ fputc(',', fp); }
            fprintf(fp, " %zu", layers[i]->bshape[j]);
        }
        fprintf(fp, "  };\n");


        fprintf(fp,"\n");
    }


    fputs("\n#endif /* HEADER_H__ */", fp);
    fclose(fp);
}
