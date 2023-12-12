#include <math.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Максимальная длина пути до файла
#define MAX_PATH_LEN 256

// Директория, в которой будут храниться бэкапы
#define BACKUP_DIR "~/backups" 

// Включаем режим демона (если выключен, то включатся принтфы и выключится запуск daemon)
#define DAEMON_MODE

// Включить или выключить одинарный бэкап (для отладки)
// #define DO_ONE_BACKUP

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

char work_dir_path[MAX_PATH_LEN] = {0}; // Глобальная перменная для хранения директории процесса, не придумал, как обойтись без нее

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

pid_t parse_pid(int argc, char **argv)
{
    pid_t pid = -1;

    if (argc != 2) // Если количество аргументов командной строки не равно 2, выводим хэлпу
    {
        printf("Usage: %s <Process ID>\n"
               "Example: %s 10079\n",
               argv[0], argv[0]);

        return -1;
    }

    pid = atoi(argv[1]); // Берём второй аргумент за pid и переводим его из строки в число

    if (pid < 0) // Если вдруг было введено отрицательное число, то ID зараннее некорректен
    {
        fprintf(stderr, "Incorrect PID: %d.\n", pid);

        return -1;
    }

    return pid;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int getpwd(pid_t pid) // getpwd == get process working directory
{
    struct stat buf;
    char path[MAX_PATH_LEN] = {};
    sprintf(path, "/proc/%d/cwd", pid); // Используя id процесса, создаем путь к его папке cwd

    if (stat(path, &buf) < 0) // Если её не существует, то выходим из функции
        return -1;
    
    // /proc/.../cwd - Это символьная ссылка на реальную рабочую директорию процесса
    // Поэтому с помощью её прочтения мы получим реальный путь до рабочей директории процесса
    ssize_t work_dir_path_len = readlink(path, work_dir_path, MAX_PATH_LEN);

    if (work_dir_path_len < 0) // Если длина, прочитанного пути меньше 0, то что-то пошло не так...
    {
        #ifndef DAEMON_MODE

        fprintf(stderr, "ERROR: Can't get path to working directory of the process.\n");
        
        #endif

        return -1;
    }

    #ifndef DAEMON_MODE

    else
        printf("Process exists. Working directory: '%s'\n", work_dir_path);

    #endif

    return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int make_secure_daemon ()
{
    printf("Starting daemon...\n");

    int daemon_status = daemon(0, 0); // Можно реализовать форками, как в примере, но есть готовая функция
    
    #ifndef DAEMON_MODE

    if (daemon_status < 0)
        fprintf(stderr, "ERROR: Can't start daemon\n");

    #endif

    return daemon_status;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool is_text(char *filename)
{
    char command[512] = {};
    sprintf(command, "file %s --mime-type -b | grep text/plain", filename);

    // У команды --none-type выдаёт тип файла (text, elf и т.д.)
    // -b отрезает у вывода команды имя файла
    // Таким образом, для текстовых файлов вывод будет text/plain
    // Вывод команды мы подаём grep'у, который при совпадении вывода и text/plain вернет 0

    if (system(command) != 0)
        return false;

    return true;
}

void make_file_backup(char *filename)
{
    if (!is_text(filename))
        return;

    char backup_filename[MAX_PATH_LEN] = {};
    sprintf(backup_filename, "%s%s", BACKUP_DIR, filename);

    struct stat buf = {};
    char command[512] = {};

    if (stat(backup_filename, &buf) < 0)
    {
        // Файла еще нету в папке с бэкапами поэтому просто копируем его туда

        sprintf(command, "cp %s %s", filename, BACKUP_DIR);
        system(command); // Бэкапим файл
    }

    else
    {
        // Файл уже есть в бэкапах, смотрим на отличия. В случае их наличия - копируем

        sprintf(command, "diff %s %s", filename, backup_filename); // Проверяем отличие

        if (system(command) == 0) // Файлы одинаковые
            return;

        else // Файлы разные
        {
            sprintf(command, "cp %s %s", filename, BACKUP_DIR);
            system(command); // Бэкапим файл
        }
    }
}

void make_dir_backup(char *work_dir_path)
{
    struct dirent *dp = NULL;
    char path[MAX_PATH_LEN] = {}; // Путь до текущего файла

    DIR *dir = opendir(work_dir_path); // Открываем текущую директорию

    if (!dir) // Если не смогли открыть, то сразу выходим
        return;

    strcpy(path, work_dir_path); // В путь до текущего файла сразу кладем текущую директорию

    while ((dp = readdir(dir)) != NULL) // Обходим директорию
    {
        if (strcmp(dp->d_name, ".")  != 0 && // readdir помимо всех файлов и директорий выдает также
            strcmp(dp->d_name, "..") != 0)   // текущую директорию . и предыдущую .., они нам не интересны
        {          
            make_file_backup(path); // Пытаемся создать бэкап текстового файла

            strcpy(path, work_dir_path);
            strcat(path, "/"); // В текущий путь прописываем путь до поддиректории
            strcat(path, dp->d_name);

            make_dir_backup(path); // Вызываем рекурсию, чтобы сделать бэкап поддиректории
        }
    }

    closedir(dir); // Закрываем текущую директорию
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int main(int argc, char **argv)
{
    pid_t pid = parse_pid(argc, argv);

    if (pid < 0)
        return -1;

    #ifdef DAEMON_MODE // Если включен режим демона, переводим программу в демон

    int daemon_status = make_secure_daemon(); // Запускаем демон так, чтобы он отключился от терминала и перевел потоки вывода и ввода в /dev/null

    if (daemon_status < 0)
        return -1;

    #endif

    getpwd(pid); // По ID процесса получаем его рабочую директорию

    if (work_dir_path[0] == 0)
        return -1;
    
    system("mkdir -p " BACKUP_DIR); // Создаём директорию для бэкапов (если она уже есть ничего не произойдет)

    #ifndef DO_ONE_BACKUP

    while (true)

    #endif

    make_dir_backup(work_dir_path); // Создаём бэкапы файлов в рабочей директории и ее поддиректориях

    return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
