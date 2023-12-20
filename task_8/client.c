#include <netdb.h> 
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "integral/integral.h"

#define ARGC 2
#define PORT 8123 
#define SA struct sockaddr 
void func(int sockfd) 
{ 

    double buff1[ARGC]; 
    double buff2[1]; 

    if (read(sockfd, buff1, sizeof(buff1)) != ARGC*sizeof(double))
        return;
    
    double x1 = buff1[0];
    double x2 = buff1[1];
    bzero(buff1, sizeof(buff1));
    printf("\t\tFrom Server : x1 = %f, x2 = %f\n", x1, x2); 
    double result = monte_carlo(x1, x2);
    printf("\t\tintegration... %f\n", result);
    buff2[0] = result;
    write(sockfd, &buff2, sizeof(result));
    bzero(buff2, sizeof(buff2)); 
    func(sockfd);
} 
  
int main() 
{ 
    int sockfd, connfd; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and varification 
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
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_port = htons(PORT); 
  
    // connect the client socket to server socket 
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0); 
    } 
    else
        printf("connected to the server..\n"); 
  
    // function for chat 
    func(sockfd); 
  
    // close the socket 
    close(sockfd); 
}