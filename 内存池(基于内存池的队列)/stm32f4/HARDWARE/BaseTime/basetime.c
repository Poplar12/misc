#include "./BaseTime/basetime.h"
#include "os.h"
#define BASE_TIMER              TIM3
#define BASE_TIMER_CLK          RCC_APB1Periph_TIM3
#define BASE_TIME_IRQ           TIM3_IRQn
#define BASE_TIME_IRQHandler    TIM3_IRQHandler

void bsTime_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(BASE_TIMER_CLK,ENABLE);  ///使能TIM6时钟

    TIM_TimeBaseInitStructure.TIM_Period = arr; //自动重装载值
    TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV4; 

    TIM_TimeBaseInit(BASE_TIMER,&TIM_TimeBaseInitStructure);//初始化TIM6

    TIM_ITConfig(BASE_TIMER,TIM_IT_Update,ENABLE); //允许定时器3更新中断
    TIM_Cmd(BASE_TIMER,ENABLE); //使能定时器3

    NVIC_InitStructure.NVIC_IRQChannel=BASE_TIME_IRQ; //定时器6中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


void BASE_TIME_IRQHandler(void)
{
    if(TIM_GetITStatus(BASE_TIMER,TIM_IT_Update)==SET) //溢出中断
    {
        time_cb();
        TIM_ClearITPendingBit(BASE_TIMER,TIM_IT_Update);  //清除中断标志位
    }

}
