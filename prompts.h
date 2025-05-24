#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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
#include <sys/time.h>
#include <signal.h>
#include "proclore.h"
#include "pipes.h"
#include "redirection.h"
#include "activities.h"
#include "hop.h"
#include "reveal.h"
#include "seek.h"
#include "log.h"
#include "signals.h"
#include "fgbg.h"
#include "neonate.h"
#include "man.h"
#include "myshrc.h"
extern char previousdir[MAXL];
extern char homedir[MAXL];
extern int process_count;
#define MAX_PROCESSES 100
#define MAX_COMMAND_LENGTH 4096
#define redprompt "\e[0;91m"
#define pinkprompt  "\033[38;5;213m"
#define orangeprompt "\033[38;5;208m"
#define yellowprompt  "\e[0;33m"
#define normal "\033[0m"
#define MAXL 4096
void getprompt();
void printprompt();
void execute_command(char *command);
void handle_background_processes();
static pid_t background_pids[MAXL];
static int background_count = 0;
#endif