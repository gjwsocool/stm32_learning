// dma.h
#ifndef __DMA_H_
#define __DMA_H_

#include "stm32f10x.h"

// 声明发送缓冲区
#define   UART1DMA_TxBuff_SIZE	1024
extern u8 UART1DMA_TxBuff[UART1DMA_TxBuff_SIZE];
// +++++++++++++++++++++++
// 声明接收缓冲区 
#define   UART1DMA_RxBuff_SIZE	1024
extern u8 UART1DMA_RxBuff[UART1DMA_RxBuff_SIZE];

// 接收计数 
extern u32 UART1DMA_RxCounter;

extern void My_DMA_Init(void);

// 测试函数 
extern void UART1_DMA_Tx_Test(void);

// 0, 还没发送完成; 1, 发送完成;
extern u8 DMA_TcFlag;

#endif


