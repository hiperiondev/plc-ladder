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

// debugging tools

#include "mem.h"

double Mean = 0;
double M2 = 0;
unsigned long Loop = 0;

void vm_compute_variance(double x) {
    if (Loop == 0) { // overflow
        Mean = 0;
        M2 = 0;
    }
    Loop++;
    double delta = x - Mean;
    Mean += delta / (double) Loop;
    M2 += delta * (x - Mean);
}

void vm_get_variance(double *mean, double *var) {
    *mean = Mean;
    if (Loop > 1)
        *var = M2 / (double) (Loop - 1);
}

unsigned long vm_get_loop() {
    return Loop;
}
