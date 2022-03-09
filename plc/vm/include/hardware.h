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

#ifndef _HARDWARE_H_
#define _HARDWARE_H_

#include <inttypes.h>

#include "config.h"

typedef enum {
    HW_DRY,    //TODO
    HW_SIM,    //
    HW_USPACE, //TODO: update with current linux kernels

    N_HW       //
} HARDWARES;

typedef int (*helper_f)(); // generic helper functions only return an error code

typedef void (*dio_rd_f) (unsigned int, unsigned char*);
typedef void (*dio_wr_f) (const unsigned char*, unsigned int, unsigned char);
typedef void (*dio_bit_f)(const unsigned char*, unsigned char*);
typedef void (*data_rd_f)(unsigned int, uint64_t*);
typedef void (*data_wr_f)(unsigned int, uint64_t);
typedef  int (*config_f) (const config_t);

typedef struct hardware {
    int type;
    int status;
    char *label;

    /**
     * @brief Enable bus communication
     * @return error code
     */
    helper_f enable;
    /**
     * @brief Disable bus communication
     * @return error code
     */
    helper_f disable;
    /**
     * @brief fetch all input bytes if necessary
     * @return error code
     */
    helper_f fetch;
    /**
     * @brief flush all output bytes if necessary
     * @return error code
     */
    helper_f flush;
    /**
     * @brief read digital input
     * @param index
     * @param value
     */
    dio_rd_f dio_read;
    /**
     * @brief write bit to digital output
     * @param current output buffer
     * @param n index
     * @param bit
     */
    dio_wr_f dio_write;
    /**
     * @brief read / write binary mask to digital input / output
     * @param mask
     * @param bits
     */
    dio_bit_f dio_bitfield;
    /**
     * @brief read analog sample
     * @param the index
     * @param the raw value
     */
    data_rd_f data_read;
    /**
     * @brief write analog sample
     * @param the index
     * @param the raw value
     */
    data_wr_f data_write;

    /**
     * @brief apply a configuration to create a hardware instance
     * @param the configuration
     */
    config_f configure;

} *hardware_t;

// hardware ctor factory
hardware_t get_hardware(int type);

#endif //_HARDWARE_H_
