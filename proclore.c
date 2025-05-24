#include "proclore.h"

void get_process_info(pid_t pid) 
{
    char path[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    char status;
    int process_group;
    unsigned long virtual_memory;
    char executable_path[BUFFER_SIZE];
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
    FILE *stat_file = fopen(path, "r");
    if (!stat_file) 
    {
        perror("Failed to open stat file");
        return;
    }
    fscanf(stat_file, "%*d %*s %c %*d %d %*d %*d %*d %*u %*u %*u %*u %*u %*u %*u %*d %*d %*d %*d %*d %*d %lu",
           &status, &process_group, &virtual_memory);
    fclose(stat_file);
    snprintf(path, sizeof(path), "/proc/%d/exe", pid);
    ssize_t len = readlink(path, executable_path, sizeof(executable_path) - 1);
    if (len != -1) {
        executable_path[len] = '\0';
    } else {
        perror("Failed to read executable path");
        strcpy(executable_path, "unknown");
    }
    pid_t foreground_pid = tcgetpgrp(STDOUT_FILENO);
    if (foreground_pid == pid) {
        strcat(&status, "+");
    }
    printf("pid : %d\n", pid);
    printf("process status : %c\n", status);
    printf("Process Group : %d\n", process_group);
    printf("Virtual memory : %lu\n", virtual_memory / 1024); 
    printf("Executable path : %s\n", executable_path);
}

