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

#ifndef _CODEGEN_H_
#define _CODEGEN_H_

#include "common.h"
#include "cmp_parser-tree.h"
#include "cmp_rung.h"

/**
 * @brief generate code from an expression
 * @param expression
 * @param the rung to insert the code to
 * @param the operation to generate if called in a recursion
 * @return ok or error code
 */
int gen_expr(const item_t expression, rung_t rung, uint8_t recursive);

/**
 * @brief generate code for the left side of an expression
 * @param expression or identifier
 * @param the rung to insert the code to
 * @param the operation to generate if called in a recursion
 * @return ok or error code
 */
int gen_expr_left(const item_t left, rung_t rung, uint8_t recursive);

/**
 * @brief generate code for the right side of an expression
 * @param expression or identifier
 * @param the rung to insert the code to
 * @param the operation to generate 
 * @param the modifier 
 * @return ok or error code
 */
int gen_expr_right(const item_t right, rung_t rung, uint8_t op, uint8_t mod);

/**
 * @brief generate code from an assignment statement
 * @param an assignment statement
 * @param the rung to insert the code to
 * @return ok or error code
 */
int gen_ass(const item_t assignment, rung_t rung);

#endif //_CODEGEN_H_
