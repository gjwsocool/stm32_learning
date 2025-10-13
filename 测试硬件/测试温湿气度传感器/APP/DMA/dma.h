#ifndef __DMA_H 
#define __DMA_H

#include "stm32f10x.h"
#include "system.h"

#define 	UART1DMA_TXBUFF_SIZE			(1024*4)				//指定发送数据的内存缓冲区大小
extern u8	UART1DMA_TxBuff[UART1DMA_TXBUFF_SIZE];		//指定发送数据的内存缓冲区
extern u32	UART1DMA_TxCounter;											//指定发送数据计数
extern u32 	DMA_TcFlags;														//=1：表示DMA传输完毕，=0：传输中

extern void UART1DMA_Init(void);										//UART1 DMA初始化函数
extern void UART1DMA_Tx_Test(void);									//UART1 DMA测试函数

#define UART1DMA_RXBUFF_SIZE				1024						//指定UART1串口接收缓冲区的大小
extern u8	UART1DMA_RxBuff[UART1DMA_RXBUFF_SIZE];		//分配UART1串口接收缓冲区内存
extern u32	UART1DMA_RxCounter;											//指定UART1串口接收数据个数计数

#endif
