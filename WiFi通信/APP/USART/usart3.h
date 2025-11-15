// usart3.h
#ifndef __USART3_H_ 
#define __USART3_H_

#include "stm32f10x.h"

// DMA
// 发送缓冲区
#define UART3_TXBUFF_SIZE                1024
extern u8 UART3_TxBuff[UART3_TXBUFF_SIZE];

// 接收缓冲区 
#define UART3_RXBUFF_SIZE                1024
extern u8 UART3_RxBuff[UART3_RXBUFF_SIZE];
// 接收数据计数
extern u32 UART3_RxCounter; 

extern void UART3_Init(void);
extern void UART3DMA_Init(void);
extern void UART3_Puts(char* format, ...); // 可变参

#endif