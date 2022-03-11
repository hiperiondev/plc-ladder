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

#include "data.h"
#include "instruction.h"
#include "rung.h"
#include "log.h"
#include "config.h"
#include "schema.h"
#include "hardware.h"
#include "mem.h"
#include "common.h"

int vm_dry_config(const config_t conf) {
    return STATUS_OK;
}

// enable bus communication
int vm_dry_enable() {
    return STATUS_OK;
}

// disable bus communication
int vm_dry_disable() {

    return STATUS_OK;
}

int vm_dry_fetch() {

    return 0;
}

int vm_dry_flush() {
    return 0;
}

void vm_dry_dio_read(unsigned int n, uint8_t *bit) {
    return;
}

void vm_dry_dio_write(const unsigned char *buf, unsigned int n, uint8_t bit) {
    return;
}

void vm_dry_dio_bitfield(const uint8_t *mask, uint8_t *bits) {
    return;
}

void vm_dry_data_read(unsigned int index, uint64_t *value) {
    return;
}

void vm_dry_data_write(unsigned int index, uint64_t value) {

    return;
}

struct hardware Dry = {
        HW_DRY,
        0,                // error code
        "no hardware",
        vm_dry_enable,       // enable
        vm_dry_disable,      // disable
        vm_dry_fetch,        // fetch
        vm_dry_flush,        // flush
        vm_dry_dio_read,     // dio_read
        vm_dry_dio_write,    // dio_write
        vm_dry_dio_bitfield, // dio_bitfield
        vm_dry_data_read,    // data_read
        vm_dry_data_write,   // data_write
        vm_dry_config,       // hw_config
        };

