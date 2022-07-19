#include "cachelab.h"
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include <getopt.h>


//代表每一个缓存行
typedef struct {
    int valid;     //每一个catch行是否有效
    int tag;       //每一个catch行中的标记位(每组可能会有多行)
    int time;      //时间戳time越大代表上一次访问的时间越久
} Cache_line;


//对于每一个cache组都应该对应一个双向链表用来更新cache操作
typedef struct {
    Cache_line *line;
} Cache_group;

//代表cache缓存
/**
 * 根据cache的s,E,b来确定catch_line的情况
 * 其中行 = E * 2 ^ s
 */
typedef struct {
    int s;    //使用多少位表示组大小
    int E;    //每个组的行数
    int b;    //使用多少位表示块大小
    //cache的二维数组,i代表是哪一个组,j代表是改组的哪一行
    Cache_group *group;
} Cache;


int hits;
int misses;
int evictions;
int show;

/**
 * 声明一个缓存系统
 * @param s 组的位数表示大小
 * @param E 每个组有多少个块
 * @param b 每个块的数据位数表示
 * @return
 */
Cache *New_catch(int s, int E, int b) {
    int S = (1 << s);
    //声明一个指向cache的指针
    Cache *cache = (Cache *) malloc(sizeof(Cache));
    cache->E = E;
    cache->b = b;
    cache->s = s;
    //声明一个指向cache_line的指针数组,每一个指针指向一个数据结构cache_line的指针数组
    cache->group = (Cache_group *) malloc(sizeof(Cache_group) * S);    //声明多少个cache组
    for (int i = 0; i < S; i++) {
        //每一个指针数组指向的是E个数据结构cache_line行
        cache->group[i].line = (Cache_line *) malloc(sizeof(Cache_line) * E);
        for (int j = 0; j < E; j++) {
            cache->group[i].line[j].valid = 0; //初始时，高速缓存是空的
            cache->group[i].line[j].tag = -1;
            cache->group[i].line[j].time = 0;
        }
    }
    return cache;
}

/**
 * 释放cache缓存
 * @param cache
 */
void freeCache(Cache *cache) {
    int S = (1 << cache->s);
    for (int i = 0; i < S; ++i) {
        free(cache->group[i].line);  //将每一个cache组free
    }
    free(cache->group);
    free(cache);
}

void help() {
    printf("当前需要帮助\n");
}


//对cache中的group中非当前行进行时间自增操作
void update_LRU(Cache *cache, unsigned group) {
    for (int i = 0; i < cache->E; ++i) {
        if (cache->group[group].line[i].valid == 1) {
            cache->group[group].line[i].time++;
        }
    }
}


/**
 * 根据已知的group,tag去缓存中查找是否命中
 * @param cache
 * @param cache_group
 * @param cache_tag
 * @return 命中返回是在当前组中的哪一行,否则返回-1
 */
int is_hit(Cache *cache, unsigned cache_group, unsigned cache_tag) {
    //查找当前cache组中的每一行
    for (int i = 0; i < cache->E; ++i) {
        //命中，也就是说group中的行里面的valid==1以及tag符合要求
        if (cache->group[cache_group].line[i].valid == 1 && cache->group[cache_group].line[i].tag == cache_tag) {
            //如果命中更新当前的行的时间戳为0
            cache->group[cache_group].line[i].time = 0;
            return i;
        }
    }
    return -1;
}


int LRU(Cache *cache, unsigned group) {
    int pos = 0;    //获取当前group中的最大的时间戳
    for (int i = 1; i < cache->E; ++i) {
        if (cache->group[group].line[i].time > cache->group[group].line[pos].time) {
            pos = i;
        }
    }
    return pos;
}

/**
 * 没有命中cache进行更新,根据当前的group以及tag进行更新操作
 * @param cache
 * @param group
 * @param tag
 */
void update_cache(Cache *cache, unsigned group, unsigned tag) {
    if (show) printf(" miss ");
    misses++;
    int hang = -1;  //-1代表当前cache的group已经满了
    for (int i = 0; i < cache->E; ++i) {
        if (cache->group[group].line[i].valid != 1) {
            //找到空闲的块进行赋值
            hang = i;
            break;
        }
    }
    if (hang == -1) {
        evictions++;
        if (show) printf(" eviction ");
        hang = LRU(cache, group);
    }
    //根据group,hang,对cache进行更新操作
    cache->group[group].line[hang].valid = 1;
    cache->group[group].line[hang].tag = tag;
    cache->group[group].line[hang].time = 0;
}

/**
 * 将address进行拆分成相关的cache组和行,对cache进行查找更新
 * @param cache
 * @param address
 * @param size
 */
void find_cache(Cache *cache, unsigned address, int size) {
    int b = cache->b;
    int s = cache->s;
    int B = (1 << cache->b);
    //获取当前是哪一个组、标记位(获取group重点)
    unsigned cache_group = (address >> b) & ((unsigned) (-1) >> (8 * sizeof(unsigned) - s));
    unsigned cache_tag = address >> (b + s);
    //printf("   当前的group[%x],tag[%x]   \n", cache_group, cache_tag);
    //更新group中的time,无论是hit还是miss都需要time++
    update_LRU(cache, cache_group);
    //判断当前的size大小是否大于cache的块
    if (B >= size) {    //B(cache块的大小大于size那么就需要一块就够了)
        int hit = is_hit(cache, cache_group, cache_tag);
        if (hit != -1) {
            if (show) printf(" hit");
            hits++;
        } else {
            //不命中对缓存行进行更新操作，
            update_cache(cache, cache_group, cache_tag);
        }
    } else {
        printf("当前的size较大,需要多次进行查找");
        size -= B;
        address += size;
        find_cache(cache, address, size);
    }
}


/**
 * 读取path下的文件，根据文件的每一行的格式以及cache进行判断操作
 * [id address,size] id代表当前的操作是什么，address代表当前的地址注意是16进制的，size代表是从当前地址之后获取多少个字节数
 * @param path 根据路径获取目标文件
 * @param cache 根据当前的缓存判断当前的每一行数据操作情况
 */
void readfile(char *path, Cache *cache) {
    FILE *file;
    char id;
    unsigned address;
    int size;
    file = fopen(path, "r");
    if (file == NULL) {
        printf("%s file do not exit", path);
        exit(-1);
    }
    while (fscanf(file, " %c %x,%d", &id, &address, &size) > 0) {
        if (show) printf("%c,0x%x,%d  ", id, address, size);
        switch (id) {
            case 'L':
                find_cache(cache, address, size);
                break;
            case 'S':
                find_cache(cache, address, size);
                break;
            case 'M':    //修改会进行一次更新一次存储,当更新后操作发生,存储的cache操作必定命中
                find_cache(cache, address, size);
                hits++;
                if (show) printf(" hit ");
                break;
            default:
                if (show) printf("continue");
        }
        if (show) printf("\n");
    }
    fclose(file);
}


int main(int argc, char *argv[]) {
    char opt;
    int s, E, b;
    char t[1000];
    while (-1 != (opt = getopt(argc, argv, "hvs:E:b:t:"))) {
        switch (opt) {
            case 'h':
                help();
                exit(0);
            case 'v':
                show = 1;
                break;
            case 's':
                s = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                strcpy(t, optarg);
                break;
            default:
                help();
                exit(-1);
        }
    }
    //获取一个cache缓存
    Cache *cache = New_catch(s, E, b);
    //printf("%d,%d,%d\n",cache->s,cache->E,cache->b);
    //读取文件内容
    readfile(t, cache);
    printSummary(hits, misses, evictions);
    return 0;
}
