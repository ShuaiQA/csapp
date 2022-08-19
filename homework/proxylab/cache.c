#include "csapp.h"
#include "cache.h"

//初始化一个头节点
void init_cache(Cache *cache)
{
    sem_init(&cache->mutex,0,1);
    cache->size = 0;
    cache->head = (node *)Malloc(sizeof(node));
    cache->tail = cache->head;
    cache->tail->next = NULL;
    return;
}

//将节点cur插入到队头
void insert_head(Cache *cache, node *cur)
{
    if(cache->head->next==NULL){   
        cache->head->next = cur;
        cur->pre = cache->head;
        cur->next = NULL;
        cache->tail = cur;
        return;
    }        
    cache->head->next->pre = cur;
    cur->next = cache->head->next;
    cur->pre = cache->head;
    cache->head->next = cur;
}

//查找到当前的节点cur，移动cur节点到队头位置
void vis_node(Cache *cache, node *cur)
{
    if(cache->head->next == cur){   //当前节点已经是头节点直接返回
        return;
    }
    struct node *p = cur;
    //如果当前节点是最后一个那么
    if(p->next==NULL){
        cache->tail = cache->tail->pre;
        cache->tail->next = NULL;
        insert_head(cache,p);
        return;
    }
    p->next->pre = p->pre;
    p->pre->next = p->next;
    insert_head(cache, p);
}

//根据cache里面的节点查找url，将查找到的content放到参数中
int fund_url(Cache *cache, char *url, char *content,int fd)
{
    P(&cache->mutex);
    
    node *p = cache->head->next;
    while (p != NULL)
    {
        //返回值为0就是相等
        if (!strcmp(p->url, url))
        {
            vis_node(cache, p);
            Rio_writen(fd,p->content,p->content_len); 
            V(&cache->mutex);
            return 1;
        }
        p = p->next;
        /* code */
    }
    V(&cache->mutex);
    return 0;
}

//删除最后一个节点
void delete_node(Cache *cache)
{
    node *p = cache->tail;
    cache->tail = p->pre;
    cache->tail->next = NULL;
    p->pre = NULL;
    Free(p->content);
    Free(p);
    cache->size -= 1;
}

//添加url，content到cache队头中
void insert_node(Cache *cache, char *url, char *content,size_t len)
{
    P(&cache->mutex);
    if (cache->size > MAXCACHESIZE)
    {
        delete_node(cache);
    }
    struct node *cur = (node *)Malloc(sizeof(node));
    strncpy(cur->url, url, strlen(url));
    strncpy(cur->content, content, strlen(content));
    cur->content_len = len;
    insert_head(cache, cur);
    cache->size += 1;
    V(&cache->mutex);
}
