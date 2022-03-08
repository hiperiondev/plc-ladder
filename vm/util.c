/*
 * util.c
 *
 *  Created on: 4 mar. 2022
 *      Author: egonzalez
 */

// debugging tools

#include "mem.h"

double Mean = 0;
double M2 = 0;
unsigned long Loop = 0;

void compute_variance(double x) {
    if (Loop == 0) { // overflow
        Mean = 0;
        M2 = 0;
    }
    Loop++;
    double delta = x - Mean;
    Mean += delta / (double) Loop;
    M2 += delta * (x - Mean);
}

void get_variance(double *mean, double *var) {
    *mean = Mean;
    if (Loop > 1)
        *var = M2 / (double) (Loop - 1);
}

unsigned long get_loop() {
    return Loop;
}

