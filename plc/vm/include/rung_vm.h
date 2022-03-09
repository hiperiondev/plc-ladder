/*
 * rung_vm.h
 *
 *  Created on: 4 mar. 2022
 *      Author: egonzalez
 */

#ifndef _RUNG_VM_H_
#define _RUNG_VM_H_

#define MAXSTACK 256

     int get(const rung_t r, const unsigned int idx, instruction_t *i);
     int push(uint8_t op, uint8_t t, const data_t val, rung_t r);
  data_t pop(const data_t val, opcode_t *stack);
opcode_t take(rung_t r);
    void give(opcode_t head);

#endif /* _RUNG_VM_H_ */
