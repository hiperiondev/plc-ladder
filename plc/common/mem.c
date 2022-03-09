/*
 * Copyright 2022 Emiliano Gonzalez (egonzalez . hiperion @ gmail . com))
 * * Project Site: https://github.com/hiperiondev/plc-ladder *
 *
 * This is based on other projects:
 *    plcemu (https://github.com/kalamara/plcemu)
 *     - Antonis Kalamaras (kalamara AT ceid DOT upatras DOT gr)
 *
 * please contact their authors for more information.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 *
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
