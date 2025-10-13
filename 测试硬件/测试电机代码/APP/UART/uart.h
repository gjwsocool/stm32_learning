#ifndef __UART_H
#define __UART_H

#include "stm32f10x.h"
#include "system.h"

#define BOUND			115200										//定义串口波特率

extern void UART_Init(void);								//UART1初始化函数
extern void UART_Putc(u8 c);								//发送字符函数
extern void UART_Puts(u8 *pstr);						//发送字符串函数
extern u8 UART_Getc(void);									//获取字符函数
extern void UART_Gets(u8 *buf, u32 len);		//获取字符串函数

extern void UART_IDLEInit(void);						//初始化UART1,启动DMA发送，接收，空闲中断
#endif
