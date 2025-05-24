#include "prompts.h"
int background = 0;
int tag = 1;
char *comm = NULL;
int is_pipe = 0;
char username[MAXL];
char sysname[MAXL];
char currentdir[MAXL];
char previousdir[MAXL];
char homedir[MAXL];
void logcommand(char* command)
{
    char *redirection_ptr = strchr(command, '>');
    int out_fd = STDOUT_FILENO;
    int saved_stdout = dup(STDOUT_FILENO); 
    if(redirection_ptr != NULL) 
    {
        *redirection_ptr = '\0';        
        int append_mode = (redirection_ptr[1] == '>') ? 1 : 0;
        char *filename = strtok(redirection_ptr + (append_mode ? 2 : 1), " ");
        if(filename == NULL) 
        {
            printf(redprompt "Error: No file specified for redirection\n" normal);
            return;
        }
        if(append_mode) 
        {
            out_fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
        } 
        else 
        {
            out_fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        }
        if(out_fd == -1) 
        {
            printf(redprompt "Error: shell_log could not be opened\n" normal);
            return;
        }
        dup2(out_fd, STDOUT_FILENO);
        close(out_fd); 
    }
    char *subcommand = strtok(command + 3, " "); 
    if (subcommand == NULL || strlen(subcommand) == 0) 
    {
        display_log();  
    } 
    else if(strcmp(subcommand, "purge") == 0) 
    {
        purge_log();
    } 
    else if(strncmp(subcommand, "execute", 12) == 0) 
    {
        char *index_str = strtok(NULL, " ");
        if(index_str != NULL) 
        {
            int index = atoi(index_str);
            execute_log_command(index);
        } 
        else 
        {
            printf(redprompt "Error: No index provided for log execute\n" normal);
        }
    } 
    else 
    {
        printf(redprompt "Error: Invalid log command\n" normal);
    }
    if(out_fd != STDOUT_FILENO) 
    {
        dup2(saved_stdout, STDOUT_FILENO);  
        close(saved_stdout);  
    }
}

void getprompt() 
{
    char cdir[MAXL];
    getcwd(cdir, sizeof(cdir));
    if(strstr(cdir, homedir) == cdir) 
    {
        if(strcmp(cdir, homedir) == 0) 
        {
            strcpy(currentdir, "~");
        } 
        else 
        {
            sprintf(currentdir, "~%s", cdir + strlen(homedir));
        }
    } 
    else 
    {
        strcpy(currentdir, cdir);
    }
}

void printprompt() 
{
    struct passwd *pwptr = getpwuid(getuid());
    strcpy(username, pwptr->pw_name);
    gethostname(sysname, sizeof(sysname));
    getprompt();
    printf(pinkprompt "<%s" orangeprompt "@%s" yellowprompt ":%s> ", username, sysname, currentdir);
    printf(normal);
}

void execute_command(char *command) 
{
    char *alias_command = get_alias_command(command); 
    if(alias_command != NULL) 
    {
        command = alias_command;
    }
   if(strchr(command,'|') != NULL) 
    {
        execute_piped_commands(command);
        return;
    }
    char command_copy[MAXL];
    strncpy(command_copy, command, sizeof(command_copy) - 1);
    command_copy[sizeof(command_copy) - 1] = '\0';  
    char *args[MAXL];
    int arg_count = 0;
    char *token = strtok(command_copy, " ");
    int in_quotes = 0;
    char quote_buffer[MAXL] = "";
    while(token != NULL && arg_count < MAXL) 
    {
        if(token[0] == '"' && token[strlen(token) - 1] == '"') 
        {
            token[strlen(token) - 1] = '\0';  
            args[arg_count++] = token + 1;
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
            args[arg_count++] = token;
        }
        token = strtok(NULL, " ");
    }
    args[arg_count] = NULL;
    if(arg_count == 0) 
    {
        printf(redprompt "Error: No command specified\n" normal);
        return;
    }
    int saved_stdout = dup(STDOUT_FILENO);
    int saved_stdin = dup(STDIN_FILENO);
    int in_fd = STDIN_FILENO;
    int out_fd = STDOUT_FILENO;
    if(handle_io_redirection(args, &in_fd, &out_fd) == -1) 
    {
        return;
    }
    if(strcmp(args[0], "hop") == 0) 
    {
        if(out_fd != STDOUT_FILENO) 
        {
            dup2(out_fd, STDOUT_FILENO);
        }
        hop(args + 1, arg_count - 1, homedir, previousdir);
        getprompt(); 
    }
    else if(strcmp(args[0], "mk_hop") == 0 || strcmp(args[0], "hop_seek") == 0) 
    {
        execute_myshrc_function(args[0], args, arg_count);
    }
    else if(strcmp(args[0], "reveal") == 0) 
    {
        if(out_fd != STDOUT_FILENO) 
        {
            dup2(out_fd, STDOUT_FILENO);
        }
        char flags[MAXL] = "";
        char *path = ".";
        int actual_count=arg_count;
        for(int i=0;i<arg_count;i++)
        {
            if(args[i]==NULL)
            {
                actual_count = i;
                break;
            }
        }
        for(int i = 1; i < actual_count; i++) 
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
    else if(strcmp(args[0], "seek") == 0) 
    {
        if(out_fd != STDOUT_FILENO) 
        {
            dup2(out_fd, STDOUT_FILENO);
        }
        seek(arg_count, args, homedir, previousdir);
    } 
    else if(strcmp(args[0], "activities") == 0) 
    {
        if(out_fd != STDOUT_FILENO) 
        {
            dup2(out_fd, STDOUT_FILENO);
        }
        activities();
    }
    else if(strcmp(args[0], "proclore") == 0) 
    {
        for(int i=0;i<arg_count;i++)
        {
            if(args[i]==NULL)
            {
                arg_count = i;
                break;
            }
        }
        if(out_fd != STDOUT_FILENO) 
        {
            dup2(out_fd, STDOUT_FILENO);
        }
        if(arg_count == 1) 
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
        if(arg_count < 2) 
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
         if(out_fd != STDOUT_FILENO) 
        {
            dup2(out_fd, STDOUT_FILENO); 
        }
        if(arg_count < 3) 
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
        if(out_fd != STDOUT_FILENO) 
        {
            dup2(out_fd, STDOUT_FILENO); 
        }
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
        if(out_fd != STDOUT_FILENO) 
        {
            dup2(out_fd, STDOUT_FILENO);
        }
        if(arg_count < 2)
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
        if(out_fd != STDOUT_FILENO) 
        {
            dup2(out_fd, STDOUT_FILENO);
        }
        if(arg_count < 2)
        {
            printf(redprompt "Usage: bg <pid> \n" normal);
        } 
        else 
        {
            pid_t target_pid = atoi(args[1]);
            handle_bg(target_pid);
        }
        
    } 
    else
    {
        struct timeval start, end;
        gettimeofday(&start, NULL);
        pid_t pid = fork();
        if(pid == 0) 
        {
            setpgid(0, 0);  

            if (in_fd != STDIN_FILENO) {
                dup2(in_fd, STDIN_FILENO);
                close(in_fd);
            }

            if (out_fd != STDOUT_FILENO) {
                dup2(out_fd, STDOUT_FILENO);
                close(out_fd);
            }
            execvp(args[0], args);
            fprintf(stderr, redprompt "ERROR : '%s' is not a valid command\n" normal, args[0]);
            exit(EXIT_FAILURE);
        } 
        else if(pid > 0) 
        {
            setpgid(pid, pid);
            if(background) 
            {
                printf("%d\n", pid);
                add_process(pid, args[0]);
            } 
            else 
            {
                foreground_pid = pid;
                int status;
                waitpid(pid, &status, WUNTRACED);
                foreground_pid = 0;
                gettimeofday(&end, NULL);
                long seconds = end.tv_sec - start.tv_sec;
                long microseconds = end.tv_usec - start.tv_usec;
                double elapsed = seconds + microseconds * 1e-6;
                if(elapsed > 2.0) 
                {
                    printf(pinkprompt "<%s" orangeprompt "@%s" yellowprompt ": %s sleep : %lds>" normal, username, sysname, currentdir, (long)elapsed);
                    tag = 0;
                }
            }
        }
        else 
        {
            perror("fork");
        }
    }

    if(out_fd != STDOUT_FILENO) 
    {
        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdout);
    }

    if(in_fd != STDIN_FILENO) 
    {
        dup2(saved_stdin, STDIN_FILENO);
        close(saved_stdin);
    }
}

void handle_background_processes() 
{
    int status;
    pid_t pid;
    for(int i = 0; i < process_count; i++) 
    {
        if(processes[i].is_running) 
        {
            pid = waitpid(processes[i].pid, &status, WNOHANG); 

            if(pid > 0) 
            {
                if(WIFEXITED(status))
                {
                    printf("\nProcess %d (%s) exited normally.\n", processes[i].pid, processes[i].command);
                    remove_process(processes[i].pid);  
                } 
                else  
                {
                    printf("\nProcess %d (%s) exited abnormally.\n", processes[i].pid, processes[i].command);
                    update_process_status(processes[i].pid, 0);
                }
                printprompt();
                fflush(stdout);
            }
        }
    }
}


int main() 
{   
    getcwd(homedir, sizeof(homedir));
    getcwd(previousdir, sizeof(previousdir));
    char *input = NULL;
    size_t len = 0;
    init_log();
    setup_signal_handlers();
    load_myshrc(".myshrc");
    while(1) 
    {        
        if(tag == 1)
        {
            printprompt();
        }
        if(getline(&input, &len, stdin) == -1) 
        {
            if(feof(stdin)) 
            {
                terminate_all_processes();
                printf("\nBye\n");
                exit(0);
            }
            continue;
        }
        tag = 1;
        input[strlen(input) - 1] = '\0';
        char *commands[MAXL];
        int cmd_count = 0;
        char *token = strtok(input, ";");
        while(token != NULL) 
        {
            commands[cmd_count++] = token;
            token = strtok(NULL, ";");

        }
        for(int i = 0; i < cmd_count; i++)
        {
            char *command = commands[i];
            char *ispipe = strchr(command, '|');
            if(ispipe!= NULL)
            {
                is_pipe = 1;
            }
            char *ampersand = strchr(command, '&');
            if(is_pipe == 0)
            {
                if(ampersand != NULL) 
                {
                    *ampersand = '\0';
                    background = 1;
                } 
                else
                {
                    background = 0;
                }
            }
            while(isspace((unsigned char)command[0])) command++;
            char *end = command + strlen(command) - 1;
            while(end > command && isspace((unsigned char)*end)) end--;
            *(end + 1) = '\0';
            comm = command;
            if(strncmp(command, "log", 3) == 0) 
            {
                logcommand(command);
            }
            else 
            {
                if(command[0] != '\0' && strncmp(command, "log", 3) != 0) 
                {
                    add_command_to_log(command);
                }
                execute_command(command);
            }
        }
    }
    free(input);
    return 0;
}