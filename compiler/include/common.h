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

#define MAXRUNG  256

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

/**
 *possible LD line statuses
 */
enum {
    STATUS_UNRESOLVED, //
    STATUS_RESOLVED,   //
    STATUS_FINAL,      //
    STATUS_ERROR,      //
    N_STATUS           //
};

/**
 *accepted LD symbols: 0-9 for digits, and
 */
enum {
    // LD specific operators:
    LD_BLANK = 10,  // blank character
    LD_AND,         // -
    LD_NOT,         // !
    LD_OR,          // |
    LD_NODE,        // +
    LD_COIL,        // ( contact coil
    LD_SET,         // [ set
    LD_RESET,       // ] reset,
    LD_DOWN,        // ) negate coil
    N_LD_SYMBOLS    //
};

#endif /* INCLUDE_COMMON_H_ */
