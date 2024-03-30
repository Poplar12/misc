#ifndef OS_H
#define OS_H
#include "list.h"

enum
{
    /* ����δʹ�õ�λ���Ը�������֪ͨ */
    NOTIFY_TIMES = 14,            /* ��λ��1, ��ʾ������֪ͨ��ִ��һ��, ����һֱִ�� */
    WILL_DEL = 15,              /* ��λ��1, ���ʾ������Ҫ��ɾ�� */
};

typedef struct
{
    void (*task)(void* param);
    void *param;
    unsigned short flag;
    unsigned short time_slice;
    unsigned short slice_count;
    struct list_head next;
}task_t;  

void add_task(task_t *task, void* param);
void notify_task(task_t *task, void *param);
void wakeup_task(task_t *task, void *param);
void delet_task(task_t *task);
void run_task(void);

void time_cb(void);
unsigned int current_time(void);
unsigned int time_interval(unsigned int *start_time);

#endif
