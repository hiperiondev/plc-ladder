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

#include <fcntl.h>

#include "vm_data.h"
#include "cmp_instruction.h"
#include "cmp_rung.h"

#include "log.h"

#include "config.h"
#include "vm_schema.h"
#include "vm_hardware.h"
#include "mem.h"
#include "common.h"

#define ASCIISTART 0x30

FILE *Ifd = NULL;
FILE *Qfd = NULL;
char *BufIn = NULL;
char *BufOut = NULL;
char *AdcIn = NULL;
char *AdcOut = NULL;

int BufOut_len = 0;
unsigned int Ni = 0;
unsigned int Nq = 0;
unsigned int Nai = 0;
unsigned int Naq = 0;

struct hardware Sim;

int vm_sim_config(const config_t conf) {
    int r = STATUS_OK;
    config_t hw = get_recursive_entry(CONFIG_HW, conf);
    config_t ifc = get_recursive_entry(HW_IFACE, hw);
    char *istr = get_string_entry(SIM_INPUT, ifc);
    if (istr) {
        if (!(Ifd = fopen(istr, "r+"))) {
            plc_log("Failed to open simulation input from %s", istr);
            r = STATUS_ERR;
        } else {
            plc_log("Opened simulation input from %s", istr);
        }
    }
    char *ostr = get_string_entry(SIM_OUTPUT, ifc);
    if (ostr) {
        if (!(Qfd = fopen(ostr, "w+"))) {
            plc_log("Failed to open simulation output to %s", ostr);
            r = STATUS_ERR;
        } else {
            plc_log("Opened simulation output to %s", ostr);
        }
    }

    sequence_t s = get_sequence_entry(CONFIG_DI, conf);
    if (s) {
        Ni = s->size / BYTESIZE + 1;
    }
    s = get_sequence_entry(CONFIG_DQ, conf);
    if (s) {
        Nq = s->size / BYTESIZE + 1;
    }
    s = get_sequence_entry(CONFIG_AI, conf);
    if (s) {
        Nai = s->size;
    }
    s = get_sequence_entry(CONFIG_AQ, conf);
    if (s) {
        Naq = s->size;
    }
    Sim.label = get_string_entry(HW_LABEL, hw);

    return r;
}

// enable bus communication
int vm_sim_enable() {
    int r = STATUS_OK;

    // open input and output streams
    if (!(BufIn = (char*) MEM_MALLOC(Ni, "sim_enable A"))) {
        r = STATUS_ERR;
    } else {
        memset(BufIn, 0, Ni);
    }
    if (!(BufOut = (char*) MEM_MALLOC(Nq, "sim_enable B"))) {
        r = STATUS_ERR;
    } else {
        BufOut_len = Nq;
        memset(BufOut, 0, Nq);
    }
    if (!(AdcIn = (char*) MEM_MALLOC( LONG_BYTES * Nai, "sim_enable C"))) {
        r = STATUS_ERR;
    } else {
        memset(AdcIn, 0, LONG_BYTES * Nai);
    }
    if (!(AdcOut = (char*) MEM_MALLOC( LONG_BYTES * Naq, "sim_enable D"))) {
        r = STATUS_ERR;
    } else {
        memset(AdcOut, 0, LONG_BYTES * Naq);
    }
    return r;
}

// disable bus communication
int vm_sim_disable() {
    int r = STATUS_OK;

    // close streams
    if (Ifd && !fclose(Ifd)) {
        r = STATUS_ERR;
    }
    plc_log("Closed simulation input");
    if (Qfd && !fclose(Qfd)) {
        r = STATUS_ERR;
    }
    plc_log("Closed simulation output");
    if (BufIn) {
        MEM_FREE(BufIn, "sim_disable A");
        BufIn = NULL;
    }
    if (BufOut) {
        MEM_FREE(BufOut, "sim_disable B");
        BufOut = NULL;
    }
    return r;
}

int vm_sim_fetch() {
    unsigned int digital = Ni;
    unsigned int analog = Nai;
    int bytes_read = 0;
    if (Ifd) {
        bytes_read = fread(BufIn, sizeof(uint8_t), digital, Ifd ? Ifd : stdin);
        int i = 0;
        for (; i < bytes_read; i++) {
            if (BufIn[i] >= ASCIISTART) {
                BufIn[i] -= ASCIISTART;
            }
        }
        bytes_read += fread(AdcIn, sizeof(uint8_t),
        LONG_BYTES * analog, Ifd ? Ifd : stdin);

        if (bytes_read < digital + LONG_BYTES * analog) {
            if (feof(Ifd)) {
                rewind(Ifd);
            } else {
                vm_sim_disable();
                vm_sim_enable();
            }
        }
    }
    return bytes_read;
}

int vm_sim_flush() {
    int bytes_written = 0;
    unsigned int digital = Nq;
    unsigned int analog = Naq;
    if (Qfd) {
        bytes_written = fwrite(BufOut, sizeof(uint8_t), digital, Qfd);
        bytes_written += fwrite(AdcOut, sizeof(uint8_t), analog * LONG_BYTES, Qfd);
        fputc('\n', Qfd);
        fflush(Qfd);
    }
    return bytes_written;
}

// write input n to bit
void vm_sim_dio_read(unsigned int n, uint8_t *bit) {
    unsigned int b, position;
    position = n / BYTESIZE;
    uint8_t i = 0;
    if (strlen(BufIn) > position) {
        // read a byte from input stream
        i = BufIn[position];
    }
    b = (i >> n % BYTESIZE) % 2;
    *bit = (uint8_t) b;
}

// write bit to n output
void vm_sim_dio_write(const unsigned char *buf, unsigned int n, uint8_t bit) {
    uint8_t q;
    unsigned int position = n / BYTESIZE;
    q = buf[position];
    q |= bit << n % BYTESIZE;
    // write a byte to output stream
    q += ASCIISTART; // ASCII
    if (BufOut_len == 0) {
        return;
    }
    if (BufOut_len < position) {
        return;
    }

    if (BufOut == NULL) {
        return;
    }
    if (strlen(BufOut) >= position) {
        BufOut[position] = q;
    }
}

// simultaneusly write output bits defined by mask and read all inputs
void vm_sim_dio_bitfield(const uint8_t *mask, uint8_t *bits) {
    /* FIXME
     int i=0;
     unsigned int w = (unsigned int) (*mask);
     unsigned int b = (unsigned int) (*bits);
     comedi_dio_bitfield(it, Comedi_subdev_i, w, &b);
     */
}

void vm_sim_data_read(unsigned int index, uint64_t *value) {
    unsigned int pos = index * LONG_BYTES;
    int i = LONG_BYTES - 1;
    *value = 0;
    if (strlen(AdcIn) > pos) {
        uint64_t mult = 1;
        for (; i >= 0; i--) {
            *value += (uint64_t) AdcIn[pos + i] * mult;
            mult *= 0x100;
        }
    }
}

void vm_sim_data_write(unsigned int index, uint64_t value) {
    if (AdcOut == NULL) {
        return;
    }
    unsigned int pos = index * LONG_BYTES;
    sprintf(AdcOut + pos, "%lx", value);
    return;
}

struct hardware Sim = {
        HW_SIM,
        0,                // error code
        "simulated hardware",
        vm_sim_enable,       // enable
        vm_sim_disable,      // disable
        vm_sim_fetch,        // fetch
        vm_sim_flush,        // flush
        vm_sim_dio_read,     // dio_read
        vm_sim_dio_write,    // dio_write
        vm_sim_dio_bitfield, // dio_bitfield
        vm_sim_data_read,    // data_read
        vm_sim_data_write,   // data_write
        vm_sim_config,       // hw_config
        };
