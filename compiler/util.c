
#include <stdarg.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "util.h"
#include "data.h"

FILE *ErrLog = NULL;

void plc_log(const char *msg, ...) {
    va_list arg;
    time_t now;
    time(&now);
    char msgstr[MAXSTR];
    memset(msgstr, 0, MAXSTR);
    va_start(arg, msg);
    vsprintf(msgstr, msg, arg);
    va_end(arg);
    if (!ErrLog)
        ErrLog = fopen(LOG, "w+");
    if (ErrLog) {
        fprintf(ErrLog, "%s", msgstr);
        fprintf(ErrLog, ":%s", ctime(&now));
        fflush(ErrLog);
    }
    printf("%s\n",msgstr);
}

void close_log() {
    if (ErrLog)
        fclose(ErrLog);
}
