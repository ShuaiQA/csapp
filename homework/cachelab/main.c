#include<stdio.h>
#include<stdlib.h>

typedef struct {
    int age;   //每个人的年龄
} User;


typedef struct {
    int num;    //班里的人数
    User *user;
} Class;


Class *NewClass(int num) {
    Class *class = (Class *) malloc(sizeof(Class));
    class->num = num;
    class->user = (User *) malloc(sizeof(User) * num);
    for (int i = 0; i < num; ++i) {
        class->user[i].age = i;
    }
    return class;
}

int main() {
    Class *class = NewClass(20);
    for (int i = 0; i < 20; ++i) {
        printf("%d   ",class->user[i].age);
    }
    return 0;
}