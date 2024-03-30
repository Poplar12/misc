#include "os.h"
#include "string.h"
#include <stdio.h>


#define ARR_LEN                     8       /* 修改这个值就可以调节最大延时时间(ARR_LEN乘CHAR_LEN的结果必须是2的n次方)，目前设置的最大延时为 8x8 =64ms */

#define CHAR_LEN                    8       /* char类型的长度为8 */
#define BIT_NUM                     ARR_LEN * CHAR_LEN
#define MAX_SLICE_SUPPORT           BIT_NUM - 1
#if (BIT_NUM & (MAX_SLICE_SUPPORT))
#error "BIT_NUM must be 2^n"
#endif

#define CHAR_BIT_MAX    7
#define R_SHIFT         3       /* 使用右移3位替代除以8 */

typedef enum
{
    ALWAYS = 0,
    ONCE,
}notify_times_t;

#define MILLISECOND_MAX     0xFFFFFFFF
volatile static unsigned int millisecond; 

typedef struct
{
    unsigned char time_que[ARR_LEN];
    unsigned short bit_head;
    unsigned short bit_tail;
}bit_time_t;

static volatile bit_time_t task_time = {0};

/* 任务等待队列,任务就绪队列, 任务运行队列 */
struct list_head list_wait = LIST_HEAD_INIT(list_wait);
struct list_head list_ready = LIST_HEAD_INIT(list_ready);
struct list_head list_run = LIST_HEAD_INIT(list_run);

static void move_task(task_t *task, struct list_head* soure_list)
{
    if(soure_list == &list_wait)    /* if the task in list_wait, then move to list_ready */
    {
        list_del(&task->next);
        list_add_tail(&task->next, &list_ready);
    }
    else if(soure_list == &list_ready)  /* if the task in list_ready, then move to list_run */
    {
        list_del(&task->next);
        list_add_tail(&task->next, &list_run);
    }
    else
    {
        if(task->time_slice)    /* if the task in list_run and time_slice is not zero, then move to list_wait */
        {
            list_del(&task->next);
            list_add_tail(&task->next, &list_wait);
        }
        else
        {
            list_del(&task->next);
            list_add_tail(&task->next, &list_ready);
        }
    }
}
/* 
 *@brief    add task
 *@param    task
 *@retval   NONE
 *@note     can add task only before system run.
 */
void add_task(task_t *task, void *param)
{
    task->param = param;
    if(task->time_slice == 0)   /* 如果时间片设置为0, 则直接挂到就绪队列 */
    {
        list_add_tail(&task->next, &list_ready);
    }
    else    /* 否则将任务挂到等待队列 */
    {
        list_add_tail(&task->next, &list_wait);
    }
}
static void call_task(task_t *task, void *param, notify_times_t times)
{
    task_t  *node;

    if(times == ONCE)
    {
        task->flag |= 1 << NOTIFY_TIMES;
    }

    /* 被通知的任务存在运行队列 */
    list_for_each_entry(node, &list_run, next, task_t)
    {
        if(node == task)
        {
            node->param = param;
            node->flag &= ~(1 << WILL_DEL);     /* 其他任务将处于运行队列的任务删除, 而此时又有任务要唤醒它, 清除删除标志位, 不将该任务删除 */
            return;
        }
    }

    /* 被通知的任务存在就绪队列,直接退出 */
    list_for_each_entry(node, &list_ready, next, task_t)
    {
        if(node == task)
        {
            node->param = param;
            return;
        }
    }

    /* 被通知的任务存在等待队列,直接将其移到就绪队列 */
    list_for_each_entry(node, &list_wait, next, task_t)
    {
        if(node == task)
        {
            node->param = param;
            node->slice_count = node->time_slice;
            move_task(node, &list_wait);
            return;
        }
    }

    /* 当前任务没在任何队列, 则将其添加到就绪队列 */
    task->slice_count = task->time_slice;
    task->time_slice = 0;
    add_task(task, param);
    task->time_slice = task->slice_count;
}
/* 
 *@brief    wake up the task
 *@param    task need to notify task; param: must be a Static variable
 *@retval   NONE
 *@note     can add task when system is runing and task will run immediately.
 */
void wakeup_task(task_t *task, void *param)
{
    call_task(task, param, ALWAYS);
}
/* 
 *@brief    notify the task(only run once)
 *@param    task need to notify task; param: must be a Static variable
 *@retval   NONE
 */
void notify_task(task_t *task, void *param)
{
    call_task(task, param, ONCE);
}
/* 
 *@brief    delete Task
 *@param    task
 *@retval   NONE
 */
void delet_task(task_t *task)
{
    task_t  *node;

    /* 被删除的队列存在等待队列,可以删除 */
    list_for_each_entry(node, &list_wait, next, task_t)
    {
        if(node == task)
        {
            list_del(&node->next);
            return;
        }
    }

    /* 被删除的队列存在运行队列,则在运行到该任务的时候进行删除 */
    list_for_each_entry(node, &list_run, next, task_t)
    {
        if(node == task)
        {
            node->flag |= (1 << WILL_DEL);
            return;
        }
    }

    /* 被删除的队列存在就绪队列,可以删除 */
    list_for_each_entry(node, &list_ready, next, task_t)
    {
        if(node == task)
        {
            list_del(&node->next);
            return;
        }
    }
}

/* 
 *@brief    Task scheduling function
 *@param    NONE
 *@retval   NONE
 */
void run_task()
{
    task_t  *node, temp_node;

    /* 时间队列里面是否有时间 */
    if(task_time.time_que[task_time.bit_head >> R_SHIFT] & (1 << (task_time.bit_head & CHAR_BIT_MAX)))
    {
        /* 将延时等待队列的时间减一 */
        list_for_each_entry(node, &list_wait, next, task_t)
        {
            node->slice_count--;
            if(node->slice_count == 0)  /* 如果时间减完了, 则将当前任务挂到就绪队列 */
            {
                memcpy(&temp_node, node, sizeof(task_t));
                node->slice_count = node->time_slice;
                move_task(node, &list_wait);
                node = &temp_node;
            }
        }

        /* 将当前bit的时间清零, 并让bit_head指向下一个位置 */
        task_time.time_que[task_time.bit_head / CHAR_LEN] &= ~(1 << (task_time.bit_head & CHAR_BIT_MAX));
        task_time.bit_head++;
        task_time.bit_head &= MAX_SLICE_SUPPORT;
    }

    /* 将就绪队列的任务移到运行队列 */
    list_for_each_entry(node, &list_ready, next, task_t)
    {
        memcpy(&temp_node, node, sizeof(task_t));
        move_task(node, &list_ready);
        node = &temp_node;
    }

    /* 执行运行队列中的任务 */
    list_for_each_entry(node, &list_run, next, task_t)
    {
        memcpy(&temp_node, node, sizeof(task_t));

        /* 根据时间片,选择将任务挂到等待队列,还是就绪队列 */
        if(node->time_slice)
        {
            move_task(node, &list_run);
        }
        else
        {
            move_task(node, &list_run);
        }

        /* 当前运行的任务是否已被删除的任务? */
        if(node->flag & (1 << WILL_DEL))
        {
            delet_task(node);
            node->flag &= ~(1 << WILL_DEL); /* 清除状态 */
        }
        else
        {
            node->task(node->param);

            /* 被通知的任务只执行一次 */
            if(node->flag & (1 << NOTIFY_TIMES))
            {
                node->flag &= ~(1 << NOTIFY_TIMES); /* 清除只执行一次标志位 */
                delet_task(node);
            }
        }
        node = &temp_node;
    }
}

/* 
 *@brief    time callback(Put in the timer interrupt)
 *@param    NONE
 *@retval   NONE
 */
inline void time_cb()
{ 
    task_time.bit_tail = millisecond & MAX_SLICE_SUPPORT;
    task_time.time_que[task_time.bit_tail >> R_SHIFT] |=  1 << (task_time.bit_tail & CHAR_BIT_MAX);
    millisecond++;
}

/* 
 *@brief    get system current time
 *@param    NONE
 *@retval   system current time
 */
unsigned int current_time()
{
    return millisecond;
}

/* 
 *@brief    get time interval
 *@param    start_time: must be a Static variable, save start time.
 *@retval   time interval(if the retval more than set value, start time must clear)
 */
unsigned int time_interval(unsigned int *start_time)
{
    if(*start_time == 0)
    {
        *start_time = millisecond;
    }
    return (millisecond >= *start_time) ? (millisecond - *start_time) : (MILLISECOND_MAX - *start_time + millisecond);
}
