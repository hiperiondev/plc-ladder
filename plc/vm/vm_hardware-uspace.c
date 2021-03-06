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

#include <unistd.h>
#include <sys/io.h>
#include <sys/types.h>

#include "vm_data.h"
#include "cmp_instruction.h"
#include "cmp_rung.h"
#include "log.h"
#include "config.h"
#include "vm_hardware.h"
#include "vm_schema.h"
#include "mem.h"
#include "common.h"

int Io_base = 0;
int Wr_offs = 0;
int Rd_offs = 0;

struct hardware Uspace;

int vm_usp_config(const config_t conf) {
    config_t u = get_recursive_entry(CONFIG_HW, conf); // TODO: this will be rewritten
    Io_base = get_numeric_entry(USPACE_BASE, u);
    Wr_offs = get_numeric_entry(USPACE_WR, u);
    Rd_offs = get_numeric_entry(USPACE_RD, u);
    Uspace.label = get_string_entry(CONFIG_HW, conf);
    if (Io_base >= 0 && Wr_offs >= 0 && Rd_offs >= 0)
        return STATUS_OK;
    else
        return STATUS_ERR;
}

// enable bus communication
int vm_usp_enable() {
    int uid = getuid(); // get User id
    int r = seteuid(0); // set User Id to root (0)
    if (r < 0 || geteuid() != 0) {
        fprintf(stderr, "FATAL ERROR: UNABLE TO CHANGE TO ROOT\n");
        return STATUS_ERR;
    }
    if (iopl(3)) {
        // request bus WR i/o permission
        fprintf(stderr, "FATAL ERROR: UNABLE TO GET I/O PORT PERMISSION\n");
        perror("iopl() ");
        r = seteuid(uid);
        return STATUS_ERR;
    }
    r = seteuid(uid); // reset User Id
    outb(0, Io_base + Wr_offs); // clear outputs port
    printf("io card enabled\n");
    return STATUS_OK;
}

// disable bus communication
int vm_usp_disable() {
    int uid = getuid(); // get User id
    int r = setuid(0); // set User Id to root (0)
    if (r < 0 || getuid() != 0) {
        fprintf(stderr, "Unable to change id to root\nExiting\n");
        return STATUS_ERR;
    }
    if (iopl(0)) { // normal i/o privilege level
        perror("iopl() ");
        r = setuid(uid);
        return STATUS_ERR;
    }
    r = setuid(uid); // reset User Id
    return STATUS_OK;
}

int vm_usp_fetch() {
    return 0;
}

int vm_usp_flush() {
    return 0;
}

// write input n to bit
void vm_usp_dio_read(unsigned int n, uint8_t *bit) {
    unsigned int b;
    uint8_t i;
    i = inb(Io_base + Rd_offs + n / BYTESIZE);
    b = (i >> n % BYTESIZE) % 2;
    *bit = (uint8_t) b;
}

// write bit to n output
void vm_usp_dio_write(const uint8_t *buf, unsigned int n, unsigned char bit) {
    uint8_t q;
    q = buf[n / BYTESIZE];
    q |= bit << n % BYTESIZE;
    outb(q, Io_base + Wr_offs + n / BYTESIZE);
}

// simultaneusly write output bits defined my mask and read all inputs
void vm_usp_dio_bitfield(const uint8_t *write_mask, uint8_t *bits) {
    /*FIXME
     int i;
     for (i = 0; i < Dq; i++)
     outb(bits[i] & write_mask[i], Base + Wr_offs + i);
     for (i = 0; i < Di; i++)
     bits[i] = inb(Base + Rd_offs + i);
     */
}

void vm_usp_data_read(unsigned int index, uint64_t *value) {
    return; //unimplemented for user space
}

void vm_usp_data_write(unsigned int index, uint64_t value) {
    return; // unimplemented for user space
}

struct hardware Uspace = {
        HW_USPACE,
        0,                // error code
        "",
        vm_usp_enable,       // enable
        vm_usp_disable,      // disable
        vm_usp_fetch,        // fetch
        vm_usp_flush,        // flush
        vm_usp_dio_read,     // dio_read
        vm_usp_dio_write,    // dio_write
        vm_usp_dio_bitfield, // dio_bitfield
        vm_usp_data_read,    // data_read
        vm_usp_data_write,   // data_write
        vm_usp_config,       // hw_config
        };

