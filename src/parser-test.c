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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmp_parser-il.h"
#include "cmp_parser-ld.h"
#include "cmp_parser-tree.h"
#include "vm_plclib.h"
#include "mem.h"
#include "common.h"

#include "vm_tests.h"

int main() {

    rung_t *rungs;

    char lines[MAXBUF][MAXSTR];
    memset(lines, 0, MAXBUF * MAXSTR);
    sprintf(lines[0], "%s\n", " i0/1-----------+--+-!i0/5---+---i0/6--+--+--+---(Q0/0 ");
    sprintf(lines[1], "%s\n", "                |  |         |         |  |  |         ");
    sprintf(lines[2], "%s\n", " i0/2---!i1/1---+  |         +---i0/7--+  |  |         ");
    sprintf(lines[3], "%s\n", "                |  |         |         |  |  |         ");
    sprintf(lines[4], "%s\n", " i0/3-----------+  |         +---i1/0--+  |  |         ");
    sprintf(lines[5], "%s\n", "                   +------i1/2------------+  |         ");
    sprintf(lines[6], "%s\n", " i0/4----------------------------------------+         ");

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

    printf("test  1: %s\n",                                        result == STATUS_OK            ? "ok" : "error");
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

    vm_tests();
}
