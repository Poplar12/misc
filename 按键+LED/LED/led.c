#include "led.h"
#include "que.h"
#include "./UART/uart.h"
#include "LED/bsp_led.h"

/* 
    1. ��LED��״̬�Ͷ�Ӧ����ʾ����һ��, �����õ�״̬��, �Ϳ��Խ�����ʾ��.
    2. ʹ��workmode_t�ṹ������������led�Ĺ���״̬, �������δ����Ʒ��Ҫ����, 
       �Ϳ��Խ�����ṹ���ó�ȥ, �������ߵ�ʱ�����״̬.(�����ʱ���ں���, 
       ����ʱ�豸Ҫ��������, ������״̬�Ѿ�����λ��, �´λ���״̬��������
       ǰ��״̬)
*/

/* ��¼��ǰ��ִ����� */
typedef struct {
    ledsta_t cur_sta;               /* ��ǰ����״̬ */
    unsigned int enable;            /* ÿһbit��Ӧһ��״̬ʹ�� */
    unsigned char lock;             /* ��ֹ״̬���, ��Щ��Ч�����ܴ�� */
}workmode_t;

/* LED�¼��� */
struct led_evt{
    ledsta_t sta;
    void(*event)(workmode_t* workmode);
};

/* ����״̬������� */
creat_que(led_que, 10);

static workmode_t workmode = {NONE, 0, 0}; 

static void led_off(workmode_t* workmode)
{
    LED0(0);
    workmode->enable &= ~(1 << OFF);    /* ִ������������¼�ʹ�� */
}
static void led_on(workmode_t* workmode)
{
    LED0(1);
    workmode->enable &= ~(1 << ON);
}
static void led_3times(workmode_t* workmode)
{
    static int time = 0;
    static int led = 0;

    led = led ? 0 : 1;
    LED0(led);

    time++;
    workmode->lock = 1;

    /* ����6��, ��3�κ�ر��¼� */
    if(time > 6)
    {
        LED0(0);
        time = 0;
        workmode->lock = 0;
        workmode->enable &= ~(1 << ON_3Times);
    }
}
static void led_breath(workmode_t* workmode)
{
    static char led = 0;

    //�����¼�����һֱִ��, ���������¼�����, ���رո��¼�.
    if(workmode->cur_sta > NONE && workmode->cur_sta < BREATH)
    {
        led = 0;
        workmode->enable &= ~(1 << BREATH);
        return;
    }
 
    led = led ? 0 : 1;
    LED0(led);
}

static const struct led_evt evt_group[] = {
    {BREATH, led_breath},
    {ON_3Times, led_3times},
    {OFF, led_off},
    {ON, led_on},
};

/* �ⲿ����led��״̬ */
void send_led_state(ledsta_t sta)
{
    InQue(led_que, sta);
}
/* ����led��״̬��ʾ */
void analyze_led_state()
{
    int i, j;

    /* ��ȡled��״̬ */
    workmode.cur_sta = NONE;
    if(!workmode.lock && OutQue(led_que, (unsigned char*)&workmode.cur_sta))
    {
        for(i = 0; i < sizeof(evt_group); i++)
        {
            if(workmode.cur_sta == evt_group[i].sta)
            {
                workmode.enable |= 1 << evt_group[i].sta;    /* ʹ�ܸ��¼� */
                workmode.cur_sta = evt_group[i].sta;
                break;
            }
        }
    }

    /* ִ��ʹ�ܵ��¼� */
    for(i = 0; i < LED_MODE_MAX; i++)
    {
        if(workmode.enable & (1 << i))                  /* ���¼���ʹ���� */
        {
            for(j = 0; j < sizeof(evt_group);j++)
            {
                if((1 << i) == (1 << evt_group[j].sta)) /* ���ұ��б�ʹ�ܵ��¼� */
                {
                    evt_group[j].event(&workmode);
                    break;
                }
            }
        }
    }
}

