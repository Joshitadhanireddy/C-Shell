#ifndef MYSHRC_H
#define MYSHRC_H

void load_myshrc(const char *filepath);
void parse_alias(const char *line);
char* get_alias_command(const char *alias);
void execute_myshrc_function(const char *function, char **args, int count);

#endif
