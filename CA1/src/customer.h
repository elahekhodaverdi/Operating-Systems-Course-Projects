#ifndef __CUSTOMER_H
#define __CUSTOMER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <termios.h>

#include "consts.h"
#include "udp.h"
#include "tcp.h"
#include "utils.h"
#include "fdset.h"
#include "signal.h"
#include "types.h"
#include "log.h"

void interface(Customer *cst, Recipe *recipes, int *num_food);
void brinput(Customer *cst);
void clinput(Customer *cst, Recipe *recipes, int *num_food);
void show_restaurants(Customer *cst);
void show_menu(Recipe *recipes, int *num_food);
void submit_order_food(Customer *cst);
void order_food(Customer *cst, int server_fd, char *name);
void send_name(Customer *cst, uint16_t port);
char * get_unique_name(Customer*cst);
void get_order_answer(char buf[MSG_BUF_LEN],int nb);
#endif