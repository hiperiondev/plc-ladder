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

rung_t mk_rung(const char *name, plc_t p) {
    rung_t r = (rung_t) malloc(sizeof(struct rung));
    memset(r, 0, sizeof(struct rung));
    r->id = strdup(name);
    if (p->rungs == NULL) {     //lazy allocation
        p->rungs = (rung_t*) malloc(MAXRUNG * sizeof(rung_t));
        memset(p->rungs, 0, MAXRUNG * sizeof(rung_t));
    }
    p->rungs[p->rungno++] = r;

    return r;
}

rung_t get_rung(const plc_t p, const unsigned int idx) {
    if (p == NULL || idx >= p->rungno) {
        return NULL;
    }
    return p->rungs[idx];
}
