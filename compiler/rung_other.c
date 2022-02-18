/*
 * rung_other.c
 *
 *  Created on: 17 feb. 2022
 *      Author: egonzalez
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "plc_common.h"
#include "rung.h"

rung_t mk_rung(const char *name, rung_t *rungs, BYTE *rungno) {
    rung_t r = (rung_t) malloc(sizeof(struct rung));
    memset(r, 0, sizeof(struct rung));
    r->id = strdup(name);
    if (rungs == NULL) {     //lazy allocation
        rungs = (rung_t*) malloc(MAXRUNG * sizeof(rung_t));
        memset(rungs, 0, MAXRUNG * sizeof(rung_t));
    }
    rungs[(*rungno)++] = r;

    return r;
}

rung_t get_rung(rung_t *rungs, BYTE *rungno, const unsigned int idx) {
    if (rungs == NULL || idx >= *rungno) {
        return NULL;
    }
    return rungs[idx];
}
