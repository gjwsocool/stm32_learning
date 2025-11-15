// interrupt.h
#ifndef __INTERRUPT_H_ 
#define __INTERRUPT_H_

#include "stm32f10x.h"

// 定义接收缓冲区数组长度 
#define UART_RXBUFF_SIZE	1024

extern char UART_RxBuff[UART_RXBUFF_SIZE];

// 用于计数
extern int UART_RxCounter; 

// 标记数据是否接收完毕
extern int UART_Send_Flag;

#endif
