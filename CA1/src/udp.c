#include "udp.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <asm-generic/socket.h>

void broadcast_msg(int sockfd, struct sockaddr_in *addr, const char *msg)
{
    struct sockaddr *dest = (struct sockaddr *)addr;
    int check = sendto(sockfd, msg, strlen(msg), 0, dest, sizeof(*dest)) ;
    if (check == -1)
        exit(0);
}

int connect_udp(const int port, struct sockaddr_in *addrOut)
{
    int sockfd, broadcast = 1, reuse_port = 1;

    struct sockaddr_in addr;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &reuse_port, sizeof(reuse_port));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr("10.0.2.255");

    bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));

    *addrOut = addr;
    return sockfd;
}

