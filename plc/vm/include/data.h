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

#ifndef _DATA_H_
#define _DATA_H_

#include <inttypes.h>

#define BIT(x) uint8_t x:1

// TODO: add type for checkings and castings
typedef union accdata {
    uint64_t u;
    double r;
} data_t; // what can the accumulator be

uint64_t operate_u(unsigned char op, uint64_t a, uint64_t b);
  double operate_d(unsigned char op, double a, double b);
   float operate_f(unsigned char op, float a, float b);

/**
 * @brief operate operator op of type t on data a and b
 * @param operator
 * @param type
 * @param a
 * @param b
 * @return result if available
 */
data_t operate(unsigned char op, unsigned char t, const data_t a, const data_t b);

#endif //_DATA_H_
