/*
 * plc_common.h
 *
 *  Created on: 17 feb. 2022
 *      Author: egonzalez
 */

#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_

#include <poll.h>

#include "data.h"
#include "rung.h"

#define MILLION 1000000
#define THOUSAND 1000

#define MAXRUNG 256

#define FLOAT_PRECISION 0.000001

typedef enum {
    ST_STOPPED, //
    ST_RUNNING  //
} STATUSES;

typedef enum {
    N_ERR = -20,     // ERROR CODES are negative
    ERR_OVFLOW,      //
    ERR_TIMEOUT,     //
    ERR_HARDWARE,    //
    ERR_BADOPERATOR, //
    ERR_BADCOIL,     //
    ERR_BADINDEX,    //
    ERR_BADOPERAND,  //
    ERR_BADFILE,     //
    ERR_BADCHAR,     //
    ERR_BADPROG,     //
} ERRORCODES;

typedef enum {
    IE_PLC,         //
    IE_BADOPERATOR, //
    IE_BADCOIL,     //
    IE_BADINDEX,    //
    IE_BADOPERAND,  //
    IE_BADFILE,     //
    IE_BADCHAR,     //
    N_IE            //
} IL_ERRORCODES;

#endif /* INCLUDE_COMMON_H_ */
