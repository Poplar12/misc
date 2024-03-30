#include "led.h"
#include "que.h"
#include "./UART/uart.h"
#include "LED/bsp_led.h"

creat_que(led_que, 10);



struct led_evt{
    ledsta_t sta;
    unsigned char enable;   /* Ϊ������Щ��Ч��Ҫѭ��ִ��, ����Щ��Чֻ��Ҫִ��һ�� */
    void(*event)(unsigned char* enable, ledsta_t sta);
};

static void led_off(unsigned char* enable, ledsta_t sta)
{
    LED0(0);
    *enable = NONE;
}
static void led_on(unsigned char* enable, ledsta_t sta)
{
    LED0(1);
    *enable = NONE;
}
static void led_3times(unsigned char* enable, ledsta_t sta)
{
    static int time = 0;
    static int led = 0;
    led = led ? 0 : 1;
    LED0(led);
    time++;
    if(time > 6)
    {
        LED0(0);
        time = 0;
        *enable = NONE;
    }
}
static void led_breath(unsigned char* enable, ledsta_t sta)
{
    static char led = 0;
    if(sta > NONE && sta < BREATH)
    {
        led = 0;
        *enable = NONE;
        return;
    }
 
    led = led ? 0 : 1;
    LED0(led);
}
static struct led_evt evt_group[] = {
    {OFF, 0, led_off},
    {ON, 0, led_on},
    {ON_3Times, 0, led_3times},
    {BREATH, 0, led_breath},
};


/* �ⲿ����led��״̬ */
void send_led_state(ledsta_t sta)
{
    InQue(led_que, sta);
}
/* ����led��״̬��ʾ */
void analyze_led_state()
{
    int i;
    ledsta_t sta = NONE;

    /* ��ȡled��״̬ */
    if(OutQue(led_que, (unsigned char*)&sta))
    {
        for(i = 0; i < sizeof(evt_group); i++)
        {
            if(sta == evt_group[i].sta)
            {
                evt_group[i].enable = 1;    /* ���ҵ���Ӧ��״̬, ��ʹ�ܸ��¼� */
                break;
            }
        }
    }

    /* ִ��ʹ�ܵ��¼� */
    for(i = 0; i < sizeof(evt_group); i++)
    {
        if(evt_group[i].enable)
        {
            evt_group[i].event(&evt_group[i].enable, sta);
        }
    }

}

