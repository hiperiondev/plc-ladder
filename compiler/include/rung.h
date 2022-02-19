#ifndef _RUNG_H_
#define _RUNG_H_

#include "common.h"
#include "instruction.h"

#define MAXSTACK 256
#define PLC_OK   0
#define PLC_ERR  -1

typedef struct codeline {
               char *line;
    struct codeline *next;
} *codeline_t;

/**
 * @brief The instruction list executable rung
 */
typedef struct rung {
    instruction_t *instructions;
             char *id;
       codeline_t code;    // original code for visual representation
     unsigned int insno; // actual no of active lines
      struct rung *next;  // linked list of rungs
} *rung_t;

/**
 * @brief get instruction reference from rung
 * @param r a rung AKA instructions list
 * @param i the instruction reference
 * @param idx the index
 * @return OK or error
 */
int get(const rung_t r, unsigned int idx, instruction_t *i);

/**
 * @brief append instruction to rung
 * @param i a new instruction
 * @param r a rung AKA instructions list
 * @return OK or error
 */
int append(const instruction_t i, rung_t r);

/**
 * @brief append codeline string to rung code
 * @param l a code line
 * @param code the existing code lines
 * @return rung code including new line
 */
codeline_t append_line(const char *l, codeline_t code);

/**
 * @brief clear rung from instructions and free memory
 * @param r a rung AKA instructions list
 */
void clear_rung(rung_t r);

/**
 * @brief lookup instruction by label
 * @param label
 * @param r a rung AKA instructions list
 * @return the index (pc) of the instruction, or error if not found
 */
int lookup(const char *label, rung_t r);

/**
 * @brief intern  labels
 * for each lookup command (JMP etc.) the lookup label is 
 * looked up and if found the resulting index stored to the modifier
 * @param r a rung AKA instructions list
 * @return OK, or error if : a label is not found or found duplicate
 */
int intern(rung_t r);

/**
 * @brief add a new rung to a plc
 * @param a unique identifier
 * @param the plc
 * @return reference to the new rung
 */
rung_t mk_rung(const char *name, rung_t *rungs, BYTE *rungno);

/**
 * @brief get rung reference from plc
 * @param p a plc
 * @param r the rung reference
 * @param idx the index
 * @return reference to rung or NULL
 */
rung_t get_rung(rung_t *rungs, BYTE *rungno, const unsigned int idx);

void dump_rung(rung_t ins, char *dump);

#endif //_RUNG_H_
