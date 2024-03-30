#include "os.h"
#include "string.h"
#include <stdio.h>


#define ARR_LEN                     8       /* �޸����ֵ�Ϳ��Ե��������ʱʱ��(ARR_LEN��CHAR_LEN�Ľ��������2��n�η�)��Ŀǰ���õ������ʱΪ 8x8 =64ms */

#define CHAR_LEN                    8       /* char���͵ĳ���Ϊ8 */
#define BIT_NUM                     ARR_LEN * CHAR_LEN
#define MAX_SLICE_SUPPORT           BIT_NUM - 1
#if (BIT_NUM & (MAX_SLICE_SUPPORT))
#error "BIT_NUM must be 2^n"
#endif

#define CHAR_BIT_MAX    7
#define R_SHIFT         3       /* ʹ������3λ�������8 */

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

/* ����ȴ�����,�����������, �������ж��� */
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
    if(task->time_slice == 0)   /* ���ʱ��Ƭ����Ϊ0, ��ֱ�ӹҵ��������� */
    {
        list_add_tail(&task->next, &list_ready);
    }
    else    /* ��������ҵ��ȴ����� */
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

    /* ��֪ͨ������������ж��� */
    list_for_each_entry(node, &list_run, next, task_t)
    {
        if(node == task)
        {
            node->param = param;
            node->flag &= ~(1 << WILL_DEL);     /* �������񽫴������ж��е�����ɾ��, ����ʱ��������Ҫ������, ���ɾ����־λ, ����������ɾ�� */
            return;
        }
    }

    /* ��֪ͨ��������ھ�������,ֱ���˳� */
    list_for_each_entry(node, &list_ready, next, task_t)
    {
        if(node == task)
        {
            node->param = param;
            return;
        }
    }

    /* ��֪ͨ��������ڵȴ�����,ֱ�ӽ����Ƶ��������� */
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

    /* ��ǰ����û���κζ���, ������ӵ��������� */
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

    /* ��ɾ���Ķ��д��ڵȴ�����,����ɾ�� */
    list_for_each_entry(node, &list_wait, next, task_t)
    {
        if(node == task)
        {
            list_del(&node->next);
            return;
        }
    }

    /* ��ɾ���Ķ��д������ж���,�������е��������ʱ�����ɾ�� */
    list_for_each_entry(node, &list_run, next, task_t)
    {
        if(node == task)
        {
            node->flag |= (1 << WILL_DEL);
            return;
        }
    }

    /* ��ɾ���Ķ��д��ھ�������,����ɾ�� */
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

    /* ʱ����������Ƿ���ʱ�� */
    if(task_time.time_que[task_time.bit_head >> R_SHIFT] & (1 << (task_time.bit_head & CHAR_BIT_MAX)))
    {
        /* ����ʱ�ȴ����е�ʱ���һ */
        list_for_each_entry(node, &list_wait, next, task_t)
        {
            node->slice_count--;
            if(node->slice_count == 0)  /* ���ʱ�������, �򽫵�ǰ����ҵ��������� */
            {
                memcpy(&temp_node, node, sizeof(task_t));
                node->slice_count = node->time_slice;
                move_task(node, &list_wait);
                node = &temp_node;
            }
        }

        /* ����ǰbit��ʱ������, ����bit_headָ����һ��λ�� */
        task_time.time_que[task_time.bit_head / CHAR_LEN] &= ~(1 << (task_time.bit_head & CHAR_BIT_MAX));
        task_time.bit_head++;
        task_time.bit_head &= MAX_SLICE_SUPPORT;
    }

    /* ���������е������Ƶ����ж��� */
    list_for_each_entry(node, &list_ready, next, task_t)
    {
        memcpy(&temp_node, node, sizeof(task_t));
        move_task(node, &list_ready);
        node = &temp_node;
    }

    /* ִ�����ж����е����� */
    list_for_each_entry(node, &list_run, next, task_t)
    {
        memcpy(&temp_node, node, sizeof(task_t));

        /* ����ʱ��Ƭ,ѡ������ҵ��ȴ�����,���Ǿ������� */
        if(node->time_slice)
        {
            move_task(node, &list_run);
        }
        else
        {
            move_task(node, &list_run);
        }

        /* ��ǰ���е������Ƿ��ѱ�ɾ��������? */
        if(node->flag & (1 << WILL_DEL))
        {
            delet_task(node);
            node->flag &= ~(1 << WILL_DEL); /* ���״̬ */
        }
        else
        {
            node->task(node->param);

            /* ��֪ͨ������ִֻ��һ�� */
            if(node->flag & (1 << NOTIFY_TIMES))
            {
                node->flag &= ~(1 << NOTIFY_TIMES); /* ���ִֻ��һ�α�־λ */
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
