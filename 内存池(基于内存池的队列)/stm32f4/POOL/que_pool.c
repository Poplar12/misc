#include "que_pool.h"
#include "string.h"
#include "pool.h"
/* 入队 */
int PushQue(list_t** list, unsigned char* buf, int len)
{
    int ret = 1;
    list_t* temp;

    /* 如果队列为空 */
    if (!(*list)) {
        temp = (list_t*)pool_malloc(len + sizeof(list_t));
        if(!temp) {
            ret = 0;
            goto exti;
        }
        memcpy(temp->buf, buf, len);
        temp->next = 0;
        temp->buflen = len;
        
    } else {    /* 队列不为空, 申请新的节点放到末尾 */
        while(temp->next) {
            temp = temp->next;
        }
        temp->next = (list_t*)pool_malloc(len + sizeof(list_t));
        if(!temp->next) {
            ret = 0;
            goto exti;
        }
        memcpy(temp->next->buf, buf, len);
        temp->next->next = 0;
        temp->next->buflen = len;
    }
    *list = temp;
exti:
    return ret;
}

/* 出队 */
int PopQue(list_t** list, unsigned char* buf, int len)
{
    int cpy_len = 0;
    list_t *temp = *list;
    if (*list) {
        len = len > (*list)->buflen ? (*list)->buflen : len;
        memcpy(buf, (*list)->buf, len);
        *list = (*list)->next;
        pool_free((void*)(&temp));  /* 释放内存 */
        cpy_len = len;
    }
    return cpy_len;
}
