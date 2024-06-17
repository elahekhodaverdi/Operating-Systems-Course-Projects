#ifndef __TCP_H
#define __TCP_H

#include "consts.h"
#include <stdint.h>
#include <stdio.h>
int connect_tcp_server(int port, int lsn);

int accept_tcp_client(int server_fd);

int connect_tcp_client(int port);

uint16_t find_unused_port();

#endif
