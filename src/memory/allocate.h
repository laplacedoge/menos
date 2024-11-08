#ifndef __ME_MEMORY_ALLOCATE_H__
#define __ME_MEMORY_ALLOCATE_H__

#include "menos.h"

void *
MeMem_Malloc(usize size);

void *
MeMem_Realloc(void * ptr, usize size);

void
MeMem_Free(void * ptr);

#endif