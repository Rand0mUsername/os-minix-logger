#include "inc.h"
#include <stdio.h>
#include <lib.h>
#include <string.h>
#include "../lgs/logging.h"

static struct logger loggers[MAX_LOGGERS];

void loggers_dmp() {
    if(getsysinfo(LGS_PROC_NR, SI_LOGGERS, loggers, sizeof(loggers)) != OK) {
        printf("Error obtaining loggers from LGS...\n");
    } 
    printf("### LGS ###\n");
    printf("Loggers:\n");
    int i;
    for(int i = 0; i < MAX_LOGGERS; i++) {
        if(strlen(loggers[i].name) > 0) {
            printf("Logger #%d: %s [%s|%s]\n", i, loggers[i].name, 
                   levels[loggers[i].level], types[loggers[i].type]);
            printf("    Format: %s\n", loggers[i].format);    
            if(loggers[i].type == LT_FILE) {
                printf("    Filename=%s Append=%c\n", loggers[i].filename, loggers[i].mode);
            }
            if(loggers[i].active) {
                printf("    Endpoint # using this logger: %d\n", loggers[i].proc);  
                printf("    FD = %d\n", loggers[i].fd);
            } else {
                printf("    Logger free\n");
            }
        }
    }
    printf("### LGS ###\n");
}