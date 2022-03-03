
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "config.h"
#include "instruction.h"
#include "rung.h"
#include "parser-tree.h"

// TODO: memory optimization: create a factory to allocate different
// size per node type

item_t tree_mk_identifier(const uint8_t operand, const uint8_t byte, const uint8_t bit) {
    item_t r = (item_t)calloc(1, sizeof(struct item));
    r->tag = TAG_IDENTIFIER;
    r->v.id.operand = operand;
    r->v.id.byte = byte;
    r->v.id.bit = bit;
    return r;
}

item_t tree_mk_expression(const item_t a, const item_t b, const uint8_t op, const uint8_t mod) {
    item_t r = (item_t)calloc(1, sizeof(struct item));
    r->tag = TAG_EXPRESSION;
    r->v.exp.op = op;
    r->v.exp.mod = mod;
    r->v.exp.a = a;
    r->v.exp.b = b;
    return r;
}

item_t tree_mk_assignment(const item_t identifier, const item_t expression, const uint8_t type) {
    item_t r = (item_t)calloc(1, sizeof(struct item));
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
            free(r);
            memset(r, 0, sizeof(struct item));
            r = (item_t) NULL;
        }
    }
    return r;
}
