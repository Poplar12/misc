#include "os.h"
#include <stdio.h>
#include "print.h"
void task2(void* p);
task_t task_2 = {
    .task = task2,
    .time_slice = 300,
    .slice_count = 300,
};
void task2(void *p)
{
    static int count = 0;
    count++;
    if(count  > 100)
    {
        LOG_DEBUG("task2\r\n");
        count = 0;
    }
    if(count > 25)
    {
        LOG_DEBUG("delete by onself\n");
        delet_task(&task_2);
        count = 0;
    }
    if(p != NULL)
    {
        LOG_DEBUG("p= %d\n", *(int*)p);
    }
    LOG_DEBUG("task2\r\n");
}

__attribute__((constructor)) void task2_add()
{
    add_task(&task_2, NULL);
}
