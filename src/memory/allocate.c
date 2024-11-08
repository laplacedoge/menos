#include <stdlib.h>

#include "allocate.h"

void *
MeMem_Malloc(usize size) {
    return malloc(size);
}

void *
MeMem_Realloc(void * ptr, usize size) {
    return realloc(ptr, size);
}

void
MeMem_Free(void * ptr) {
    free(ptr);
}