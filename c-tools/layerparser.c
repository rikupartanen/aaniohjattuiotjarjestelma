#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "hasher.h"
#include "layerparser.h"
#include "weights.h"

#define FILENAME "map.toml"


const char COMMENT = '#';

/* Trim a string in place */
static size_t trim(char *s){
    char *p = s;
    size_t len = strlen(s);

    if(len == 0){
        return len;
    }

    /* Remove whitespace at the end */
    while (len && isspace((unsigned char)(p[len - 1]))){
        p[--len] = 0;
    }

    /* Remove whitespace at the start */
    while(*p && isspace((unsigned char)*p)){
        ++p, --len;
    }
    memmove(s, p, len + 1);
    s[len] = 0;
    return len;
}

size_t trim_value(char *s){
    char *p = s;
    size_t len = strlen(s);
    if(len == 0){
        return len;
    }

    /* There shouldn't really be anything after the value but trim that too */
    while(isspace((unsigned char)p[len -1])){
        --len;
        p[len] = 0;
    }

    /* Trim '=' as well */
    while(*p && (isspace((unsigned char)*p) || *p == '=')){
        ++p, --len;
    }

    memmove(s, p, len + 1);
    s[len] = 0;
    return len;

}

static size_t trim_header(char *s){
    char *p = s;
    size_t len = strlen(s);

    if(len == 0){
        return len;
    }

    /* Trim all whitespace and [] signs */
    while(isspace((unsigned char)p[len - 1]) || p[len - 1] == ']'){
        --len;
        p[len] = 0;
    }

    while(*p && (isspace((unsigned char)*p) || *p == '[')){
        ++p, --len;
    }

    memmove(s, p, len + 1);
    s[len] = 0;
    return len;
}

/* it seems trimming headers and arrays
 * is exactly the same thing so reuse that */
static inline size_t trim_shape(char *s){
    return trim_header(s);
}

/* Probably unnecessary as we don't care about it 
 * but keep this here for now */
static char *parse_header(char *s){
    trim_header(s);

    return s;
}

/* Returns the length of the shape array, actual array will be returned to *out
 * WARNING: this will mangle s through strtok */
static size_t parse_shape(char *s, size_t **out){
    //size_t len = strlen(s);

    size_t len = trim(s);
    /* malformed input */
    if(s[0] != '[' || s[len - 1] != ']'){
        printf("Error: array %s is missing a [ or ]\n", s);
        *out = NULL;
        return 0;
    }
    len = trim_shape(s);

    /* Create a copy since strtok() will destroy the original */
    char *copy = malloc((len + 1) * sizeof(char));
    strncpy(copy, s, len + 1);

    /* First loop through to see how many numbers we have */
    size_t i = 0;
    char *tok = strtok(s, ",");
    while(tok != NULL){
        i++;
        tok = strtok(NULL, ",");
    }

    *out = malloc(i * sizeof(size_t));

    /* And then loop through the string again, this time storing the numbers */
    i = 0;
    tok = strtok(copy, ",");
    while(tok != NULL){
        (*out)[i++] = atol(tok);
        tok = strtok(NULL, ",");
    }
    free(copy);

    return i;
}

static char *parse_name(char *s){
    size_t len = strlen(s);

    if(len == 0){
        printf("Error: Name is null\n");
        return NULL;
    }

    len = trim(s);

    if(s[0] != '"' || s[len-1] != '"' ){
        printf("Error: Malformed name %s is missing a \"\n", s);
        return NULL;
    }

    while(s[len-1] == '"'){
        s[--len] = 0;
    }

    while(*s == '"'){
        len--;
        s++;
    }

    char *ret = malloc((len + 1) * sizeof(char));
    strncpy(ret, s, len + 1);


    return ret;
}


static int parse_key_value(char *key, char *value, struct layer *l){
    if(strcmp(key, "name") == 0){
        l->name = parse_name(value);

        return 0;
    }

    if(strcmp(key, "kshape") == 0){
        l->kshape_len = parse_shape(value, &l->kshape);
        return 0;
    }

    if(strcmp(key, "bshape") == 0){
        l->bshape_len = parse_shape(value, &l->bshape);

        return 0;
    }


    return -1;
}


static inline void free_layer(struct layer *l){
    free(l->name);
    free(l->kshape);
    free(l->bshape);
    free(l->offsets);
    free(l);
}

void free_layers(struct layer **layers, size_t n){
    for(size_t i = 0; i < n; ++i){
        free_layer(layers[i]);
    }
}

static inline void print_shape(size_t *shape, size_t len){
    printf("{");
    for(size_t i = 0; i < len; ++i){
        if(i != 0){ putchar(','); }
        printf(" %zu", shape[i]);
    }
    printf(" }");
}


static inline void print_layer(struct layer *l){
    printf("Layer %s:\n", l->name);
    printf("\tHash: %#x\n", u8_hash(l->name));
    printf("\tKshape: ");
    print_shape(l->kshape, l->kshape_len);
    printf("\n\tBshape: ");
    print_shape(l->bshape, l->bshape_len);
    printf("\n\n");
}

void get_weights(struct layer *l){
    if(&map[u8_hash(l->name)] == NULL){
        printf("Error: no weights found for hash %#x\n", u8_hash(l->name));
        l->weights = NULL;
    }

    l->weights = &map[u8_hash(l->name)];
}

void print_layers(struct layer **layers, size_t n){
    for(size_t i = 0; i < n; ++i){
        print_layer(layers[i]);
    }

}


size_t get_layers(struct layer ***layers){
    FILE *fp = fopen(FILENAME, "r");
    ssize_t read = 0;
    char *line = NULL;
    size_t n;

    size_t len;
    char *input = NULL;
    char *tok = NULL;
    char *key = NULL;
    char *value = NULL;

    /* This will be the number of sections
     * NOT the section index, that's (section - 1) */
    size_t section = 0;

    //struct layer **layers = NULL;

    while((read = getline(&line, &n, fp)) != -1){
        /* skip ahead if we've only read an empty line */
        if(read <= 1 && isspace((unsigned char)(*line))){
            continue;
        }

        len = trim(line);


        // Skip comments
        if(line[0] == COMMENT){
            continue;
        }

        /* check for section and allocate a new struct
         * for a new layer if it is one */
        if(line[0] == '[' && line[len - 1] == ']'){
            section++;
            *layers = reallocarray(*layers, section, sizeof(struct layer*));
            (*layers)[section - 1] = malloc(sizeof(struct layer));
        }

        if(section == 0){
            continue;
        }

        // start looking for key = value pairs
        tok = strchr(line, '='); 

        // Not a key = value pair
        if(tok == NULL){
            continue;
        }

        value = calloc(sizeof(char), (strlen(tok) + 1) * sizeof(char));
        key = calloc(sizeof(char), (tok - line + 1) * sizeof(char));

        strncpy(value, tok, strlen(tok) + 1);
        strncpy(key, line, ((tok - line - 1 )) * sizeof(char));
        key[tok - line] = 0;


        /* Key can be trimmed with just regular trim */
        trim(key);
        trim_value(value);


        parse_key_value(key, value, (*layers)[section-1]);

        free(key);
        free(value);

        key = NULL;
        value = NULL;
    }

    free(line);
    fclose(fp);


    return section;
}
