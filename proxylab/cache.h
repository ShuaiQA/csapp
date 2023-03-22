#define MAXCACHESIZE 100
#define MAX_OBJ 102400
#include "csapp.h"

typedef struct node {
  char url[MAXLINE];
  char content[MAX_OBJ];
  struct node *pre;
  struct node *next;
  size_t content_len;
  /* data */
} node;

typedef struct Cache {
  size_t size;       //最大cache为100
  sem_t mutex;       //互斥访问cache
  struct node *head; //常访问的放到头节点
  struct node *tail;
  /* data */
} Cache;

//初始化cache
void init_cache(Cache *cache);
// find查找cache是否有当前的目标值
int fund_url(Cache *cache, char *url, char *content, int fd);
//没有找到插入当前的url和content
void insert_node(Cache *cache, char *url, char *content, size_t len);
