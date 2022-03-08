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

#include "parser-il.h"
#include "parser-ld.h"
#include "parser-tree.h"
#include "plclib.h"
#include "mem.h"
#include "common.h"

#include "vm_tests.h"

int main() {

    /*
    rung_t *rungs;

    char lines[MAXBUF][MAXSTR];
    memset(lines, 0, MAXBUF * MAXSTR);
    sprintf(lines[0], "%s\n", " i0/1--+--+-!i0/5---+---i0/6--+--+--+---(Q0/0 ");
    sprintf(lines[1], "%s\n", "       |  |         |         |  |  |         ");
    sprintf(lines[2], "%s\n", " i0/2--+  |         +---i0/7--+  |  |         ");
    sprintf(lines[3], "%s\n", "       |  |         |         |  |  |         ");
    sprintf(lines[4], "%s\n", " i0/3--+  |         +---i1/0--+  |  |         ");
    sprintf(lines[5], "%s\n", "          +------i1/2------------+  |         ");
    sprintf(lines[6], "%s\n", " i0/4-------------------------------+         ");

    printf("\n");
    for (int n = 0; n <= 6; n++) {
        printf("%s", lines[n]);
    }
    printf("\n");

    printf("-- parse_ld_program START\n");
    rungs = parse_ld_program("test", lines);
    if (rungs == NULL)
        printf("rungs error\n");

    char dump[MAXBUF];
    memset(dump, 0, MAXBUF);
    rung_dump(rungs[0], dump);
    printf("\n%s", dump);

    printf("\n-- parse_ld_program END\n\n");

    printf("-- parse_ld_line START\n\n");
    int result;
    struct ld_line line;

    memset(&line, 0, sizeof(line));
    line.buf = " ---!i0/5--(Q0/3 ";
    result = parse_ld_line(&line);

    printf("test  1: %s\n",                                        result == STATUS_OK               ? "ok" : "error");
    printf("test  2: %s\n",                                   line.cursor == strlen(line.buf) - 4 ? "ok" : "error");
    printf("test  3: %s\n",                                   line.status == STATUS_RESOLVED      ? "ok" : "error");
    printf("test  4: %s\n",                                line.stmt->tag == TAG_ASSIGNMENT       ? "ok" : "error");
    printf("test  5: %s\n",                         line.stmt->v.ass.type == LD_COIL              ? "ok" : "error");
    printf("test  6: %s\n",              line.stmt->v.ass.right->v.exp.op == IL_AND               ? "ok" : "error");
    printf("test  7: %s\n",             line.stmt->v.ass.right->v.exp.mod == IL_NEG               ? "ok" : "error");
    printf("test  8: %s\n",          line.stmt->v.ass.right->v.exp.a->tag == TAG_IDENTIFIER       ? "ok" : "error");
    printf("test  9: %s\n", line.stmt->v.ass.right->v.exp.a->v.id.operand == OP_INPUT             ? "ok" : "error");
    printf("test 10: %s\n",    line.stmt->v.ass.right->v.exp.a->v.id.byte == 0                    ? "ok" : "error");
    printf("test 11: %s\n",     line.stmt->v.ass.right->v.exp.a->v.id.bit == 5                    ? "ok" : "error");
    printf("test 12: %s\n",               line.stmt->v.ass.right->v.exp.b == NULL                 ? "ok" : "error");

    printf("\n-- parse_ld_line END\n\n");
    */

    vm_tests();
}
