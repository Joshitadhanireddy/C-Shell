#include "log.h"
#include "prompts.h"

static char *log_entries[MAX_LOG_ENTRIES];
static int log_count = 0;
#define LOG_FILE_NAME "shell_log"  

void init_log() 
{
    load_log();
}


void add_command_to_log(const char *command) 
{
    if (log_count > 0 && strcmp(log_entries[log_count - 1], command) == 0) 
    {    
        return;
    }
    if(log_count == MAX_LOG_ENTRIES) 
    {
        free(log_entries[0]);
        memmove(&log_entries[0], &log_entries[1], (MAX_LOG_ENTRIES - 1) * sizeof(char *));
        log_count--;
    }
    log_entries[log_count] = strdup(command);
    log_count++;
    save_log();
}


void display_log() 
{
    for(int i = 0; i < log_count; i++) 
    {
        printf("%d %s\n", i + 1, log_entries[i]);
    }
}


void purge_log() 
{
    for(int i = 0; i < log_count; i++)
    {
        free(log_entries[i]);
    }
    log_count = 0;
    save_log();
}

void execute_log_command(int index) 
{
    if(index < 1 || index > log_count)
    {
        printf("ERROR: Invalid log index\n");
        return;
    }
    const char *command = log_entries[log_count - index];
    printf("%s\n", command);
    if(index != 1) 
    {
        add_command_to_log(command);
    }
    execute_command(command);
}


void save_log() 
{
    char full_path[MAXL];
    snprintf(full_path, sizeof(full_path), "%s/%s", homedir, LOG_FILE_NAME);
    FILE *file = fopen(full_path, "w");
    if(file == NULL)
    {
        perror("fopen");
        return;
    }
    for(int i = 0; i < log_count; i++) 
    {
        fprintf(file, "%s\n", log_entries[i]);
    }
    fclose(file);
}


void load_log() 
{
    char full_path[MAXL];
    if(homedir != NULL)
    {
        snprintf(full_path, sizeof(full_path), "%s/%s", homedir, LOG_FILE_NAME);
    }
    else 
    {
        fprintf(stderr, "Error: HOME environment variable not set.\n");
        return;
    }
    FILE *file = fopen(full_path, "r");
    if(file == NULL) 
    {
        return; 
    }
    char line[MAXL];
    while(fgets(line, sizeof(line), file) != NULL) 
    {
        line[strcspn(line, "\n")] = 0; 
        add_command_to_log(line);
    }
    fclose(file);
}