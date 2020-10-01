// udp client driver program
#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdlib.h>

#define PORT 1234
#define MAXLINE 1000

// Driver code
typedef struct udp_client {
    struct sockaddr_in servaddr;
    int sockfd;
} udp_client;


void init_udp_client(char *ip, udp_client* uc)
{


    // clear servaddr
    bzero(&uc->servaddr, sizeof(uc->servaddr));
    uc->servaddr.sin_addr.s_addr = inet_addr(ip);
    uc->servaddr.sin_port = htons(PORT);
    uc->servaddr.sin_family = AF_INET;

    // create UDP Socket
    uc->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
}

void send_message (udp_client* uc, char* message) {

    // connect to server
    if(connect(uc->sockfd, (struct sockaddr *)&uc->servaddr, sizeof(uc->servaddr)) < 0)
    {
        printf("\n Error : Connect Failed \n");
        exit(0);
    }

    sendto(uc->sockfd, message, MAXLINE, 0, (struct sockaddr*)NULL, sizeof(uc->servaddr));
}