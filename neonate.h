#ifndef NEONATE_H
#define NEONATE_H

#include <sys/types.h>

void set_nonblocking_input();
void restore_input();
pid_t get_most_recent_pid();
void neonate_n(int time_arg);

#endif
