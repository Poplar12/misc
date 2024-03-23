#include "os.h"
#include <stdio.h>
#include "./UART/uart.h"
void task2(void* p);
task_t task_2 = {
    .task = task2,
    .time_slice = 30,
    .slice_count = 30,
};
void task2(void *p)
{
    static int count = 0;
    count++;
    if(count  > 100)
    {
        my_printf("task2\r\n");
        count = 0;
    }
    if(count > 25)
    {
        my_printf("delete by onself\n");
        delet_task(&task_2);
        count = 0;
    }
    if(p != NULL)
    {
        my_printf("p= %d\n", *(int*)p);
    }
    my_printf("task2\r\n");
}

__attribute__((constructor)) void task2_add()
{
    add_task(&task_2, NULL);
}
