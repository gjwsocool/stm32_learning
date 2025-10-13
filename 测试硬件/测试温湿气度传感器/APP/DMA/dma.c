#include "dma.h"
#include "systick.h"
#include "led.h"
#include "stdio.h"

//定义UART1 DMA发送数据的内存和发送数据计数
u32 UART1DMA_TxCounter = 0;
u8 UART1DMA_TxBuff[UART1DMA_TXBUFF_SIZE];
u32	DMA_TcFlags = 0;

//定义UART1 DMA接收数据的内存和接收数据计数
u8	UART1DMA_RxBuff[UART1DMA_RXBUFF_SIZE];
u32 UART1DMA_RxCounter = 0;

//定义UART1对应的DMA通道初始化函数
void UART1DMA_Init(void){
	DMA_InitTypeDef	DMA_Config;
	
	//使能DMA1时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	//配置DMA1的通道4为UART1的发送TX
	DMA_Config.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);							//指定DMA搬移对应的UART1数据寄存器的基地址
	DMA_Config.DMA_MemoryBaseAddr	= (u32)UART1DMA_TxBuff;								//指定DMA搬移对应的内存基地址
	DMA_Config.DMA_DIR = DMA_DIR_PeripheralDST;													//指定DMA搬移方向：内存->UART1数据寄存器
	DMA_Config.DMA_BufferSize = UART1DMA_TXBUFF_SIZE;										//指定DMA缓冲区的大小
	DMA_Config.DMA_PeripheralInc = DMA_PeripheralInc_Disable;						//指定UART1数据寄存器地址不增加
	DMA_Config.DMA_MemoryInc = DMA_MemoryInc_Enable;										//指定存储数据的内存地址增加
	DMA_Config.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;		//指定UART1数据寄存器数据宽度8位
	DMA_Config.DMA_MemoryDataSize	= DMA_MemoryDataSize_Byte;						//指定存储数据的内存数据宽度为8位
	DMA_Config.DMA_Mode = DMA_Mode_Normal;															//指定DMA的工作模式为正常模式
	DMA_Config.DMA_Priority = DMA_Priority_Medium;											//指定DMA1通道4的优先级为中等优先级
	DMA_Config.DMA_M2M = DMA_M2M_Disable;																//指定DMA不是内存到内存的传输方式
	DMA_Init(DMA1_Channel4, &DMA_Config);																//配置DMA1的通道4为UART1
	
	//使能DMA1通道4发送完成中断
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
	
	//配置NVIC的DMA1中断
	NVIC_InitTypeDef	NVIC_Config;
	NVIC_Config.NVIC_IRQChannel = DMA1_Channel4_IRQn;										//指定DMA1的通道4作为NVIC的输入中断信号
	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 0;									//指定抢占式优先级
	NVIC_Config.NVIC_IRQChannelSubPriority = 0;													//指定子优先级
	NVIC_Config.NVIC_IRQChannelCmd = ENABLE;														//使能NVIC的DMA1中断信号
	NVIC_Init(&NVIC_Config);
	
	//配置DMA1通道5为串口1的接收
	DMA_Config.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);							//指定UART1数据寄存器基地址
	DMA_Config.DMA_MemoryBaseAddr = (u32)UART1DMA_RxBuff;								//指定保存接收数据的内存首地址
	DMA_Config.DMA_DIR = DMA_DIR_PeripheralSRC;													//指定DMA搬移的方向：寄存器->内存
	DMA_Config.DMA_BufferSize = UART1DMA_RXBUFF_SIZE;										//指定DMA搬移数据的大小
	DMA_Config.DMA_PeripheralInc = DMA_PeripheralInc_Disable;						//数据寄存器地址不自增
	DMA_Config.DMA_MemoryInc = DMA_MemoryInc_Enable;										//内存地址自增
	DMA_Config.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;		//指定数据寄存器的宽度为8位
	DMA_Config.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;						//指定内存的数据宽度为8位
	DMA_Config.DMA_Mode = DMA_Mode_Normal;															//正常工作模式
	DMA_Config.DMA_Priority = DMA_Priority_High;												//拥有高优先级
	DMA_Config.DMA_M2M = DMA_M2M_Disable;																//禁止内存到内存搬移功能
	DMA_Init(DMA1_Channel5, &DMA_Config);																//初始化DMA1的通道5
	DMA_Cmd(DMA1_Channel5, ENABLE);																			//使能DMA1通道5
}

//定义UART1通过DMA1的通道4发送数据
void UART1DMA_Tx_Test(void) {
	//初始化DMA搬移的内存，构造要发送的测试数据
	int i;
	for(i = 0; i < UART1DMA_TXBUFF_SIZE; i++) 
		UART1DMA_TxBuff[i] = 'A';
	
	//先关闭DMA1通道4,指定要发送的数据长度,P144
	DMA_Cmd(DMA1_Channel4, DISABLE);
	//使能UART1的DMA1发送数据功能
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	//指定要发送的数据长度
	DMA_SetCurrDataCounter(DMA1_Channel4, UART1DMA_TXBUFF_SIZE);
	//启动DMA传输
	DMA_Cmd(DMA1_Channel4, ENABLE);
	//一开始标记还没有传输数据，即将开始
	DMA_TcFlags = 0;
	
	//采用轮询方式判断DMA1通道4是否传输完成
	while(1) {
		if(DMA_TcFlags) {
			printf("UART1 DMA TX DONE!\n");
			break;
		}
		LED1 = !LED1;
		delay_ms(50);
	}
}
