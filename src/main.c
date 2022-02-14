/*
 * main.c
 *
 *  Created on: 13 feb. 2022
 *      Author: egonzalez
 */

#include "plclib.h"

// plc_t parse_ld_program(const char *name, const char lines[][MAXSTR], plc_t p);

plc_t plc1, plc2;

int main() {
    plc1 = malloc(sizeof(plc_t));

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
    plc_t r = parse_ld_program("test", lines, plc1);

    printf("-- parse END: %d\n", r->rungno);

}
