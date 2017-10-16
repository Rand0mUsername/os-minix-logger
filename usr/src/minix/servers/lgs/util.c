#include "util.h"
#include "inc.h"
#include <fcntl.h>
#include <lib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include "../pm/mproc.h"

/* A function that outputs a string using the
 * given file descriptor depending on stream
 * type. printf(kprintf) is used for stdout/stderr
 * and regular write call is used for files. 
 */
void swrite(const char* s, int fd) {
    if(fd == PRINT_STDOUT) {
        printf("[stdout] %s", s);
    } else if(fd == PRINT_STDERR) {
        printf("[stderr] %s", s);
    } else {
        write(fd, s, strlen(s));
    }
}

/* Appends a string to output buffer */
void buff_append(char *buff, int *it, const char *data) {

    snprintf(buff+(*it), strlen(data)+1, "%s", data);
    (*it) += strlen(data);
}

/* Tries to read next sz bytes
 * from the stream given by fd and then strips
 * all whitespace - used for extracting tokens
 * from a configuration file.
 */
char *read_token(int sz, int fd) {
    static char buff[MAX_CONF];
    char* token;
    if(read(fd, buff, sz+1) <= 0) {
        return NULL;
    }
    buff[sz+1] = '\0';
    token = buff;
    while((*token) == ' ') {
        ++token;
    }
    if(token[strlen(token)-1] != '\n') {
        swrite("LGS: Invalid conf file\n", STDOUT_FILENO);
        return NULL;
    }
    token[strlen(token)-1] = '\0'; // \n
    return token;
}

/* Gets current time (GMT+2) as a "HH:MM:SS" 
 * formatted string. 
 */
char *get_time_now() {
    static char date[9];
    message m;
    memset(&m, 0, sizeof(m));
    int status = _syscall(PM_PROC_NR, PM_GETTIMEOFDAY, &m); 
    if(status != 0) {
        return NULL;
    }
    time_t total_secs = m.m_pm_lc_time.sec % (60*60*24);   
    time_t hrs = total_secs / (60*60);
    time_t min = (total_secs % (60*60)) / 60;
    time_t sec = (total_secs % (60*60)) % 60;
    snprintf(date, 9, "%02ld:%02ld:%02ld", ((long)hrs+2)%24, 
             (long)min, (long)sec);
    return date;
}

/* Gets process name from endpoint number. */
char *get_process_name(endpoint_t ep) {
    static struct mproc mproc[NR_PROCS];
    if (getsysinfo(PM_PROC_NR, SI_PROC_TAB, mproc, sizeof(mproc)) != OK) {
        return NULL;
    }
    int i;
    struct mproc *mp;
    static char mp_name[PROC_NAME_LEN];
    for(i=0; i<NR_PROCS; i++) {
        mp = &mproc[i];
        if(mp->mp_endpoint == ep) {
            strcpy(mp_name, mp->mp_name);
            return mp_name;
        }
    }
    return NULL;
}