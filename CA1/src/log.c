#include "log.h"

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include "consts.h"
void logNormal(int fd, const char *msg)
{
    write(fd, msg, strlen(msg));
    write(fd, "\n", 1);
}

void logInput(int fd, const char *msg)
{
    write(fd, "[Input] ", 8);
    write(fd, msg, strlen(msg));
}

void logMsg(int fd, const char *msg)
{
    write(fd, "[Message] ", 10);
    write(fd, msg, strlen(msg));
    write(fd, "\n", 1);
}

void logInfo(int fd, const char *msg)
{
    write(fd, "[Info] ", 7);
    write(fd, msg, strlen(msg));
    write(fd, "\n", 1);
}

void logWarning(int fd, const char *msg)
{
    write(fd, "[Warning] ", 10);
    write(fd, msg, strlen(msg));
    write(fd, "\n", 1);
}

void logError(int fd, const char *msg)
{
    write(fd, "[Error] ", 8);
    write(fd, msg, strlen(msg));
    write(fd, "\n", 1);
}

int open_log_file(char *name){
    char filename[NAME_SIZE+4];
    sprintf(filename, "%s.txt", name);
    int fd = open(filename, O_WRONLY | O_CREAT, 0644);
    return fd;
}
