#ifndef SIGNALS_H
#define SIGNALS_H

#include <sys/types.h>
#include "activities.h"
#include "prompts.h"

void ping(pid_t pid, int signal_number);
void setup_signal_handlers();
void handle_sigint(int sig);
void handle_sigtstp(int sig);
void sigchld_handler(int sig);
void terminate_all_processes();
extern pid_t foreground_pid;

#endif