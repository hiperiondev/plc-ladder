
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