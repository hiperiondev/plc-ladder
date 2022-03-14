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

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "config.h"
#include "cmp_instruction.h"
#include "cmp_rung.h"
#include "cmp_parser-tree.h"
#include "mem.h"

// TODO: memory optimization: create a factory to allocate different
// size per node type

item_t tree_mk_identifier(const uint8_t operand, const uint8_t byte, const uint8_t bit) {
    item_t r = (item_t) MEM_CALLOC(1, sizeof(struct item), "tree_mk_identifier A");
    r->tag = TAG_IDENTIFIER;
    r->v.id.operand = operand;
    r->v.id.byte = byte;
    r->v.id.bit = bit;
    return r;
}

item_t tree_mk_expression(const item_t a, const item_t b, const uint8_t op, const uint8_t mod) {
    item_t r = (item_t) MEM_CALLOC(1, sizeof(struct item), "tree_mk_expression A");
    r->tag = TAG_EXPRESSION;
    r->v.exp.op = op;
    r->v.exp.mod = mod;
    r->v.exp.a = a;
    r->v.exp.b = b;
    return r;
}

item_t tree_mk_assignment(const item_t identifier, const item_t expression, const uint8_t type) {
    item_t r = (item_t) MEM_CALLOC(1, sizeof(struct item), "tree_mk_assignment A");
    r->tag = TAG_ASSIGNMENT;
    r->v.ass.left = identifier;
    r->v.ass.right = expression;
    r->v.ass.type = type;
    return r;
}

item_t tree_clear(item_t root) {
    item_t r = root;
    if (root) {
        switch (root->tag) {
            case TAG_EXPRESSION:
                r->v.exp.a = tree_clear(root->v.exp.a);
                r->v.exp.b = tree_clear(root->v.exp.b);
                break;

            case TAG_ASSIGNMENT:
                r->v.ass.left = tree_clear(root->v.ass.left);
                r->v.ass.right = tree_clear(root->v.ass.right);
                break;

            default:
                break;
        }
        // tree leaves
        if (r != NULL) {
            MEM_FREE(r, "tree_clear A");
            memset(r, 0, sizeof(struct item));
            r = (item_t) NULL;
        }
    }
    return r;
}
