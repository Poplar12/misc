#include "./LED/bsp_led.h"

void LED_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(LED0_GPIO_CLK, ENABLE);
    RCC_AHB1PeriphClockCmd(LED1_GPIO_CLK, ENABLE);
    RCC_AHB1PeriphClockCmd(LED2_GPIO_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Pin = LED0_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(LED0_GPIO_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = LED1_GPIO_PIN;
    GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = LED2_GPIO_PIN;
    GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStructure);
    
    LED0(0);
    LED1(0);
    LED2(0);
}

void led0_on()
{
    LED0(1);
}
void led1_on()
{
    LED1(1);
}
void led2_on()
{
    LED2(1);
}
void led0_off()
{
    LED0(0);
}
void led1_off()
{
    LED1(0);
}
void led2_off()
{
    LED2(0);
}

//void dev_led_init()
//{
//    
//    const led_t led0 = {NULL, led0_on, led0_off};
//    const led_t led1 = {NULL, led1_on, led1_off};
//    const led_t led2 = {LED_Init, led2_on, led2_off};
//    
//    reg_dev(LED0, (void*)&led0);
//    reg_dev(LED1, (void*)&led1);
//    reg_dev(LED2, (void*)&led2);
//    
//}



