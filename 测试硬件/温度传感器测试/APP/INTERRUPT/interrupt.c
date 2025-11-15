// interrupt.c
#include "interrupt.h"

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
}

