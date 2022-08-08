/* 
 * echoserveri.c - An iterative echo server 
 */
/* $begin echoserverimain */
#include "csapp.h"

void echo(int connfd);

int main(int argc, char **argv) {
    int listenfd, connfd;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    //服务器开启服务监听端口号
    listenfd = Open_listenfd(argv[1]);
    while (1) {
//        clientlen = sizeof(struct sockaddr_storage);
//        connfd = Accept(listenfd, (SA *) &clientaddr, &clientlen);
//        Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAXLINE,
//                    client_port, MAXLINE, 0);
//        printf("Connected to (%s, %s)\n", client_hostname, client_port);
        connfd = maccept(listenfd);
        echo(connfd);
        Close(connfd);
    }
    exit(0);
}
/* $end echoserverimain */
