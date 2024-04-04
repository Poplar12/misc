#include "./UART/uart.h"
#include "./BaseTime/basetime.h"
#include "os.h"
#include "button.h"
#include "key.h"
#include "led.h"
#include "./LED/bsp_led.h"

void send_led_on(BtnEvt_t evt)
{
    if(evt == PRESS_SHORT)
    {
        send_led_state(ON);
    }
}
ADD_EVENT(send_led_on);



void send_led_3times(BtnEvt_t evt)
{
    if(evt == PRESS_200MS)
    {
        send_led_state(ON_3Times);
    }
}
ADD_EVENT(send_led_3times);

void send_led_breath(BtnEvt_t evt)
{
    if(evt == PRESS_2TIMES)
    {
        send_led_state(BREATH);
    }
}
ADD_EVENT(send_led_breath);


/* 创建按键任务 */
void key_task(void* param)
{
    static int key_device = 0;

    static Btn_t btn = {
        .PreSta = KEY_RELEASE,
        .StartTime = 0,
#if SUPPORT_MULTI_CLICK
        .ClickCount = 0,
        .ReleaseTime = 0,
#endif
    };

    if(key_device == 0)
    {
        KEY_Init();
        key_device = 1;
    }

    GetBtnEvt(&btn, (BtnSta_t)KEY0);
#if 0
    switch(GetBtnEvt(&btn, (BtnSta_t)KEY0))
    {
        case PRESS_SHORT:
            printf("press short \r\n");
            send_led_state(ON);
            break;
        case PRESS_200MS:
            printf("press 200ms\r\n");
            send_led_state(ON_3Times);
            break;
        case PRESS_1000MS:
            printf("press 1000ms\r\n");
            send_led_state(OFF);
            break;
#if SUPPORT_MULTI_CLICK
        case PRESS_2TIMES:
            printf("press 2 times\r\n");
            send_led_state(BREATH);
            break;
        case PRESS_3TIMES:
            printf("press 3 times\r\n");
            send_led_state(BREATH);
            break;
#endif
        default:
            break;
    }
#endif

}
static task_t task_key = {
    .task = key_task,
    .time_slice = 1,
    .slice_count = 1,
};

/* 创建led任务 */
void led_task(void* param)
{
    analyze_led_state();
}

static task_t task_led = {
    .task = led_task,
    .time_slice = 200,
    .slice_count = 200,
};


int main(void)
{ 
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    uart_init(115200);
    bsTime_Init(999, 83);//1ms中断
    BtnInit(current_time);
    KEY_Init();
    LED_Init();

    add_task(&task_key, 0);
    add_task(&task_led, 0);
    printf("system start...\r\n");
    
    while(1)
    {
        run_task();
    } 
}

