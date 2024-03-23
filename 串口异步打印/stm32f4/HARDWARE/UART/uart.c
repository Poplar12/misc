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
    static char buff[BUF_SIZE / 2]; /* 如果是裸机可以使用临时变量 */
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

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟 
    
    //串口1对应引脚复用映射
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);  //GPIOA9复用为USART1
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1
    
    //USART1端口配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      //复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   //上拉
    GPIO_Init(GPIOA,&GPIO_InitStructure);          //初始化PA9，PA10
    
    //USART1 初始化设置
    USART_InitStructure.USART_BaudRate = bound;//波特率设置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;  //一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART1, &USART_InitStructure); //初始化串口1	
    USART_Cmd(USART1, ENABLE);  //使能串口1 

    USART_ClearFlag(USART1, USART_FLAG_TC);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);         //开启相关中断
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;      //串口1中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		   //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			   //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	  //根据指定的参数初始化VIC寄存器、
}

void USART1_IRQHandler(void)
{
    unsigned char data;
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        InQue(RX_buf, USART_ReceiveData(USART1));           /*将数据放入接收缓冲区*/             
    }
    if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET) {
        if (OutQue(TX_buf, &data))      /*从缓冲区中取出数据---*/
            USART_SendData(USART1, data);            
        else{
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);    
        }
    }
} 
