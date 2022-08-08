/*
 * echoclient.c - An echo client
 */
/* $begin echoclientmain */
#include "csapp.h"

int main(int argc, char **argv) {
    int clientfd;
    char *host, *port, buf[MAXLINE];
    rio_t rio;

    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }
    host = argv[1];
    port = argv[2];

    //客户端与服务器地址host端口号port进行建立连接
    clientfd = Open_clientfd(host, port);
    //将clientfd相关的信息存在rio变量结构体中，方便后续的Rio_readlineb读操作
    Rio_readinitb(&rio, clientfd);
    //获取用户输入的信息到buf中
    while (Fgets(buf, MAXLINE, stdin) != NULL) {
        //将buf的内容写入clientfd中
        Rio_writen(clientfd, buf, strlen(buf));
        //将服务器端绑定的rio缓冲区的内容放到buf中
        Rio_readlineb(&rio, buf, MAXLINE);
        //输出服务器发送来的数据
        Fputs(buf, stdout);
    }
    //关闭连接标识符
    Close(clientfd); //line:netp:echoclient:close
    exit(0);
}
/* $end echoclientmain */
