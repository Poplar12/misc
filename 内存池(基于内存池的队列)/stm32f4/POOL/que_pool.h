#ifndef QUE_POOL_H
#define QUE_POOL_H

typedef struct node{
    struct node* next;
    int buflen;
    unsigned char buf[0];
}list_t;

int PushQue(list_t** list, unsigned char* buf, int len);
int PopQue(list_t** list, unsigned char* buf, int len);

#endif
