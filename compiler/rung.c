
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "instruction.h"
#include "rung.h"

/*****************************rung***********************************/
int get(const rung_t r, const unsigned int idx, instruction_t *i) {
    if (r == NULL || idx >= r->insno)
        return STATUS_ERR;
    *i = r->instructions[idx];
    return STATUS_OK;
}

int append(const instruction_t i, rung_t r) {
    if (r == NULL || r->insno == MAXSTACK)
        return STATUS_ERR;
    if (i != NULL) {
        if (r->instructions == NULL) { // lazy allocation
            r->instructions = (instruction_t*) malloc(MAXSTACK * sizeof(instruction_t));
            memset(r->instructions, 0, MAXSTACK * sizeof(instruction_t));
        }
        if (lookup(i->label, r) >= 0)
            return STATUS_ERR; // don't allow duplicate labels

        instruction_t ins = (instruction_t) malloc(sizeof(struct instruction));
        memset(ins, 0, sizeof(struct instruction));
        deepcopy(i, ins);

        r->instructions[(r->insno)++] = ins;
    }
    return STATUS_OK;
}

codeline_t append_line(const char *l, codeline_t code) {
    if (l == NULL) {

        return code;
    }
    codeline_t r = (codeline_t) malloc(sizeof(struct codeline));
    memset(r, 0, sizeof(struct codeline));
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

void clear_rung(rung_t r) {
    int i = 0;
    if (r != NULL && r->instructions != NULL) {
        for (; i < MAXSTACK; i++) {
            if (r->instructions[i] != NULL)
                free(r->instructions[i]);
        }
        free(r->instructions);
        free(r->code);
        r->instructions = NULL;
        r->insno = 0;
        // TODO: also free rung, return null
    }
}

int lookup(const char *label, rung_t r) {
    int ret = STATUS_ERR;
    if (label == NULL || r == NULL)
        return ret;

    int i = 0;
    instruction_t ins = NULL;
    for (; i < r->insno; i++) {
        get(r, i, &ins);
        if (strlen(ins->label) > 0 && strcmp(ins->label, label) == 0) {
            ret = i;
            break;
        }
    }
    return ret;
}

int intern(rung_t r) {
    if (r == NULL)
        return STATUS_ERR;

    int i = 0;
    instruction_t ins = NULL;
    for (; i < r->insno; i++) {
        get(r, i, &ins);
        if (strlen(ins->lookup) > 0) {
            int l = lookup(ins->lookup, r);
            if (l < 0)
                return STATUS_ERR;
            else
                ins->operand = l;
        }
    }
    return STATUS_OK;
}

void dump_rung(rung_t r, char *dump) {
    if (r == NULL || dump == NULL)
        return;
    instruction_t ins;
    unsigned int pc = 0;
    char buf[4] = "";
    for (; pc < r->insno; pc++) {
        if (get(r, pc, &ins) < STATUS_OK)
            return;
        sprintf(buf, "%d.", pc);
        strcat(dump, buf);
        dump_instruction(ins, dump);
    }
}

rung_t mk_rung(const char *name, rung_t *rungs, uint8_t *rungno) {
    rung_t r = (rung_t) malloc(sizeof(struct rung));
    memset(r, 0, sizeof(struct rung));
    r->id = strdup(name);
    if (rungs == NULL) {     // lazy allocation
        rungs = (rung_t*) malloc(MAXRUNG * sizeof(rung_t));
        memset(rungs, 0, MAXRUNG * sizeof(rung_t));
    }
    rungs[(*rungno)++] = r;

    return r;
}

rung_t get_rung(rung_t *rungs, uint8_t *rungno, const unsigned int idx) {
    if (rungs == NULL || idx >= *rungno) {
        return NULL;
    }
    return rungs[idx];
}
