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

unsigned int totalAlloc = 0;
void *ptrs[1000];
char debug_mem_tags[1000][256];

int ptr_cnt = 0;
bool mem_init = false;

static void debug_mem_init(void) {
    for (int n = 0; n < 1000; n++)
        ptrs[n] = 0;
    mem_init = true;
}

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

void* debug_malloc(size_t size, char *mem_tag) {
    if (!mem_init)
        debug_mem_init();

    void *p;
    totalAlloc += size;

    p = a_malloc(size + sizeof(int));

    ptrs[ptr_cnt] = p;
    strcpy(debug_mem_tags[ptr_cnt++], mem_tag);

    printf("debug_malloc [%s (%d)] (total: %d)\n", mem_tag, ptr_cnt - 1, totalAlloc);
    return p;
}

void* debug_calloc(size_t count, size_t size, char *mem_tag) {
    if (!mem_init)
        debug_mem_init();
    void *p;
    totalAlloc += size * count;

    p = a_calloc(count, size + sizeof(int));

    ptrs[ptr_cnt] = p;
    strcpy(debug_mem_tags[ptr_cnt++], mem_tag);

    printf("debug_calloc [%s (%d)] (total: %d)\n", mem_tag, ptr_cnt - 1, totalAlloc);
    return p;
}

void debug_free(void *ptr, char *mem_tag) {
    if (!mem_init)
        debug_mem_init();

    if (ptr == NULL) {
        printf("debug_free [WARNING: pointer NULL (%s)] (total: %d/%d)\n", mem_tag, totalAlloc, ptr_cnt);
        return;
    }

    int ptr_id = -1;
    ptr = (void*) (((int*) ptr) - 1);
    totalAlloc -= *(int*) ptr;
    for (int n = 0; n < ptr_cnt; n++)
        if (ptrs[n] == ptr) {
            ptr_id = n;
            break;
        }

    a_free(ptr);

    if (ptr_id != -1)
        ptrs[ptr_id] = 0;
    printf("debug_free   [%s/%s (%d)] (total: %d/%d)\n", ptr_id == -1 ? "{unknown}" : debug_mem_tags[ptr_id], mem_tag, ptr_id, totalAlloc, ptr_cnt);
}

void debug_free_all(void) {
    int cnt = 0;
    for (int n = 0; n < 1000; n++)
        if (ptrs[n] != 0) {
            printf("debug_free_all: %s\n", debug_mem_tags[n]);
            free(ptrs[n]);
            ++cnt;
        }
    if (cnt != 0)
        printf("WARNING!!! ");
    printf("debug_free_all: %d\n", cnt);
}
