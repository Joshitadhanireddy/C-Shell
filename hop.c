#include "hop.h"

void hop(char **args, int arg_count, char *home_directory, char *previous_directory) 
{
    char current_directory[MAXL];
    char target_directory[MAXL];
    int hop_success;

    if(arg_count == 0) 
    {
        strcpy(target_directory, home_directory);
        hop_success = chdir(target_directory);
        if(hop_success == 0) 
        {
            getcwd(current_directory, sizeof(current_directory));
            printf("%s\n", current_directory);
            strcpy(previous_directory, current_directory);
        } 
        else 
        {
            perror(redprompt "hop");
            printf(normal);
        }
        return;
    }

    for(int i = 0; i < arg_count; i++) 
    {
        getcwd(current_directory, sizeof(current_directory));

        if(strcmp(args[i], "~") == 0) 
        {
            strcpy(target_directory, home_directory);
        } 
        else if(strcmp(args[i], "-") == 0) 
        {
            strcpy(target_directory, previous_directory);
        } 
        else if(strcmp(args[i], "..") == 0) 
        {
            strcpy(target_directory, current_directory);
            char *last_slash = strrchr(target_directory, '/');
            if(last_slash != NULL) 
            {
                *last_slash = '\0';
            }
        } 
        else if(strcmp(args[i], ".") == 0) 
        {
            continue;
        } 
        else if(args[i][0] == '~') 
        {
            snprintf(target_directory, sizeof(target_directory), "%s%s", home_directory, args[i] + 1);
        } 
        else 
        {
            strcpy(target_directory, args[i]);
        }

        hop_success = chdir(target_directory);
        if(hop_success == 0) 
        {
            strcpy(previous_directory, current_directory);
            getcwd(current_directory, sizeof(current_directory));
            printf("%s\n", current_directory);
        } 
        else 
        {
            perror(redprompt "hop");
            printf(normal);
            return; 
        }
    }
}