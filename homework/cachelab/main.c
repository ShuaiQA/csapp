//
// Created by shuai on 22-8-4.
//


#include <stdio.h>

int main(){
    int a[5][5];
    int b[5][5];
    int num = 0;
    int *c = a;
    int *d = b;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            a[i][j] = num;
            b[i][j] = num;
            num++;
        }
    }
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            printf("  %d,%d  ",a[i][j],b[i][j]);
        }
        printf("\n");
    }
    return 0;
}