#ifndef __UDP_H
#define __UDP_H


#include <netinet/in.h>

int connect_udp(const int port,struct sockaddr_in* addr);
void broadcast_msg(int sockfd, struct sockaddr_in* addr, const char* msg);


#endif
