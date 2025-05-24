#include "neonate.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/select.h>

void set_nonblocking_input() 
{
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~(ICANON | ECHO); 
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void restore_input() 
{
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= ICANON | ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

pid_t get_most_recent_pid() 
{
    FILE *file = fopen("/proc/sys/kernel/ns_last_pid", "r");
    if (file == NULL) 
    {
        perror("fopen failed");
        return -1;
    }

    pid_t recent_pid;
    if (fscanf(file, "%d", &recent_pid) != 1) 
    {
        perror("fscanf failed");
        fclose(file);
        return -1;
    }

    fclose(file);
    return recent_pid;
}

void neonate_n(int time_arg) 
{
    set_nonblocking_input(); 
    
    char keypress;
    struct timeval timeout;
    fd_set readfds;

    while (1) 
    {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        
        timeout.tv_sec = time_arg;
        timeout.tv_usec = 0;

        int activity = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);

        if (activity < 0)
        {
            perror("select error");
            break;
        }

        if (activity == 0) 
        {
            pid_t recent_pid = get_most_recent_pid();
            if(recent_pid == -1) {
                break;
            }
            printf("%d\n", recent_pid);
            fflush(stdout);
        } else if (FD_ISSET(STDIN_FILENO, &readfds)) 
        {
            if (read(STDIN_FILENO, &keypress, 1) > 0 && keypress == 'x') 
            {
                break;
            }
        }
    }

    restore_input(); 
}
