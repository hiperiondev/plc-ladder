/*
 * mem.h
 *
 *  Created on: 7 mar. 2022
 *      Author: egonzalez
 */

#ifndef DEBUG_MEM_H_
#define DEBUG_MEM_H_

#define MEM_ALIGN 8 // 32bits: 4 | 64bits: 8

//#define DEBUG_MEM

#ifdef DEBUG_MEM
#define MEM_MALLOC(size, tag)         debug_malloc(size, tag)
#define MEM_CALLOC(count, size, tag)  debug_calloc(count, size, tag)
#define MEM_FREE(ptr, tag)            debug_free(ptr, tag)
#else
#define MEM_MALLOC(size, tag)         a_malloc(size)
#define MEM_CALLOC(count, size, tag)  a_calloc(count, size)
#define MEM_FREE(ptr, tag)            a_free(ptr)
#endif

#include <stddef.h>

void*    debug_malloc (size_t size, char* mem_tag);
void*    debug_calloc (size_t count, size_t size, char* mem_tag);
void       debug_free (void *ptr, char* mem_tag);
void   debug_free_all (void);
void*        a_malloc (size_t size);
void*        a_calloc (size_t count, size_t size);
void           a_free (void *ptr);

#endif /* INCLUDE_MEM_H_ */
