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
#include "cmp_instruction.h"
#include "mem.h"

int vm_get_type(const instruction_t ins) {
    int rv = -1; // err

    if (ins != NULL && OP_VALID(ins->operand)) {
        unsigned char x = ins->bit;
        if (OP_REAL(ins->operand))
            rv = T_REAL;
        else {
            switch (x) {
                case BYTESIZE:
                    rv = T_BYTE;
                    break;
                case WORDSIZE:
                    rv = T_WORD;
                    break;
                case DWORDSIZE:
                    rv = T_DWORD;
                    break;
                case LWORDSIZE:
                    rv = T_LWORD;
                    break;
                default:
                    if (0 <= x && x < BYTESIZE)
                        rv = T_BOOL;
            }
        }
    }
    return rv;
}

void vm_deepcopy(const instruction_t from, instruction_t to) {
    to->operation = from->operation;
    to->operand = from->operand;
    to->modifier = from->modifier;
    to->byte = from->byte;
    to->bit = from->bit;
    if (from->label != NULL)
        strcpy(to->label, from->label);
    if (from->lookup != NULL)
        strcpy(to->lookup, from->lookup);
}
