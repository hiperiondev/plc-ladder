
#include <string.h>
#include <stdio.h>

#include "common.h"
#include "config.h"
#include "codegen.h"
#include "instruction.h"
#include "parser-ld.h"
#include "parser-tree.h"
#include "rung.h"
#include "debug_mem.h"

int gen_expr(const item_t expression, rung_t rung, uint8_t recursive) {
    int rv = STATUS_OK;
    if (expression == NULL || rung == NULL)
        return STATUS_ERR;

    if (expression->tag != TAG_EXPRESSION)
        return STATUS_ERR;

    uint8_t operator = expression->v.exp.op;
    if (!IS_OPERATION(operator))
        return ERR_BADOPERATOR;

    uint8_t modifier = expression->v.exp.mod;
    if (!IS_MODIFIER(modifier))
        return ERR_BADOPERATOR;
    // left operand
    rv = gen_expr_left(expression->v.exp.a, rung, recursive);
    if (rv < 0)
        return rv;

    // right operand
    rv = gen_expr_right(expression->v.exp.b, rung, operator, modifier);

    struct instruction ins;
    memset(&ins, 0, sizeof(struct instruction));

    if (rv == STATUS_OK && modifier == IL_PUSH && IS_OPERATION(operator) && expression->v.exp.b != NULL) {
        ins.operation = IL_POP;
        rv = rung_append(&ins, rung);
    }
    return rv;
}

int gen_expr_left(const item_t left, rung_t rung, uint8_t recursive) {
    int rv = STATUS_OK;
    if (left == NULL)
        return ERR_BADOPERAND;
    uint8_t inner = IL_LD;
    uint8_t mod = IL_NORM;
    if (IS_OPERATION(recursive)) {
        inner = recursive;
        mod = IL_PUSH;
    }
    struct instruction ins;
    memset(&ins, 0, sizeof(struct instruction));
    switch (left->tag) {
        case TAG_IDENTIFIER: // LD
            ins.operation = inner;
            ins.operand = left->v.id.operand;
            ins.modifier = mod;
            ins.byte = left->v.id.byte;
            ins.bit = left->v.id.bit;
            rv = rung_append(&ins, rung);
            break;
        case TAG_EXPRESSION:
            // recursion
            rv = gen_expr(left, rung, inner);
            break;
        default:
            rv = ERR_BADOPERAND;
    }
    return rv;
}

int gen_expr_right(const item_t right, rung_t rung, uint8_t op, uint8_t mod) {
    int rv = STATUS_OK;

    if (right != NULL) {
        struct instruction ins;
        memset(&ins, 0, sizeof(struct instruction));
        switch (right->tag) {
            case TAG_IDENTIFIER:
                ins.operation = op;
                ins.modifier = mod;
                ins.operand = right->v.id.operand;
                ins.byte = right->v.id.byte;
                ins.bit = right->v.id.bit;
                rv = rung_append(&ins, rung);
                break;
            case TAG_EXPRESSION:
                // recursion
                rv = gen_expr(right, rung, op);
                break;
            default:
                rv = ERR_BADOPERAND;
        }
    }
    return rv;
}

int gen_ass(const item_t assignment, rung_t rung) {
    int rv = STATUS_OK;

    if (rung == NULL || assignment == NULL || assignment->tag != TAG_ASSIGNMENT)
        return STATUS_ERR;

    if (assignment->v.ass.left == NULL || assignment->v.ass.left->tag != TAG_IDENTIFIER)
        return ERR_BADOPERAND;

    uint8_t type = assignment->v.ass.type;
    if (!IS_COIL(type))
        return ERR_BADCOIL;

    item_t left = assignment->v.ass.left;

    struct instruction ins;
    memset(&ins, 0, sizeof(struct instruction));

    if (assignment->v.ass.right == NULL)
        return ERR_BADOPERATOR;

    item_t right = assignment->v.ass.right;
    switch (right->tag) {
        case TAG_IDENTIFIER:
            ins.operation = IL_LD;
            ins.modifier = IL_NORM;
            ins.operand = right->v.id.operand;
            ins.byte = right->v.id.byte;
            ins.bit = right->v.id.bit;
            rv = rung_append(&ins, rung);
            break;
        case TAG_EXPRESSION:
            rv = gen_expr(right, rung, 0);
            break;
        default:
            return ERR_BADOPERATOR;
    }

    if (rv == STATUS_OK) {
        switch (type) {
            case LD_DOWN:
                ins.operation = IL_ST;
                ins.modifier = IL_NEG;
                break;

            case LD_RESET:
                ins.operation = IL_RESET;
                ins.modifier = IL_COND;
                break;

            case LD_SET:
                ins.operation = IL_SET;
                ins.modifier = IL_COND;
                break;
            default: // coil
                ins.operation = IL_ST;
                ins.modifier = IL_NORM;
        }
        ins.operand = left->v.id.operand;
        ins.byte = left->v.id.byte;
        ins.bit = left->v.id.bit;
        rv = rung_append(&ins, rung);
    }
    return rv;
}
