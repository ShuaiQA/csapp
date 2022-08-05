//
// Created by shuai on 22-8-5.
//

#include<stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>



//SIGCHLD
int main() {
    sleep(3);
    printf("HelloWord\n");
}