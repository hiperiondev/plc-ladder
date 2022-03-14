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

#ifndef _RUNG_H_
#define _RUNG_H_

#include "common.h"
#include "cmp_instruction.h"
#include "vm_data.h"

#define MAXSTACK 256
#define MAXRUNG  256

typedef struct codeline {
               char *line;
    struct codeline *next;
} *codeline_t;

typedef struct opcode {
          uint8_t operation;
          uint8_t type;
          uint8_t depth;
    union accdata value;
    struct opcode *next;
} *opcode_t;

// the instruction list executable rung
typedef struct rung {
    instruction_t *instructions;
             char *id;
       codeline_t code;               // original code for visual representation
     unsigned int insno;              // actual no of active lines
      struct rung *next;              // linked list of rungs
         opcode_t stack;              // head of stack
    struct opcode prealloc[MAXSTACK]; // preallocated stack
    union accdata acc;                // accumulator
              int status;
} *rung_t;

/**
 * @brief get instruction reference from rung
 * @param r a rung AKA instructions list
 * @param i the instruction reference
 * @param idx the index
 * @return OK or error
 */
int rung_get_instruction(const rung_t r, unsigned int idx, instruction_t *i);

/**
 * @brief append instruction to rung
 * @param i a new instruction
 * @param r a rung AKA instructions list
 * @return OK or error
 */
int rung_append(const instruction_t i, rung_t r);

/**
 * @brief append codeline string to rung code
 * @param l a code line
 * @param code the existing code lines
 * @return rung code including new line
 */
codeline_t rung_append_line(const char *l, codeline_t code);

/**
 * @brief clear rung from instructions and free memory
 * @param r a rung AKA instructions list
 */
void rung_clear(rung_t r);

/**
 * @brief lookup instruction by label
 * @param label
 * @param r a rung AKA instructions list
 * @return the index (pc) of the instruction, or error if not found
 */
int rung_lookup(const char *label, rung_t r);

/**
 * @brief intern  labels
 * for each lookup command (JMP etc.) the lookup label is 
 * looked up and if found the resulting index stored to the modifier
 * @param r a rung AKA instructions list
 * @return OK, or error if : a label is not found or found duplicate
 */
int rung_intern(rung_t r);

/**
 * @brief add a new rung to a plc
 * @param a unique identifier
 * @param the plc
 * @return reference to the new rung
 */
rung_t rung_make(const char *name, rung_t *rungs, uint8_t *rungno);

/**
 * @brief get rung reference from plc
 * @param p a plc
 * @param r the rung reference
 * @param idx the index
 * @return reference to rung or NULL
 */
rung_t rung_get(rung_t *rungs, uint8_t *rungno, const unsigned int idx);

/**
 * @brief
 * @param
 * @param
 * @return
 */
void rung_dump(rung_t ins, char *dump);

#endif //_RUNG_H_
