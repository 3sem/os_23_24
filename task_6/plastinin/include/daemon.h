#include <signal.h>
#include <stdio.h>

struct Config {
    int check_time;
    int pid;
    char dump_dir[100];
    int inotify_fd;
    int inotify_size;
    int inotify_capacity;
    int* inotify_wds;
};

void set_pid_file(char* filename);

int monitor_proc(char* filename);

int cfg_ctor(Config* cfg, char* cfg_file_name);

int cfg_detor(Config* cfg);

void sigusr1_handler(int sig, siginfo_t* info, void* context);

void add_watch_dir(const char* wd, Config* cfg, FILE* dump);

int backup_file(char* dir_path, char* name, Config* cfg, FILE* dump);

int create_new_backup(char* path, char* name, Config* cfg, FILE* dump);
