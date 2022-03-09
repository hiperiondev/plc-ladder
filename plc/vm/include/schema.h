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

#ifndef _SCHEMA_H_
#define _SCHEMA_H_

typedef enum{
    MAP_ROOT,          //
    MAP_HW,            //
    MAP_USPACE,        //
    //MAP_COMEDI,        //
    //MAP_COMEDI_SUBDEV, //
    MAP_SIM,           //
    MAP_VARIABLE,      //
    N_MAPPINGS         //
} CONFIG_MAPPINGS;

typedef enum{
    USPACE_BASE,       //
    USPACE_WR,         //
    USPACE_RD,         //
    N_USPACE_VARS      //
} USPACE_VARS;

typedef enum{
    SUBDEV_IN,         //
    SUBDEV_OUT,        //
    SUBDEV_ADC,        //
    SUBDEV_DAC,        //
    N_SUBDEV_VARS      //
} SUBDEV_VARS;

//typedef enum{
//    COMEDI_FILE,       //
//    COMEDI_SUBDEV,     //
//    N_COMEDI_VARS      //
//} COMEDI_VARS;

typedef enum {
    SIM_INPUT,         //
    SIM_OUTPUT,        //
    N_SIM_VARS         //
} SIM_VARS;

typedef enum {
    HW_LABEL,          //
    HW_IFACE,          //
    N_HW_VARS          //
} HW_VARS;

typedef enum{
    CONFIG_STATUS,     //
    CONFIG_STEP,       //
    CONFIG_HW,         //
     // (runtime updatable) sequences,
    CONFIG_PROGRAM,    //
    CONFIG_AI,         //
    CONFIG_AQ,         //
    CONFIG_DI,         //
    CONFIG_DQ,         //
    CONFIG_MVAR,       //
    CONFIG_MREG,       //
    CONFIG_TIMER,      //
    CONFIG_PULSE,      //
    N_CONFIG_VARIABLES //
} CONFIG_VARIABLES;

#endif //_SCHEMA_H_
