//  Пример иллюстрирует, что у потоков в одном процессе один и тот же PID, но различные TID.
//  Сравнение с выводом htop и pstree иллюстрирует, что в этих инструментах по факту ведётся учёд именно по TID.

#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

#define NUM_THREADS 5

void *thread_func(void *data)
{
	printf("In thread:Process ID:%u\t Thread ID:%u\n",
		   getpid(), syscall(SYS_gettid));
	return NULL;
}

int main(int argc, char *argv[])
{
	pthread_t thread[NUM_THREADS];
	int i = 0;
	printf("Before Creating Thread:Process ID:%u\t Thread ID:%u\n",
		   getpid(), syscall(SYS_gettid));
	for (i = 0; i < NUM_THREADS; i++)
		pthread_create(&thread[i], NULL, thread_func, NULL);
	for (i = 0; i < NUM_THREADS; i++)
		pthread_join(thread[i], NULL);
	printf("After Creating Thread:Process ID:%u\t Thread ID:%u\n",
		   getpid(), syscall(SYS_gettid));
	return 0;
}
