#ifndef HOP_H
#define HOP_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <pwd.h>

#define MAXL 4096
#define redprompt "\e[0;91m"
#define normal "\033[0m"
void hop(char **args, int arg_count, char *home_directory, char *previous_directory);

#endif 