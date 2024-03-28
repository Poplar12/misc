#include "./UART/uart.h"
#include "./BaseTime/basetime.h"
#include "os.h"
#include "print.h"
#include "pool.h"
#include "que_pool.h"
#include "string.h"

char* p = __DATE__;
char* t = __TIME__;
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    set_send_cb(read_data);
    reg_transmit(start_transmit);
    init_pool(6, 50, 12, 200, 2);
    uart_init(115200);

    bsTime_Init(1000-1, 83);//
   
    list_t* list = 0;   /* 定义链表头, 并初始化为空指针 */
    while(1)
    {
        static unsigned int t1 = 0, t2 = 0;
        unsigned char buff[20] = {0}, len;

        run_task();

        while(time_interval(&t1) > 100)
        {
            char * str = "hello world";
            if(!PushQue(&list, (unsigned char*)str, strlen(str)))
            {
                LOG_ERR("pushQue");
            }
            get_pool_info();
            //LOG_ERR("test test1");
            memset(buff, 0, 20);
            if(!(len = PopQue(&list, buff, 20)))
            {
                LOG_ERR("PopQue");
            }
            LOG_ERR("test: %d, %s", len, buff);
            t1 = 0;
        }

    } 
}

