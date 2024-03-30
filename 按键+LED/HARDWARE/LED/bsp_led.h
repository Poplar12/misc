#ifndef __BSP_LED_H
#define __BSP_LED_H
#include "stm32f4xx.h"

#define LED0_GPIO_PORT                      GPIOE
#define LED0_GPIO_PIN                       GPIO_Pin_3
#define LED0_GPIO_CLK                       RCC_AHB1Periph_GPIOE

#define LED1_GPIO_PORT                      GPIOE
#define LED1_GPIO_PIN                       GPIO_Pin_4
#define LED1_GPIO_CLK                       RCC_AHB1Periph_GPIOE

#define LED2_GPIO_PORT                      GPIOG
#define LED2_GPIO_PIN                       GPIO_Pin_9
#define LED2_GPIO_CLK                       RCC_AHB1Periph_GPIOG

#define LED0(x) do{ \
    (x)? \
    GPIO_ResetBits(LED0_GPIO_PORT, LED0_GPIO_PIN) \
    : GPIO_SetBits(LED0_GPIO_PORT, LED0_GPIO_PIN); \
}while(0)

#define LED1(x) do{ \
    (x)? \
    GPIO_ResetBits(LED1_GPIO_PORT, LED1_GPIO_PIN) \
    : GPIO_SetBits(LED1_GPIO_PORT, LED1_GPIO_PIN); \
}while(0)

#define LED2(x) do{ \
    (x)? \
    GPIO_ResetBits(LED2_GPIO_PORT, LED2_GPIO_PIN) \
    : GPIO_SetBits(LED2_GPIO_PORT, LED2_GPIO_PIN); \
}while(0)

void LED_Init(void);
void dev_led_init(void);
#endif
