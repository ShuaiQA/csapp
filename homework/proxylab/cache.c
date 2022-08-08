#include "cache.h"

static int readcnt;
static sem_t mutex, w;

// Node
void init_node(Node *node) {
    node->url = NULL;
    node->data = NULL;
    node->prev = NULL;
    node->next = NULL;
    node->size = 0;
}

void init_node_with_data(Node *node, char *url, char *data, int n) {
    // url
    int m = strlen(url);
    node->url = (char *) malloc(m);
    strcpy(node->url, url);
    // data
    node->data = (char *) malloc(n);
    strcpy(node->data, data);
    node->prev = NULL;
    node->next = NULL;
    node->size = n;
}

void free_node(Node *node) {
    free(node->url);
    free(node->data);
    free(node);
}

// 两者都不在list中
void link_node(Node *node1, Node *node2) {
    node1->next = node2;
    node2->prev = node1;
}

void change_link(Node *node) {
    link_node(node->prev, node->next);
}

// 插入node2到node1后面, node1在list中
void insert_node(Node *node1, Node *node2) {
    link_node(node2, node1->next);
    link_node(node1, node2);
}

void print_node(Node *node) {
    printf("url is %s, data is %s\n", node->url, node->data);
}

// Cache
void init_cache(Cache *cache, int max_cache_size, int max_object_size) {
    cache->max_cache_size = max_cache_size;
    cache->max_object_size = max_object_size;
    cache->used_size = 0;
    cache->head = NULL;
    cache->tail = NULL;
    readcnt = 0;
    Sem_init(&mutex, 0, 1);
    Sem_init(&w, 0, 1);
    cache->head = (Node *) malloc(sizeof(Node));
    cache->tail = (Node *) malloc(sizeof(Node));
    init_node(cache->head);
    init_node(cache->tail);
    link_node(cache->head, cache->tail);
}

Node *find_cache(Cache *cache, char *url, Node *ptr) {
    ptr = NULL;
    for (ptr = cache->head->next; ptr != cache->tail; ptr = ptr->next) {
        printf("%s %s\n", ptr->url, url);
        if (strcmp(ptr->url, url) == 0) {
            printf("write\n");
            // 移动到第一个位置
            insert_node(cache->head, ptr);
            return ptr;
        }
    }

    return ptr;
}

void free_cache_block(Cache *cache) {
    Node *ptr = cache->tail->prev;
    if (ptr != cache->head && ptr) {
        link_node(ptr->prev, ptr->next);
        cache->used_size -= ptr->size;
        free_node(ptr);
    }
}

void free_cache(Cache *cache) {
    Node *ptr = cache->head;
    while (ptr) {
        Node *next = ptr->next;
        free_node(ptr);
        ptr = next;
    }
}

int insert_cache(Cache *cache, char *url, char *data) {
    int n = strlen(data);
    if (n > cache->max_object_size) {
        printf("This file exceeds the maximum file size!\n");
        return -1;
    }

    while (n + cache->used_size > cache->max_cache_size) {
        free_cache_block(cache);
    }

    cache->used_size += n;
    Node *node = (Node *) malloc(sizeof(Node));
    init_node_with_data(node, url, data, n);
    insert_node(cache->head, node);

    return 0;
}

void print_cache(Cache *cache) {
    Node *ptr = NULL;
    int i = 0;
    for (ptr = cache->head->next; ptr != cache->tail && ptr; ptr = ptr->next) {
        print_node(ptr);
        i++;
    }
}

int reader(Cache *cache, char *url, int fd) {
    P(&mutex);
    readcnt++;
    if (readcnt == 1) {
        P(&w);
    }
    V(&mutex);

    Node *ptr = NULL;
    ptr = find_cache(cache, url, ptr);
    if (ptr != NULL) {
        rio_writen(fd, ptr->data, ptr->size);
        return 1;
    }

    P(&mutex);
    readcnt--;
    if (readcnt == 0) {
        V(&w);
    }
    V(&mutex);

    return -1;
}

void writer(Cache *cache, char *url, char *data) {
    P(&w);
    printf("write to cache!\n");
    insert_cache(cache, url, data);
    V(&w);
}