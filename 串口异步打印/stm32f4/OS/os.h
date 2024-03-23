#ifndef OS_H
#define OS_H
#include "list.h"

enum
{
    /* 其他未使用的位可以给任务做通知 */
    NOTIFY_TIMES = 14,            /* 该位置1, 表示该任务被通知后执行一次, 否则一直执行 */
    WILL_DEL = 15,              /* 该位置1, 则表示该任务要被删除 */
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
