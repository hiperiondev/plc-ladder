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

#ifndef _UT_CG_H_
#define _UT_CG_H_

#include <stddef.h>
#include <CUnit/CUnit.h>

#include "common.h"
#include "parser-tree.h"
#include "rung_vm.h"

void ut_gen_expr() {
    struct rung ru;

    memset(&ru, 0, sizeof(struct rung));

    instruction_t ins;
    // ERROR: null
    int result = gen_expr(NULL, NULL, 0);
    CU_ASSERT(result == STATUS_ERR);

    item_t id1 = tree_mk_identifier(OP_INPUT, 0, 0);
    item_t id2 = tree_mk_identifier(OP_MEMORY, 1, 5);

    // ERROR: wrong tag
    result = gen_expr(id1, &ru, 0);
    CU_ASSERT(result == STATUS_ERR);

    // ERROR: bad operator
    item_t it = tree_mk_expression(NULL, NULL, -1, -1);
    result = gen_expr(it, &ru, 0);
    CU_ASSERT(result == ERR_BADOPERATOR);
    tree_clear(it);

    // ERROR: null left operand
    it = tree_mk_expression(NULL, NULL, IL_AND, IL_NORM);
    result = gen_expr(it, &ru, 0);
    CU_ASSERT(result == ERR_BADOPERAND);
    tree_clear(it);

    // I0.0 AND M1.5 =>
    //
    // LD I0.0
    // AND M1.5
    it = tree_mk_expression(id1, id2, IL_AND, IL_NORM);

    result = gen_expr(it, &ru, 0);
    get(&ru, 0, &ins);

    CU_ASSERT(ins->operation == IL_LD);
    CU_ASSERT(ins->operand == OP_INPUT);
    CU_ASSERT(ins->modifier == IL_NORM);
    CU_ASSERT(ins->byte == 0);
    CU_ASSERT(ins->bit == 0);

    get(&ru, 1, &ins);

    CU_ASSERT(ins->operation == IL_AND);
    CU_ASSERT(ins->operand == OP_MEMORY);
    CU_ASSERT(ins->modifier == IL_NORM);
    CU_ASSERT(ins->byte == 1);
    CU_ASSERT(ins->bit == 5);

    tree_clear(it);
    rung_clear(&ru);

    // null right operand (just evaluate left branch)
    // A AND(null)
    id1 = tree_mk_identifier(OP_INPUT, 0, 0);
    id2 = tree_mk_identifier(OP_MEMORY, 1, 5);
    it = tree_mk_expression(id1, NULL, IL_AND, IL_PUSH);

    result = gen_expr(it, &ru, 0);
    get(&ru, 0, &ins);
    // LD A
    CU_ASSERT(ru.insno == 1);
    CU_ASSERT(ins->operation == IL_LD);
    CU_ASSERT(ins->operand == OP_INPUT);
    CU_ASSERT(ins->modifier == IL_NORM);
    CU_ASSERT(ins->byte == 0);
    CU_ASSERT(ins->bit == 0);

    tree_clear(it);
    rung_clear(&ru);

    // I0.0 OR (M1.1 AND M1.2) =>
    id1 = tree_mk_identifier(OP_INPUT, 0, 0);
    id2 = tree_mk_identifier(OP_MEMORY, 1, 1);
    item_t id3 = tree_mk_identifier(OP_MEMORY, 1, 2);

    item_t inner = tree_mk_expression(id2, id3, IL_AND, IL_NORM);
    item_t outer = tree_mk_expression(id1, inner, IL_OR, IL_PUSH);

    result = gen_expr(outer, &ru, 0);
    //
    // LD I0.0
    // OR(M1.1
    // AND M1.2
    // )
    CU_ASSERT(ru.insno == 4);
    get(&ru, 0, &ins);

    CU_ASSERT(ins->operation == IL_LD);
    CU_ASSERT(ins->operand == OP_INPUT);
    CU_ASSERT(ins->modifier == IL_NORM);
    CU_ASSERT(ins->byte == 0);
    CU_ASSERT(ins->bit == 0);

    get(&ru, 1, &ins);

    CU_ASSERT(ins->operation == IL_OR);
    CU_ASSERT(ins->operand == OP_MEMORY);
    CU_ASSERT(ins->modifier == IL_PUSH);
    CU_ASSERT(ins->byte == 1);
    CU_ASSERT(ins->bit == 1);

    get(&ru, 2, &ins);

    CU_ASSERT(ins->operation == IL_AND);
    CU_ASSERT(ins->operand == OP_MEMORY);
    CU_ASSERT(ins->modifier == IL_NORM);
    CU_ASSERT(ins->byte == 1);
    CU_ASSERT(ins->bit == 2);

    get(&ru, 3, &ins);

    CU_ASSERT(ins->operation == IL_POP);

    tree_clear(outer);
    rung_clear(&ru);

    // (I0.0 AND I0.1) OR (M1.1 AND M1.2) =>
    id1 = tree_mk_identifier(OP_INPUT, 0, 0);
    id2 = tree_mk_identifier(OP_MEMORY, 1, 1);
    id3 = tree_mk_identifier(OP_MEMORY, 1, 2);
    item_t id4 = tree_mk_identifier(OP_INPUT, 0, 1);

    item_t left = tree_mk_expression(id1, id4, IL_AND, IL_NORM);
    item_t right = tree_mk_expression(id2, id3, IL_AND, IL_NORM);
    outer = tree_mk_expression(left, right, IL_OR, IL_PUSH);
    //
    // LD I0.0
    // AND I0.1
    // OR(M1.1
    // AND M1.2
    // )
    result = gen_expr(outer, &ru, 0);

    CU_ASSERT(ru.insno == 5);

    get(&ru, 0, &ins);

    CU_ASSERT(ins->operation == IL_LD);
    CU_ASSERT(ins->operand == OP_INPUT);
    CU_ASSERT(ins->modifier == IL_NORM);
    CU_ASSERT(ins->byte == 0);
    CU_ASSERT(ins->bit == 0);

    get(&ru, 1, &ins);

    CU_ASSERT(ins->operation == IL_AND);
    CU_ASSERT(ins->operand == OP_INPUT);
    CU_ASSERT(ins->modifier == IL_NORM);
    CU_ASSERT(ins->byte == 0);
    CU_ASSERT(ins->bit == 1);

    get(&ru, 2, &ins);

    CU_ASSERT(ins->operation == IL_OR);
    CU_ASSERT(ins->operand == OP_MEMORY);
    CU_ASSERT(ins->modifier == IL_PUSH);
    CU_ASSERT(ins->byte == 1);
    CU_ASSERT(ins->bit == 1);

    get(&ru, 3, &ins);

    CU_ASSERT(ins->operation == IL_AND);
    CU_ASSERT(ins->operand == OP_MEMORY);
    CU_ASSERT(ins->modifier == IL_NORM);
    CU_ASSERT(ins->byte == 1);
    CU_ASSERT(ins->bit == 2);

    get(&ru, 4, &ins);

    CU_ASSERT(ins->operation == IL_POP);

    tree_clear(outer);
    rung_clear(&ru);

    // (I0.0 AND (I0.1 OR I0.2)) OR (M1.1 AND M1.2) =>
    id1 = tree_mk_identifier(OP_INPUT, 0, 0);
    id2 = tree_mk_identifier(OP_MEMORY, 1, 1);
    id3 = tree_mk_identifier(OP_MEMORY, 1, 2);
    id4 = tree_mk_identifier(OP_INPUT, 0, 1);
    item_t id5 = tree_mk_identifier(OP_INPUT, 0, 2);

    inner = tree_mk_expression(id4, id5, IL_OR, IL_NORM);

    left = tree_mk_expression(id1, inner, IL_AND, IL_PUSH);
    right = tree_mk_expression(id2, id3, IL_AND, IL_NORM);

    outer = tree_mk_expression(left, right, IL_OR, IL_PUSH);

    //
    // LD I0.0
    // AND(I0.1
    // OR I0.2
    // )
    // OR(M1.1
    // AND M1.2
    // )

    result = gen_expr(outer, &ru, 0);

    CU_ASSERT(ru.insno == 7);

    get(&ru, 0, &ins);

    CU_ASSERT(ins->operation == IL_LD);
    CU_ASSERT(ins->operand == OP_INPUT);
    CU_ASSERT(ins->modifier == IL_NORM);
    CU_ASSERT(ins->byte == 0);
    CU_ASSERT(ins->bit == 0);

    get(&ru, 1, &ins);

    CU_ASSERT(ins->operation == IL_AND);
    CU_ASSERT(ins->operand == OP_INPUT);
    CU_ASSERT(ins->modifier == IL_PUSH);
    CU_ASSERT(ins->byte == 0);
    CU_ASSERT(ins->bit == 1);

    get(&ru, 2, &ins);

    CU_ASSERT(ins->operation == IL_OR);
    CU_ASSERT(ins->operand == OP_INPUT);
    CU_ASSERT(ins->modifier == IL_NORM);
    CU_ASSERT(ins->byte == 0);
    CU_ASSERT(ins->bit == 2);

    get(&ru, 3, &ins);
    CU_ASSERT(ins->operation == IL_POP);

    get(&ru, 4, &ins);
    CU_ASSERT(ins->operation == IL_OR);
    CU_ASSERT(ins->operand == OP_MEMORY);
    CU_ASSERT(ins->modifier == IL_PUSH);
    CU_ASSERT(ins->byte == 1);
    CU_ASSERT(ins->bit == 1);

    get(&ru, 5, &ins);
    CU_ASSERT(ins->operation == IL_AND);
    CU_ASSERT(ins->operand == OP_MEMORY);
    CU_ASSERT(ins->modifier == IL_NORM);
    CU_ASSERT(ins->byte == 1);
    CU_ASSERT(ins->bit == 2);

    get(&ru, 6, &ins);

    CU_ASSERT(ins->operation == IL_POP);

    tree_clear(outer);
    rung_clear(&ru);
}

void ut_gen_ass() {
    struct rung ru;
    memset(&ru, 0, sizeof(struct rung));

    char dump[MAXSTR * MAXBUF];
    memset(dump, 0, MAXBUF * MAXSTR);
    //ERROR: null
    int result = gen_ass(NULL, NULL);
    CU_ASSERT(result == STATUS_ERR);

    item_t id1 = tree_mk_identifier(OP_INPUT, 0, 0);
    item_t exp = tree_mk_expression(id1, NULL, IL_LD, IL_NORM);

    //ERROR: not an assignment

    result = gen_ass(exp, &ru);
    CU_ASSERT(result == STATUS_ERR);

    //ERROR: left part not an identifier

    item_t ass = tree_mk_assignment(exp, id1, LD_COIL);
    result = gen_ass(ass, &ru);
    CU_ASSERT(result == ERR_BADOPERAND);
    //tree_clear(ass);

    //ERROR: wrong type
    ass = tree_mk_assignment(id1, NULL, 0);
    result = gen_ass(ass, &ru);
    CU_ASSERT(result == ERR_BADCOIL);
    //tree_clear(ass);

    //ERROR: right part not an identifier or expression
    ass = tree_mk_assignment(id1, NULL, LD_COIL);
    result = gen_ass(ass, &ru);
    CU_ASSERT(result == ERR_BADOPERATOR);
    //tree_clear(ass);

    //assign identifier1 := identifier2
    item_t id2 = tree_mk_identifier(OP_MEMORY, 1, 8);
    id1 = tree_mk_identifier(OP_INPUT, 0, 0);
    exp = tree_mk_expression(id1, NULL, IL_LD, IL_NORM);
    ass = tree_mk_assignment(id2, id1, LD_COIL);

    result = gen_ass(ass, &ru);
    CU_ASSERT(result == STATUS_OK);
    //LD A
    //ST B
    CU_ASSERT(ru.insno == 2);

    instruction_t ins;

    get(&ru, 0, &ins);

    CU_ASSERT(ins->operation == IL_LD);
    CU_ASSERT(ins->operand == OP_INPUT);
    CU_ASSERT(ins->modifier == IL_NORM);
    CU_ASSERT(ins->byte == 0);
    CU_ASSERT(ins->bit == 0);

    get(&ru, 1, &ins);

    CU_ASSERT(ins->operation == IL_ST);
    CU_ASSERT(ins->operand == OP_MEMORY);
    CU_ASSERT(ins->modifier == IL_NORM);
    CU_ASSERT(ins->byte == 1);
    CU_ASSERT(ins->bit == 8);

    tree_clear(ass);
    rung_clear(&ru);
    //assign identifier1 := expression
    id2 = tree_mk_identifier(OP_MEMORY, 1, 8);
    id1 = tree_mk_identifier(OP_INPUT, 0, 0);
    exp = tree_mk_expression(id1, NULL, IL_AND, IL_NORM);
    ass = tree_mk_assignment(id2, exp, LD_DOWN);

    result = gen_ass(ass, &ru);
    CU_ASSERT(result == STATUS_OK);
    //LD A
    //ST B
    CU_ASSERT(ru.insno == 2);

    get(&ru, 1, &ins);

    CU_ASSERT(ins->operation == IL_ST);
    CU_ASSERT(ins->operand == OP_MEMORY);
    CU_ASSERT(ins->modifier == IL_NEG);
    CU_ASSERT(ins->byte == 1);
    CU_ASSERT(ins->bit == 8);

    tree_clear(ass);
    rung_clear(&ru);

    //a := (b or null)
    id2 = tree_mk_identifier(OP_CONTACT, 0, 1);
    id1 = tree_mk_identifier(OP_INPUT, 0, 0);
    exp = tree_mk_expression(id1, NULL, IL_OR, IL_PUSH);
    ass = tree_mk_assignment(id2, exp, LD_COIL);

    result = gen_ass(ass, &ru);
    CU_ASSERT(result == STATUS_OK);
    //LD A
    //ST B
    CU_ASSERT(ru.insno == 2);

    get(&ru, 1, &ins);

    CU_ASSERT(ins->operation == IL_ST);
    CU_ASSERT(ins->operand == OP_CONTACT);
    CU_ASSERT(ins->modifier == IL_NORM);
    CU_ASSERT(ins->byte == 0);
    CU_ASSERT(ins->bit == 1);

    //dump_rung(&ru, dump);
    //printf("%s\n", dump);

    tree_clear(ass);
    rung_clear(&ru);

    //assign Q0/1 := I0/1 OR(I0/2)

    id2 = tree_mk_identifier(OP_INPUT, 0, 2);
    id1 = tree_mk_identifier(OP_INPUT, 0, 1);
    item_t q1 = tree_mk_identifier(OP_INPUT, 0, 1);
    item_t exp1 = tree_mk_expression(id1, NULL, IL_AND, IL_NORM);
    item_t exp2 = tree_mk_expression(id2, NULL, IL_AND, IL_NORM);
    exp = tree_mk_expression(exp1, exp2, IL_OR, IL_PUSH);
    ass = tree_mk_assignment(q1, exp, LD_DOWN);
    result = gen_ass(ass, &ru);

    memset(dump, 0, MAXBUF * MAXSTR);
    rung_dump(&ru, dump);
    //printf("%s\n", dump);

    const char *expected = ""
            "0.LD i0/1\n"
            "1.OR(i0/2\n"
            "2.)\n"
            "3.ST!i0/1\n";

    CU_ASSERT_STRING_EQUAL(dump, expected);
}

#endif //_UT_CG_H_
