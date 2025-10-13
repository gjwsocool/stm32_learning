#include "uart2.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"
#include "led.h"
#include "systick.h"

unsigned int UART2_RxCounter = 0; //记录串口2接收数据个数
unsigned char UART2_RxBuff[UART2_RXBUFF_SIZE];	//定义串口3接收缓冲区
unsigned char UART2_TxBuff[UART2_TXBUFF_SIZE];	//定义串口3发送缓冲区

//定义串口2初始化函数
void UART2_Init(void) {
	GPIO_InitTypeDef	GPIO_Config;
	USART_InitTypeDef UART_Config;
	NVIC_InitTypeDef	NVIC_Config;
	
	//使能USART2时钟和GPIOA/D时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD, ENABLE);
	
	//指定GPIOA2为UART2的发送TX功能
	GPIO_Config.GPIO_Pin = GPIO_Pin_2;					//GPIOA2
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;	//指定最大输出速度
	GPIO_Config.GPIO_Mode = GPIO_Mode_AF_PP;		//指定为复用推挽输出，用于UART2的发送功能,GPIOA2->UART2TX
	GPIO_Init(GPIOA, &GPIO_Config);
	
	//指定GPIOA3为UART2的接收RX功能
	GPIO_Config.GPIO_Pin = GPIO_Pin_3;							//GPIOA3
	GPIO_Config.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//指定为输入浮空，用于UART2的接收功能，GPIOA3->UART2RX
	GPIO_Init(GPIOA, &GPIO_Config);
	
	//初始化485的DE/RE读写使能引脚
	GPIO_Config.GPIO_Pin = GPIO_Pin_7;				 //GPIOD7
 	GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOD, &GPIO_Config);
	
	//配置UART2工作参数
	UART_Config.USART_BaudRate = 9600;						//指定UART2的波特率
	UART_Config.USART_WordLength = USART_WordLength_8b;	//数据位为8位
	UART_Config.USART_StopBits = USART_StopBits_1;		//停止位为1位
	UART_Config.USART_Parity = USART_Parity_No;				//无奇偶校验
	UART_Config.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件流控制
	UART_Config.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	//指定可发可收模式
	USART_Init(USART2, &UART_Config);
	USART_Cmd(USART2, ENABLE);
	
	//启动UART2空闲中断
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);		//开启空闲中断
	
	//启动NVIC的UART2中断
	NVIC_Config.NVIC_IRQChannel = USART2_IRQn;			//启用UART2中断
	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 3;	//指定抢占式优先级
	NVIC_Config.NVIC_IRQChannelSubPriority = 3;			//指定响应式优先级
	NVIC_Config.NVIC_IRQChannelCmd = ENABLE;		//使能UART2中断
	NVIC_Init(&NVIC_Config);
	
	//开启UART2的DMA接收
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
	//开启UART2的DMA发送
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
	
	//初始化UART2的DMA功能
	UART2DMA_Init();
}

//初始化UART2的DMA功能
void UART2DMA_Init(void) {
	DMA_InitTypeDef	DMA_Config;
	
	//使能DMA1通道时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	//配置DMA1的通道6为串口2的接收 DMA1.Channel6->UART2RX
	DMA_Config.DMA_PeripheralBaseAddr = (unsigned int)(&USART2->DR);	//指定外设寄存器地址
	DMA_Config.DMA_MemoryBaseAddr = (unsigned int)UART2_RxBuff;				//指定内存起始地址
	DMA_Config.DMA_DIR = DMA_DIR_PeripheralSRC;	//从外设搬移到内存
	DMA_Config.DMA_BufferSize = UART2_RXBUFF_SIZE; //指定UART2接收缓冲区大小
	DMA_Config.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //外设地址不自增
	DMA_Config.DMA_MemoryInc = DMA_MemoryInc_Enable;	//内存地址递增
	DMA_Config.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //指定外设数据宽度为8位
	DMA_Config.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//指定内存数据宽度为8位
	DMA_Config.DMA_Mode = DMA_Mode_Normal;	//配置为正常工作模式
	DMA_Config.DMA_Priority = DMA_Priority_High;	//配置为高优先级
	DMA_Config.DMA_M2M = DMA_M2M_Disable;	//禁止内存到内存的搬移模式
	DMA_Init(DMA1_Channel6, &DMA_Config);
	
	//配置DMA1的通道7为串口2的发送 DMA1.Channel7->UART2TX
	DMA_Config.DMA_PeripheralBaseAddr = (unsigned int)(&USART2->DR);	//指定外设寄存器地址
	DMA_Config.DMA_MemoryBaseAddr = (unsigned int)UART2_TxBuff;				//指定内存起始地址
	DMA_Config.DMA_DIR = DMA_DIR_PeripheralDST;	//从外设搬移到内存
	DMA_Config.DMA_BufferSize = UART2_TXBUFF_SIZE; //指定UART2接收缓冲区大小
	DMA_Config.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //外设地址不自增
	DMA_Config.DMA_MemoryInc = DMA_MemoryInc_Enable;	//内存地址递增
	DMA_Config.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //指定外设数据宽度为8位
	DMA_Config.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//指定内存数据宽度为8位
	DMA_Config.DMA_Mode = DMA_Mode_Normal;	//配置为正常工作模式
	DMA_Config.DMA_Priority = DMA_Priority_Medium;	//配置为中等优先级
	DMA_Config.DMA_M2M = DMA_M2M_Disable;	//禁止内存到内存的搬移模式
	DMA_Init(DMA1_Channel7, &DMA_Config);
	
	//使能DMA1通道6
	DMA_Cmd(DMA1_Channel6, ENABLE);	 
	//默认使能为接收模式
	RS485_TX_EN = 0;
}

//定义发送数据函数
void UART2_TxData(unsigned char *pbuf, int len) {
	//使能485的发送功能
	RS485_TX_EN=1;			
	
	//利用DMA1通道7发送数据
	DMA_Cmd(DMA1_Channel7, DISABLE);	//先关闭DMA1通道7，为了设置发送数据的长度
	DMA_SetCurrDataCounter(DMA1_Channel7, len);	//重新设置发送数据的长度
	DMA_Cmd(DMA1_Channel7, ENABLE);	//启动DMA1通道7的数据传输

	//等待发送完毕
	while(DMA_GetFlagStatus(DMA1_FLAG_TC7) == RESET);
	//清除发送完成标志位
	DMA_ClearFlag(DMA1_FLAG_TC7);
	
	delay_ms(2);
	
	//立刻设置为接收模式，等待从机响应信息
	RS485_TX_EN=0;				
}

//定义485测试函数
void RS485_Test(void) {
	//构造要发送的数据
	int i;
	for(i = 0; i < UART2_TXBUFF_SIZE; i++)
		UART2_TxBuff[i] = 'A';
	
	UART2_TxData(UART2_TxBuff, UART2_TXBUFF_SIZE);
}

