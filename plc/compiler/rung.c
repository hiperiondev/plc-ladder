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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "instruction.h"
#include "rung.h"
#include "mem.h"

int rung_get_instruction(const rung_t r, const unsigned int idx, instruction_t *i) {
    if (r == NULL || idx >= r->insno)
        return STATUS_ERR;
    *i = r->instructions[idx];
    return STATUS_OK;
}

int rung_append(const instruction_t i, rung_t r) {
    if (r == NULL || r->insno == MAXSTACK)
        return STATUS_ERR;
    if (i != NULL) {
        if (r->instructions == NULL) { // lazy allocation
            r->instructions = (instruction_t*) MEM_MALLOC(MAXSTACK * sizeof(instruction_t), "rung_append A");
            memset(r->instructions, 0, MAXSTACK * sizeof(instruction_t));
        }
        if (rung_lookup(i->label, r) >= 0)
            return STATUS_ERR; // don't allow duplicate labels

        instruction_t ins = (instruction_t) MEM_CALLOC(1, sizeof(struct instruction), "rung_append B");
        //memset(ins, 0, sizeof(struct instruction));
        instruction_deepcopy(i, ins);

        r->instructions[(r->insno)++] = ins;
    }
    return STATUS_OK;
}

codeline_t rung_append_line(const char *l, codeline_t code) {
    if (l == NULL) {

        return code;
    }
    codeline_t r = (codeline_t) MEM_CALLOC(1, sizeof(struct codeline), "rung_append_line A");
    //memset(r, 0, sizeof(struct codeline));
    r->line = strdup(l);

    if (code != NULL) {

        codeline_t i = code;
        while (i->next != NULL) {

            i = i->next;
        }
        i->next = r;

        return code;
    }
    return r;
}

void rung_clear(rung_t r) {
    int i = 0;
    if (r != NULL && r->instructions != NULL) {
        for (; i < MAXSTACK; i++) {
            if (r->instructions[i] != NULL) {
                MEM_FREE(r->instructions[i], "rung_clear A");
            }
        }
        if (r->instructions[i] != NULL)
        MEM_FREE(r->instructions, "rung_clear B");
        if (r->code != NULL)
        MEM_FREE(r->code, "rung_clear C");
        r->instructions = NULL;
        r->insno = 0;
        // TODO: also free rung, return null
    }
}

int rung_lookup(const char *label, rung_t r) {
    int ret = STATUS_ERR;
    if (label == NULL || r == NULL)
        return ret;

    int i = 0;
    instruction_t ins = NULL;
    for (; i < r->insno; i++) {
        rung_get_instruction(r, i, &ins);
        if (strlen(ins->label) > 0 && strcmp(ins->label, label) == 0) {
            ret = i;
            break;
        }
    }
    return ret;
}

int rung_intern(rung_t r) {
    if (r == NULL)
        return STATUS_ERR;

    int i = 0;
    instruction_t ins = NULL;
    for (; i < r->insno; i++) {
        rung_get_instruction(r, i, &ins);
        if (strlen(ins->lookup) > 0) {
            int l = rung_lookup(ins->lookup, r);
            if (l < 0)
                return STATUS_ERR;
            else
                ins->operand = l;
        }
    }
    return STATUS_OK;
}

rung_t rung_make(const char *name, rung_t *rungs, uint8_t *rungno) {
    rung_t r = (rung_t) MEM_CALLOC(1, sizeof(struct rung), "rung_make A");
    //memset(r, 0, sizeof(struct rung));
    r->id = strdup(name);
    if (rungs == NULL) { // lazy allocation
        rungs = (rung_t*) MEM_CALLOC(1, MAXRUNG * sizeof(rung_t), "rung_make B");
        //memset(rungs, 0, MAXRUNG * sizeof(rung_t));
    }
    rungs[(*rungno)++] = r;

    return r;
}

rung_t rung_get(rung_t *rungs, uint8_t *rungno, const unsigned int idx) {
    if (rungs == NULL || idx >= *rungno) {
        return NULL;
    }
    return rungs[idx];
}

void rung_dump(rung_t r, char *dump) {
    if (r == NULL || dump == NULL)
        return;
    instruction_t ins;
    unsigned int pc = 0;
    char buf[4] = "";
    for (; pc < r->insno; pc++) {
        if (rung_get_instruction(r, pc, &ins) < STATUS_OK)
            return;
        sprintf(buf, "%d.", pc);
        strcat(dump, buf);
        instruction_dump(ins, dump);
    }
}
