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

    RCC_APB1PeriphClockCmd(BASE_TIMER_CLK,ENABLE);  ///ʹ��TIM6ʱ��

    TIM_TimeBaseInitStructure.TIM_Period = arr; //�Զ���װ��ֵ
    TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV4; 

    TIM_TimeBaseInit(BASE_TIMER,&TIM_TimeBaseInitStructure);//��ʼ��TIM6

    TIM_ITConfig(BASE_TIMER,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
    TIM_Cmd(BASE_TIMER,ENABLE); //ʹ�ܶ�ʱ��3

    NVIC_InitStructure.NVIC_IRQChannel=BASE_TIME_IRQ; //��ʱ��6�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //��ռ���ȼ�1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


void BASE_TIME_IRQHandler(void)
{
    if(TIM_GetITStatus(BASE_TIMER,TIM_IT_Update)==SET) //����ж�
    {
        time_cb();
        TIM_ClearITPendingBit(BASE_TIMER,TIM_IT_Update);  //����жϱ�־λ
    }

}
