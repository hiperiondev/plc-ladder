/*
 * mem.h
 *
 *  Created on: 7 mar. 2022
 *      Author: egonzalez
 */

#ifndef DEBUG_MEM_H_
#define DEBUG_MEM_H_

#define DEBUG_MEM

#ifdef DEBUG_MEM
#define MEM_MALLOC(size, tag)         debug_malloc(size, tag)
#define MEM_CALLOC(count, size, tag)  debug_calloc(count, size, tag)
#define MEM_FREE(ptr, tag)            debug_free(ptr, tag)
#else
#define MEM_MALLOC(size, tag)         alligned_malloc(size)
#define MEM_CALLOC(count, size, tag)  calloc(count, size)
#define MEM_FREE(ptr, tag)            check_free(ptr)
#endif

#include <stddef.h>

void*    debug_malloc (size_t size, char* mem_tag);
void*    debug_calloc (size_t count, size_t size, char* mem_tag);
void       debug_free (void *ptr, char* mem_tag);
void   debug_free_all (void);
void* alligned_malloc (size_t size);
void       check_free (void *ptr);

#endif /* INCLUDE_MEM_H_ */
