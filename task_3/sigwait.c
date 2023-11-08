#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

void handler(int signo)
{
    int res, status;
    while(1)
    {
        res = waitpid(-1, &status, WNOHANG);
        if(res != -1)
        {
            if(WIFEXITED(status)){
                printf("%d\n", WEXITSTATUS(status));
            } else if(WIFSIGNALED(status))
            {
                printf("%d\n", WTERMSIG(status))
            }
            return;
        }
    }
}


int main()
{
    signal(SIGCHLD, handler);
    pid_t pid = fork();
    if(pid == 0)
    {
        sleep(10);
        return 5;
    }else {
        pause();
    }

    return 0;
}