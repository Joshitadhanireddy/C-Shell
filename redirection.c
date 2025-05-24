#include "redirection.h"

int handle_io_redirection(char **args, int *in_fd, int *out_fd) 
{
    int i, append = 0;
    *in_fd = STDIN_FILENO;
    *out_fd = STDOUT_FILENO;

    for(i = 0; args[i] != NULL; i++) 
    {
        if(strcmp(args[i], "<") == 0) 
        {
            if(args[i+1] == NULL) 
            {
                fprintf(stderr, redprompt "Error: No input file specified\n" normal);
                return -1;
            }
            *in_fd = open(args[i+1], O_RDONLY);
            if(*in_fd == -1) 
            {
                fprintf(stderr, redprompt "No such input file found!\n" normal);
                return -1;
            }
            args[i] = NULL;
            i++;
        } 
        else if(strcmp(args[i], ">") == 0 || strcmp(args[i], ">>") == 0) 
        {
            if(strcmp(args[i], ">>") == 0) append = 1;
            if(args[i+1] == NULL) 
            {
                fprintf(stderr, redprompt "Error: No output file specified\n" normal);
                return -1;
            }
            *out_fd = open(args[i+1], O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC), 0644);
            if(*out_fd == -1) 
            {
                fprintf(stderr, redprompt "Error opening output file\n" normal);
                return -1;
            }
            args[i] = NULL;
            i++;
        }
    }
    return 0;
}

void cleanup_redirection(int in_fd, int out_fd) 
{
    if (in_fd != STDIN_FILENO) close(in_fd);
    if (out_fd != STDOUT_FILENO) close(out_fd);
}