#include "syslib.h"
#include <string.h>

int start_log(char* logger) {
    message m;
    memset(&m, 0, sizeof(m));
    strcpy(m.m_lgs.logger, logger);
    return (_syscall(LGS_PROC_NR, LGS_START, &m));
}

int set_logger_level(char* logger, int level) {
    message m;
    memset(&m, 0, sizeof(m));
    strcpy(m.m_lgs.logger, logger);
    m.m_lgs.level = level;
    return (_syscall(LGS_PROC_NR, LGS_SETLEVEL, &m));
}

int write_log(char* logger, char* msg, int message_level) {
    message m;
    memset(&m, 0, sizeof(m));
    strcpy(m.m_lgs.logger, logger);
    m.m_lgs.level = message_level;
    m.m_lgs.msg_loc = msg;
    m.m_lgs.msg_len = strlen(msg);    
    return (_syscall(LGS_PROC_NR, LGS_WRITE, &m));
}

int close_log(char* logger) {
    message m;
    memset(&m, 0, sizeof(m));
    strcpy(m.m_lgs.logger, logger);
    return (_syscall(LGS_PROC_NR, LGS_CLOSE, &m));
}

int clear_logs(char* logger) {
    message m;
    memset(&m, 0, sizeof(m));
    if(logger) {
        strcpy(m.m_lgs.logger, logger);
    }
    return (_syscall(LGS_PROC_NR, LGS_CLEAR, &m));
}