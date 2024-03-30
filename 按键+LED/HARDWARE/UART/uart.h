#ifndef UART_H
#define UART_H
#include "stm32f4xx.h"
#include "stdio.h"

/*
��������������:
1. ��DP9��rs232תTTLģ������쳣
2. printf��ӡ��ʱ��û��ѡUse MicroLIB
3. main�������ж����ȼ�����Ҫ�ǵÿ���
4. �����жϲ�����, ֱ������жϱ�־λ��Ȼ������,��֪��ʲôԭ��,���Ƕ����ջ�����Զ�����ж�λ
*/

#define USART1_REC_NUM  			100  	//�����������ֽ��� 200
extern u8 uart_byte_count;          //uart_byte_countҪС��USART_REC_LEN
extern u8 receive_str[USART1_REC_NUM];  
void uart_init(unsigned int bound);
void uart1SendChars(u8 *str, u16 strlen);

#endif
