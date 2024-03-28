#include "os.h"
#include <stdio.h>
#include "print.h"
#include "pool.h"
#include "string.h"

static void task1(void *p);
extern task_t task_2;
static task_t task_1 = {
    .task = task1,
    .time_slice = 500,
    .slice_count = 500,
};
int t1 = 0;
static void task1(void *p)
{
    char *a;
    int val = *(int*)p;
    static int count = 0;
    count++;
//    my_printf("task1\n");
    if(count > 2)
    {
        static int value = 10;
        notify_task(&task_2, &value);
        LOG_INFO("task_1 notify task_2\n");
        count = 0;
        LOG_INFO("val = %d\n", val);
    }
   
    LOG_INFO("task1\r\n");
    a = (char*)pool_malloc(100);
     
    strcpy(a, "task1 is run");
//    printf("%s", a);
    pool_free((void**)&a);
 
    
    
    
}
int a = 5;
/* 使用编译器特性, 自动运行该程序 */
__attribute__((constructor)) static void task1_add()
{
    add_task(&task_1, &a);
}
