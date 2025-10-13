#include "uart3.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"

unsigned int UART3_RxCounter = 0; //记录串口3接收数据个数
char UART3_RxBuff[UART3_RXBUFF_SIZE];	//定义串口3接收缓冲区
char UART3_TxBuff[UART3_TXBUFF_SIZE];	//定义串口3发送缓冲区

//定义串口3初始化函数
void UART3_Init(void) {
	GPIO_InitTypeDef	GPIO_Config;
	USART_InitTypeDef UART_Config;
	NVIC_InitTypeDef	NVIC_Config;
	
	//指定UART3初始值
	USART_DeInit(USART3);
	
	//使能USART3时钟和GPIOB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	//指定GPIOB10为UART3的发送TX功能
	GPIO_Config.GPIO_Pin = GPIO_Pin_10;					//GPIOB10
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;	//指定最大输出速度
	GPIO_Config.GPIO_Mode = GPIO_Mode_AF_PP;		//指定为复用推挽输出，用于UART3的发送功能,GPIOB10->UART3TX
	GPIO_Init(GPIOB, &GPIO_Config);
	
	//指定GPIOB11为UART3的接收RX功能
	GPIO_Config.GPIO_Pin = GPIO_Pin_11;							//GPIOB11
	GPIO_Config.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//指定为输入浮空，用于UART3的接收功能，GPIOB11->UART3RX
	GPIO_Init(GPIOB, &GPIO_Config);
	
	//配置UART3工作参数
	UART_Config.USART_BaudRate = 115200;						//指定UART3的波特率
	UART_Config.USART_WordLength = USART_WordLength_8b;	//数据位为8位
	UART_Config.USART_StopBits = USART_StopBits_1;		//停止位为1位
	UART_Config.USART_Parity = USART_Parity_No;				//无奇偶校验
	UART_Config.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件流控制
	UART_Config.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	//指定可发可收模式
	USART_Init(USART3, &UART_Config);
	USART_Cmd(USART3, ENABLE);
	
	//启动UART3空闲中断
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);		//开启空闲中断
	
	//启动NVIC的UART3中断
	NVIC_Config.NVIC_IRQChannel = USART3_IRQn;			//启用UART3中断
	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 0;	//指定抢占式优先级
	NVIC_Config.NVIC_IRQChannelSubPriority = 0;			//指定响应式优先级
	NVIC_Config.NVIC_IRQChannelCmd = ENABLE;		//使能UART3中断
	NVIC_Init(&NVIC_Config);
	
	//开启UART3的DMA接收
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
	//开启UART3的DMA发送
	USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);
	
	//初始化UART3的DMA功能
	UART3DMA_Init();
}

//初始化UART3的DMA功能
void UART3DMA_Init(void) {
	DMA_InitTypeDef	DMA_Config;
	NVIC_InitTypeDef NVIC_Config;
	
	//使能DMA1通道时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	//配置DMA1的通道3为串口3的接收 DMA1.Channel3->UART3RX
	DMA_Config.DMA_PeripheralBaseAddr = (unsigned int)(&USART3->DR);	//指定外设寄存器地址
	DMA_Config.DMA_MemoryBaseAddr = (unsigned int)UART3_RxBuff;				//指定内存起始地址
	DMA_Config.DMA_DIR = DMA_DIR_PeripheralSRC;	//从外设搬移到内存
	DMA_Config.DMA_BufferSize = UART3_RXBUFF_SIZE; //指定UART3接收缓冲区大小
	DMA_Config.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //外设地址不自增
	DMA_Config.DMA_MemoryInc = DMA_MemoryInc_Enable;	//内存地址递增
	DMA_Config.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //指定外设数据宽度为8位
	DMA_Config.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//指定内存数据宽度为8位
	DMA_Config.DMA_Mode = DMA_Mode_Normal;	//配置为正常工作模式
	DMA_Config.DMA_Priority = DMA_Priority_High;	//配置为高优先级
	DMA_Config.DMA_M2M = DMA_M2M_Disable;	//禁止内存到内存的搬移模式
	DMA_Init(DMA1_Channel3, &DMA_Config);
	
	//配置DMA1的通道2为串口2的发送 DMA1.Channel2->UART3TX
	DMA_Config.DMA_PeripheralBaseAddr = (unsigned int)(&USART3->DR);	//指定外设寄存器地址
	DMA_Config.DMA_MemoryBaseAddr = (unsigned int)UART3_TxBuff;				//指定内存起始地址
	DMA_Config.DMA_DIR = DMA_DIR_PeripheralDST;	//从外设搬移到内存
	DMA_Config.DMA_BufferSize = UART3_TXBUFF_SIZE; //指定UART3接收缓冲区大小
	DMA_Config.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //外设地址不自增
	DMA_Config.DMA_MemoryInc = DMA_MemoryInc_Enable;	//内存地址递增
	DMA_Config.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //指定外设数据宽度为8位
	DMA_Config.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//指定内存数据宽度为8位
	DMA_Config.DMA_Mode = DMA_Mode_Normal;	//配置为正常工作模式
	DMA_Config.DMA_Priority = DMA_Priority_Medium;	//配置为中等优先级
	DMA_Config.DMA_M2M = DMA_M2M_Disable;	//禁止内存到内存的搬移模式
	DMA_Init(DMA1_Channel2, &DMA_Config);
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);	//启动DMA1的通道2发送完成中断
	
	//初始化NVIC的DMA1的通道2中断
	NVIC_Config.NVIC_IRQChannel = DMA1_Channel1_IRQn;	//启动DMA1通道2中断
	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Config.NVIC_IRQChannelSubPriority = 0;
	NVIC_Config.NVIC_IRQChannelCmd = ENABLE;	//使能NVIC的DMA1通道2中断
	NVIC_Init(&NVIC_Config);
}

//定义UART3发送数据函数
void UART3_Puts(char *fmt, ...) {
	//本质：char *ap
	va_list ap;
	//让ap指向第一个参数fmt后面的参数列表
	va_start(ap, fmt);
	//利用vsprintf将ap指向的参数列表按照fmt格式化输出到UART3_TxBuff数组中
	vsprintf(UART3_TxBuff, fmt, ap);
	//让ap=NULL
	va_end(ap);
	
	/*
	//没有利用DMA将数据发送
	unsigned int i, lenght;
	lenght = strlen(UART3_TxBuff);
	for(i = 0; i < lenght; i++) {
		USART_SendData(USART3, UART3_TxBuff[i]);
		while((USART_GetFlagStatus(USART3, USART_FLAG_TC)) == RESET);
	}
	*/
	//利用DMA1通道2发送数据
	DMA_Cmd(DMA1_Channel2, DISABLE);	//先关闭DMA1通道2，为了设置发送数据的长度
	DMA_SetCurrDataCounter(DMA1_Channel2, UART3_TXBUFF_SIZE);	//设置发送数据的长度
	DMA_Cmd(DMA1_Channel2, ENABLE);	//启动DMA1通道2的数据传输
}

//定义通过ESP8266发送MQTT报文的函数
void UART3_TxData(unsigned char *data) {
	//发送的报文数据中，前两个字节分别存放数据长度的高字节和低字节，从第三个字节开始是原始报文数据
	int len = (data[0] << 8) + data[1];
	//利用DMA1通道2发送数据
	DMA_Cmd(DMA1_Channel2, DISABLE);	//先关闭DMA1通道2，为了设置发送数据的长度
	DMA1_Channel2->CMAR = (unsigned int)(&data[2]); //重新设置MQTT报文数据的内存首地址，让DMA硬件传输
	DMA_SetCurrDataCounter(DMA1_Channel2, len);	//重新设置发送数据的长度
	DMA_Cmd(DMA1_Channel2, ENABLE);	//启动DMA1通道2的数据传输
}
