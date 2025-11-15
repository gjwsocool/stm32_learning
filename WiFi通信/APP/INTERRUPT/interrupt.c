// interrupt.c
#include "interrupt.h"
#include "dma.h"
#include "usart3.h"

// DMA1通道2触发的中断
void DMA1_Channel2_IRQHandler(void) {
	// 判断是否是TC中断 
	if (DMA_GetITStatus(DMA1_IT_TC2) != RESET) {
		// 清除中断到来位
		DMA_ClearITPendingBit(DMA1_IT_TC2);
		// 关闭DMA1通道2
		DMA_Cmd(DMA1_Channel2, DISABLE);
	}
}
// 串口3控制器触发的中断 
void USART3_IRQHandler(void) {
	// 判断是否IDLE中断 
	if (USART_GetITStatus(USART3, USART_IT_IDLE) != RESET) {
		// 清除串口3的IDLE中断
		USART3->SR;
		USART3->DR;
		
		// 获取串口3读取的字节数
		// == 接收缓冲区长度 - 空闲字节数
		UART3_RxCounter = UART3_RXBUFF_SIZE - 
						DMA_GetCurrDataCounter(DMA1_Channel3);
		
		// 关闭DMA1通道3
		DMA_Cmd(DMA1_Channel3, DISABLE);
		// 指定长度
		DMA1_Channel3->CNDTR = UART3_RXBUFF_SIZE; 
		// 打开DMA1通道3
		DMA_Cmd(DMA1_Channel3, ENABLE);
	}
}


// DMA1通道4的中断处理函数 
void DMA1_Channel4_IRQHandler(void) {
	// 判断是否是TC中断 - 数据搬移完成 
	if (DMA_GetITStatus(DMA1_IT_TC4) != RESET) {
		// 清除中断到来位
		DMA_ClearITPendingBit(DMA1_IT_TC4);
		// 本次搬移完成 
		// 普通模式 - 搬移完成 - 关闭DMA/打开DMA
		DMA_Cmd(DMA1_Channel4, DISABLE);
		// 标记发送完成 
		DMA_TcFlag = 1;
	}
}

char UART_RxBuff[UART_RXBUFF_SIZE];
// 用于计数
int UART_RxCounter = 0; 
// 标记数据是否接收完毕
// 0, 数据还在读取; 1, 数据读完了;
int UART_Send_Flag = 0;

// "led on\r\n" -> "led on\0\n"
//  0123456 7
//					  ^-counter
// 串口1的中断处理函数 
void USART1_IRQHandler(void) {
	char c;
	// 判断是否是RXNE中断 , 
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		// 清除中断到来位 
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		// 1.每接收到一个字符,存储到接收缓冲区中 
		c = USART_ReceiveData(USART1);
		UART_RxBuff[UART_RxCounter++] = c;
		if (c == '\n') { // 读取结束
			UART_RxBuff[UART_RxCounter-2] = '\0';
			// 将计数值清0 
			UART_RxCounter = 0;
			// 数据读取结束
			UART_Send_Flag = 1;
		}
	}
	// +++++++++++++++++++++++++++++++++++++++++++++
	// 判断IDLE中断 
	if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) { 
		// 触发的IDLE中断 -> 数据搬移完毕
		// 清除中断到来位 
		USART1->SR;
		USART1->DR;
		// "led on\r\n" - 8
		// |----接收的数据个数----|-----空闲字节个数-----| 接收缓冲区
		UART1DMA_RxCounter  = UART1DMA_RxBuff_SIZE - 
																DMA_GetCurrDataCounter(DMA1_Channel5);
		// \r -> \0
		UART1DMA_RxBuff[UART1DMA_RxCounter - 2] = '\0';
		// -----> 本次数据传输结束 
		// 关闭DMA1通道5'
		DMA_Cmd(DMA1_Channel5, DISABLE);
		// 指定搬移长度
		DMA_SetCurrDataCounter(DMA1_Channel5, UART1DMA_RxBuff_SIZE);
		// 打开DMA1通道5
		DMA_Cmd(DMA1_Channel5, ENABLE);
	}
}

