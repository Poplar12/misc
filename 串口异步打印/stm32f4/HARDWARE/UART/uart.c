#include "./UART/uart.h"
#include "stdio.h"
#include "que.h"
#include "uart.h"
#include "stdarg.h"

#define BUF_SIZE   100
creat_que(TX_buf, BUF_SIZE);
creat_que(RX_buf, BUF_SIZE);

static void add_str(char *buf, unsigned int len)
{
    int i;

    for(i = 0; i < len; i++) {
        InQue(TX_buf, *(buf + i));
    }
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}

int my_printf(const char *fmt, ...)
{
    va_list argptr;
    static char buff[BUF_SIZE / 2]; /* ������������ʹ����ʱ���� */
    int len = 0;

    va_start(argptr, fmt);
    len = vsnprintf(buff, BUF_SIZE / 2, fmt, argptr);
    va_end(argptr);

    add_str(buff, len);
    return len;
}
void uart_init(unsigned int bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ�� 
    
    //����1��Ӧ���Ÿ���ӳ��
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);  //GPIOA9����ΪUSART1
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10����ΪUSART1
    
    //USART1�˿�����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      //���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   //����
    GPIO_Init(GPIOA,&GPIO_InitStructure);          //��ʼ��PA9��PA10
    
    //USART1 ��ʼ������
    USART_InitStructure.USART_BaudRate = bound;//����������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;  //һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_Init(USART1, &USART_InitStructure); //��ʼ������1	
    USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 

    USART_ClearFlag(USART1, USART_FLAG_TC);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);         //��������ж�
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
    //Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;      //����1�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		   //�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			   //IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	  //����ָ���Ĳ�����ʼ��VIC�Ĵ�����
}

void USART1_IRQHandler(void)
{
    unsigned char data;
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        InQue(RX_buf, USART_ReceiveData(USART1));           /*�����ݷ�����ջ�����*/             
    }
    if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET) {
        if (OutQue(TX_buf, &data))      /*�ӻ�������ȡ������---*/
            USART_SendData(USART1, data);            
        else{
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);    
        }
    }
} 
