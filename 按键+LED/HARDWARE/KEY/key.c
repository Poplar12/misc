#include "key.h"
#include "delay.h"

void KEY_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(KEY0_GPIO_CLK, ENABLE);
    RCC_AHB1PeriphClockCmd(KEY1_GPIO_CLK, ENABLE);
    RCC_AHB1PeriphClockCmd(KEY2_GPIO_CLK, ENABLE);
    RCC_AHB1PeriphClockCmd(KEY3_GPIO_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Pin = KEY0_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(KEY0_GPIO_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN;
    GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN;
    GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = KEY3_GPIO_PIN;
    GPIO_Init(KEY3_GPIO_PORT, &GPIO_InitStructure);
}



