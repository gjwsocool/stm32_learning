#ifndef __INTERRUPT_H
#define __INTERRUPT_H

#include "stm32f10x.h"
#include "system.h"

#define UART_RXBUFF_SIZE		1024					//指定串口接收缓冲区的大小
extern u32 UART_RxCounter;						//记录串口接收数据的长度
extern u8	UART_RxBuff[UART_RXBUFF_SIZE];	//暂存串口接收到的数据
extern u8 UART_Send_Flag;						//串口发送数据标志
#endif
