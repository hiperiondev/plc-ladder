
#include <ctype.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "common.h"
#include "config.h"
#include "codegen.h"
#include "instruction.h"
#include "parser-il.h"
#include "parser-tree.h"
#include "parser-ld.h"
#include "rung.h"
#include "util.h"

/****************** parse ladder files **********************/
/*

 1. read a text file and store it in array Lines.
 2. parse each unresolved line up to '+','(',0, blank, '|'.
 a. parse grammar.
 i. read next character
 ii. increase line position counter
 iii. if unaccepted character, return error. else return accepted character number.

 b. if blank or 0,'|', empty value for the line. if 0 set it resolved.
 c. if '(',
 i. see if it is a coil. (expect Q,S,R,T,D,M,W followed by number)
 ii. resolve coils as an IL assignment statement.
 iii. mark line as resolved.
 d. if '-' do nothing and go to next character.
 e. if '+' stop and continue with next line.
 f. if ! negate next operand by setting normally closed mode
 g. otherwise operand is expected(i,q,f,r,m,t,c,b)
 if operand read number following 
 if there is a number get the number and its number of digits
 compare number with max no for operand
 resolve operand[number] as an IL identifier
 if in normally closed mode negate 
 create IL expression with it AND current IL expression
 
 3.reset normally closed mode
 (At this point, all lines have been resolved,or paused at a '+')

 4. parse vertically
 while there are unresolved lines(minmin returns non negative)
 resolve lines
 find next eligible node for vertical parsing

 parse downwards:
 get character at (line, cursor)
 a. if '|' do nothing
 b. Make OR expression of all '+' lines
 c. Replace expression at each '+' line with total OR

 */

rung_t r, *rungs;

int minmin(const int *arr, int min, int max) {
//for an array arr of integers ,return the smallest of indices i so that 
//arr[i] =  min(arr) >= min 
    int i;
    int v = MAXSTR;		//cant be more than length  of line
    int r = STATUS_ERR;
    for (i = max - 1; i >= 0; i--) {
        if (arr[i] <= v && arr[i] >= min) {
            v = arr[i];
            r = i;
        }
    }
    return r;
}

uint8_t digits(unsigned int i) {
    if (i > 100)
        return 3;
    else if (i > 10)
        return 2;
    else
        return 1;
}

/***********************************************************************/
int handle_coil(const int type, ld_line_t line) {
//(expect Q,T,M,W followed by byte / bit)
    int rv = STATUS_OK;
    uint8_t byte = 0;
    uint8_t bit = 0;
    int c = read_char(line->buf, ++line->cursor);
    if (c >= OP_CONTACT && c < OP_END) {
        int operand = c;
        c = read_char(line->buf, line->cursor);
        rv = extract_arguments(line->buf + (++line->cursor), &byte, &bit);
        if (rv == STATUS_OK) {
            item_t identifier = mk_identifier(operand, byte, bit);
            line->stmt = mk_assignment(identifier, line->stmt, type);
            line->status = STATUS_RESOLVED;
        } else {
            rv = ERR_BADINDEX;
            line->status = STATUS_ERROR;
        }
    } else {
        rv = ERR_BADCOIL;
        line->status = STATUS_ERROR;
    }
    return rv;
}

int handle_operand(int operand, uint8_t negate, ld_line_t line) {
    int rv = STATUS_OK;
    uint8_t byte = 0;
    uint8_t bit = 0;
    if (operand >= OP_INPUT && operand < OP_CONTACT) {	//valid input symbol
        rv = extract_arguments(line->buf + (++line->cursor), &byte, &bit);
        if (rv == STATUS_OK) {
            /*byte + slash + bit*/
            line->cursor += digits((unsigned int) byte) + 2;

            item_t identifier = mk_identifier(operand, byte, bit);
            line->stmt = mk_expression(identifier, line->stmt, IL_AND, negate ? IL_NEG : IL_NORM);
        } else {
            rv = ERR_BADINDEX;
            line->status = STATUS_ERROR;
        }
    } else {
        rv = ERR_BADOPERAND;
        line->status = STATUS_ERROR;
    }
    line->cursor++;
    return rv;
}

uint8_t read_char(const char *line, unsigned int c) {
//read ONE character from line[idx]
//parse grammatically:
    int r = 0;
    if (line == NULL || c > strlen(line))
        return STATUS_ERR;
    if (line[c] == 0 || line[c] == ';' || line[c] == '\n' || line[c] == '\r')
        return OP_END;
    if (line[c] == ' ' || line[c] == '.' || line[c] == '#' || line[c] == '\t')
        return LD_BLANK;
    if (isdigit(line[c]))
        return line[c] - '0';
    switch (line[c]) {
        case '(': // COIL
            r = LD_COIL;
            break;
        case '-': // horizontal line
            r = LD_AND;
            break;
        case '|': // vertical line
            r = LD_OR;
            break;
        case '!': // normally closed
            r = LD_NOT;
            break;
        case '+': //
            r = LD_NODE;
            break;
        case '[': // set output
            r = LD_SET;
            break;
        case ']': // reset output
            r = LD_RESET;
            break;
        case ')': // down timer
            r = LD_DOWN;
            break;
        case 'i': // input
            r = OP_INPUT;
            break;
        case 'f': // falling edge
            r = OP_FALLING;
            break;
        case 'r': // rising Edge
            r = OP_RISING;
            break;
        case 'm': // pulse of counter
            r = OP_MEMORY;
            break;
        case 't': // timer.q
            r = OP_TIMEOUT;
            break;
        case 'c': // read command
            r = OP_COMMAND;
            break;
        case 'b': // blinker
            r = OP_BLINKOUT;
            break;
        case 'q': // output value
            r = OP_OUTPUT;
            break;
        case 'Q': // dry contact output
            r = OP_CONTACT;
            break;
        case 'T': // start timer
            r = OP_START;
            break;
        case 'M': // pulse to counter
            r = OP_PULSEIN;
            break;
        case 'W': // write response
            r = OP_WRITE;
            break;
        default:
            r = (uint8_t) ERR_BADCHAR; // error
    }
// return value or error
    return r;
}

int parse_ld_line(ld_line_t line) {
    int rv = STATUS_OK;
    if (line == (ld_line_t) NULL)
        return STATUS_ERR;

    int c = LD_AND; //default character = '-'
    uint8_t n_mode = false;

    while (line->status == STATUS_UNRESOLVED && c != LD_NODE) {	// loop
        c = read_char(line->buf, line->cursor);
        switch (c) {
            case LD_NODE:	// PAUSE
                break;
            case ERR_BADCHAR:
            case (uint8_t) STATUS_ERR:
                rv = STATUS_ERR;
                line->status = STATUS_ERROR;
                break;
            case OP_END: //this should happen only if line ends without a valid coil
                line->status = STATUS_RESOLVED;
                line->stmt = NULL;	// clear_tree(line->stmt);
                break;
            case LD_OR:
            case LD_BLANK:  // if blank or '|', empty value for the line.
                line->cursor++;
                line->stmt = NULL;	//clear_tree(line->stmt);
                break;
            case LD_NOT:
                n_mode = true;	// normally closed mode
                // no break
            case LD_AND:
                line->cursor++;
                break;
            case LD_COIL:	// see if it is a coil: ()[]
            case LD_SET:
            case LD_RESET:
            case LD_DOWN:
                rv = handle_coil(c, line);
                break;
            default:  // otherwise operand is expected(i,q,f,r,m,t,c,b)
                rv = handle_operand(c, n_mode, line);
                n_mode = false;
                break;
        }
    }
    if (rv < STATUS_OK)
        line->stmt = clear_tree(line->stmt);
    return rv;
}

int horizontal_parse(unsigned int length, ld_line_t *program) {
    int rv = 0;
    int i = 0;
    for (; i < length; i++) {
        rv = parse_ld_line(program[i]);
        if (rv < STATUS_OK)
            return rv;
    }
    return rv;
}

int find_next_node(const ld_line_t *program, unsigned int start, unsigned int lines) {
    int cursors[lines];
    int i = 0;
    int found = STATUS_ERR;
    for (; i < lines && program != NULL; i++) {
        if (program[i] != NULL && program[i]->status == STATUS_UNRESOLVED) {
            cursors[i] = program[i]->cursor;
            found = STATUS_OK;
        } else
            cursors[i] = -1;
    }
    if (found == STATUS_OK)
        return minmin(cursors, start, lines);
    return found;
}

int vertical_parse(unsigned int start, unsigned int length, ld_line_t *program) {
    int rv = STATUS_OK;
    if (program == NULL)
        return STATUS_ERR;

    if (program[start] == NULL)
        return STATUS_ERR;

    item_t or = NULL;
    int cursor = program[start]->cursor;
    int current = start;
    int backtrack = start;
    int last = start;
    // first pass: generate OR expression
    for (; current < length + 1; current++) {	// for each line
        if (current == length //overflow
        || program[current]->cursor < cursor || !IS_VERTICAL(read_char(program[current]->buf, cursor))) {
            // vertical line interrupted, reset OR expression
            for (backtrack = current - 1; backtrack >= last; backtrack--) {
                // backtrack, replace all expressions on nodes with OR
                if (read_char(program[backtrack]->buf, cursor) == LD_NODE)
                    program[backtrack]->stmt = or;
            }
            last = current;
            or = NULL;
            continue;
        }
        if (read_char(program[current]->buf, cursor) == LD_NODE) {
            // do an OR of all nodes expressions
            if (program[current]->stmt != NULL)
                or = mk_expression(program[current]->stmt, or, IL_OR, IL_PUSH);
        } // otherwise it's LD_OR, just continue
        program[current]->cursor++;
    }
    return rv;
}

static unsigned int program_length(const char lines[][MAXSTR], unsigned int max) {
    int i = 0;
    for (; i < max; i++) {
        if (lines == NULL || lines[i] == NULL || lines[i][0] == 0)
            break;
    }
    return i;
}

ld_line_t* construct_program(const char lines[][MAXSTR], unsigned int length) {
    ld_line_t * program = (ld_line_t *)calloc(length, sizeof(ld_line_t));

    int i = 0;
    for (; i < length; i++) { // for each line construct ld_line
        if (lines != NULL) {
            ld_line_t line  = (ld_line_t)calloc(1, sizeof(struct ld_line));
            line->cursor = 0;
            line->status = STATUS_UNRESOLVED;
            line->buf = (char *)calloc(1, MAXSTR);
            memcpy(line->buf, lines[i],MAXSTR);
            line->stmt = NULL;
            program[i] = line;
        }
    }
    return program;
}

void destroy_program(unsigned int length, ld_line_t *program) {
    int i = 0;
    for (; i < length; i++) { // for each line destroy ld_line
        free(program[i]);
        program[i] = NULL;
    }
    free(program);
}

rung_t* generate_code(unsigned int length, const char *name, const ld_line_t *program, rung_t *rungs, uint8_t *rungno) {
    int rv = STATUS_OK;
    r = mk_rung(name, rungs, rungno);

    int i = 0;
    for (; i < length && rv == STATUS_OK; i++) {
        r->code = append_line(trunk_whitespace(program[i]->buf), r->code);
        if (program[i]->stmt != NULL && program[i]->stmt->tag == TAG_ASSIGNMENT)
            rv = gen_ass(program[i]->stmt, r);
    }
    return &r;
}

/***************************entry point*******************************/
rung_t* parse_ld_program(const char *name, const char lines[][MAXSTR]) {
    rungs = NULL;
    uint8_t rungno = 0;

    int rv = STATUS_OK;

    unsigned int len = program_length(lines, MAXBUF);
    ld_line_t *program = construct_program(lines, len);

    int node = 0;
    while (rv >= STATUS_OK && node >= 0) {
        rv = horizontal_parse(len, program);
        if (rv >= STATUS_OK) {

            node = find_next_node(program, node, len);
        }
        if (node >= 0) {

            rv = vertical_parse(node, len, program);
        }
    }
    if (rv == STATUS_OK) {
        rungs = generate_code(len, name, program, rungs, &rungno);
    }

    destroy_program(len, program);
    return rungs;
}
