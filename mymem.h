//
// Created by Varun Muthanna on 26/11/24.
//

#ifndef MEMORYALLOC_MYMEM_H         //if not defined continue , else move to endif
#define MEMORYALLOC_MYMEM_H         //defined MEMORYALLOC_MYMEM_H (any unique identifier)

#include<stddef.h>

//function declarations

void* my_malloc(size_t size);
void my_free(void *block);
void* my_calloc(size_t n,  size_t bytes);
void* my_relloc(void *block , size_t size);


#endif //MEMORYALLOC_MYMEM_H
