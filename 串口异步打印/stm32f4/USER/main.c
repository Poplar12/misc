#include "./UART/uart.h"
#include "./BaseTime/basetime.h"
#include "os.h"
#include "print.h"
char* p = __DATE__;
char* t = __TIME__;
int main(void)
{
    set_send_cb(read_data);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    uart_init(115200);
    bsTime_Init(1000-1, 83);//1msÖÐ¶Ï
    
    while(1)
    {
        static unsigned int t1 = 0;
        
        run_task();
        
        if(time_interval(&t1) > 1000)
        {
            my_printf("hello: %s, %s, %d\n", p, t, 100);
            t1 = 0;
        }
    } 
}

