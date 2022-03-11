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
#include "hardware.h"
#include "data.h"
#include "instruction.h"
#include "rung.h"
#include "plclib.h"
#include "mem.h"

int vm_project_task(plc_t p) {
// start editing here
    uint8_t one, two, three;
    one = vm_resolve(p, BOOL_DI, 1);
    two = vm_fe(p, BOOL_DI, 2);
    three = vm_re(p, BOOL_DI, 3);
    //contact(p,BOOL_DQ,1,one);
    //contact(p,BOOL_DQ,2,two);
    //contact(p,BOOL_DQ,3,three);
    if (one)
        vm_set(p, BOOL_TIMER, 0);
    if (three)
        vm_reset(p, BOOL_TIMER, 0);
    if (two)
        vm_down_timer(p, 0);
    return 0;
// end of editable portion

}
int vm_project_init() {
    // same here
    return 0;
}
