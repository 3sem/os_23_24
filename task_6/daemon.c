#define _DEFAULT_SOURCE

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <signal.h>
#include <errno.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "parser.h"



#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))


#define MAX_CONFIG_VARIABLE_LEN 20
#define CONFIG_LINE_BUFFER_SIZE 100
#define MAX_LLIST_NAME_LEN 256
#define MAX_OUT_NAME_LEN 256



struct settings {
    int time;
    int pid; 
    char *dir;
};

int read_int_from_config_line(char* config_line) {    
    char prm_name[MAX_CONFIG_VARIABLE_LEN];
    int val;
    sscanf(config_line, "%s %d\n", prm_name, &val);
    return val;
}

void read_str_from_config_line(char* config_line, char* val) {    
    char prm_name[MAX_CONFIG_VARIABLE_LEN];
    sscanf(config_line, "%s %s\n", prm_name, val);
}

void SetPidFile(char* filename)
{
    FILE* f;

    f = fopen(filename, "w+");
    if (f)
    {
        fprintf(f, "%u", getpid());
        fclose(f);
    }
}


void backup(struct settings cfg, char *name) {
    char cmd[256] = "";
    char file_name[256] = "";
    strcat(file_name, cfg.dir);
    strcat(file_name, name);
    sprintf(cmd, "cp %s %s.save", name, file_name);
    system(cmd);
}

void list_dir(const char *wd, int offset)
{
    DIR *dir = opendir(wd);
    if(dir)
    {
        char path[PATH_MAX];
        char *last_fn;
        struct dirent *e;
        struct stat info;


        while((e = readdir(dir)) != NULL)
        {

            if(e->d_type == DT_DIR)
            {
                if(strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0)
                    continue;
                char p[PATH_MAX];
                strcpy(p, wd);
                list_dir(strcat(strcat(p, "/"), e->d_name), offset + 1);
            }
        }
    }
    return;
}


int MonitorProc(struct settings cfg) {
                    // Do something
    char pathname[256] = "";
    sprintf(pathname, "/proc/%d/cwd", cfg.pid);
    chdir(pathname);
    char cwd[PATH_MAX];
    if(getcwd(cwd, sizeof(cwd)) != NULL)
    {
        list_dir(cwd, 0);
    }

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;


    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGINT,  &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGSTOP, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);


    char buf[BUF_LEN];
    int fd;
    int wd;
    int len;
    int i = 0;
    double time = cfg.time;
    fd = inotify_init();

    if(fd < 0) perror("Inotify init");
    wd = inotify_add_watch(fd, ".", IN_MODIFY | IN_CREATE | IN_DELETE);
    while(1){
        sleep(cfg.time);
        i = 0;
        len = read(fd, buf, BUF_LEN);
        while(i < len){
            struct inotify_event *ev = (struct inotify_event*) &buf[i];
            if(ev->len)
            {
                if(ev->mask & IN_CREATE)
                {
                    printf("File %s is created\n", ev->name);
                    backup(cfg ,ev->name);
                }
                if(ev->mask & IN_MODIFY)
                {
                    printf("File %s is modified\n", ev->name);
                    backup(cfg ,ev->name);
                }
                if(ev->mask & IN_DELETE)
                    printf("File %s is Deleted\n", ev->name);
            }
            i += EVENT_SIZE + ev->len;
        }

        
    }
    inotify_rm_watch(fd, wd);
    close(fd);


}

int LoadConfig(char *file_name, struct settings *cfg) {
                    // Do something
    config_option_t co;
    if ((co = read_config_file("confex")) == NULL) {
        perror("read_config_file()");
        return -1;
    }
    while(1) {
        if(!strcmp("pid", co->key)) cfg->pid = read_int_from_config_line(co->value);
        if(!strcmp("time_delta", co->key)) cfg->time = read_int_from_config_line(co->value);
        if(!strcmp("address", co->key)) cfg->dir = co->key;
        if (co->prev != NULL) {
            co = co->prev;
        } else {
            break;
        }
    }

    return 0;
}

int main(int argc, char** argv)
{
    int status;
    int pid;
    struct settings c;
    // если параметров командной строки меньше двух, то покажем как использовать демона
    if (argc != 2)
    {
        printf("Usage: ./my_daemon filename.cfg\n");
        return -1;
    }

    // загружаем файл конфигурации
    status = LoadConfig(argv[1], &c);
    
    if (!status) // если произошла ошибка загрузки конфига
    {
        printf("Error: Load config failed\n");
        return -1;
    }


    // создаем потомка
    pid = fork();

    if (pid == -1) // если не удалось запустить потомка
    {
        // выведем на экран ошибку и её описание
        printf("Error: Start Daemon failed (%s)\n", strerror(errno));
        
        return -1;
    }
    else if (!pid) // если это потомок
    {
        // данный код уже выполняется в процессе потомка
        // разрешаем выставлять все биты прав на создаваемые файлы, 
        // иначе у нас могут быть проблемы с правами доступа
        umask(0);
        
        // создаём новый сеанс, чтобы не зависеть от родителя
        setsid();
        // сохраняем PID для связи с демоном через механизм сигналов
        SetPidFile("this_daemon.pid");

        // переходим в корень диска, если мы этого не сделаем, то могут быть проблемы.
        // к примеру с размантированием дисков
        chdir("/");
        
        // закрываем дискрипторы ввода/вывода/ошибок, так как нам они больше не понадобятся
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
        
        // Данная функция будет осуществлять слежение за процессом
        status = MonitorProc(c);
        
        return status;
    }
    else // если это родитель
    {
        // завершим процес, т.к. основную свою задачу (запуск демона) мы выполнили
        return 0;
    }
}