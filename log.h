#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LOG_ENTRIES 15
#define LOG_FILE ".command_log"
#define MAXL 4096
void init_log();
void add_command_to_log(const char *command);
void display_log();
void purge_log();
void execute_log_command(int index);
void save_log();
void load_log();

#endif 
