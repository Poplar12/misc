#ifndef QUE_H
#define QUE_H

typedef struct {
    unsigned char *buf;
    unsigned short len;
    unsigned short head;
    unsigned short tail;
}que_t;

/* 使用宏创建队列 */
#define creat_que(que_name, len)	\
    unsigned char que_buf_##que_name[len] = {0}; \
    que_t que_##que_name = {que_buf_##que_name, len, 0, 0}; \
    static que_t* que_name = &que_##que_name

int QueIsEmpty(const que_t* que);
void InQue(que_t* que, unsigned char val);
int OutQue(que_t* que, unsigned char *val);
int QueLen(que_t *que);

#endif
