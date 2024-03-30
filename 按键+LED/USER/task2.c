#include "os.h"
#include <stdio.h>
void task2(void* p);
task_t task_2 = {
    .task = task2,
    .time_slice = 3,
    .slice_count = 3,
};
void task2(void *p)
{
    static int count = 0;
    count++;
//    if(count  > 100)
//    {
        //printf("task2\r\n");
//        count = 0;
//    }
    if(count > 25)
    {
        //printf("delete by onself\n");
        delet_task(&task_2);
        count = 0;
    }
    if(p != NULL)
    {
        //printf("p= %d\n", *(int*)p);
    }
}

__attribute__((constructor)) void task2_add()
{
    add_task(&task_2, NULL);
}
