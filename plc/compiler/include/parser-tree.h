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

#ifndef _PARSER_TREE_H_
#define _PARSER_TREE_H_

#include "common.h"

// node type codes
typedef enum {
    TAG_IDENTIFIER, //
    TAG_EXPRESSION, //
    TAG_ASSIGNMENT, //

    N_TAGS          //
} item_tag_t;

typedef struct identifier {
    uint8_t operand;
    uint8_t byte;
    uint8_t bit;
} *identifier_t;

typedef struct expression {
    struct item *a;
    struct item *b;
        uint8_t op;
        uint8_t mod;
} *expression_t;

typedef struct assignment {
    struct item *left;
    struct item *right;
        uint8_t type; // contact, down, set, reset
} *assignment_t;

// a syntax tree node
typedef struct item {
    item_tag_t tag;
    union {
        struct identifier id;
        struct expression exp;
        struct assignment ass;
    } v;
} *item_t;

/**
 * @brief make an identifier node
 * @param operand
 * @param byte
 * @param bit
 * @return a newly allocated tree node 
 */
item_t tree_mk_identifier(const uint8_t operand, const uint8_t byte, const uint8_t bit);

/**
 * @brief make an expression node
 * @param left part
 * @param right part
 * @param operation
 * @param modifier
 * @return a newly allocated tree node 
 */
item_t tree_mk_expression(const item_t a, const item_t b, const uint8_t op, const uint8_t mod);

/**
 * @brief make an assignment node
 * @param left part
 * @param right part
 * @param type of assignment (normal, negative, set, reset)
 * @return a newly allocated tree node 
 */
item_t tree_mk_assignment(const item_t identifier, const item_t expression, const uint8_t type);

/**
 * @brief recursively clear (deallocate) 
 * tree nodes starting from a root node
 */
item_t tree_clear(item_t root);

#endif //_PARSER_TREE_H_
