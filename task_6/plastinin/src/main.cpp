#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "../include/daemon.h"

int main(int argc, char** argv)
{
    int status;
    int pid;

    FILE* dump = fopen("dump", "w");
    fclose(dump);

    // ���� ���������� ��������� ������ ������ ����, �� ������� ��� ������������ ������
    if (argc != 2)
    {
        printf("Usage: ./my_daemon filename.cfg\n");
        return -1;
    }

    // ������� �������
    pid = fork();

    if (pid == -1) // ���� �� ������� ��������� �������
    {
        // ������� �� ����� ������ � � ��������
        printf("Error: Start Daemon failed (%s)\n", strerror(errno));

        return -1;
    }
    else if (!pid) // ���� ��� �������
    {
        // ������ ��� ��� ����������� � �������� �������
        // ��������� ���������� ��� ���� ���� �� ����������� �����,
        // ����� � ��� ����� ���� �������� � ������� �������
        umask(0);

        // ������ ����� �����, ����� �� �������� �� ��������
        setsid();
        // ��������� PID ��� ����� � ������� ����� �������� ��������
        set_pid_file("this_daemon.pid");

        // ��������� � ������ �����, ���� �� ����� �� �������, �� ����� ���� ��������.
        // � ������� � ���������������� ������
        chdir("/");

        // ��������� ����������� �����/������/������, ��� ��� ��� ��� ������ �� �����������
        close(STDIN_FILENO);
        // close(STDOUT_FILENO);
        close(STDERR_FILENO);

        // ������ ������� ����� ������������ �������� �� ���������
        status = monitor_proc(argv[1]);

        return status;
    }
    else // ���� ��� ��������
    {
        // �������� ������, �.�. �������� ���� ������ (������ ������) �� ���������
        return 0;
    }
}
