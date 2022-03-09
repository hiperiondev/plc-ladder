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

int dry_config(const config_t conf) {
    return STATUS_OK;
}

// enable bus communication
int dry_enable() {
    return STATUS_OK;
}

// disable bus communication
int dry_disable() {

    return STATUS_OK;
}

int dry_fetch() {

    return 0;
}

int dry_flush() {
    return 0;
}

void dry_dio_read(unsigned int n, uint8_t *bit) {
    return;
}

void dry_dio_write(const unsigned char *buf, unsigned int n, uint8_t bit) {
    return;
}

void dry_dio_bitfield(const uint8_t *mask, uint8_t *bits) {
    return;
}

void dry_data_read(unsigned int index, uint64_t *value) {
    return;
}

void dry_data_write(unsigned int index, uint64_t value) {

    return;
}

struct hardware Dry = {
        HW_DRY, 0,        // error code
        "no hardware",
        dry_enable,       // enable
        dry_disable,      // disable
        dry_fetch,        // fetch
        dry_flush,        // flush
        dry_dio_read,     // dio_read
        dry_dio_write,    // dio_write
        dry_dio_bitfield, // dio_bitfield
        dry_data_read,    // data_read
        dry_data_write,   // data_write
        dry_config,       // hw_config
        };

