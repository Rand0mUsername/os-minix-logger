#include <lib.h>
#include <minix/syslib.h>
#include <string.h>

int main()  {
    int kom, ret;
    char logger[32];
    char message[1024];
    int level, bit;
    while(1) {
        printf("1 - int start_log(char* logger)\n");
        printf("2 - int set_logger_level(char* logger, int level)\n");
        printf("3 - int write_log(char* logger, char* message, int message_level)\n");
        printf("4 - int close_log(char* logger)\n");
        printf("5 - int clear_logs(char* logger)\n\n");
        scanf("%d", &kom);
        switch(kom) {
        case 1:
            printf("Logger name: ");
            scanf("%s", logger);
            ret = start_log(logger);
            break;
        case 2:
            printf("Logger name: ");
            scanf("%s", logger);
            printf("Level(1 - TRACE, 2 - DEBUG, 3 - ERROR): ");
            scanf("%d", &level);
            ret = set_logger_level(logger, level);
            break;
        case 3:
            printf("Logger name: ");
            scanf("%s", logger);
            printf("Message: ");
            scanf("%s", message);
            printf("Level(1 - TRACE, 2 - DEBUG, 3 - ERROR): ");
            scanf("%d", &level);
            ret = write_log(logger , message, level);
            break;
        case 4:
            printf("Logger name: ");
            scanf("%s", logger);
            ret = close_log(logger);
            break;
        case 5:
            printf("Send NULL (1 - da, 0 - ne): ");
            scanf("%d", &bit);
            if(bit) {
                ret = clear_logs(NULL);
            } else {
                printf("Logger names (comma-separated): ");
                scanf("%s", logger);
                ret = clear_logs(logger);
            }
            break;
        }
        printf("Exit code: %d\n", ret);
        switch(ret) {
        case 0:
            printf("Success.\n");
            break;
        case 1:
            printf("Fail: logger is active.\n");
            break;
        case 2:
            printf("Fail: unknown logger.\n");
            break;
        case 3:
            printf("Fail: internal error.\n");
            break;
        case 4:
            printf("Fail: logger not active.\n");
            break;
        }
    }
}