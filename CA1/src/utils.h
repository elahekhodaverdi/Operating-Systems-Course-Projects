#ifndef __UTILS_H
#define __UTILS_H

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include "consts.h"
#include "types.h"
#include "fdset.h"
#include "cJSON.h"
#include <termios.h>
#include <fcntl.h>

uint16_t strtoint(const char *str);
void get_input(const char *prompt, char *buf, size_t bufLen);
void decode(char msg[MSG_BUF_LEN], char **name, uint16_t *port, char **role, char **command, char **data);
Recipe* read_json_file(int *num_recipes);
void alarm_handler(int sig);
char * read_file();


#endif


