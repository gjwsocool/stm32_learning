#ifndef __UART3_H 
#define __UART3_H

#include "stm32f10x.h"
#include "system.h"

#define UART3_TXBUFF_SIZE		1024			//定义串口3发送缓冲区大小
#define UART3_RXBUFF_SIZE		1024			//定义串口3接收缓冲区大小

extern unsigned int UART3_RxCounter;			//指定串口3接收数据计数
extern char UART3_RxBuff[UART3_RXBUFF_SIZE];	//指定串口3接收数据缓冲区
extern char UART3_TxBuff[UART3_TXBUFF_SIZE];	//指定串口3发送数据缓冲区

extern void UART3_Init(void);	//初始化串口3函数
extern void UART3_Puts(char *, ...);	//串口3发送数据可变参函数
extern void UART3DMA_Init(void);			//启动串口3的DMA功能

extern void UART3_TxData(unsigned char *); //MQTT发送数据函数
#endif
