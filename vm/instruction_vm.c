/*
 * instruction_vm.c
 *
 *  Created on: 4 mar. 2022
 *      Author: egonzalez
 */
#include <stddef.h>
#include <string.h>

#include "common.h"
#include "instruction.h"

int get_type(const instruction_t ins) {
    int rv = -1; //ERR

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

void deepcopy(const instruction_t from, instruction_t to) {
    //deepcopy
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

