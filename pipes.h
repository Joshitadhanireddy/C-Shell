#ifndef PIPES_H
#define PIPES_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "redirection.h"
#include <ctype.h>
#include "prompts.h"  

#define MAX_PIPES 10

int execute_piped_commands(char *command);

#endif 