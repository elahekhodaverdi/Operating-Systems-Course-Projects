#ifndef __TYPES_H
#define __TYPES_H

#include "consts.h"
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

typedef enum
{
    REJECT,
    WAITING,
    ACCEPT
} OrderState;
typedef struct
{
    int fd;
    struct sockaddr_in addr;
} BroadcastInfo;

typedef struct
{
    int fd;
} TCPInfo;

typedef struct
{
    char name[NAME_SIZE];
    int amount;
} Ingredient;

typedef struct
{
    char name[NAME_SIZE];
    int num_ing;
    Ingredient *ingredients;
} Recipe;

typedef enum
{
    CLOSED,
    OPENED
} ResState;
typedef struct
{
    char food_name[NAME_SIZE];
    uint16_t port;
    int fd;
    char name[NAME_SIZE];
    OrderState state;
} OrderFood;
typedef struct
{
    int file_fd;
    uint16_t port;
    char name[NAME_SIZE];
    ResState state;
    BroadcastInfo brcinfo;
    TCPInfo tcpinfo;
    int num_order;
    int num_recipes;
    OrderFood *orders;
    int num_ing;
    Ingredient *ingredients;
} Restaurant;

typedef struct
{
    int file_fd;
    uint16_t port;
    char name[NAME_SIZE];
    BroadcastInfo brcinfo;
    TCPInfo tcpinfo;

} Customer;

typedef struct
{
    int file_fd;
    uint16_t port;
    char name[NAME_SIZE];
    BroadcastInfo brcinfo;
    TCPInfo tcpinfo;
    OrderState order;
    int order_fd;
} Supplier;

#endif