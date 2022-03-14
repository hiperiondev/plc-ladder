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

#include "config.h"
#include "vm_schema.h"
#include "mem.h"

struct entry HwSchema[N_HW_VARS] = {
        {
                .type_tag = ENTRY_STR,
                .name = "LABEL",
                .e = {
                        .scalar_str = "DRY"
                }
        }, {
                .type_tag = ENTRY_MAP,
                .name = "IFACE",
                .e = {
                        .conf = NULL
                }
        },
};

struct sequence default_seq = {
        .size = 2
};

struct entry ConfigSchema[N_CONFIG_VARIABLES] = {
        {   // CONFIG_STATUS,
                .type_tag = ENTRY_STR,
                .name = "STATUS",
                .e = {
                        .scalar_str = "STOPPED"
                }
        }, { // CONFIG_STEP,
                .type_tag = ENTRY_INT,
                .name = "STEP",
                .e = {
                        .scalar_int = 100
                }
        }, { // CONFIG_HW,
                .type_tag = ENTRY_MAP,
                .name = "HW",
                .e = {
                        .conf = (struct config*) &HwSchema
                }
        }, { // CONFIG_PROGRAM
                .type_tag = ENTRY_SEQ,
                .name = "PROGRAM",
                .e = {
                        .seq = &default_seq
                }
        }, { // CONFIG_AI
                .type_tag = ENTRY_SEQ,
                .name = "AI",
                .e = {
                        .seq = &default_seq
                }
        }, { // CONFIG_AQ
                .type_tag = ENTRY_SEQ,
                .name = "AQ",
                .e = {
                        .seq = &default_seq
                }
        }, { // CONFIG_DI
                .type_tag = ENTRY_SEQ,
                .name = "DI",
                .e = {
                        .seq = &default_seq
                }
        }, { // CONFIG_DQ
                .type_tag = ENTRY_SEQ,
                .name = "DQ",
                .e = {
                        .seq = &default_seq
                }
        }, { // CONFIG_MVAR
                .type_tag = ENTRY_SEQ,
                .name = "MVAR",
                .e = {
                        .seq = &default_seq
                }
        }, { // CONFIG_MREG
                .type_tag = ENTRY_SEQ,
                .name = "MREG",
                .e = {
                        .seq = &default_seq
                }
        }, { // CONFIG_TIMERS
                .type_tag = ENTRY_SEQ,
                .name = "TIMERS",
                .e = {
                        .seq = &default_seq
                }
        }, { // CONFIG_PULSES
                .type_tag = ENTRY_SEQ,
                .name = "PULSES",
                .e = {
                        .seq = &default_seq
                }
        },
};
