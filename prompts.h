#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <limits.h>
#include <pwd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <grp.h>
#include <dirent.h>
#include <errno.h>
#include <ctype.h>

char homedir[4096];
#define redprompt "\e[0;91m"
#define pinkprompt  "\033[38;5;213m"
#define orangeprompt "\033[38;5;208m"
#define yellowprompt  "\e[0;33m"
#define normal "\033[0m"
#define MAXL 4096
void getprompt();
void printprompt();
void execute_command(char *command);

#endif