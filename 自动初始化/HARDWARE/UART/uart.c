#include "./UART/uart.h"

unsigned char receive_str[USART1_REC_NUM];     //���ջ�������,���USART_REC_LEN���ֽ� 
unsigned char uart_byte_count=0;

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
void uart1SendChar(u8 ch)
{      
    while((USART1->SR&0x40)==0);  
    USART1->DR = (u8) ch;  
}
u8 arr[200], i = 0;
u16 end = 0;;

void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж� 
    {
        //USART_SendData(USART1, rec_data);
        arr[i] = USART_ReceiveData(USART1);
        i ++;
    } 
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        end = USART_ReceiveData(USART1);
        end = 0xFFFF;
        arr[i] = 0;
        i = 0;
    }
} 
int fputc(int ch, FILE *f)   //�ض�����printf���������  
{
    USART_SendData(USART1, (u8)ch);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    return ch;
}
void uart1SendChars(u8 *str, u16 strlen)
{ 
    u16 k= 0 ; 
    do { uart1SendChar(*(str + k)); k++; }   //ѭ������,ֱ���������   
    while (k < strlen); 
} 


