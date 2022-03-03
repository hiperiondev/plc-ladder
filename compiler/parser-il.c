
#include <ctype.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "config.h"
#include "instruction.h"
#include "parser-ld.h"
#include "parser-tree.h"
#include "rung.h"
#include "util.h"
#include "parser-il.h"

/*IL_task
 parsing:
 2. switch operator:
 valid ones:
 )    pop
 only boolean:(bitwise if operand is byte)

 S     set
 R     reset

 AND
 OR
 XOR
 any:
 LD    load
 ST    store
 ADD
 SUB
 MUL
 DIV
 GT    >
 GE    >=
 NE    <>
 EQ    ==
 LE    <=
 LT    <
 JMP
 unimplemented:
 CAL
 RET

 3. switch modifier
 ) cannot have one
 S, R can only have ' '
 LD, ST cannot have (
 AND, OR, XOR can have all 3
 the rest can only have '('
 if '!' set negate
 if '(' push stack
 4. switch operand
 valid ones:
 BOOL_DI    digital input
 R          rising edge
 F          falling edge
 DQ         digital output
 MH         memory high byte
 ML         memory low byte
 MP         pulse byte: 0000-SET-RESET-EDGE-VALUE
 B          blinker
 TQ         timer output
 TI         timer start
 W          serial output
 C          serial input
 5. resolve operand byte: 0 to MAX
 6. resolve operand bit: 0 to BYTESIZE
 7. execute command if no errors
 */

rung_t il_r;

extern const char IlCommands[N_IL_INSN][LABELLEN];

const char *IlErrors[N_IE] = {
        "Unknown error",         //
        "Invalid operator",      //
        "Invalid output",        //
        "Invalid numeric index", //
        "Invalid operand",       //
        "File does not exist",   //
        "Unreadable character"   //
        };

// TODO: IL multi byte type operations
// literals
// MOD

///////////////////// INSTRUCTION LIST /////////////////////

/// LEX ///

int parse_il_extract_number(const char *line) { //read characters from string line
    int i, n = 0;
    if (line == NULL) {

        return STATUS_ERR;
    }
    for (i = 0; isdigit(line[i]); i++) {
        n = 10 * n + (line[i] - '0');
    }
    if (i == 0) {
        // no digits read
        return STATUS_ERR;
    }
    return n;
// return number read or error
}

int parse_il_extract_arguments(const char *buf, uint8_t *byte, uint8_t *bit) {
    // read first numeric chars after operand
    // store byte
    *byte = parse_il_extract_number(buf);
    if (*byte == (uint8_t) STATUS_ERR) {

        return ERR_BADINDEX;
    }
    // find '/'. if not found truncate, return.
    char *cursor = strchr(buf, '/');
    *bit = BYTESIZE;
    if (cursor) {
        if (!isdigit(cursor[1]) || cursor[1] > '7') {

            return ERR_BADINDEX;
        } else {
            *bit = cursor[1] - '0';
        }
    }
    return STATUS_OK;
}

uint8_t parse_il_read_operand(const char *line, unsigned int index) { // read ONE character from line[idx]
// parse grammatically:
    int r = STATUS_OK;
    if (line == NULL || index > strlen(line))
        return STATUS_ERR;

    char c = tolower(line[index]);

    switch (c) {
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
        default:
            r = (uint8_t) ERR_BADCHAR; // error
    }
// return value or error
    return r;
}

uint8_t parse_il_read_type(const char *line, uint8_t *operand, unsigned int index) { // read characters from line[idx]
// parse grammatically:
    int r = STATUS_OK;
    if (line == NULL || index > strlen(line))
        return STATUS_ERR;

    if (tolower(line[index]) == 'f') {
        switch (*operand) {
            case OP_INPUT:
                *operand = OP_REAL_INPUT;
                break;
            case OP_MEMORY:
                *operand = OP_REAL_MEMORY;
                break;
            case OP_OUTPUT:
                *operand = OP_REAL_OUTPUT;
                break;
            default:
                r = STATUS_ERR;
                break;
        }
    }
// return ok or error
    return r;
}

void parse_il_read_line_trunk_comments(char *line) {
    unsigned int i = 0;
    unsigned int idx = 0;
    while (line != NULL && line[idx] != 0 && line[idx] != '\n' && line[idx] != ';')
        idx++;

    for (i = idx; line != NULL && i < MAXSTR; i++)
        line[i] = 0; // trunc comments
}

void parse_il_trunk_label(const char *line, char *buf, char *label_buf) {
    int i = 0;
    if (line == NULL || buf == NULL || label_buf == NULL)
        return;

    char *str = strrchr(line, ':');
    if (str) {
        while (line + i + 1 != str + 1) {
            label_buf[i] = line[i];
            i++;
        }
        strcpy(buf, str + 1);
    } else
        strcpy(buf, line);
}

#define IS_WHITESPACE(x) (x == ' ' || x == '\t' || x == '\n' || x == '\r')
char* parse_il_trunk_whitespace(char *line) {
    if (line == NULL) {

        return NULL;
    }
    int n = strlen(line);
    char *buf = (char*) malloc(n + 1);
    memset(buf, 0, n + 1);

    // trim left
    int i = 0;
    while (i < n && IS_WHITESPACE(line[i]))
        i++;
    int j = 0;
    while (i < n)
        buf[j++] = line[i++];

    // trim right
    for (j = strlen(buf) - 1; j >= 0; j--)
        if (IS_WHITESPACE(buf[j]))
            buf[j] = 0;
        else if (buf[j] != 0)
            break;
    memset(line, 0, n);
    sprintf(line, "%s", buf);
    free(buf);

    return line;
}

uint8_t parse_il_read_modifier(const char *buf, char **pos) {
    uint8_t modifier = 0;
    if (buf == NULL || pos == NULL)
        return STATUS_ERR;

    char *str = strchr(buf, '(');
    if (str) // push stack
        modifier = IL_PUSH;
    else { // negate
        str = strchr(buf, '!');
        if (str)
            modifier = IL_NEG;
        else { // conditional branch
            str = strchr(buf, '?');
            if (str)
                modifier = IL_COND;
            else { // normal
                str = strchr(buf, ' ');
                if (str)
                    modifier = IL_NORM;
                else
                    modifier = NOP;
            }
        }
    }
    *pos = str;
    return modifier;
}

uint8_t parse_il_read_operator(const char *buf, const char *stop) {
    uint8_t op = 0;
    int i = 0;
    char *cursor = 0;
    char op_buf[LABELLEN];
    memset(op_buf, 0, LABELLEN);
    if (buf == NULL)
        return STATUS_ERR;
    if (stop) {
        cursor = (char*) buf;
        i = 0;
        memset(op_buf, 0, LABELLEN);
        while (cursor + i != stop && i < LABELLEN) {
            op_buf[i] = cursor[i];
            i++;
        }
    } else {
        if (strlen(buf) < LABELLEN)
            strcpy(op_buf, buf);
        else
            return N_IL_INSN;
    }
    op = N_IL_INSN;
    for (i = 0; i < N_IL_INSN; ++i) {
        if (!strcmp(op_buf, IlCommands[i]))
            op = i;
    }
    return op;
}

int parse_il_find_arguments(const char *buf, uint8_t *operand, uint8_t *byte, uint8_t *bit) {
    int ret = STATUS_OK;

    if (buf == NULL)
        return STATUS_ERR;

    char *str = strchr(buf, '%');
    if (!str)
        return ERR_BADCHAR;

    // read first non-numeric char after '%'. if not found return error. store operand. check if invalid (return error).
    int index = 1;
    if (isalpha(str[index])) {
        *operand = parse_il_read_operand(str, index);
        index++;
    } else {

        return ERR_BADOPERAND;
    }
    if (*operand == (uint8_t) ERR_BADCHAR) {

        return ERR_BADCHAR;
    }
    if (isalpha(str[index])) {
        ret = parse_il_read_type(str, operand, index);
        index++;
        if (ret != STATUS_OK) {

            return ret;
        }
    }
    if (index > strlen(str)) {

        return ERR_BADINDEX;
    }
    ret = parse_il_extract_arguments(str + index, byte, bit);

    return ret;
}

/// PARSE & GENERATE CODE ///
int parse_il_line(const char *line, rung_t r) { // line format:[label:]<operator>[<modifier>[%<operand><byte>[/<bit>]]|<label>][;comment]
    char tmp[MAXSTR];
    char buf[MAXSTR];
    char label_buf[MAXSTR];
    char *pos = NULL;
    uint8_t byte = 0;
    uint8_t bit = 0;
    uint8_t modifier = 0;
    uint8_t operand = 0;
    uint8_t oper = 0;
    struct instruction op;

    memset(&op, 0, sizeof(struct instruction));
    memset(label_buf, 0, MAXBUF);
    memset(tmp, 0, MAXSTR);
    memset(buf, 0, MAXSTR);

    if (line == NULL || r == NULL)
        return STATUS_ERR;

    sprintf(tmp, "%s", line);

    r->code = rung_append_line(parse_il_trunk_whitespace(tmp), r->code);

    parse_il_read_line_trunk_comments(tmp);
    parse_il_trunk_label(tmp, buf, label_buf);
    parse_il_trunk_whitespace(label_buf);
    parse_il_trunk_whitespace(buf);

    sprintf(op.label, "%s", label_buf);

    modifier = parse_il_read_modifier(buf, &pos);
    oper = parse_il_read_operator(buf, pos);

    if (oper == N_IL_INSN)
        return ERR_BADOPERATOR;

    if (oper > IL_CAL)
        parse_il_find_arguments(buf, &operand, &byte, &bit);
    else if (oper == IL_JMP)
        strcpy(op.lookup, pos + 1);

    op.operation = oper;
    op.modifier = modifier;
    op.operand = operand;
    op.byte = byte;
    op.bit = bit;

    if (parse_il_check_modifier(&op) < 0) {
        return ERR_BADOPERATOR;
    }
    if (parse_il_check_operand(&op) < 0) {
        return ERR_BADOPERAND;
    }
    if (op.operation != IL_NOP) {
        rung_append(&op, r);
    }
    return STATUS_OK;
}

///////////////////// entry point /////////////////////
rung_t* parse_il_program(const char *name, const char lines[][MAXSTR]) {
    int rv = STATUS_OK;
    unsigned int i = 0;
    rung_t *rungs = NULL;;
    uint8_t rungno = 0;

    il_r = rung_make(name, rungs, &rungno);
    while (rv == STATUS_OK && i < MAXBUF && lines[i][0] != 0) {
        const char *line = lines[i++];
        rv = parse_il_line(line, il_r);
        switch (rv) {
            case STATUS_ERR:
                plc_log("Line %d :%s %s", i, IlErrors[IE_PLC], line);
                break;
            case ERR_BADOPERATOR:
                plc_log("Line %d :%s %s", i, IlErrors[IE_BADOPERATOR], line);
                break;
            case ERR_BADCOIL:
                plc_log("Line %d :%s %s", i, IlErrors[IE_BADCOIL], line);
                break;
            case ERR_BADINDEX:
                plc_log("Line %d :%s %s", i, IlErrors[IE_BADINDEX], line);
                break;
            case ERR_BADOPERAND:
                plc_log("Line %d :%s %s", i, IlErrors[IE_BADOPERAND], line);
                break;
            case ERR_BADFILE:
                plc_log("Line %d :%s %s", i, IlErrors[IE_BADFILE], line);
                break;
            case ERR_BADCHAR:
                plc_log("Line %d :%s %s", i, IlErrors[IE_BADCHAR], line);
                break;
            default:
                break;
        }
    }
    rv = rung_intern(il_r);
    if (rv < STATUS_OK) {
        plc_log("Labels are messed up");
    }
    return &il_r;
}

/// CHECK ///
int parse_il_check_modifier(const instruction_t op) {
    int r = 0;
    if (op->operation > IL_XOR && op->operation < IL_ADD && op->modifier != IL_NEG && op->modifier != IL_NORM) //only negation
        r = ERR_BADOPERATOR;

    if (op->operation > IL_ST // only push
    && op->modifier != IL_PUSH && op->modifier != IL_NORM)
        r = ERR_BADOPERATOR;

    if (op->operation > IL_CAL && op->operation < IL_AND && op->modifier != IL_NORM) //no modifier
        r = ERR_BADOPERATOR;
    return r;
}

int parse_il_check_operand(instruction_t op) {
    int r = 0;
    if (op->operation == IL_SET || op->operation == IL_RESET || op->operation == IL_ST) {
        if (op->operand < OP_CONTACT) {
            if (op->operand == OP_OUTPUT)
                op->operand = OP_CONTACT;
            else if (op->operand == OP_MEMORY)
                op->operand = OP_PULSEIN;
            else if (op->operand == OP_TIMEOUT)
                op->operand = OP_START;
            else if (op->operand == OP_REAL_OUTPUT)
                op->operand = OP_REAL_CONTACT;
            else if (op->operand == OP_REAL_MEMORY)
                op->operand = OP_REAL_MEMIN;
            else
                r = ERR_BADOPERAND; // outputs
        }
    } else if (op->operation > IL_CAL && (op->operand < OP_INPUT || op->operand > OP_CONTACT))
        r = ERR_BADOPERAND;
    return r;
}
