#include "reveal.h"

void print_permissions(struct stat st) 
{
    printf((S_ISDIR(st.st_mode)) ? "d" : "-");
    printf((st.st_mode & S_IRUSR) ? "r" : "-");
    printf((st.st_mode & S_IWUSR) ? "w" : "-");
    printf((st.st_mode & S_IXUSR) ? "x" : "-");
    printf((st.st_mode & S_IRGRP) ? "r" : "-");
    printf((st.st_mode & S_IWGRP) ? "w" : "-");
    printf((st.st_mode & S_IXGRP) ? "x" : "-");
    printf((st.st_mode & S_IROTH) ? "r" : "-");
    printf((st.st_mode & S_IWOTH) ? "w" : "-");
    printf((st.st_mode & S_IXOTH) ? "x" : "-");
}

void print_file_info(struct stat st, char *name) 
{
    print_permissions(st);
    printf(" %hu", st.st_nlink);
    struct passwd *pw = getpwuid(st.st_uid);
    struct group *gr = getgrgid(st.st_gid);
    printf(" %s %s", pw->pw_name, gr->gr_name);
    printf(" %lld", st.st_size);
    char time[20];
    strftime(time, 20, "%b %d %H:%M", localtime(&st.st_mtime));
    printf(" %s", time);
    printf(" %s", name);
}

void reveal(char *flags, char *path, char *home_directory, char *previous_directory) 
{
    int show_hidden = 0;
    int long_format = 0;
    for(size_t i = 0; i < strlen(flags); i++) 
    {
        if (flags[i] == 'a') show_hidden = 1;
        if (flags[i] == 'l') long_format = 1;
    }
    if(strcmp(path, "~") == 0) 
    {
        path = home_directory;
    } 
    else if(strcmp(path, "-") == 0) 
    {
        path = previous_directory;
    }
    DIR *dir;
    struct dirent *entry;
    struct stat st;
    if((dir = opendir(path)) == NULL) 
    {
        perror("reveal");
        return;
    }
    char entries[1024][256];
    int count = 0;
    while((entry = readdir(dir)) != NULL) 
    {
        if(!show_hidden && entry->d_name[0] == '.') continue;
        strcpy(entries[count++], entry->d_name);
    }
    closedir(dir);
    qsort(entries, count, sizeof(entries[0]), (int (*)(const void *, const void *))strcmp);
    for(int i = 0; i < count; i++) 
    {
        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entries[i]);

        if(stat(fullpath, &st) == -1) 
        {
            perror("reveal");
            continue;
        }

        if(long_format) 
        {
            print_file_info(st, entries[i]);
        }
        else
        {
            if(S_ISDIR(st.st_mode)) 
            {
                printf(blueprompt"%s\n"normal, entries[i]); 
            }
            else if(st.st_mode & S_IXUSR) 
            {
                printf(greenprompt"%s\n"normal, entries[i]); 
            }
            else{
                printf(whiteprompt"%s\n"normal, entries[i]);
            }
        }
        printf("\n");
    }
}

