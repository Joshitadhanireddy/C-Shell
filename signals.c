#include "signals.h"
#include "activities.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "prompts.h"
pid_t foreground_pid = 0;
extern int process_count;
extern struct ProcessInfo processes[MAX_PROCESSES];
extern char *comm;
void ping(pid_t pid, int signal_number) 
{
    signal_number = signal_number % 32;
    if(kill(pid, 0) == -1) 
    {
        printf(redprompt "No such process found\n" normal);
        return;
    }
    if(kill(pid, signal_number) == 0) 
    {
        if (signal_number == SIGSTOP || signal_number == SIGTSTP) 
        {
            update_process_status(pid, 0); 
        } 
        else if (signal_number == SIGCONT) 
        {
            update_process_status(pid, 1); 
        }
    } 
    else 
    {
        perror("Failed to send signal");
    }
}
void handle_sigint(int sig) 
{
    (void)sig; 
    if (foreground_pid > 0) 
    {
        kill(foreground_pid, SIGINT);
        foreground_pid = 0;
    }
}
void terminate_all_processes() 
{
    for (int i = 0; i < process_count; i++) 
    {
        kill(processes[i].pid, SIGKILL); 
        waitpid(processes[i].pid, NULL, 0); 
        remove_process(processes[i].pid);
    }
}

void handle_sigtstp(int sig) 
{
    (void)sig; 
    if (foreground_pid > 0) 
    {
        sigset_t mask, oldmask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGCHLD); 
        sigprocmask(SIG_BLOCK, &mask, &oldmask); 
        add_process(foreground_pid, comm);  
        if (kill(foreground_pid, SIGTSTP) == 0) 
        {
            update_process_status(foreground_pid, 0); 
        } 
        else 
        {
            perror("Failed to stop foreground process");
        }
        foreground_pid = 0;
        sigprocmask(SIG_SETMASK, &oldmask, NULL);
    }
}

void sigchld_handler(int sig) 
{
    (void)sig; 
    handle_background_processes();
}

void setup_signal_handlers() 
{
    signal(SIGINT, handle_sigint);
    signal(SIGTSTP, handle_sigtstp);
    signal(SIGCHLD, sigchld_handler);
    signal(SIGTTOU, SIG_IGN);
}