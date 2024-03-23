#include "os.h"
#include <stdio.h>
#include "./UART/uart.h"
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
    my_printf("task3\r\n");
}

task_t task_3 = {
    .task = task3,
    .time_slice = 20,
    .slice_count = 20,
};

__attribute__((constructor)) void task3_add()
{
    add_task(&task_3, NULL);
}
