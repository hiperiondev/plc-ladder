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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <CUnit/CUnit.h>

#include "config.h"
#include "hardware.h"
#include "data.h"
#include "instruction.h"
#include "rung.h"
#include "plclib.h"
#include "mem.h"
#include "common.h"

// stubbed hardware
unsigned char Mock_din = 0;
unsigned char Mock_dout = 0;
int Mock_flush_count = 0;
uint64_t Mock_ain = 0;
uint64_t Mock_aout = 0;

int stub_config(const config_t conf) {
    return STATUS_OK;
}

// enable bus communication
int stub_enable() {
    Mock_din = 0;
    Mock_ain = 0;

    int r = STATUS_OK;

    return r;
}

// enable bus communication
int stub_enable_fails() {
    Mock_din = 0;
    Mock_ain = 0;

    int r = STATUS_ERR;

    return r;
}

// Disable bus communication
int stub_disable() {
    Mock_aout = 0;
    Mock_dout = 0;
    Mock_flush_count = 0;

    return STATUS_OK;
}

int stub_fetch() {
    Mock_din = 1;
    Mock_ain = 0xABCDEF01;
    return STATUS_OK;
}

int stub_flush() {
    Mock_flush_count = 1;
    return STATUS_OK;
}

void stub_dio_read(unsigned int n, uint8_t *bit) {
    *bit = Mock_din;
}

// write bit to n output
void stub_dio_write(const unsigned char *buf, unsigned int n, unsigned char bit) {
    if (n < 8)
        Mock_dout += (bit << n);
}

// simultaneously write output bits defined by mask and read all inputs
void stub_dio_bitfield(const unsigned char *mask, unsigned char *bits) {
}

void stub_data_read(unsigned int index, uint64_t *value) {
    *value = Mock_ain;
}

void stub_data_write(unsigned int index, uint64_t value) {
    Mock_aout = value;
}

struct hardware Hw_stub = {
        HW_SIM, 0,         // error code
        "stubbed hardware",
        stub_enable,       // enable
        stub_disable,      // disable
        stub_fetch,        // fetch
        stub_flush,        // flush
        stub_dio_read,     // dio_read
        stub_dio_write,    // dio_write
        stub_dio_bitfield, // dio_bitfield
        stub_data_read,    // data_read
        stub_data_write,   // data_write
        stub_config,       // hw_config
        };

//hardware_t get_hardware(int type) {
//    return &Hw_stub;
//}
