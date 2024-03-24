#ifndef UART_H
#define UART_H
#include "stm32f4xx.h"
/*
��������������:
1. ��DP9��rs232תTTLģ������쳣
2. printf��ӡ��ʱ��û��ѡUse MicroLIB
3. main�������ж����ȼ�����Ҫ�ǵÿ���
4. �����жϲ�����, ֱ������жϱ�־λ��Ȼ������,��֪��ʲôԭ��,���Ƕ����ջ�����Զ�����ж�λ
*/

typedef int(*uart_send)(unsigned char* data); 
void uart_init(unsigned int bound);
void set_send_cb(uart_send cb);
void start_transmit(void);

#endif
