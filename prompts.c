#include "prompts.h"
#include "hop.h"
#include "reveal.h"
#include "seek.h"
#include "log.h"
#include <sys/time.h>
#include <signal.h>
#include "proclore.h"

int background;
char username[MAXL];
char sysname[MAXL];
char homedir[MAXL];
char currentdir[MAXL];
char previousdir[MAXL];
int tag = 1;
static pid_t background_pids[MAXL];
static int background_count = 0;

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
    char command_copy[MAXL];
    strncpy(command_copy, command, sizeof(command_copy) - 1);
    command_copy[sizeof(command_copy) - 1] = '\0';  
    char *args[MAXL];
    int arg_count = 0;
    char *token = strtok(command_copy, " ");
    while(token != NULL && arg_count < MAXL) 
    {
        args[arg_count++] = token;
        token = strtok(NULL, " ");
    }
    args[arg_count] = NULL;
    if(arg_count > 0) 
    {
        if(strcmp(args[0], "hop") == 0) 
        {
            hop(args + 1, arg_count - 1, homedir, previousdir);
            getprompt(); 
        } 
        else if (strcmp(args[0], "reveal") == 0) 
        {
            char flags[MAXL] = "";
            char *path = ".";
            for (int i = 1; i < arg_count; i++) 
            {
                if (args[i][0] == '-') 
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
            seek(arg_count, args, homedir, previousdir);
        } 
        else if(strcmp(args[0], "proclore") == 0) 
        {
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
        else 
        {
            struct timeval start, end;
            gettimeofday(&start, NULL);
            pid_t pid = fork();
            if(pid == 0) 
            {
                setpgid(0, 0);  
                execvp(args[0], args);
                fprintf(stderr, redprompt "ERROR : '%s' is not a valid command\n" normal, args[0]);
                exit(EXIT_FAILURE);
            } 
            else if(pid > 0) 
            {
                if(background) 
                {
                    printf("%d\n", pid);
                    background_pids[background_count++] = pid;
                } 
                else 
                {
                    int status;
                    waitpid(pid, &status, 0);
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
    }
    else 
    {
        printf(redprompt "Error: No command specified\n" normal);
    }
}

void handle_background_processes() 
{
    int status;
    pid_t pid;
    while((pid = waitpid(-1, &status, WNOHANG)) > 0) 
    {
        for(int i = 0; i < background_count; i++) 
        {
            if(background_pids[i] == pid) 
            {
                if(WIFEXITED(status)) 
                {
                    printf("\nProcess %d exited normally.\n", pid);
                } 
                else 
                {
                    printf("\nProcess %d exited abnormally.\n", pid);
                }
                for(int j = i; j < background_count - 1; j++) 
                {
                    background_pids[j] = background_pids[j + 1];
                }
                background_count--;
                break;
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
    while (1) 
    {
        handle_background_processes();
        if(tag == 1)
        {
            printprompt();
        }
        getline(&input, &len, stdin);
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
            char *ampersand = strchr(command, '&');
            if(ampersand != NULL) 
            {
                *ampersand = '\0';
                background = 1;
            } 
            else
            {
                background = 0;
            }
            while(isspace((unsigned char)command[0])) command++;
            char *end = command + strlen(command) - 1;
            while(end > command && isspace((unsigned char)*end)) end--;
            *(end + 1) = '\0';
            if (strncmp(command, "log", 3) == 0) 
            {
                if(strcmp(command, "log purge") == 0) 
                {
                    purge_log();
                } 
                else if(strcmp(command, "log") == 0) 
                {
                    display_log();
                }
                else if(strncmp(command, "log execute ", 12) == 0) 
                {
                    int index = atoi(command + 12);
                    execute_log_command(index);
                } 
                else
                {
                    printf("Error: Invalid log command\n");
                }
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


