/*
 * Copyright 2022 Emiliano Gonzalez (egonzalez . hiperion @ gmail . com))
 * * Project Site: https://github.com/hiperiondev/plc-ladder *
 *
 * This is based on other projects:
 *    plcemu (https://github.com/kalamara/plcemu)
 *     - Antonis Kalamaras (kalamara AT ceid DOT upatras DOT gr)
 *
 * please contact their authors for more information.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef _UT_IO_
#define _UT_IO_

#include <stdbool.h>
#include <CUnit/CUnit.h>

#include "common.h"
#include "plclib.h"

void ut_read() {
    // nulls shouldn't crash
    read_inputs(NULL);
    struct PLC_regs p;
    memset(&p, 0, sizeof(struct PLC_regs));
    read_inputs(&p);
    init_mock_plc(&p);
    // mock fetch makes all digitals 1s, and all analogs magic
    // so magic numbers should appear in input regs
    read_inputs(&p);
    CU_ASSERT(p.inputs[p.ni - 1] == 0xFF);
    CU_ASSERT(p.real_in[p.nai - 1] == 0xABCDEF01);
}

void ut_write() {
    extern int Mock_flush_count;
    extern unsigned char Mock_dout;
    extern uint64_t Mock_aout;
    // nulls shouldn't crash
    write_outputs(NULL);
    struct PLC_regs p;
    memset(&p, 0, sizeof(struct PLC_regs));
    write_outputs(&p);
    init_mock_plc(&p);
    unsigned char out[8] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    memcpy(p.outputs, out, 8);

    uint64_t rout[2] = { 0xABCDEF01, 0xABCDEF01 };
    memcpy(p.real_out, rout, 16);
    // write outputs copies magic numbers to hardware
    // mock flush also should be called
    write_outputs(&p);
    CU_ASSERT(Mock_dout == 0xFF);
    CU_ASSERT(Mock_aout == 0xABCDEF01);
    CU_ASSERT(Mock_flush_count == 1);

    plc_stop(NULL);
    p.status = ST_RUNNING;
    plc_stop(&p);
    // this should reset outputs
    CU_ASSERT(Mock_flush_count == 0);
}

#endif //_UT_IO_
