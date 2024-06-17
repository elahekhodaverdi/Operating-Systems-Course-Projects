#ifndef __RESTUARANT_H
#define __RESTUARANT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "fdset.h"
#include <termios.h>
#include <string.h>
#include "consts.h"
#include "tcp.h"
#include "types.h"
#include "signal.h"
#include "log.h"
void interface(Restaurant *rst, Recipe *recipes, int *num_recipes);
void clinput(Restaurant *rst, Recipe *recipes, int *num_recipes, FDSet *fdset);
void brinput(Restaurant *rst);
void show_suppliers(Restaurant *rst);
void show_recipes(Recipe *recipes, int *num_recipes);
void show_ingredients(Ingredient *ingredients, int *num_ing);
void show_restaurants(Restaurant *rst, uint16_t port);
void start_working(Restaurant *rst);
void request_ingredient(Restaurant *rst);
int send_request_ingredient(Restaurant *rst, uint16_t port);
void answer_ingredient(Restaurant *rst, int server_fd, char *name, uint16_t port, int amount);
void add_ingredient(Restaurant *rst, char *name, int amount);
void tcpinput(Restaurant *rst, FDSet *fdset);
void add_order(Restaurant *rst, uint16_t port, int fd, char *name, char *food_name);
void answer_request(Restaurant *rst, Recipe *recipes, int *num_recipes, FDSet *fdset);
void delete_order(Restaurant *rst, int fd);
int find_order(Restaurant *rst, uint16_t port);
int find_recipe(Recipe *rst, int *num_recipes, char *name);
void show_order_queue(Restaurant *rst);
void show_order_history(Restaurant *rst);
void send_name(Restaurant *rst, uint16_t port);
int enough_ingredients(Restaurant *rst, Recipe *recipes, int *num_recipes, int index_food);
char *get_unique_name(Restaurant *rst);
int enough_ingredient(Restaurant*rst,Ingredient ingredient);
void decrease_ingredient(Restaurant*rst,Ingredient ingredient);
#endif