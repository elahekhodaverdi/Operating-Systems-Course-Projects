#include "tcp.h"

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <stdio.h>

#include "consts.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <asm/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

uint16_t find_unused_port()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Socket creation failed");
        return -1;
    }
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 
    addr.sin_port = 0;
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Socket binding failed");
        close(sockfd);
        return -1;
    }
    socklen_t addrLen = sizeof(addr);
    if (getsockname(sockfd, (struct sockaddr *)&addr, &addrLen) < 0)
    {
        perror("getsockname failed");
        close(sockfd);
        return -1;
    }
    uint16_t port = (uint16_t)ntohs(addr.sin_port);
    close(sockfd);
    return port;
}

int connect_tcp_server(int port,int lsn)
{
    struct sockaddr_in address;
    int server_fd;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int op = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1)
    {
        close(server_fd);
        return -1;
    }

    listen(server_fd, lsn);

    return server_fd;
}

int accept_tcp_client(int server_fd)
{
    int client_fd;
    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);

    client_fd = accept(server_fd, (struct sockaddr *)&client_address, (socklen_t *)&address_len);

    return client_fd;
}

int connect_tcp_client(int port)
{
    int fd;
    struct sockaddr_in server_address;

    fd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    { 
        write(STDOUT_FILENO, "Error in connecting to server\n", 31);
        return -1;
    }

    return fd;
}

