#include "que.h"

int QueIsEmpty(const que_t* que)
{
    if(que->head == que->tail) {
        return 1;
    }
    return 0;
}

void InQue(que_t* que, unsigned char val)
{
    if (que->head == (que->tail + 1) % que->len)
    {
        que->buf[que->tail] = val;
        que->tail = que->head;
        que->head = (que->head + 1) % que->len;
    }
    else
    {
        que->buf[que->tail] = val;
        que->tail = (que->tail + 1) % que->len;
    }
}

int OutQue(que_t* que, unsigned char *val)
{
    if (QueIsEmpty(que))
    {
        return 0;
    }
    else
    {
        *val = que->buf[que->head];
        que->head = (que->head + 1) % que->len;
    }
    return 1;
}

int QueLen(que_t *que)
{
    return (que->head <= que->tail) ? que->len - (que->tail - que->head) : que->head - que->tail;
}
