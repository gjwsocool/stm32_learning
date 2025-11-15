// dma.c
#include "dma.h"
#include "stdio.h"

// 发送缓冲区 
u8 UART1DMA_TxBuff[UART1DMA_TxBuff_SIZE];
// 0, 还没发送完成; 1, 发送完成;
u8 DMA_TcFlag = 0;
// 接收缓冲区
u8 UART1DMA_RxBuff[UART1DMA_RxBuff_SIZE];
// 接收计数 
u32 UART1DMA_RxCounter = 0;

void My_DMA_Init(void){
	// 内存 -> DMA1通道4 -> 串口1(DR)
	// 1.打开DMA1控制器, USART1控制器的时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	// 2.配置DMA1通道4
	DMA_InitTypeDef DMA_Config;
	// 内存配置 
	DMA_Config.DMA_MemoryBaseAddr = (u32)UART1DMA_TxBuff; // 内存首地址
	DMA_Config.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; // 搬移单位
	DMA_Config.DMA_MemoryInc      = DMA_MemoryInc_Enable; // 内存自增
	DMA_Config.DMA_BufferSize     = UART1DMA_TxBuff_SIZE; // 缓冲区大小
	
	// 寄存器配置
	DMA_Config.DMA_PeripheralBaseAddr = (u32)&USART1->DR; // 寄存器首地址
	DMA_Config.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 搬移单位字节
	DMA_Config.DMA_PeripheralInc      = DMA_PeripheralInc_Disable; // 地址不自增
	
	// 其他配置 
	DMA_Config.DMA_DIR  = DMA_DIR_PeripheralDST; // 内存 -> 寄存器
	DMA_Config.DMA_M2M  = DMA_M2M_Disable; // 进制内存到内存
	DMA_Config.DMA_Mode = DMA_Mode_Normal; // 普通模式
	DMA_Config.DMA_Priority = DMA_Priority_High; // 优先级高
	DMA_Init(DMA1_Channel4, &DMA_Config);
	
	// 3.配置使能DMA1通道4发送完成中断 - TC
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
	// 4.配置NVIC支持DMA1通道4的中断 抢0响2
	NVIC_InitTypeDef NVIC_Config;
	NVIC_Config.NVIC_IRQChannel = DMA1_Channel4_IRQn; 
	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Config.NVIC_IRQChannelSubPriority = 2;
	NVIC_Config.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_Config);
	// 5.配置DMA1通道5 串口1的接收 
	// 串口1(DR) -> DMA1通道5 -> 内存(接收缓冲区)
	// 内存配置 
	DMA_Config.DMA_MemoryBaseAddr = (u32)UART1DMA_RxBuff; // 内存首地址
	DMA_Config.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; // 搬移单位
	DMA_Config.DMA_MemoryInc      = DMA_MemoryInc_Enable; // 内存自增
	DMA_Config.DMA_BufferSize     = UART1DMA_RxBuff_SIZE; // 缓冲区大小
	
	// 寄存器配置
	DMA_Config.DMA_PeripheralBaseAddr = (u32)&USART1->DR; // 寄存器首地址
	DMA_Config.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 搬移单位字节
	DMA_Config.DMA_PeripheralInc      = DMA_PeripheralInc_Disable; // 地址不自增
	
	// 其他配置 
	DMA_Config.DMA_DIR  = DMA_DIR_PeripheralSRC; // 寄存器 -> 内存
	DMA_Config.DMA_M2M  = DMA_M2M_Disable; // 禁止内存到内存
	DMA_Config.DMA_Mode = DMA_Mode_Normal; // 普通模式
	DMA_Config.DMA_Priority = DMA_Priority_High; // 优先级高
	DMA_Init(DMA1_Channel5, &DMA_Config);
	
	// 使能DMA1通道5
	DMA_Cmd(DMA1_Channel5, ENABLE); 
	// 使能后该通道自动从串口1的DR中获取数据到内存(接收缓冲区)
}

// 测试函数 
void UART1_DMA_Tx_Test(void){
	// 1.初始化内存缓冲区 
	u32 i = 0;
	for(i = 0; i < UART1DMA_TxBuff_SIZE; i++) 
		UART1DMA_TxBuff[i] = 'A';
	// 2.关闭DMA1通道4
	DMA_Cmd(DMA1_Channel4, DISABLE);
	// 3.配置串口1支持DMA1通道4 - 发送
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	// 4.指定搬移长度
	DMA_SetCurrDataCounter(DMA1_Channel4, UART1DMA_TxBuff_SIZE);
	// 5.打开DMA1通道4
	DMA_Cmd(DMA1_Channel4, ENABLE);
	// 一旦打开DMA1通道4, 该通道自动从内存发送缓冲区中搬移数据
	// 到串口1的DR中, 直到搬移完成
	// 一旦搬移完成后, 触发DMA1通道4的发送完成中断, 调用对应的中断处理函数
	
	// 每次发送前都将发送完成标志清0
	DMA_TcFlag = 0;
	
	// 6.中断判断是否搬移完成
	while(1) {
		// 做其他事情
		if (DMA_TcFlag) { // 搬移完成, 1, 真
			printf("\n DMA Tx Success, Interrupt\n");
			break;
		}
	}
}



