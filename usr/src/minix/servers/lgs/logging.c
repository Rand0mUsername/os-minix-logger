#include "logging.h"
#include "util.h"

#include "inc.h"
#include <lib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

#include "../pm/mproc.h"

/* Space for the loggers. */
struct logger loggers[MAX_LOGGERS];
int num_loggers;

/* Finds the logger with given name. */
struct logger* find_logger(char *name) {
    int i;
    for(i = 0; i < num_loggers; i++) {
        if(strcmp(name, loggers[i].name) == 0) {
            return &loggers[i];
        }
    }
    return NULL;
}


/* Builds and outputs the message. */
int do_log(char *format, char *proc_name, char *curr_time, 
           const char *level, char *msg, int fd) {
    char out_buff[MAX_MSG];
    char tmp_format[MAX_FORMAT];
    strcpy(tmp_format, format);
    int len = strlen(tmp_format);
    int i, nxt_char = 0, out_it = 0;
    for(i = 0; i < len; i++) {
        if(i < len - 1 && tmp_format[i] == '%') {
            tmp_format[i] = '\0';
            buff_append(out_buff, &out_it, tmp_format+nxt_char);
            nxt_char = i + 2;
            switch(tmp_format[i+1]) {
                case '%':
                    buff_append(out_buff, &out_it, "%");
                    break;
                case 'n':
                    buff_append(out_buff, &out_it, proc_name);
                    break;
                case 't':
                    buff_append(out_buff, &out_it, curr_time);
                    break;
                case 'l':
                    buff_append(out_buff, &out_it, level);
                    break;
                case 'm':
                    buff_append(out_buff, &out_it, msg);
                    break;
                default:
                    swrite("LGS: Unknown specifier in format string, aborting.\n", fd);
                    return (EINVAL);
            }
            i++;
        }
    }
    if(nxt_char < len) {
        buff_append(out_buff, &out_it, tmp_format+nxt_char);       
    }
    buff_append(out_buff, &out_it, "\n");
    swrite(out_buff, fd);
    return (OK);
}

/*===========================================================================*
 *              do_start_log                    
 *===========================================================================*/
int do_start_log(message *m_ptr) {
    swrite("LGS: do_start_log\n", PRINT_STDOUT);
    struct logger* lgr = find_logger(m_ptr->m_lgs.logger);
    if(!lgr) {
        return (ERR_UNKNOWN);
    }
    if(lgr->active) {
        return (ERR_ACTIVE);
    }
    lgr->active = 1;
    lgr->proc = m_ptr->m_source;
    if(lgr->type == LT_FILE) {
        if(lgr->mode == 'a') {
            lgr->fd = open(lgr->filename, O_WRONLY|O_APPEND|O_CREAT);
        } else {
            lgr->fd = open(lgr->filename, O_WRONLY|O_CREAT);
        }
    } else if(lgr -> type == LT_STDOUT) {
        lgr-> fd = PRINT_STDOUT;
    } else {
        lgr -> fd = PRINT_STDERR;
    }
    return (OK);
}

/*===========================================================================*
 *              do_set_logger_level                    
 *===========================================================================*/
int do_set_logger_level(message *m_ptr) {
    swrite("LGS: do_set_logger_level\n", PRINT_STDOUT);
    struct logger* lgr = find_logger(m_ptr->m_lgs.logger);
    if(!lgr) {
        return (ERR_UNKNOWN);
    }
    if(lgr->active) {
        return (ERR_ACTIVE);
    }
    lgr->level = m_ptr->m_lgs.level;
    return (OK);
}

/*===========================================================================*
 *              do_write_log                    
 *===========================================================================*/
int do_write_log(message *m_ptr) {
    char msg[MAX_MSG];
    swrite("LGS: do_write_log\n", PRINT_STDOUT);
    struct logger* lgr = find_logger(m_ptr->m_lgs.logger);
    if(!lgr) {
        return (ERR_UNKNOWN);
    }
    if(!lgr->active) {
        return (ERR_NOTACTIVE);
    }
    if(lgr->proc != m_ptr->m_source) {
        return (ERR_ACTIVE);
    }
    if(lgr->level <= m_ptr->m_lgs.level) {
        char *curr_time = get_time_now();
        if(!curr_time) {
            return ERR_INTERNAL;
        }
        char *pname = get_process_name(m_ptr->m_source);
        if(!pname) {
            return ERR_INTERNAL;
        }
        const char *level = levels[m_ptr->m_lgs.level];
        int status = sys_datacopy(m_ptr->m_source, m_ptr->m_lgs.msg_loc, SELF, 
                                  (vir_bytes) msg, m_ptr->m_lgs.msg_len);
        if(status != OK) {
            return ERR_INTERNAL;
        }
        status = do_log(lgr->format, pname, curr_time, 
                            level, msg, lgr->fd);                                
        if(status != OK) {
            return ERR_INTERNAL;
        }
    }
    return (OK);
}

/*===========================================================================*
 *              do_close_log                    
 *===========================================================================*/
int do_close_log(message *m_ptr) {
    swrite("LGS: do_close_log\n", PRINT_STDOUT);
    struct logger* lgr = find_logger(m_ptr->m_lgs.logger);
    if(!lgr) {
        return (ERR_UNKNOWN);
    }
    if(!lgr->active) {
        return (ERR_NOTACTIVE);
    }
    if(lgr->proc != m_ptr->m_source) {
        return (ERR_ACTIVE);
    }
    lgr->active = lgr->proc = 0;
    if(lgr->type == LT_FILE) {
        close(lgr->fd); 
    }
    lgr->fd = -1;
    return (OK);
}

/*===========================================================================*
 *              do_clear_logs                    
 *===========================================================================*/
int do_clear_logs(message *m_ptr) {
    swrite("LGS: do_clear_logs\n", PRINT_STDOUT);
    char* ptr = (m_ptr->m_lgs.logger);
    if(!(*m_ptr->m_lgs.logger)) {
        int i;
        for(i = 0; i < num_loggers; i++) {
            if(loggers[i].type == LT_FILE) {
                if(loggers[i].active) {
                    return (ERR_ACTIVE);
                }
                unlink(loggers[i].filename);
            }
        }
    } else {
        char *token = m_ptr->m_lgs.logger;
        char *curr = m_ptr->m_lgs.logger;
        char *end = m_ptr->m_lgs.logger + strlen(m_ptr->m_lgs.logger);
        struct logger* lgr;
        while(token < end) {
            while((*curr)!='\0' && (*curr)!=',') {
                ++curr;
            }
            (*curr) = '\0';
            lgr = find_logger(token);
            if(!lgr) {
                return (ERR_UNKNOWN);
            }
            if(lgr->type == LT_FILE) {
                if(lgr->active) {
                    return (ERR_ACTIVE);
                }
                unlink(lgr->filename);
            }
            ++curr;
            token = curr;
        }
    }
    return (OK);
}

/*===========================================================================*
 *              do_getsysinfo                    
 *===========================================================================*/
int do_getsysinfo(const message *m_ptr) {
    swrite("LGS: do_getsysinfo\n", PRINT_STDOUT);
    vir_bytes src_addr;
    size_t length;
    int status;

    switch(m_ptr->m_lsys_getsysinfo.what) {
    case SI_LOGGERS:
      src_addr = (vir_bytes)loggers;
      length = sizeof(struct logger) * MAX_LOGGERS;
      break;
    default:
      return (EINVAL);
    }

    if(length != m_ptr->m_lsys_getsysinfo.size) {
        return (EINVAL);
    }

    status = sys_datacopy(SELF, src_addr, m_ptr->m_source, 
                          m_ptr->m_lsys_getsysinfo.where, length);
    if(status != OK) {
        swrite("LGS: Copy failed.\n", PRINT_STDOUT);
        return status;
    }
    return (OK);
}


/*===========================================================================*
 *              sef_cb_init                    
 *===========================================================================*/
int sef_cb_init(int type, sef_init_info_t *info) {
    swrite("LGS: sef_cb_init\n", PRINT_STDOUT);
    memset(loggers, 0, sizeof(loggers));
    return (OK);
}

/* Loads logger configuration from the conf file. */
int do_load_conf(char* conf_path) {
    int conf = open(conf_path, O_RDONLY);
    if(conf == -(EINVAL)) {
        swrite("LGS: Couldn't open configuration file.\n", PRINT_STDOUT);
        return errno;
    }
    num_loggers = 0;
    char* token;
    while((token = read_token(MAX_NAME, conf))) {
        if(num_loggers == MAX_LOGGERS) {
            swrite("LGS: Too many loggers, stopped reading conf file.\n", PRINT_STDOUT);
            return (EINVAL);
        }
        strcpy(loggers[num_loggers].name, token);

        // Loads logger level.
        if(!(token = read_token(MAX_LEVEL, conf))) {
            swrite("LGS: Error parsing conf file, unexpected EOF.\n", PRINT_STDOUT);
            return (EINVAL);
        }
        if(strcmp(token, "TRACE") == 0) {
            loggers[num_loggers].level = LL_TRACE;
        } else if(strcmp(token, "DEBUG") == 0) {
            loggers[num_loggers].level = LL_DEBUG;
        } else if(strcmp(token, "ERROR") == 0) {
            loggers[num_loggers].level = LL_ERROR;
        } else {
            swrite("LGS: Error parsing conf file, invalid logger level.\n", PRINT_STDOUT);
            return (EINVAL);
        }

        // Loads logger format.
        if(!(token = read_token(MAX_FORMAT, conf))) {
            swrite("LGS: Error parsing conf file, unexpected EOF.\n", PRINT_STDOUT);
            return (EINVAL);
        }
        strcpy(loggers[num_loggers].format, token);

        // Loads logger type.
        if(!(token = read_token(MAX_TYPE, conf))) {
            swrite("LGS: Error parsing conf file, unexpected EOF.\n", PRINT_STDOUT);
            return (EINVAL);
        }
        if(strcmp(token, "STDOUT") == 0) {
            loggers[num_loggers].type = LT_STDOUT;
        } else if(strcmp(token, "STDERR") == 0) {
            loggers[num_loggers].type = LT_STDERR;
        } else if(strcmp(token, "FILE") == 0) {
            loggers[num_loggers].type = LT_FILE;

            // Loads filename.
            if(!(token = read_token(MAX_FILENAME, conf))) {
                swrite("LGS: Error parsing conf file, unexpected EOF.\n", PRINT_STDOUT);
                return (EINVAL);
            }
            strcpy(loggers[num_loggers].filename, token);

            // Loads append flag.
            if(!(token = read_token(MAX_APPEND, conf))) {
                swrite("LGS: Error parsing conf file, unexpected EOF.\n", PRINT_STDOUT);
                return (EINVAL);
            }
            if(strcmp(token, "A") == 0) {
                loggers[num_loggers].mode = 'a';
            } else if(strcmp(token, "W") == 0) {
                loggers[num_loggers].mode = 'w';
            } else {
                swrite("LGS: Error parsing conf file, invalid append flag.\n", PRINT_STDOUT);
                return (EINVAL);
            }
        } else {
            swrite("LGS: Error parsing conf file, invalid logger type.\n", PRINT_STDOUT);
            return (EINVAL);
        }

        read_token(0, conf); // Empty line
        ++num_loggers;
    }
    return (OK);
}
