#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h>
#include <sys/socket.h> 
#include <sys/types.h> 
#define ARGC 2
#define PORT 8123 
#define SA struct sockaddr 
  
double x1 = 1;
double x2 = 2;

// Function designed for chat between client and server. 
void func(int sockfd) 
{ 
    double buff[ARGC]; 
    double result = -2;

    int n; 
    // infinite loop for chat 
    for (double x2; x2 < 10; x2+=1) { 
        buff[0] = x1;
        buff[1] = x2;
        write(sockfd, buff, sizeof(buff));
        printf("x1 and x2 sended \n");

        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(double));
        result = buff[0];
        printf("RESULT IS: integral from %f to %f = %f\n", x1, x2, result);
    }
    double tmp_small_buf[1] = {-1};
    write(sockfd, tmp_small_buf, sizeof(tmp_small_buf));
} 
  
// Driver function 
int main() 
{ 
    int sockfd, connfd, len; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and verification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
  
    // Binding newly created socket to given IP and verification 
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully binded..\n"); 
  
    // Now server is ready to listen and verification 
    if ((listen(sockfd, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 
    len = sizeof(cli); 
  
    // Accept the data packet from client and verification 
    connfd = accept(sockfd, (SA*)&cli, &len); 
    if (connfd < 0) { 
        printf("server acccept failed...\n"); 
        exit(0); 
    } 
    else
        printf("server acccept the client...\n"); 
  
    // Function for chatting between client and server 
    func(connfd); 
  
    // After chatting close the socket 
    close(sockfd); 
}