#include "seek.h"
bool is_directory(const char *path) 
{
    struct stat statbuf;
    if(stat(path, &statbuf) != 0)
        return false;
    return S_ISDIR(statbuf.st_mode);
}

void print_colored(const char *path, bool is_dir) 
{
    if(is_dir) 
    {
        printf(blueprompt "%s\n" normal, path);
    } 
    else 
    {
        printf(greenprompt "%s\n" normal, path);
    }
}

bool match_prefix(const char *filename, const char *target) 
{
    return (strncmp(filename, target, strlen(target)) == 0);
}

typedef struct 
{
    char path[MAX_PATH];
    bool is_dir;
} MatchInfo;

void search_directory(const char *base_path, const char *current_path, const char *target, bool only_dirs, bool only_files, MatchInfo *matches, int *match_count) 
{
    DIR *dir;
    struct dirent *entry;
    char full_path[MAX_PATH];
    char relative_path[MAX_PATH];

    snprintf(full_path, sizeof(full_path), "%s/%s", base_path, current_path);

    if((dir = opendir(full_path)) == NULL) 
    {
        return;
    }

    while((entry = readdir(dir)) != NULL) 
    {
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(full_path, sizeof(full_path), "%s/%s/%s", base_path, current_path, entry->d_name);
        snprintf(relative_path, sizeof(relative_path), "%s%s%s", current_path, (*current_path ? "/" : ""), entry->d_name);

        bool is_dir = is_directory(full_path);

        if(match_prefix(entry->d_name, target)) 
        {
            if((is_dir && !only_files) || (!is_dir && !only_dirs)) 
            {
                if(*match_count < MAX_PATH) 
                {
                    strcpy(matches[*match_count].path, full_path);
                    matches[*match_count].is_dir = is_dir;
                    (*match_count)++;
                }
                print_colored(relative_path, is_dir);
            }
        }

        if(is_dir) 
        {
            search_directory(base_path, relative_path, target, only_dirs, only_files, matches, match_count);
        }
    }

    closedir(dir);
}

void seek(int argc, char *argv[], const char *homedir, char *previousdir)
{
    bool only_dirs = false, only_files = false, exec_flag = false;
    char *target = NULL, *dir_path = NULL;
    int i;
    bool use_previous_dir = false;
    int actual_count=argc;
    for(int i=0;i<argc;i++)
    {
        if(argv[i]==NULL)
        {
            actual_count=i;
            // printf("%d",arg_count);
            break;
        }
    }
    for(i = 1; i < actual_count; i++) 
    {
        if (strcmp(argv[i], "-") == 0) 
        {
            use_previous_dir = true;
        } 
        else if (argv[i][0] == '-') 
        {
            for(int j = 1; argv[i][j]; j++) 
            {
                switch (argv[i][j]) 
                {
                    case 'd': only_dirs = true; break;
                    case 'f': only_files = true; break;
                    case 'e': exec_flag = true; break;
                    default:
                        printf("Invalid flag: -%c\n", argv[i][j]);
                        return;
                }
            }
        } 
        else if (target == NULL) 
        {
            target = argv[i];
        } 
        else if (dir_path == NULL && !use_previous_dir) 
        {
            dir_path = argv[i];
        } 
        else 
        {
            printf(redprompt "Too many arguments\n"normal);
            return;
        }
    }

    if (only_dirs && only_files) 
    {
        printf(redprompt "Invalid flags!\n"normal);
        return;
    }

    if (target == NULL) 
    {
        printf(redprompt "No search target specified\n" normal);
        return;
    }

    char full_path[MAX_PATH];
    if (use_previous_dir) 
    {
        if (previousdir[0] == '\0') 
        {
            printf(redprompt "No previous directory available\n"normal);
            return;
        }
        strncpy(full_path, previousdir, sizeof(full_path));
    } 
    else if (dir_path == NULL) 
    {
        getcwd(full_path, sizeof(full_path));
    } 
    else if (dir_path[0] == '~') 
    {
        snprintf(full_path, sizeof(full_path), "%s%s", homedir, dir_path + 1);
    } 
    else 
    {
        strncpy(full_path, dir_path, sizeof(full_path));
    }
    full_path[sizeof(full_path) - 1] = '\0';

    MatchInfo matches[MAX_PATH];
    int match_count = 0;
    search_directory(full_path, "", target, only_dirs, only_files, matches, &match_count);

    if(match_count == 0) 
    {
        printf(redprompt "No match found!\n" normal);
    } 
    else if(exec_flag && match_count == 1) 
    {
        if(matches[0].is_dir) 
        {
            if (chdir(matches[0].path) != 0) 
            {
                printf(redprompt "Missing permissions for task!\n" normal);
            }
        }
        else 
        {
            FILE *file = fopen(matches[0].path, "r");
            if(file) 
            {
                char buffer[1024];
                while(fgets(buffer, sizeof(buffer), file)) 
                {
                    printf("%s", buffer);
                }
                fclose(file);
            } 
            else 
            {
                printf(redprompt "Missing permissions for task!\n" normal);
            }
        }
    } 
}