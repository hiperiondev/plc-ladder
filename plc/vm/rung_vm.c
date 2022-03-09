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

#include <stddef.h>
#include <string.h>

#include "common.h"
#include "rung.h"
#include "data.h"
#include "mem.h"

opcode_t take(rung_t r) {
    if (r->stack == NULL)
        return &(r->prealloc[0]);
    else if (r->stack->depth < MAXSTACK - 1)
        return &(r->prealloc[r->stack->depth]);
    else
        return NULL;
}

void give(opcode_t head) {
    memset(head, 0, sizeof(struct opcode));
}

int get(const rung_t r, const unsigned int idx, instruction_t *i) {
    if (r == NULL || idx >= r->insno)
        return STATUS_ERR;
    *i = r->instructions[idx];
    return STATUS_OK;
}

int push(uint8_t op, uint8_t t, const data_t val, rung_t r) {
// push an opcode and a value into stack.
    struct opcode *p = take(r);
    if (!p)
        return STATUS_ERR;
    //initialize
    p->operation = op;
    p->value = val;
    p->type = t;
    p->next = r->stack; //*stack;
    p->depth = (r->stack == NULL) ? 1 : r->stack->depth + 1;
    //set stack head pointer to point at it
    r->stack = p;
    return STATUS_OK;
}

data_t pop(const data_t val, opcode_t *stack) {
// retrieve stack heads operation and operand, apply it to val and return result
    data_t r = val; // return value
    opcode_t p;
    if (*stack != NULL) {
        // safety
        r = operate((*stack)->operation, (*stack)->type, (*stack)->value, val); // execute instruction
        p = *stack;
        *stack = (*stack)->next;
        // set stack head to point to next opcode in stack
        give(p);
    }
    return r;
}
