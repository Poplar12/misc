#ifndef POOL_H
#define POOL_H
#include <stdio.h>
#include <stdarg.h>

#define SHOW_POOL_INFO      1

typedef struct block{
    unsigned char is_used;
    unsigned int block_size;
    struct block* next;
    unsigned char buf[0];
}pool_t;

int init_pool(int arg_num, ...);
void* pool_malloc(unsigned int size);
unsigned char pool_free(void** p);

#if SHOW_POOL_INFO
    typedef struct {
        unsigned int total_block_num;
        unsigned int used_block_num;
        unsigned int used_pool_mm;
    }pool_info_t;
    void get_pool_info(void);
#endif

#endif
