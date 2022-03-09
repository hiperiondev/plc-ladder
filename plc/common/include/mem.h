/*
 * mem.h
 *
 *  Created on: 7 mar. 2022
 *      Author: egonzalez
 */

#ifndef DEBUG_MEM_H_
#define DEBUG_MEM_H_

#include <stddef.h>

#define MEM_ALIGN 8 // 32bits: 4 | 64bits: 8

#define MEM_MALLOC(size, tag)         a_malloc(size)
#define MEM_CALLOC(count, size, tag)  a_calloc(count, size)
#define   MEM_FREE(ptr, tag)          a_free(ptr)

void* a_malloc (size_t size);
void* a_calloc (size_t count, size_t size);
void    a_free (void *ptr);

#endif /* INCLUDE_MEM_H_ */
