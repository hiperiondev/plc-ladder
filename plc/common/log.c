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

#include <stdarg.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "config.h"
#include "log.h"

#ifdef LOG_TO_FILE
FILE *ErrLog = NULL;
#define LOG "plc_compiler.log"
#endif

void plc_log(const char *msg, ...) {
    va_list arg;
    time_t now;
    time(&now);
    char msgstr[MAXSTR];
    memset(msgstr, 0, MAXSTR);
    va_start(arg, msg);
    vsprintf(msgstr, msg, arg);
    va_end(arg);

#ifdef LOG_TO_FILE
    if (!ErrLog)
        ErrLog = fopen(LOG, "w+");
    if (ErrLog) {
        fprintf(ErrLog, "%s", msgstr);
        fprintf(ErrLog, ":%s", ctime(&now));
        fflush(ErrLog);
    }
#endif

    printf("%s\n", msgstr);
}

void plc_close_log() {
#ifdef LOG_TO_FILE
    if (ErrLog)
        fclose(ErrLog);
#endif
}
