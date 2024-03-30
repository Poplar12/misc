#ifndef UART_H
#define UART_H
#include "stm32f4xx.h"
#include "stdio.h"

/*
调试遇到的问题:
1. 接DP9的rs232转TTL模块输出异常
2. printf打印的时候没勾选Use MicroLIB
3. main函数的中断优先级分组要记得开启
4. 接收中断产生后, 直接清除中断标志位居然有问题,不知道什么原因,但是读接收缓冲会自动清除中断位
*/

#define USART1_REC_NUM  			100  	//定义最大接收字节数 200
extern u8 uart_byte_count;          //uart_byte_count要小于USART_REC_LEN
extern u8 receive_str[USART1_REC_NUM];  
void uart_init(unsigned int bound);
void uart1SendChars(u8 *str, u16 strlen);

#endif
