/*
 * instruction_vm.h
 *
 *  Created on: 4 mar. 2022
 *      Author: egonzalez
 */

#ifndef INCLUDE_INSTRUCTION_VM_H_
#define INCLUDE_INSTRUCTION_VM_H_

 int get_type(const instruction_t ins);
void deepcopy(const instruction_t from, instruction_t to);

#endif /* INCLUDE_INSTRUCTION_VM_H_ */
