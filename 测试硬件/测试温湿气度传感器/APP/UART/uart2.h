#ifndef __UART2_H 
#define __UART2_H

#include "stm32f10x.h"
#include "system.h"

#define UART2_TXBUFF_SIZE		1024			//定义串口2发送缓冲区大小
#define UART2_RXBUFF_SIZE		1024			//定义串口2接收缓冲区大小

extern unsigned int UART2_RxCounter;			//指定串口2接收数据计数
extern unsigned char UART2_RxBuff[UART2_RXBUFF_SIZE];	//指定串口2接收数据缓冲区
extern unsigned char UART2_TxBuff[UART2_TXBUFF_SIZE];	//指定串口2发送数据缓冲区

#define RS485_TX_EN		PDout(7)	//485的发送接收使能 =1:发送 =0:接收

extern void UART2_Init(void);	//初始化串口2函数
extern void UART2DMA_Init(void);			//启动串口3的DMA功能
extern void UART2_TxData(unsigned char *, int len); //UART2发送数据函数

extern void RS485_Test(void);		//485测试函数
#endif
