#ifndef _LGS_UTIL_H_
#define _LGS_UTIL_H_

#include <sys/types.h>
#include <minix/config.h>
#include <minix/ds.h>
#include <minix/bitmap.h>
#include <minix/param.h>
#include <regex.h>

/* maximum message length */
#define MAX_MSG         1024  

/* maximum config file line length */
#define MAX_CONF        128

/* special fd values for stdout/stderr (open() returns
 * file descriptors for actual files starting from 
 * 1 = STDOUT_FILENO)
*/
#define PRINT_STDOUT (-3)
#define PRINT_STDERR (-2)

void swrite(const char* s, int fd);
void buff_append(char *buff, int *it, const char *data);
char *read_token(int sz, int fd);
char *get_time_now();
char *get_process_name(endpoint_t ep);

#endif /* _LGS_UTIL_H_ */
