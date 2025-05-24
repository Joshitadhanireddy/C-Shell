#ifndef REVEAL_H
#define REVEAL_H

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <unistd.h>

#define redprompt "\e[0;91m"
#define blueprompt  "\033[94m"
#define whiteprompt "\033[37m"
#define greenprompt  "\033[32m"
#define normal "\033[0m"
#define MAXL 4096
void reveal(char *flags, char *path, char *home_directory, char *previous_directory);

#endif
