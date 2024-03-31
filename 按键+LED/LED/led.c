#include "led.h"
#include "que.h"
#include "./UART/uart.h"
#include "LED/bsp_led.h"

/* 
    1. 将LED的状态和对应的显示绑定在一起, 这样拿到状态后, 就可以进行显示了.
    2. 使用workmode_t结构体来管理整个led的工作状态, 这样如果未来产品需要休眠, 
       就可以将这个结构体拿出去, 便于休眠的时候清除状态.(比如此时灯在呼吸, 
       而此时设备要进入休眠, 而呼吸状态已经被置位了, 下次唤醒状态保存休眠
       前的状态)
*/

/* 记录当前灯执行情况 */
typedef struct {
    ledsta_t cur_sta;               /* 当前工作状态 */
    unsigned int enable;            /* 每一bit对应一个状态使能 */
    unsigned char lock;             /* 防止状态打架, 有些灯效果不能打断 */
}workmode_t;

/* LED事件表 */
struct led_evt{
    ledsta_t sta;
    void(*event)(workmode_t* workmode);
};

/* 创建状态缓存队列 */
creat_que(led_que, 10);

static workmode_t workmode = {NONE, 0, 0}; 

static void led_off(workmode_t* workmode)
{
    LED0(0);
    workmode->enable &= ~(1 << OFF);    /* 执行完立即清除事件使能 */
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

    /* 工作6次, 亮3次后关闭事件 */
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

    //呼吸事件触发一直执行, 当有其他事件发送, 将关闭该事件.
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

/* 外部发送led的状态 */
void send_led_state(ledsta_t sta)
{
    InQue(led_que, sta);
}
/* 处理led的状态显示 */
void analyze_led_state()
{
    int i, j;

    /* 获取led的状态 */
    workmode.cur_sta = NONE;
    if(!workmode.lock && OutQue(led_que, (unsigned char*)&workmode.cur_sta))
    {
        for(i = 0; i < sizeof(evt_group); i++)
        {
            if(workmode.cur_sta == evt_group[i].sta)
            {
                workmode.enable |= 1 << evt_group[i].sta;    /* 使能该事件 */
                workmode.cur_sta = evt_group[i].sta;
                break;
            }
        }
    }

    /* 执行使能的事件 */
    for(i = 0; i < LED_MODE_MAX; i++)
    {
        if(workmode.enable & (1 << i))                  /* 有事件被使能了 */
        {
            for(j = 0; j < sizeof(evt_group);j++)
            {
                if((1 << i) == (1 << evt_group[j].sta)) /* 查找表中被使能的事件 */
                {
                    evt_group[j].event(&workmode);
                    break;
                }
            }
        }
    }
}

