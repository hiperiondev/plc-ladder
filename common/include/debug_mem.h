/*
 * mem.h
 *
 *  Created on: 7 mar. 2022
 *      Author: egonzalez
 */

#ifndef DEBUG_MEM_H_
#define DEBUG_MEM_H_

//#define DEBUG_MEM

#ifdef DEBUG_MEM
#define MEM_MALLOC(size, tag)         debug_malloc(size, tag)
#define MEM_CALLOC(count, size, tag)  debug_calloc(count, size, tag)
#define MEM_FREE(ptr)                 debug_free(ptr)
#else
#define MEM_MALLOC(size, tag)         malloc(size)
#define MEM_CALLOC(count, size, tag)  calloc(count, size)
#define MEM_FREE(ptr)                 free(ptr)
#endif

#include <stddef.h>

void*   debug_malloc (size_t size, char* mem_tag);
void*   debug_calloc (size_t count, size_t size, char* mem_tag);
void      debug_free (void *ptr);
void  debug_free_all (void);

#endif /* INCLUDE_MEM_H_ */
