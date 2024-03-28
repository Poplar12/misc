#include "os.h"
#include <stdio.h>
#include "print.h"
extern task_t task_2;
void task3(void *p)
{
    static int count = 0;
    count++;
    if(count > 50)
    {
//        printf("task3\r\n");
        count = 0;
        delet_task(&task_2);
    }
//    LOG_ERR("task3\r\n");
}

task_t task_3 = {
    .task = task3,
    .time_slice = 2000,
    .slice_count = 2000,
};

__attribute__((constructor)) void task3_add()
{
    add_task(&task_3, NULL);
}
