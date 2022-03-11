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

#ifndef _UT_DATA_H_
#define _UT_DATA_H_

#include <stdbool.h>
#include <CUnit/CUnit.h>

#include "common.h"
#include "plclib.h"

// boolean ops
void ut_operate_b(void) {
    // invalid operation
    uint8_t op = -1;
    data_t a;
    a.u = 1;
    data_t b;
    b.u = 2;
    // boolean, no modifier
    op = IL_SET;         // S
    data_t r = vm_operate(op, T_BOOL, a, b);
    CU_ASSERT(r.u == 0);

    op = IL_RESET; // R
    r = vm_operate(op, T_BOOL, a, b);
    CU_ASSERT(r.u == 0);

    // boolean, all modifiers
    a.u = 13;
    b.u = 7;

    op = IL_AND; // AND

    r = vm_operate(op, T_BOOL, a, b);
    CU_ASSERT(r.u == true); //logical and(13,7)

    op = IL_AND + NEGATE;

    r = vm_operate(op, T_BOOL, a, b);
    CU_ASSERT(r.u == true); // logical and(13,248)

    op = IL_OR; // OR
    r = vm_operate(op, T_BOOL, a, b);
    CU_ASSERT(r.u == true); // logical or(13,7)

    op = IL_OR + NEGATE;
    r = vm_operate(op, T_BOOL, a, b);
    CU_ASSERT(r.u == true); // logical or(13,248)

    op = IL_XOR; // XOR
    r = vm_operate(op, T_BOOL, a, b);
    CU_ASSERT(r.u == false); // logical xor(13,7)

    op = IL_XOR + NEGATE;
    r = vm_operate(op, T_BOOL, a, b);
    CU_ASSERT(r.u == false); // logical xor(13,248)

    // any operand, only push
    a.u = 2;
    b.u = 0;
    op = IL_ADD; // ADD
    r = vm_operate(op, T_BOOL, a, b);
    CU_ASSERT(r.u == true); // true + false = true

    a.u = 5;
    b.u = 2;
    op = IL_ADD + NEGATE; // ADD

    r = vm_operate(op, T_BOOL, a, b);
    //printf("%d\n", r);
    CU_ASSERT(r.u == true); // true + true = true

    op = IL_SUB; // SUBTRACT

    a.u = 0;
    b.u = 2;
    r = vm_operate(op, T_BOOL, a, b);
    CU_ASSERT(r.u == true); // false - ( - true) = true

    a.u = 5;
    b.u = 2;
    op = IL_SUB + NEGATE; // SUBTRACT

    r = vm_operate(op, T_BOOL, a, b);
    CU_ASSERT(r.u == false); // true - ( - true) = overflow

    op = IL_MUL; // MULTIPLY

    a.u = 5;
    b.u = 5;
    r = vm_operate(op, T_BOOL, a, b); //true * true =  true
    CU_ASSERT(r.u == true);

    a.u = 1;
    b.u = 0;
    op = IL_DIV; // DIVIDE

    r = vm_operate(op, T_BOOL, a, b);
    CU_ASSERT(r.u == true);  //true DIV false = true

    a.u = 5;
    b.u = 2;
    r = vm_operate(op, T_BOOL, a, b);
    CU_ASSERT(r.u == true);  // true / true = true

    op = IL_GT; // GREATER THAN
    r = vm_operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u == true);  //

    op = IL_LE;
    r = vm_operate(op, T_BOOL, a, b);
    CU_ASSERT(r.u == true); //true == true
}

// floating point ops
void ut_operate_r(void) {
    // invalid operation
    uint8_t op = -1;
    data_t a;
    data_t b;

    // any operand, only push
    a.r = 2.0l;
    b.r = 1.0l;
    op = IL_ADD; // ADD
    data_t r = vm_operate(op, T_REAL, a, b);

    CU_ASSERT_DOUBLE_EQUAL(r.r, 3.0l, FLOAT_PRECISION);

    a.r = 5.999l;
    b.r = 2.000l;
    op = IL_ADD + NEGATE; // ADD
    r = vm_operate(op, T_REAL, a, b); // 

    CU_ASSERT_DOUBLE_EQUAL(r.r, 3.999l, FLOAT_PRECISION);

    a.r = 1234.5678l;
    b.r = 0.0001;
    op = IL_SUB; // SUBTRACT
    r = vm_operate(op, T_REAL, a, b);
    CU_ASSERT_DOUBLE_EQUAL(r.r, 1234.5677l, FLOAT_PRECISION);

    a.r = 0.0l;
    b.r = 0.0l;
    op = IL_MUL; // MULTIPLY
    r = vm_operate(op, T_REAL, a, b);
    CU_ASSERT_DOUBLE_EQUAL(r.r, 0.0l, FLOAT_PRECISION);

    a.r = 5.5l;
    b.r = 5.5l;

    op = IL_MUL;
    r = vm_operate(op, T_REAL, a, b);
    CU_ASSERT_DOUBLE_EQUAL(r.r, 30.25l, FLOAT_PRECISION);

    a.r = 1.0l;
    b.r = 0.0l;
    op = IL_DIV; // DIVIDE
    r = vm_operate(op, T_REAL, a, b);
    CU_ASSERT_DOUBLE_EQUAL(r.r, -1.0l, FLOAT_PRECISION);
    //printf("%f\n", r.r); // ??

    a.r = 111111.111111l;
    b.r = -111111.111111l;
    op = IL_DIV; // DIVIDE
    r = vm_operate(op, T_REAL, a, b);
    CU_ASSERT_DOUBLE_EQUAL(r.r, -1.0l, FLOAT_PRECISION);

    a.r = 999999.999999;
    b.r = 333333.333333;
    op = IL_DIV; // DIVIDE
    r = vm_operate(op, T_REAL, a, b);
    CU_ASSERT_DOUBLE_EQUAL(r.r, 3.0l, FLOAT_PRECISION);

    a.r = 555555555555.5l;
    b.r = 0.0000000002;

    op = IL_GT; // GREATER THAN
    r = vm_operate(op, T_REAL, a, b);
    CU_ASSERT_DOUBLE_EQUAL(r.r, 1.0l, FLOAT_PRECISION);

    op = IL_GE; // GREATER OR EQUAL
    r = vm_operate(op, T_REAL, a, b);
    CU_ASSERT_DOUBLE_EQUAL(r.r, 1.0l, FLOAT_PRECISION);

    op = IL_EQ; // EQUAL
    r = vm_operate(op, T_REAL, a, b);
    CU_ASSERT_DOUBLE_EQUAL(r.r, 0.0l, FLOAT_PRECISION);

    op = IL_NE; // NOT EQUAL
    r = vm_operate(op, T_REAL, a, b);
    CU_ASSERT_DOUBLE_EQUAL(r.r, 1.0l, FLOAT_PRECISION);

    op = IL_LT; // LESS THAN
    r = vm_operate(op, T_REAL, a, b);
    CU_ASSERT_DOUBLE_EQUAL(r.r, 0.0l, FLOAT_PRECISION);
}

// scalar ops
void ut_operate(void) {
    // invalid operation
    uint8_t op = -1;
    data_t a;
    a.u = 1;
    data_t b;
    b.u = 2;
    data_t r = vm_operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u == 0);

    op = N_IL_INSN;
    r = vm_operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u == 0);

    // no operand
    op = IL_POP;
    r = vm_operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u == 0);

    op = IL_RET;
    r = vm_operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u == 0);

    // arithmetic LABEL
    op = IL_JMP;
    r = vm_operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u == 0);

    // subroutine call (unimplemented)
    op = IL_CAL;  // CAL
    r = vm_operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u == 0);

    // bitwise, all modifiers
    a.u = 0x1122334455667788;
    b.u = 0xabcdef1234567890;

    op = IL_AND; // AND
    r = vm_operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u == 0x80); // bitwise and(x88,x90)

    op = IL_AND + NEGATE;
    r = vm_operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u == 0x8); // bitwise and(x88, xff - x90)
    //printf("%d\n", r.u);

    op = IL_AND; // AND
    r = vm_operate(op, T_WORD, a, b);
    CU_ASSERT(r.u == 0x7080); // bitwise and(x7788,x7890)

    op = IL_AND + NEGATE;
    r = vm_operate(op, T_WORD, a, b);
    CU_ASSERT(r.u == 0x708); // bitwise and(x7788,xffff-x7890)

    op = IL_AND; // AND
    r = vm_operate(op, T_DWORD, a, b);
    CU_ASSERT(r.u == 0x14467080); // bitwise and(x55667788,x34567890)

    op = IL_AND + NEGATE;
    r = vm_operate(op, T_DWORD, a, b);
    CU_ASSERT(r.u == 0x41200708); // bitwise and(x55667788,
                                  // xffffffff - x34567890)

    op = IL_AND; // AND
    r = vm_operate(op, T_LWORD, a, b);
    CU_ASSERT(r.u == 0x100230014467080); // bitwise and (x1122334455667788,xabcdef1234567890)

    op = IL_AND + NEGATE;
    r = vm_operate(op, T_LWORD, a, b);
    CU_ASSERT(r.u == 0x1022104441200708);

    // bitwise and(
    // x1122334455667788,
    // xffffffffffffffff - xabcdef1234567890)

    op = IL_OR; // OR
    r = vm_operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u == 0x98); // bitwise OR(x88,x90)

    op = IL_OR + NEGATE;
    r = vm_operate(op, T_WORD, a, b);
    CU_ASSERT(r.u == 0xf7ef); // bitwise or(x7788,xffff-x7890)

    op = IL_XOR; // XOR
    r = vm_operate(op, T_DWORD, a, b);
    CU_ASSERT(r.u == 0x61300F18); // bitwise XOR(x55667788,x34567890)

    op = IL_XOR + NEGATE;
    r = vm_operate(op, T_LWORD, a, b);
    CU_ASSERT(r.u == 0x451023a99ECFF0E7);
    // bitwise xor(
    // x1122334455667788,
    // xffffffffffffffff - xabcdef1234567890)

    //any operand, only negation
    op = IL_LD; // LD
    r = vm_operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u == 0);

    op = IL_ST; // ST
    r = vm_operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u == 0);

    // any operand, only push
    a.u = 2;
    b.u = 0;
    op = IL_ADD; // ADD
    r = vm_operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u == 2);

    a.u = 5;
    b.u = 2;
    op = IL_ADD + NEGATE; // ADD
    r = vm_operate(op, T_BYTE, a, b); // 255 - 2 + 5 = 258 = 256 + 2
    //printf("%d\n", r);
    CU_ASSERT(r.u == 2);

    a.u = 0x12345678;
    b.u = 0x1;
    op = IL_SUB; // SUBTRACT
    r = vm_operate(op, T_DWORD, a, b);
    CU_ASSERT(r.u == 0x12345677);
    //printf("%d\n", r.u);

    a.u = 5;
    b.u = 2;
    op = IL_SUB + NEGATE; // SUBTRACT
    r = vm_operate(op, T_BYTE, a, b); // 256 - (255 -2) + 5
    CU_ASSERT(r.u == 8);

    a.u = 0xffffffffffffffff;
    b.u = 0;
    op = IL_MUL; // MULTIPLY
    r = vm_operate(op, T_LWORD, a, b);
    CU_ASSERT(r.u == 0);

    a.u = 0xffffffff;
    b.u = 2;
    op = IL_MUL; // overflow
    r = vm_operate(op, T_WORD, a, b);
    CU_ASSERT(r.u == 0xfffe);

    //printf("%x\n", r.u);

    a.u = 5;
    b.u = 5;

    op = IL_MUL + NEGATE;
    r = vm_operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u == 226);

    a.u = 1;
    b.u = 0;
    op = IL_DIV; // DIVIDE
    r = vm_operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u == 255); // NaN is max int

    a.u = 100000;
    b.u = 100000;
    op = IL_DIV; // DIVIDE
    r = vm_operate(op, T_WORD, a, b);
    CU_ASSERT(r.u == 1); // NaN is max int

    a.u = 1000000000;
    b.u = 100000000;
    op = IL_DIV; // DIVIDE
    r = vm_operate(op, T_DWORD, a, b);
    CU_ASSERT(r.u == 10); // NaN is max int

    a.u = 0x555555555555555;
    b.u = 0x2;

    op = IL_GT; // GREATER THAN
    r = vm_operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u == true); //

    op = IL_GE; // GREATER OR EQUAL
    r = vm_operate(op, T_WORD, a, b);
    CU_ASSERT(r.u == true);

    op = IL_EQ; // EQUAL
    r = vm_operate(op, T_DWORD, a, b);
    CU_ASSERT(r.u == false);

    op = IL_NE; // NOT EQUAL
    r = vm_operate(op, T_LWORD, a, b);
    CU_ASSERT(r.u == true);

    op = IL_LT + NEGATE; // LESS THAN
    r = vm_operate(op, T_BYTE, a, b); // 5 < 253
    CU_ASSERT(r.u == true);

}

#endif //_UT_DATA_H_
