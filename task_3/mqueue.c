#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/msg.h>


#define MSG_SIZE  20


typedef struct msgbuf {
    long    mtype;
    char    mtext[MSG_SIZE];
} message_buf;

void client()
{
    FILE* in = fopen("total.txt", "r");
    int msqid;
    int msgflg = IPC_CREAT | 0666;
    key_t key;
    message_buf sbuf;
    size_t buf_length;

    key = 1024; //rewrite with ftok

    if ((msqid = msgget(key, msgflg )) < 0) {
        perror("msgget");
        exit(1);
    }
    else 
     //printf("msgget: msgget succeeded: msqid = %d\n", msqid);


    sbuf.mtype = 1;
    int num;
    //printf("msgget: msgget succeeded: msqid = %d\n", msqid);
    while((num = fread(sbuf.mtext, sizeof(char), MSG_SIZE, in)) > 0)
    {
        sbuf.mtext[num] = '\0';
        buf_length = strlen(sbuf.mtext) + 1 ; // данные сообщения по прежнему считаем '\0'-terminated - строкой.
        printf("sent: %s \n:", sbuf.mtext);
        if (msgsnd(msqid, &sbuf, buf_length, IPC_NOWAIT) < 0) {
            printf ("%d, %d, %s, %d\n", msqid, sbuf.mtype, sbuf.mtext, buf_length);
            perror("msgsnd");
            exit(1);
        }
    }
}

void server()
{
    FILE* out = fopen("resqueue.txt", "wb+");
    int msqid;
    key_t key;
    message_buf  rbuf;

    key = 1024;

    if ((msqid = msgget(key, 0666)) < 0) {
        
        perror("msgget");
        exit(1);
    }

    
    
    while(1) {
        //printf("pid: %d\n", getpid());
        if(msgrcv(msqid, &rbuf, MSG_SIZE + sizeof(long), 1, 0) < 0)
        {
            perror("msgrcv");
            continue;
        }
        
        printf("got: %s\n", rbuf.mtext);
        rbuf.mtext[MSG_SIZE - 1] = '\0';
        int len = strlen(rbuf.mtext);
        fwrite(rbuf.mtext, sizeof(char), len, out);
        perror("fwrite");
        //exit(1);
    }

    fclose(out);
    

}

int main() {
    struct timeval ts, te;
    double elapsed;
    gettimeofday(&ts, NULL);
    pid_t pid = fork();
    if(pid == -1){
        exit(1);
    }else if(pid)
    {
        client();
    }else{ 
        server(); 
    }
    gettimeofday(&te, NULL);
    elapsed = (te.tv_sec - ts.tv_sec) * 1000;
    elapsed += (te.tv_usec - ts.tv_usec) / 1000;
    //printf("time: %lf", elapsed);
    return 0;
}