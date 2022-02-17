/*
 ============================================================================
 Name        : parser-ld-orig.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "plc_common.h"
#include "parser-tree.h"
#include "parser-ld.h"


plc_t plc;

int main() {
    plc = malloc(sizeof(plc_t));

    char lines[MAXBUF][MAXSTR];
    memset(lines, 0, MAXBUF * MAXSTR);
    sprintf(lines[0], "%s\n", " i0/1--+--+-!i0/5---+---i0/6--+--+--+---(Q0/0 ");
    sprintf(lines[1], "%s\n", "       |  |         |         |  |  |         ");
    sprintf(lines[2], "%s\n", " i0/2--+  |         +---i0/7--+  |  |         ");
    sprintf(lines[3], "%s\n", "       |  |         |         |  |  |         ");
    sprintf(lines[4], "%s\n", " i0/3--+  |         +---i1/0--+  |  |         ");
    sprintf(lines[5], "%s\n", "          +------i1/2------------+  |         ");
    sprintf(lines[6], "%s\n", " i0/4-------------------------------+         ");

    printf("-- parse START\n\n");
    plc = parse_ld_program("test", lines, plc);
    printf("-- parse END: %d\n", plc-> rungno);

    int result;
    struct ld_line line;

    memset(&line, 0, sizeof(line));
    line.buf = " ---!i0/5--(Q0/3 ";
    result = parse_ld_line(&line);

    printf("test: %d\n",result == PLC_OK);
    printf("test: %d\n",line.cursor == strlen(line.buf) - 4);
    printf("test: %d\n",line.status == STATUS_RESOLVED);
    printf("test: %d\n",line.stmt->tag == TAG_ASSIGNMENT);
    printf("test: %d\n",line.stmt->v.ass.type == LD_COIL);

    printf("test: %d\n",line.stmt->v.ass.right->v.exp.op == IL_AND);
    printf("test: %d\n",line.stmt->v.ass.right->v.exp.mod == IL_NEG);
    printf("test: %d\n",line.stmt->v.ass.right->v.exp.a->tag == TAG_IDENTIFIER);
    printf("test: %d\n",line.stmt->v.ass.right->v.exp.a->v.id.operand == OP_INPUT);
    printf("test: %d\n",line.stmt->v.ass.right->v.exp.a->v.id.byte == 0);
    printf("test: %d\n",line.stmt->v.ass.right->v.exp.a->v.id.bit == 5);
    printf("test: %d\n",line.stmt->v.ass.right->v.exp.b == NULL);

}
