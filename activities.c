#include "activities.h"
#include "prompts.h"
int process_count = 0;
struct ProcessInfo processes[MAX_PROCESSES];
void add_process(pid_t pid, const char *command) 
{
    if (process_count < MAX_PROCESSES) 
    {
        processes[process_count].pid = pid;
        strncpy(processes[process_count].command, command, MAX_COMMAND_LENGTH - 1);
        processes[process_count].command[MAX_COMMAND_LENGTH - 1] = '\0';
        processes[process_count].is_running = 1;
        process_count++;
    }
}

void remove_process(pid_t pid) 
{
    for(int i = 0; i < process_count; i++) 
    {
        if(processes[i].pid == pid) 
        {
            for(int j = i; j < process_count - 1; j++) 
            {
                processes[j] = processes[j + 1];
            }
            process_count--;
            break;
        }
    }
}

void update_process_status(pid_t pid, int is_running) 
{
    for(int i = 0; i < process_count; i++) 
    {
        if(processes[i].pid == pid) 
        {
            processes[i].is_running = is_running;
            break;
        }
    }
}


int compare_processes(const void *a, const void *b) 
{
    return strcmp(((struct ProcessInfo*)a)->command, ((struct ProcessInfo*)b)->command);
}

void activities() 
{
    qsort(processes, process_count, sizeof(struct ProcessInfo), compare_processes);
    for (int i = 0; i < process_count; i++) 
    {
        printf("%d : %s - %s\n", 
               processes[i].pid, 
               processes[i].command, 
               processes[i].is_running ? "Running" : "Stopped");
    }
}