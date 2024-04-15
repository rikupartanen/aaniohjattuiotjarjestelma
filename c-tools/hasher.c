#include <stdint.h>
#include "hasher.h"

uint8_t u8_hash(char *s){
    uint32_t hash = 0;
    while(*s){
        hash += *s++;
    }
    return (uint8_t)hash;
}
