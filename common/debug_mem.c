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

#include "debug_mem.h"

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

void* debug_malloc(size_t size, char* mem_tag) {
    if (!mem_init)
        debug_mem_init();

    void *p;
    totalAlloc += size;

    p = malloc(size + sizeof(int));
    *(int*) p = size;
    ptrs[ptr_cnt] = p;
    strcpy(debug_mem_tags[ptr_cnt++], mem_tag);

    printf("debug_malloc [%s (%d)] (total: %d)\n", mem_tag, ptr_cnt - 1, totalAlloc);
    return (void*) (((int*) p) + 1);
}

void* debug_calloc(size_t count, size_t size, char* mem_tag) {
    if (!mem_init)
        debug_mem_init();
    void *p;
    totalAlloc += size * count;

    p = calloc(count, size + sizeof(int));
    *(int*) p = size;
    ptrs[ptr_cnt] = p;
    strcpy(debug_mem_tags[ptr_cnt++], mem_tag);

    printf("debug_calloc [%s (%d)] (total: %d)\n", mem_tag, ptr_cnt - 1, totalAlloc);
    return (void*) (((int*) p) + 1);
}

void debug_free(void *ptr, char* mem_tag) {
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

    free(ptr);
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

void* alligned_malloc(size_t size) {
    if (!mem_init)
        debug_mem_init();

    void *p;
    totalAlloc += size;

    p = malloc(size + sizeof(int));
    *(int*) p = size;

    return (void*) (((int*) p) + 1);
}

void check_free(void *ptr) {
    if (ptr == NULL) {
        printf("free [WARNING: pointer NULL]");
        return;
    }
    free(ptr);
}
