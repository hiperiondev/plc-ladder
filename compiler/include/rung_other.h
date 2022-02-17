/*
 * rung_other.h
 *
 *  Created on: 17 feb. 2022
 *      Author: egonzalez
 */

#ifndef INCLUDE_RUNG_OTHER_H_
#define INCLUDE_RUNG_OTHER_H_

#include "plc_common.h"

/**
 * @brief add a new rung to a plc
 * @param a unique identifier
 * @param the plc
 * @return reference to the new rung
 */
rung_t mk_rung(const char *name, plc_t p);

/**
 * @brief get rung reference from plc
 * @param p a plc
 * @param r the rung reference
 * @param idx the index
 * @return reference to rung or NULL
 */
rung_t get_rung(const plc_t p, unsigned int idx);



#endif /* INCLUDE_RUNG_OTHER_H_ */
