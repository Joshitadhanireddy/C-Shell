#ifndef ACTIVITIES_H
#define ACTIVITIES_H

#include <sys/types.h>
#include "signals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#define MAX_PROCESSES 100
#define MAX_COMMAND_LENGTH 4096

struct ProcessInfo {
    pid_t pid;
    char command[MAX_COMMAND_LENGTH];
    int is_running;
};
extern struct ProcessInfo processes[MAX_PROCESSES];
void add_process(pid_t pid, const char *command);
void remove_process(pid_t pid);
void update_process_status(pid_t pid, int is_running);
void activities();

#endif 