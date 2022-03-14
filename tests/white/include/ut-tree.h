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

#ifndef _UT_TREE_H_
#define _UT_TREE_H_

#include <stdbool.h>
#include <CUnit/CUnit.h>

#include "common.h"
#include "vm_plclib.h"
#include "cmp_parser-tree.h"

void ut_mk_identifier() {
    uint8_t op = OP_INPUT;

    uint8_t byte = 1;
    uint8_t bit = 1;

    item_t id = tree_mk_identifier(op, byte, bit);

    CU_ASSERT(id->tag == TAG_IDENTIFIER);
    CU_ASSERT(id->v.id.operand == op);
    CU_ASSERT(id->v.id.byte == byte);
    CU_ASSERT(id->v.id.bit == bit);

    id = tree_clear(id);
    CU_ASSERT(id == NULL);
}

void ut_mk_expression() {
    item_t id1 = tree_mk_identifier(OP_INPUT, 0, 0);
    item_t id2 = tree_mk_identifier(OP_MEMORY, 1, 8);
    //degenarate input does not understand christ
    item_t dummy = tree_mk_expression(NULL, NULL, 0, 0);
    dummy = tree_clear(dummy);

    item_t it = tree_mk_expression(id1, id2, IL_AND, IL_PUSH);
    CU_ASSERT(it->tag == TAG_EXPRESSION);
    CU_ASSERT(it->v.exp.op == IL_AND);
    CU_ASSERT(it->v.exp.mod == IL_PUSH);
    CU_ASSERT(it->v.exp.a->v.id.operand == OP_INPUT);
    CU_ASSERT(it->v.exp.b->v.id.operand == OP_MEMORY);
    it = tree_clear(it);
    CU_ASSERT(it == NULL);
    CU_ASSERT(id1->tag == 0);
    CU_ASSERT(id2->tag == 0);
}

void ut_mk_assignment() {
    item_t dummy = tree_mk_assignment(NULL, NULL, 0);
    CU_ASSERT(dummy->tag == TAG_ASSIGNMENT);
    dummy = tree_clear(dummy);

    item_t id1 = tree_mk_identifier(OP_INPUT, 0, 0);
    item_t id2 = tree_mk_identifier(OP_MEMORY, 1, 8);
    item_t exp = tree_mk_expression(id1, NULL, IL_LD, IL_NORM);
    item_t ass = tree_mk_assignment(id2, exp, LD_COIL);
    CU_ASSERT(ass->v.ass.left->v.id.operand == OP_MEMORY);
    CU_ASSERT(ass->v.ass.right->v.exp.a->v.id.operand == OP_INPUT);
    CU_ASSERT(ass->v.ass.type == LD_COIL);
    ass = tree_clear(ass);
}

#endif //_UT_TREE_H_
