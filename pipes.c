#include "pipes.h" 
extern int background;
extern int tag;
extern int is_pipe;
extern char username[MAXL];
extern char sysname[MAXL];
extern char currentdir[MAXL];
int arg_c;
int is_custom_or_log_command(const char *command) 
{
    return(strcmp(command, "hop") == 0 ||
            strcmp(command, "reveal") == 0 ||
            strcmp(command, "seek") == 0 ||
            strcmp(command, "proclore") == 0 ||
            strcmp(command, "log") == 0 ||
            strcmp(command, "activities") == 0 ||
            strcmp(command, "iMan") == 0 ||
            strcmp(command, "ping") == 0 ||
            strcmp(command, "neonate") == 0 ||
            strcmp(command, "fg") == 0 ||
            strcmp(command, "bg") == 0
            );

}

int execute_custom_or_log_command(char *command, char **args) 
{   
    
    if(strcmp(command, "hop") == 0) 
    {
        arg_c = 0;
        while (args[arg_c] != NULL) arg_c++;
        hop(args + 1, arg_c, homedir, previousdir); 
    } 
    else if(strcmp(command, "reveal") == 0) 
    {
        char flags[MAXL] = "";
        char *path = ".";
        for(int i = 1; args[i] != NULL; i++) 
        {
            if(args[i][0] == '-') 
            {
                strcat(flags, args[i] + 1);
            } 
            else 
            {
                path = args[i];
            }
        }
        reveal(flags, path, homedir, previousdir);
    } 
    else if(strcmp(command, "seek") == 0) 
    {
        int arg_c = 0;
        while (args[arg_c] != NULL) arg_c++;
        seek(arg_c, args, homedir, previousdir);
    } 
    else if(strcmp(command, "activities") == 0) 
    {
        activities();
    } 
    else if(strcmp(command, "proclore") == 0) 
    {
        if(args[1] == NULL) 
        {
            get_process_info(getpid());
        } 
        else 
        {
            pid_t pid = atoi(args[1]);
            get_process_info(pid);
        }
    } 
    else if(strcmp(args[0], "iMan") == 0) 
    {
        arg_c = 0;
        while (args[arg_c] != NULL) arg_c++;
        if(arg_c < 2)
        {
            printf(redprompt "Usage: iMan <man_page>\n" normal);
        }
        else
        {
            iman(args[1]);
        }
    }
    else if(strcmp(args[0], "ping") == 0) 
    {
        arg_c = 0;
        while (args[arg_c] != NULL) arg_c++;
        if(arg_c < 3) 
        {
            printf(redprompt "Usage: ping <pid> <signal_number>\n" normal);
        } 
        else 
        {
            pid_t target_pid = atoi(args[1]);
            int signal_num = atoi(args[2]);
            ping(target_pid, signal_num);
        }
    } 
    else if(strcmp(args[0], "neonate") == 0)
    {
        if (strcmp(args[1], "-n") != 0) 
        {
            printf(redprompt "Usage: neonate -n [time_arg]\n" normal);
            return;
        }
        if (args[2] == NULL) 
        {
            printf(redprompt "Error: Missing time argument\n" normal);
            return;
        }
        int time_arg = atoi(args[2]);
        if(time_arg < 0) 
        {
            printf(redprompt "Invalid time_arg: must be a positive integer\n" normal);
            return;
        }
        neonate_n(time_arg);
    }
    else if(strcmp(args[0], "fg") == 0) 
    {
        arg_c = 0;
        while (args[arg_c] != NULL) arg_c++;
        if(arg_c < 2)
        {
            printf(redprompt "Usage: fg <pid> \n" normal);
        } 
        else 
        {
            pid_t target_pid = atoi(args[1]);
            handle_fg(target_pid);
        }
    } 
    else if(strcmp(args[0], "bg") == 0) 
    {
        arg_c = 0;
        while (args[arg_c] != NULL) arg_c++;
        if(arg_c < 2)
        {
            printf(redprompt "Usage: bg <pid> \n" normal);
        } 
        else 
        {
            pid_t target_pid = atoi(args[1]);
            handle_bg(target_pid);
        }
        
    } 
    else if(strcmp(command, "log") == 0) 
    {
        if(args[1] == NULL || strcmp(args[1], "") == 0) 
        {
            display_log();
        } 
        else if(strcmp(args[1], "purge") == 0) 
        {
            purge_log();
        }else if(strncmp(args[1], "execute", 7) == 0) 
        {
            if(args[2] != NULL) 
            {
                int index = atoi(args[2]);
                execute_log_command(index);
            } 
            else 
            {
                fprintf(stderr, "Error: No index provided for log execute\n");
            }
        } 
        else 
        {
            fprintf(stderr, "Error: Invalid log command\n");
        }
    }   
    return 0;
}

int execute_piped_commands(char *command) 
{
    char *commands[MAX_PIPES + 1];
    int command_count = 0;
    char *token = strtok(command, "|");
    while(token != NULL && command_count < MAX_PIPES) 
    {
        commands[command_count++] = token;
        token = strtok(NULL, "|");
    }
    commands[command_count] = NULL;
    
    if(command_count == 0) 
    {
        printf(redprompt"Invalid use of pipe\n"normal);
        return -1;
    }
    
    int pipes[MAX_PIPES][2];
    for(int i = 0; i < command_count - 1; i++) 
    {
        if(pipe(pipes[i]) == -1) 
        {
            perror("pipe");
            return -1;
        }
    }

    for(int i = 0; i < command_count; i++) 
    {
        int is_background = 0;
        char *ampersand = strchr(commands[i], '&');
        if(ampersand != NULL) 
        {
            *ampersand = '\0';
            is_background = 1;
        }

        pid_t pid = fork();
        if(pid == 0) 
        {
            if(i > 0) 
            {
                dup2(pipes[i-1][0], STDIN_FILENO);
                close(pipes[i-1][0]);
                close(pipes[i-1][1]);
            }
            if(i < command_count - 1) 
            {
                dup2(pipes[i][1], STDOUT_FILENO);
                close(pipes[i][0]);
                close(pipes[i][1]);
            }

            for(int j = 0; j < command_count - 1; j++) 
            {
                if(j != i - 1 && j != i) 
                {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }
            }

            char *args[MAX_ARGS];
            int arg_count = 0;
            char *cmd_copy = strdup(commands[i]);
            token = strtok(cmd_copy, " ");
            int in_quotes = 0;
            char quote_buffer[MAXL] = "";
            
            while(token != NULL && arg_count < MAX_ARGS - 1) 
            {
                if(token[0] == '"' && token[strlen(token) - 1] == '"') 
                {
                    token[strlen(token) - 1] = '\0';
                    args[arg_count++] = strdup(token + 1);
                } 
                else if(token[0] == '"') 
                {
                    in_quotes = 1;
                    strcpy(quote_buffer, token + 1);
                    strcat(quote_buffer, " ");
                } 
                else if(token[strlen(token) - 1] == '"' && in_quotes) 
                {
                    in_quotes = 0;
                    strcat(quote_buffer, token);
                    quote_buffer[strlen(quote_buffer) - 1] = '\0';
                    args[arg_count++] = strdup(quote_buffer);
                    quote_buffer[0] = '\0';
                } 
                else if(in_quotes) 
                {
                    strcat(quote_buffer, token);
                    strcat(quote_buffer, " ");
                } 
                else 
                {
                    args[arg_count++] = strdup(token);
                }
                token = strtok(NULL, " ");
            }
            args[arg_count] = NULL;
            free(cmd_copy);
            int saved_stdout = dup(STDOUT_FILENO);
            int saved_stdin = dup(STDIN_FILENO);
            int in_fd = STDIN_FILENO;
            int out_fd = STDOUT_FILENO; 
            if(handle_io_redirection(args, &in_fd, &out_fd) == -1) 
            {
                exit(EXIT_FAILURE);
            }

            if(in_fd != STDIN_FILENO) 
            {
                dup2(in_fd, STDIN_FILENO);
                close(in_fd);
            }

            if(out_fd != STDOUT_FILENO) 
            {
                dup2(out_fd, STDOUT_FILENO);
                close(out_fd);
            }

            if (strcmp(args[0], "mk_hop") == 0 || strcmp(args[0], "hop_seek") == 0) 
            {
                execute_myshrc_function(args[0], args, arg_count);
            } 
            else if(is_custom_or_log_command(args[0])) 
            {
                execute_custom_or_log_command(args[0], args);
                exit(EXIT_SUCCESS);
            } 
            else 
            {
                execvp(args[0], args);
                fprintf(stderr, "ERROR: '%s' is not a valid command\n", args[0]);
                exit(EXIT_FAILURE);
            }

            for(int j = 0; j < arg_count; j++) 
            {
                free(args[j]);
            }
            exit(EXIT_FAILURE);
        } 
        else if(pid < 0) 
        {
            perror("fork");
            return -1;
        }
        
        if (!is_background) 
        {
            waitpid(pid, NULL, 0);
        } 
        else 
        {
            add_process(pid, commands[i]);
        }
    }

    for(int i = 0; i < command_count - 1; i++) 
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    
    return 0;
}
