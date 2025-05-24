
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include "signals.h"
#include "activities.h"
#include "prompts.h"
extern struct ProcessInfo processes[MAX_PROCESSES];
extern int process_count;
void handle_fg(int pid) 
{
    int found = 0;
    for (int i = 0; i < process_count; i++) 
    {
        if (processes[i].pid == pid) 
        {
            if (processes[i].is_running == 0) 
            {
                if(kill(pid, SIGCONT) == 0)
                {
                    processes[i].is_running = 1;
                }
                else 
                {
                    perror("Error continuing process");
                }
            
            }
                    pid_t pgid = getpgid(pid);
                    if (tcsetpgrp(STDIN_FILENO, pgid) == -1) 
                    {
                        perror("Error setting terminal foreground process group");
                    }
                    int status;
                    waitpid(pid, &status, WUNTRACED);
                    tcsetpgrp(STDIN_FILENO, getpgrp());
                    if (WIFSTOPPED(status)) 
                    {
                        printf("Process %d stopped.\n", pid);
                        processes[i].is_running = 0; 
                    } 
                    else if (WIFEXITED(status)) 
                    {
                        printf("Process %d exited with status %d.\n", pid, WEXITSTATUS(status));
                    } 
                    else if (WIFSIGNALED(status)) 
                    {
                        printf("Process %d terminated by signal %d.\n", pid, WTERMSIG(status));
                    }
                } 
            found = 1;
            break;
        
    }
    if (!found) 
    {
        printf("No such process found\n");
    }
}

void handle_bg(int pid) {
    int found = 0;
    for (int i = 0; i < process_count; i++) 
    {
        if (processes[i].pid == pid) 
        {
            if (processes[i].is_running == 0) 
            {
                if (kill(pid, SIGCONT) == 0) 
                {
                    processes[i].is_running = 1; 
                } 
                else 
                {
                    perror("Error continuing process");
                }
            } 
            else 
            {
                printf("Process %d is already running.\n", pid);
            }
            found = 1;
            break;
        }
    }
    if (!found) 
    {
        printf("No such process found\n");
    }
}

