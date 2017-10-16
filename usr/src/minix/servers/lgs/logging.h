#ifndef _LGS_LOGGING_H_
#define _LGS_LOGGING_H_

/* Type definitions for the Logging Server. */
#include <sys/types.h>
#include <minix/config.h>
#include <minix/ds.h>
#include <minix/bitmap.h>
#include <minix/param.h>
#include <minix/type.h>
#include <regex.h>

/* maximum number of loggers */
#define MAX_LOGGERS     64  

/* config file length constraints */
#define MAX_NAME        32 
#define MAX_LEVEL       5  
#define MAX_FORMAT      64 
#define MAX_TYPE        6   
#define MAX_FILENAME    64 
#define MAX_APPEND      1 

/* logger types */
typedef int logger_type;
#define LT_FILE         1
#define LT_STDOUT       2
#define LT_STDERR       3
const char *types[4] = {"", "FILE", "STDOUT", "STDERR"};

/* logger levels */
typedef int logger_level;
#define LL_TRACE        1
#define LL_DEBUG        2
#define LL_ERROR        3
const char *levels[4] = {"", "TRACE", "DEBUG", "ERROR"};

/* error codes */
#define ERR_ACTIVE      1
#define ERR_UNKNOWN     2
#define ERR_INTERNAL    3
#define ERR_NOTACTIVE   4

/* Holds information about one logger. */
struct logger {
    char                name[MAX_NAME];
    logger_type         type;
    char                filename[MAX_FILENAME]; /* if type is LT_FILE */
    logger_level        level;
    char                format[MAX_FORMAT];
    char                mode;                   /* if type is LT_FILE */
    
    endpoint_t          proc;                   /* endpoint of the process using the logger */
    int                 active;
    int                 fd;
};

#endif /* _LGS_LOGGING_H_ */
