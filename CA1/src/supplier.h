#ifndef __SUPPLIER_H
#define __SUPPLIER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#include "consts.h"
#include "types.h"
#include "fdset.h"
#include "udp.h"
#include "tcp.h"
#include "utils.h"
#include "log.h"
#include "signal.h"
void interface(Supplier *spl);
void clinput(Supplier *spl);
void brinput(Supplier *spl);
void tcpinput(Supplier *spl,FDSet *fdset);
void show_suppliers(Supplier *spl, uint16_t port);
void answer_request(Supplier *spl);
void request_ingredient(Supplier*spl);
void cancel_request(Supplier*spl);
char *get_unique_name(Supplier *spl);
void send_name(Supplier *spl, uint16_t port);
#endif