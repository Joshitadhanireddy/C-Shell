#include "myshrc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "prompts.h"
#define MAX_ALIAS 100
#define MAX_CMD_LENGTH 256
typedef struct
{
    char alias[MAX_CMD_LENGTH];
    char command[MAX_CMD_LENGTH];
} Alias;

Alias aliases[MAX_ALIAS];
int alias_count = 0;

void load_myshrc(const char *filepath) 
{
    FILE *file = fopen(filepath, "r");
    if(!file) 
    {
        printf(redprompt "Error: Unable to open .myshrc\n" normal);
        return;
    }
    char line[MAX_CMD_LENGTH];
    while(fgets(line, sizeof(line), file)) 
    {
        if(strncmp(line, "alias", 5) == 0) 
        {
            parse_alias(line + 6);
        }
    }
    fclose(file);
}

void parse_alias(const char *line) 
{
    char alias[MAX_CMD_LENGTH], command[MAX_CMD_LENGTH];
    if (sscanf(line, "%s = %s", alias, command) == 2) 
    {
        strcpy(aliases[alias_count].alias, alias);
        strcpy(aliases[alias_count].command, command);
        alias_count++;
    } 
    else 
    {
        printf(redprompt "Error: Invalid alias format\n" normal);
    }
}
char* get_alias_command(const char *alias) 
{
    for(int i = 0; i < alias_count; i++) 
    {
        if (strncmp(aliases[i].alias, alias, strlen(aliases[i].command)) == 0) 
        {
            char *result = malloc(strlen(aliases[i].command) + strlen(alias) - strlen(aliases[i].alias) + 1);
            if(!result) 
            {
                printf(redprompt "Memory allocation error\n" normal);
                exit(EXIT_FAILURE);
            }
            strcpy(result, aliases[i].command);
            strcat(result, alias + strlen(aliases[i].alias));
            return result;
        }
    }
    return NULL;
}


void execute_myshrc_function(const char *function, char **args, int count) 
{
    if(strcmp(function, "mk_hop") == 0) 
    {
        mkdir(args[1], 0755);
        hop(args, count - 1, homedir, previousdir);
    } 
    else if(strcmp(function, "hop_seek") == 0) 
    {
        hop(args, count - 1, homedir, previousdir);
        seek(1, args + 1, homedir, previousdir);
    } 
    else
    {
        printf(redprompt "Error: Unknown function in .myshrc\n" normal);
    }
}
