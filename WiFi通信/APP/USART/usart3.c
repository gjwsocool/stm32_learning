// usart3.c
#include "usart3.h"
#include "stdio.h"
#include "stdarg.h" // va_list va_start va_end  

u8 UART3_TxBuff[UART3_TXBUFF_SIZE];
u8 UART3_RxBuff[UART3_RXBUFF_SIZE];
// 接收数据计数
u32 UART3_RxCounter = 0; 

void UART3_Init(void){
	// 1.打开串口3(APB1), GPIOB(APB2)控制器时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	// 2.配置PB10 - USART3 TX - 推挽复用输出, 50MHz
	GPIO_InitTypeDef GPIO_Config;
	GPIO_Config.GPIO_Pin = GPIO_Pin_10;
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Config.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_Config);
	
	// 3.配置PB11 - USART3 RX - 浮空输入
	GPIO_Config.GPIO_Pin = GPIO_Pin_11;
	GPIO_Config.GPIO_Mode  = GPIO_Mode_IN_FLOATING; 
	GPIO_Init(GPIOB, &GPIO_Config);
	
	// 4.配置串口3 - 
	//		115200 数据位8位,不校验,停止位1位,发送/接收模式,不要硬件流控制
	USART_InitTypeDef UART_Config;
	UART_Config.USART_BaudRate = 115200;
	UART_Config.USART_WordLength = USART_WordLength_8b;
	UART_Config.USART_Parity = USART_Parity_No;
	UART_Config.USART_StopBits = USART_StopBits_1;
	UART_Config.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	UART_Config.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART3, &UART_Config);
	
	// 5.使能串口3
	USART_Cmd(USART3, ENABLE);
	
	// 6.配置串口3支持IDLE中断
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
	
	// 7.配置NVIC支持串口3中断
	NVIC_InitTypeDef NVIC_Config;
	NVIC_Config.NVIC_IRQChannel = USART3_IRQn; 
	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Config.NVIC_IRQChannelSubPriority = 1;
	NVIC_Config.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_Config);
	
	// 8.配置串口3支持DMA的发送和接收模式
	USART_DMACmd(USART3, USART_DMAReq_Tx | USART_DMAReq_Rx, ENABLE);
	
	// 9.初始化DMA相关内容
	UART3DMA_Init();
}
void UART3DMA_Init(void){
	// 1.打开DMA1控制器的时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	// 2.配置DMA1通道2作为串口3的发送
	// 		内存(发送缓冲区) -> 寄存器(串口3的DR)
	DMA_InitTypeDef DMA_Config;
	// 内存配置 
	DMA_Config.DMA_MemoryBaseAddr = (u32)UART3_TxBuff; // 内存首地址
	DMA_Config.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; // 搬移单位
	DMA_Config.DMA_MemoryInc      = DMA_MemoryInc_Enable; // 内存自增
	DMA_Config.DMA_BufferSize     = UART3_TXBUFF_SIZE; // 缓冲区大小
	
	// 寄存器配置
	DMA_Config.DMA_PeripheralBaseAddr = (u32)&USART3->DR;
	DMA_Config.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 搬移单位字节
	DMA_Config.DMA_PeripheralInc      = DMA_PeripheralInc_Disable; // 地址不自增
	
	// 其他配置 
	DMA_Config.DMA_DIR  = DMA_DIR_PeripheralDST; // 内存 -> 寄存器
	DMA_Config.DMA_M2M  = DMA_M2M_Disable; // 禁止内存到内存
	DMA_Config.DMA_Mode = DMA_Mode_Normal; // 普通模式
	DMA_Config.DMA_Priority = DMA_Priority_High; // 优先级高
	DMA_Init(DMA1_Channel2, &DMA_Config);
	
	// 3.配置DMA1通道3作为串口3的接收
	// 		寄存器(串口3的DR) -> 内存(接收缓冲区) 
	// 内存配置 
	DMA_Config.DMA_MemoryBaseAddr = (u32)UART3_RxBuff; // 内存首地址
	DMA_Config.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; // 搬移单位
	DMA_Config.DMA_MemoryInc      = DMA_MemoryInc_Enable; // 内存自增
	DMA_Config.DMA_BufferSize     = UART3_RXBUFF_SIZE; // 缓冲区大小
	
	// 寄存器配置
	DMA_Config.DMA_PeripheralBaseAddr = (u32)&USART3->DR;	// 寄存器地址
	DMA_Config.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 搬移单位字节
	DMA_Config.DMA_PeripheralInc      = DMA_PeripheralInc_Disable; // 地址不自增
	
	// 其他配置 
	DMA_Config.DMA_DIR  = DMA_DIR_PeripheralSRC; // 寄存器 -> 内存 
	DMA_Config.DMA_M2M  = DMA_M2M_Disable; // 禁止内存到内存
	DMA_Config.DMA_Mode = DMA_Mode_Normal; // 普通模式
	DMA_Config.DMA_Priority = DMA_Priority_High; // 优先级高
	DMA_Init(DMA1_Channel3, &DMA_Config);
	
	// 4.启动DMA1通道3
	DMA_Cmd(DMA1_Channel3, ENABLE);
	
	// 5.配置DMA1通道2打开TC中断
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);
	
	// 6.配置NVIC支持DMA1通道2的中断
	NVIC_InitTypeDef NVIC_Config;
	NVIC_Config.NVIC_IRQChannel = DMA1_Channel2_IRQn; // DMA1通道2触发的中断
	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Config.NVIC_IRQChannelSubPriority = 1;
	NVIC_Config.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_Config);
	
}
// 构建要发送的数据, 放入到发送缓冲区中
// 打开DMA1通道2 -> 该通道自动从发送缓冲区中搬运数据到串口3的DR -> WiFi模块
void UART3_Puts(char* format, ...){
	va_list ap;
	va_start(ap, format);
	vsprintf(UART3_TxBuff, format, ap);
	va_end(ap);
	
	// 关闭DMA1通道2
	DMA_Cmd(DMA1_Channel2, DISABLE);
	// 指定搬移长度 
	DMA_SetCurrDataCounter(DMA1_Channel2, UART3_TXBUFF_SIZE);
	// 打开DMA1通道2
	DMA_Cmd(DMA1_Channel2, ENABLE);
}


