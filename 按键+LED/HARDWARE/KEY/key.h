#ifndef KEY_H
#define KEY_H
#include "stm32f4xx.h"

#define KEY0_GPIO_PORT                      GPIOF
#define KEY0_GPIO_PIN                       GPIO_Pin_6
#define KEY0_GPIO_CLK                       RCC_AHB1Periph_GPIOF

#define KEY1_GPIO_PORT                      GPIOF
#define KEY1_GPIO_PIN                       GPIO_Pin_7
#define KEY1_GPIO_CLK                       RCC_AHB1Periph_GPIOF

#define KEY2_GPIO_PORT                      GPIOF
#define KEY2_GPIO_PIN                       GPIO_Pin_8
#define KEY2_GPIO_CLK                       RCC_AHB1Periph_GPIOF

#define KEY3_GPIO_PORT                      GPIOF
#define KEY3_GPIO_PIN                       GPIO_Pin_9
#define KEY3_GPIO_CLK                       RCC_AHB1Periph_GPIOF

#define KEY0        GPIO_ReadInputDataBit(KEY0_GPIO_PORT, KEY0_GPIO_PIN)
#define KEY1        GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY1_GPIO_PIN)
#define KEY2        GPIO_ReadInputDataBit(KEY2_GPIO_PORT, KEY2_GPIO_PIN)
#define KEY3        GPIO_ReadInputDataBit(KEY3_GPIO_PORT, KEY3_GPIO_PIN)

void KEY_Init(void);
unsigned char KEY_Scan(unsigned char mode);
void key_test(void);

#endif
