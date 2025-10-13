#include "uart.h"
#include "stdio.h"

//定义UART初始化函数
void UART_Init(void) {
	//用于配置UART的GPIO引脚信息
	GPIO_InitTypeDef	GPIO_Config;
	//用于配置UART1串口
	USART_InitTypeDef	UART_Config;
	
	//打开GPIO和USART时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	//配置UART的TX引脚工作参数
	GPIO_Config.GPIO_Pin = GPIO_Pin_9;									//UART1的TX->PA9
	GPIO_Config.GPIO_Mode = GPIO_Mode_AF_PP;						//配置为复用推挽输出
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;					//输出最大时钟
	GPIO_Init(GPIOA, &GPIO_Config);											//初始化UART的TX
	
	//配置UART的RX引脚的工作参数
	GPIO_Config.GPIO_Pin = GPIO_Pin_10; 								//UART的RX->PA10
	GPIO_Config.GPIO_Mode = GPIO_Mode_IN_FLOATING;			//配置为浮空输入
	GPIO_Init(GPIOA, &GPIO_Config);											//初始化UART的RX
	
	//配置UART控制器的工作参数
	UART_Config.USART_BaudRate = BOUND;									//配置为115200bps波特率
	UART_Config.USART_WordLength = USART_WordLength_8b;	//数据位为8位
	UART_Config.USART_StopBits = USART_StopBits_1;			//停止位为1位
	UART_Config.USART_Parity = USART_Parity_No;					//不校验
	UART_Config.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	UART_Config.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		//使能发送和接收
	USART_Init(USART1, &UART_Config);
	USART_Cmd(USART1, ENABLE);	//使能USART1串口
	
	//配置NVIC的USART1中断信息
	NVIC_InitTypeDef NVIC_Config;
	NVIC_Config.NVIC_IRQChannel = USART1_IRQn; 	//具体参见stm32f10x.h,指定USART1串口中断通道
	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 2;	//指定抢占优先级
	NVIC_Config.NVIC_IRQChannelSubPriority = 3;	//指定子优先级
	NVIC_Config.NVIC_IRQChannelCmd = ENABLE; //使能USART1中断通道
	NVIC_Init(&NVIC_Config);		//初始化UART1的NVIC中断
	
	//开启串口接收中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

//定义发送字符函数
void UART_Putc(u8 c) {
	//轮询等待数据发送完毕，SET=1,表示发送完毕，RESET=0，发送还没有完成
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); 
	//发送字符
	USART_SendData(USART1, c);	
}

//定义发送字符串函数
void UART_Puts(u8 *pstr) {
	while(*pstr) {
		UART_Putc(*pstr);
		pstr++;
	}
}

//定义接收字符函数
u8 UART_Getc(void) {
	//轮询判断接收缓冲区是否有数据，如果没有数据轮询死等，否则读取数据
	while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
	//有数据则读走
	return (u8)USART_ReceiveData(USART1);
}

//定义接收字符串函数
void UART_Gets(u8 buf[], u32 len) {
	int i;
	for(i = 0; i < len - 1; i++) {
		buf[i] = UART_Getc();
		if(buf[i] == '\n')
			break;
	}
	buf[i-1] = '\0'; //串口工具发送来的数据格式是："hello\r\n"
}

//重定向printf
int fputc(int c, FILE *fp) {
	//轮询等待数据发送完毕，SET=1,表示发送完毕，RESET=0，发送还没有完成
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	//发送字符
	USART_SendData(USART1, (u8)c);	 
	return c;
}

//初始化串口1，开启DMA发送,接收，空闲中断
void UART_IDLEInit(void) {
	//用于配置UART的GPIO引脚信息
	GPIO_InitTypeDef	GPIO_Config;
	//用于配置UART1串口
	USART_InitTypeDef	UART_Config;
	
	//打开GPIO和USART时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	//配置UART的TX引脚工作参数
	GPIO_Config.GPIO_Pin = GPIO_Pin_9;									//UART1的TX->PA9
	GPIO_Config.GPIO_Mode = GPIO_Mode_AF_PP;						//配置为复用推挽输出
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;					//输出最大时钟
	GPIO_Init(GPIOA, &GPIO_Config);											//初始化UART的TX
	
	//配置UART的RX引脚的工作参数
	GPIO_Config.GPIO_Pin = GPIO_Pin_10; 								//UART的RX->PA10
	GPIO_Config.GPIO_Mode = GPIO_Mode_IN_FLOATING;			//配置为浮空输入
	GPIO_Init(GPIOA, &GPIO_Config);											//初始化UART的RX
	
	//配置UART控制器的工作参数
	UART_Config.USART_BaudRate = BOUND;									//配置为115200bps波特率
	UART_Config.USART_WordLength = USART_WordLength_8b;	//数据位为8位
	UART_Config.USART_StopBits = USART_StopBits_1;			//停止位为1位
	UART_Config.USART_Parity = USART_Parity_No;					//不校验
	UART_Config.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	UART_Config.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		//使能发送和接收
	USART_Init(USART1, &UART_Config);
	USART_Cmd(USART1, ENABLE);	//使能USART1串口
	
	//配置NVIC的USART1中断信息
	NVIC_InitTypeDef NVIC_Config;
	NVIC_Config.NVIC_IRQChannel = USART1_IRQn; 	//具体参见stm32f10x.h,指定USART1串口中断通道
	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 2;	//指定抢占优先级
	NVIC_Config.NVIC_IRQChannelSubPriority = 3;	//指定子优先级
	NVIC_Config.NVIC_IRQChannelCmd = ENABLE; //使能USART1中断通道
	NVIC_Init(&NVIC_Config);		//初始化UART1的NVIC中断
	
	//开启串口空闲中断
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
	
	//开启DMA接收和发送功能
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);	
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
}


