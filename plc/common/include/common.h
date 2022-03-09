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

#ifndef _COMMON_H_
#define _COMMON_H_

#include <poll.h>
#include <inttypes.h>
#include <stdbool.h>

//#define LOG_TO_FILE

#define STATUS_OK   0
#define STATUS_ERR  -1

#define NEGATE     128 // negate second operand, not return value.
#define BOOLEAN    64
#define NOP        0

#define MAXBUF     256
#define MAXSTR     1024
#define MEDSTR     256
#define SMALLSTR   128
#define SMALLBUF   64
#define TINYBUF    24
#define TINYSTR    32
#define COMMLEN    16

#define LONG_BYTES 8
#define LWORDSIZE  64
#define DWORDSIZE  32
#define WORDSIZE   16
#define BYTESIZE   8

#define BOOL(x) x > 0 ? true : false

#define FIRST_BITWISE    IL_AND
#define FIRST_ARITHMETIC IL_ADD
#define FIRST_COMPARISON IL_GT

#define IS_BITWISE(x)    (x >= FIRST_BITWISE && x < FIRST_ARITHMETIC)
#define IS_ARITHMETIC(x) (x >= FIRST_ARITHMETIC && x < FIRST_COMPARISON)
#define IS_COMPARISON(x) (x >= FIRST_COMPARISON && x < N_IL_INSN)
#define IS_OPERATION(x)  (x >= FIRST_BITWISE && x < N_IL_INSN)

#define OP_VALID(x)      x >= OP_INPUT && x < N_OPERANDS
#define OP_REAL(x)       x == OP_REAL_INPUT          \
                             || x == OP_REAL_MEMORY  \
                             || x == OP_REAL_OUTPUT  \
                             || x == OP_REAL_CONTACT \
                             || x == OP_REAL_MEMIN

typedef enum {
    N_ERR = -20,     // error codes are negative
    ERR_OVFLOW,      //
    ERR_TIMEOUT,     //
    ERR_HARDWARE,    //
    ERR_BADOPERATOR, //
    ERR_BADCOIL,     //
    ERR_BADINDEX,    //
    ERR_BADOPERAND,  //
    ERR_BADFILE,     //
    ERR_BADCHAR,     //
    ERR_BADPROG,     //
} ERRORCODES;

typedef enum {
    IE_PLC,         //
    IE_BADOPERATOR, //
    IE_BADCOIL,     //
    IE_BADINDEX,    //
    IE_BADOPERAND,  //
    IE_BADFILE,     //
    IE_BADCHAR,     //

    N_IE            //
} IL_ERRORCODES;

typedef enum {
    T_BOOL,  // 1 bit
    T_BYTE,  // 8 bit (1 byte)
    T_WORD,  // 16 bit (2 byte)
    T_DWORD, // 32 bit (4 byte)
    T_LWORD, // 64 bit (8 byte)
    T_REAL,  // (8 byte) double floating point number

    N_TYPES  //
} DATATYPES;

typedef enum {
    // operands
    OP_INPUT = 20,   // i
    OP_REAL_INPUT,   // if 21
    OP_FALLING,      // f  22
    OP_RISING,       // r  23
    OP_MEMORY,       // m  24
    OP_REAL_MEMORY,  // mf 25
    OP_COMMAND,      // c  26
    OP_BLINKOUT,     // b  27
    OP_TIMEOUT,      // t  28
    OP_OUTPUT,       // q  29
    OP_REAL_OUTPUT,  // qf 30

    // coils
    OP_CONTACT,      // Q  31
    OP_REAL_CONTACT, // QF 32
    OP_START,        // T  33
    OP_PULSEIN,      // M  34
    OP_REAL_MEMIN,   // MF 35
    OP_WRITE,        // W  36
    OP_END,          // 0  37

    N_OPERANDS       //
} IL_OPERANDS;

// IL instructions
typedef enum {
    // IL OPCODES:
    IL_NOP,   // no operand
    IL_POP,   // )
    IL_RET,   // RET

    // arithmetic LABEL
    IL_JMP,   // JMP

    // subroutine call (unimplemented)
    IL_CAL,   // CAL

    // boolean, no modifier
    IL_SET,   // S
    IL_RESET, // R

    // any operand, only negation
    IL_LD,    // LD
    IL_ST,    // ST

    // any operand, only push
    // boolean, all modifiers
    IL_AND,   // AND
    IL_OR,    // OR
    IL_XOR,   // XOR
    IL_ADD,   // ADD
    IL_SUB,   // SUBTRACT
    IL_MUL,   // MULTIPLY
    IL_DIV,   // DIVIDE
    IL_GT,    // GREATER THAN
    IL_GE,    // GREATER OR EQUAL
    IL_EQ,    // EQUAL
    IL_NE,    // NOT EQUAL
    IL_LT,    // LESS THAN
    IL_LE,    // LESS OR EQUAL

    N_IL_INSN //
} IL_INSN;

// possible LD line statuses
enum LD_STATUS {
    STATUS_UNRESOLVED, //
    STATUS_RESOLVED,   //
    STATUS_FINAL,      //
    STATUS_ERROR,      //

    N_STATUS           //
};

// accepted LD symbols: 0-9 for digits, and
enum LD_SYMBOLS {
    // LD specific operators:
    LD_BLANK = 10,  // blank character
    LD_AND,         // -
    LD_NOT,         // !
    LD_OR,          // |
    LD_NODE,        // +
    LD_COIL,        // ( contact coil
    LD_SET,         // [ set
    LD_RESET,       // ] reset,
    LD_DOWN,        // ) negate coil

    N_LD_SYMBOLS    //
};

#endif /* _COMMON_H_ */
