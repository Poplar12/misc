#include "dma.h"

void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 channel,u32 PeripheralAddr,u32 MemoryAddr,u16 BufferSize)
{
    DMA_InitTypeDef  DMA_InitStructure;

    /* 使能DMA时钟 */
    if((u32)DMA_Streamx>(u32)DMA2)//得到当前stream是属于DMA2还是DMA1
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA2时钟使能 
        
    }else 
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);//DMA1时钟使能 
    }

    /* DMA复位 */
    DMA_DeInit(DMA_Streamx);

    /* 等待DMA可配置 */
    while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}

    /* 配置 DMA Stream */
    DMA_InitStructure.DMA_Channel = channel;  //通道选择
    DMA_InitStructure.DMA_PeripheralBaseAddr = PeripheralAddr;//DMA外设地址
    DMA_InitStructure.DMA_Memory0BaseAddr = MemoryAddr;//DMA 存储器0地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;//存储器到外设模式
    DMA_InitStructure.DMA_BufferSize = BufferSize;//数据传输量 
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据长度:8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器数据长度:8位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;// 使用循环模式 
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//中等优先级
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;//循环模式
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
    DMA_Init(DMA_Streamx, &DMA_InitStructure);//初始化DMA Stream
        
} 