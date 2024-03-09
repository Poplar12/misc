#include "./UART/uart.h"
#include "./BaseTime/basetime.h"
#include "os.h"
#include "auto.h"
#include "bsp.h"
int main(void)
{ 
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    uart_init(115200);
    bsTime_Init(1004, 80);//1ms÷–∂œ
    
    
    const device_t* led = find_device("led");
    const device_t* key = find_device("key");
    
    const driver_t* bsp_led = find_driver("io_led");
    
    while(1)
    {
        unsigned int time = 0;
        while(time_interval(&time) < 2000);
        if(led->write)
        {
            char a = 1;
            printf("found led\n");
            led->write(0, &a, 1);
        }else
        {
            printf("no found\n");
        }
        
        if(key)
        {
            char state;
            key->read(0, &state, 1);
            printf("state = %d", state);
        }
        
        if(bsp_led)
        {
            io_opt_t* led = (io_opt_t*)(bsp_led->opt);
            led->set_io(1);
        }
    } 
}

