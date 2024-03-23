#include "os.h"
#include "string.h"
#include <stdio.h>

#define MAX_SLICE_SUPPORT    0x1F   /* �������й��������������ʱ��, �������32ms, ���ᵼ�¼�ʱ��׼ */

typedef enum
{
    ALWAYS = 0,
    ONCE,
}notify_times_t;

#define MILLISECOND_MAX     0xFFFFFFFF
volatile static unsigned int millisecond; 

typedef struct
{
    unsigned int time_que;
    unsigned char bit_head;
    unsigned char bit_tail;
}bit_time_t;

bit_time_t task_time = {0};

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

    /* ��֪ͨ�Ķ��д������ж��� */
    list_for_each_entry(node, &list_run, next, task_t)
    {
        if(node == task)
        {
            node->param = param;
            node->flag &= ~(1 << WILL_DEL);     /* �������񽫴������ж��е�����ɾ��, ����ʱ��������Ҫ������, ���ɾ����־λ, ����������ɾ�� */
            return;
        }
    }

    /* ��֪ͨ�Ķ��д��ھ�������,ֱ���˳� */
    list_for_each_entry(node, &list_ready, next, task_t)
    {
        if(node == task)
        {
            node->param = param;
            return;
        }
    }

    /* ��֪ͨ�Ķ��д��ڵȴ�����,ֱ�ӽ����Ƶ��������� */
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
    if(task_time.time_que & (1 << task_time.bit_head))
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
        task_time.time_que &= ~(1 << task_time.bit_head);
        task_time.bit_head++;
        if(task_time.bit_head > MAX_SLICE_SUPPORT)
        {
            task_time.bit_head = 0;
        }
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
    task_time.time_que |=  1 << task_time.bit_tail;
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
