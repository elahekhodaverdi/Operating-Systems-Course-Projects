#ifndef __FDSET_H
#define __FDSET_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include "fdset.h"
typedef struct
{
    fd_set working;
    fd_set master;
    int max;
} FDSet;

void set_fd(int socket, FDSet *fdset);
void clear_fd(int socket, FDSet *fdset);
#endif