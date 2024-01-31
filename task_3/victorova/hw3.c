#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <stdlib.h>
#include <wait.h>
#include <time.h>

#define BUFSZ 4095

#define MSG_SIZE 1024 // < /proc/sys/fs/mqueue/msg_max 

typedef struct msgbuf {
    long    mtype;
    char    mtext[MSG_SIZE];
} message_buf;

int shm(FILE* inpf, FILE* outf, key_t key) {

    sem_open("sem", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 1);

    const pid_t pid = fork(); //child appearance 
    if (pid < 0) { 
        printf("fork failed!\n"); 
        return -1; 
    } 
    if (pid != 0) { //parent transmits the data to child 
        int res = shmget(key, BUFSZ, 0644 | IPC_CREAT);
        if (res == -1) {
            perror("shmget");
            return 1;
        }
        char data[BUFSZ];
        void* ptr = shmat(res, NULL, 0);

        while (1) {
            if (((char*) ptr)[BUFSZ] == '*') {
                size_t num = 0;
                num = fread(data, sizeof(char), BUFSZ - 1, inpf);
                data[num] = '\0';
                sprintf((char*) ptr, "%s", data);
                ((char*) ptr)[BUFSZ] = '\0';
                if (num == 0) {
                    break;
                }
            }
        }

        shmdt(ptr);
        shmctl(key, IPC_RMID, NULL);

        int status;
        waitpid(pid, &status, 0);
        return 0;
    }
    else {
        int res = shmget(key, BUFSZ, 0644 | IPC_CREAT);
        if (res == -1) {
            perror("shmget");
            return 1;
        }
        void* ptr = shmat(res, NULL, 0);

        ((char*) ptr)[BUFSZ] = '*';

        while (1) {
            if (((char*) ptr)[BUFSZ] == '\0')
            {
                size_t lenght = strlen((char*) ptr);
                if (lenght)
                {
                    fwrite((char*)ptr, sizeof(char), lenght, outf);
                    ((char*) ptr)[BUFSZ] = '*';
                }
                else
                    break;
            }
        }

        shmdt(ptr);
        shmctl(key, IPC_RMID, NULL);
        exit(0);
    }
}

//////////////////////////////////////////////////////////////////////////////////////

int mque(FILE* inpf, FILE* outf, key_t key) {
    int msqid;
    int msgflg = IPC_CREAT | 0666;
    message_buf sbuf;
    size_t buf_length;
    sbuf.mtype = 1;

    pid_t pid = fork();

    if (pid < 0) { 
        printf("fork failed!\n"); 
        return -1; 
    } 
    if (pid != 0) { //parent transmits the data to child 
        if ((msqid = msgget(key, msgflg)) < 0) {
            perror("msgget");
            exit(1);
        } //else 
        //printf("msgget: msgget succeeded: msqid = %d\n", msqid);
        
        
        //printf("msgget: msgget succeeded: msqid = %d\n", msqid);

        size_t cnt_read = 0;
        while((cnt_read = fread(sbuf.mtext, sizeof(char), MSG_SIZE - 1, inpf))) {
            sbuf.mtext[cnt_read] = '\0';
            size_t buf_length = cnt_read + 1;
            msgsnd(msqid, &sbuf, buf_length, 0);
        }

        strcpy(sbuf.mtext, "quit");
        
        //printf("msgget: msgget succeeded: msqid = %d\n", msqid);
        
        buf_length = strlen(sbuf.mtext) + 1;
        
        if (msgsnd(msqid, &sbuf, buf_length, IPC_NOWAIT) < 0) {
            printf ("%d, %ld, %s, %ld\n", msqid, sbuf.mtype, sbuf.mtext, buf_length);
            perror("msgsnd");
            exit(1);
        } //else 
            //printf("Message: \"%s\" Sent\n", sbuf.mtext);
        
        msgsnd(msqid, &sbuf, buf_length, 0);

        int status;
        waitpid(pid, &status, 0);
        return 0;
    }
    else {
        if ((msqid = msgget(key, 0666)) < 0) {
            perror("msgget");
            exit(1);
        }

        while(1) {
            if (msgrcv(msqid, &sbuf, MSG_SIZE, sbuf.mtype, 0) < 0) {
                continue;
            }
            sbuf.mtext[MSG_SIZE - 1] = '\0';
            if (strcmp(sbuf.mtext, "quit") == 0) {
                break;
            }
            
            size_t len = strlen(sbuf.mtext);
            fwrite(sbuf.mtext, sizeof(char), len, outf);
        }
        exit(0);
    }

}

//////////////////////////////////////////////////////////////////////////////////////////

int fifo(FILE* inpf, FILE* outf)
{
    pid_t pid = fork();

    if (pid < 0) { 
        printf("fork failed!\n"); 
        return -1; 
    } 
    if (pid != 0) { //parent transmits the data to child 
        int fd = open("fifo_stream", O_WRONLY);

        if (!fd) return 2;

        size_t size = 0;
        char buf[BUFSZ] = "";
        while ((size = fread(buf, sizeof(char), BUFSZ, inpf)) > 0)
            write(fd, buf, size);

        close(fd);
        int status;
        waitpid(pid, &status, 0);
        return 0;
    }
    else
    {
        int fd = open("fifo_stream", O_RDONLY);
        if (!fd) exit(1);

        size_t size = 0;
        char buf[BUFSZ] = "";
        do
        {
            size = read(fd, buf, BUFSZ);
            fwrite(buf, sizeof(char), size, outf);
        } while (size == BUFSZ);

        close(fd);
        exit(0);
    }
}

int main(int argc, char *argv[]) { 

    if (argc != 3) {
        printf("please, input names of two files: the sender and the receiver in command line\n");
        //scanf("%s %s", argv[1], argv[2]);
        return 0;
    }
    
    FILE* inpf = fopen(argv[1], "r");
    FILE* outf = fopen(argv[2], "w");
    
    int way;
    printf("print 1 for fifo or 2 for shm or 3 for queue:\n");
    scanf("%d", &way);
    

    if (inpf == NULL) {
        printf("inp file problem\n");
        return -1;
    }
    if (outf == NULL) {
        printf("out file problem\n");
        return -1;
    }

    struct timespec start, stop; 
    clock_gettime( CLOCK_MONOTONIC, &start);
    
    key_t key;
    
    key = ftok(argv[1], 1024);
    if (key == -1) {
       perror("ftok\n");
       exit(EXIT_FAILURE);
    }
    
    //printf("1\n");
    
    if (way == 2) {
        shm(inpf, outf, key);
        
    } else if (way == 3) {  
        mque(inpf, outf, key);
        
    } else {
        //printf("fifo");
        int dev = IPC_CREAT | 0666;
        mknod("fifo_stream", dev, 0); // fifo only    
        fifo(inpf, outf);
    }
    
    clock_gettime( CLOCK_MONOTONIC, &stop);

    printf("%lf\n", ( stop.tv_sec - start.tv_sec ) + (double)( stop.tv_nsec - start.tv_nsec ) * 1e-6);

    fclose(inpf);
    fclose(outf);
    
    return 0;
}
