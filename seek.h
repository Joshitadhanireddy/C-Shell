

#ifndef SEEK_H
#define SEEK_H

#include "prompts.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_PATH 1024
#define redprompt "\e[0;91m"
#define pinkprompt  "\033[38;5;213m"
#define orangeprompt "\033[38;5;208m"
#define yellowprompt  "\e[0;33m"
#define normal "\033[0m"
#define blueprompt  "\033[94m"
#define whiteprompt "\033[37m"
#define greenprompt  "\033[32m"
#define redprompt "\e[0;91m"
#define pinkprompt  "\033[38;5;213m"
#define orangeprompt "\033[38;5;208m"
#define yellowprompt  "\e[0;33m"

void seek(int argc, char *argv[], const char *homedir, char *previousdir);
bool is_directory(const char *path);
void print_colored(const char *path, bool is_dir);

#endif 