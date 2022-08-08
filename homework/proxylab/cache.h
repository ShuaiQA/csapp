//
// Created by shuai on 22-8-8.
//
#ifndef __CACHE__
#define __CACHE__

#include <stdio.h>
#include "csapp.h"
// #include <csapp.h>

// #define MAX_URL_LEN 512;

typedef struct Node {
    char *url;
    char *data;
    struct Node *prev;
    struct Node *next;
    int size;
} Node;

// ptr->p0->p1
typedef struct Cache {
    int max_cache_size;
    int max_object_size;
    int used_size;
    struct Node *head;
    struct Node *tail;
} Cache;

// Node
void init_node(Node *node);

void init_node_with_data(Node *node, char *url, char *data, int n);

void free_node(Node *node);

void link_node(Node *node1, Node *node2);

void change_link(Node *node);

void insert_node(Node *node1, Node *node2);

void print_node(Node *node);

// Cache
void init_cache(Cache *cache, int max_cache_size, int max_object_size);

Node* find_cache(Cache *cache, char *url, Node *ptr);

void free_cache_block(Cache *cache);

void free_cache(Cache *cache);

int insert_cache(Cache *cache, char *url, char *data);

void print_cache(Cache *cache);

int reader(Cache *cache, char *url, int fd);

void writer(Cache *cache, char *url, char *data);

#endif