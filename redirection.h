#ifndef REDIRECTION_H
#define REDIRECTION_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_ARGS 100
#define redprompt "\e[0;91m"
#define normal "\033[0m"

int handle_io_redirection(char **args, int *in_fd, int *out_fd);
void cleanup_redirection(int in_fd, int out_fd);

#endif