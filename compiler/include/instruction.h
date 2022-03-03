#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_ 

#include "common.h"

#define LABELLEN 4

// IL modifiers
typedef enum {
    IL_NEG = 1,    // '!'
    IL_PUSH,       // '('
    IL_NORM,       // ' '
    IL_COND,       // '?'
    N_IL_MODIFIERS //
} IL_MODIFIERS;

#define IS_MODIFIER(x) (x >= IL_NEG && x < N_IL_MODIFIERS) 
#define MAX_LABEL 1024

// the instruction struct
typedef struct instruction {
             char label[MAX_LABEL];
             char lookup[MAX_LABEL]; // label to lookup (applies to JMP etc.)
    unsigned char operation;
    unsigned char operand;
    unsigned char modifier;
    unsigned char byte;
    unsigned char bit;
} *instruction_t;

extern const char IlCommands[N_IL_INSN][LABELLEN];
extern const char IlOperands[N_OPERANDS][3];
extern const char IlModifiers[N_IL_MODIFIERS][2];

/**
 * @brief get type of instruction
 * @convention type is encoded in the instruction
 * the length is given by the bit part, and the scalar / real is defined by 
 * the operand (analog / digital)
 * @param instruction
 * @return the type, or error
 */
int instruction_get_type(const instruction_t ins);

/**
 *@brief deepcopy instructions
 *@param from
 *@param to
 */
void instruction_deepcopy(const instruction_t from, instruction_t to);

void instruction_dump(instruction_t ins, char *dump);
void instruction_dump_label(char *label, char *dump);

#endif // _INSTRUCTION_H_
