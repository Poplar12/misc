#include "os.h"
#include <stdio.h>
#include "./UART/uart.h"
static void task1(void *p);
extern task_t task_2;
static task_t task_1 = {
    .task = task1,
    .time_slice = 50,
    .slice_count = 50,
};

static void task1(void *p)
{
    int val = *(int*)p;
    static int count = 0;
    count++;
//    my_printf("task1\n");
    if(count > 2)
    {
        static int value = 10;
        notify_task(&task_2, &value);
        my_printf("task_1 notify task_2\n");
        count = 0;
        my_printf("val = %d\n", val);
    }
    my_printf("task1\r\n");
    
}
int a = 5;
/* 使用编译器特性, 自动运行该程序 */
__attribute__((constructor)) static void task1_add()
{
    add_task(&task_1, &a);
}
