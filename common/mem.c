/*
 * mem.c
 *
 *  Created on: 7 mar. 2022
 *      Author: egonzalez
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mem.h"

void* a_malloc(size_t size) {
    void *ptr;
    void **aligned;

    if ((ptr = (void*) malloc(size + (MEM_ALIGN - 1) + sizeof(void*))) == NULL)
        return NULL;

    aligned = (void**) ((size_t) ptr + sizeof(void*));
    if (((size_t) (aligned) % MEM_ALIGN) > 0)
        aligned += (MEM_ALIGN - (((size_t) (aligned)) % MEM_ALIGN)) / sizeof(void*);
    *(aligned - 1) = ptr;

    return aligned;
}

void* a_calloc(size_t count, size_t eltsize) {
    void *ptr = a_malloc(count * eltsize);
    if (ptr == NULL)
        return (ptr);

    memset(ptr, 0, count * eltsize);
    return (ptr);
}

void a_free(void *ptr) {
    free(*(((void**) ptr) - 1));
}
