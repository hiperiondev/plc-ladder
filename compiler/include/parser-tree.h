#ifndef _PARSER_TREE_H_
#define _PARSER_TREE_H_

#include "common.h"

/*
 *  Node type codes
 */
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
           uint8_t type; //contact, down, set, reset
} *assignment_t;

/**
 * a syntax tree node
 */
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
item_t mk_identifier(const uint8_t operand, const uint8_t byte, const uint8_t bit);

/**
 * @brief make an expression node
 * @param left part
 * @param right part
 * @param operation
 * @param modifier
 * @return a newly allocated tree node 
 */
item_t mk_expression(const item_t a, const item_t b, const uint8_t op, const uint8_t mod);

/**
 * @brief make an assignment node
 * @param left part
 * @param right part
 * @param type of assignment (normal, negative, set, reset)
 * @return a newly allocated tree node 
 */
item_t mk_assignment(const item_t identifier, const item_t expression, const uint8_t type);

/**
 * @brief recursively clear (deallocate) 
 * tree nodes starting from a root node
 */
item_t clear_tree(item_t root);

#endif //__PARSER_TREE_H_
