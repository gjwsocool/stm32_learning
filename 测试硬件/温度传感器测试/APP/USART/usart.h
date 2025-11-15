// usart.h
#ifndef __USART_H_ 
#define __USART_H_

#include "stm32f10x.h"

extern void UART_Init(void);

extern void UART_Putc(char c); // 发送一个字符 
extern void UART_Puts(char* pstr); // 发送一个字符串

extern char UART_Getc(void); // 接收一个字符 
// char buf[len];
extern void UART_Gets(char* buf, u32 len); // 获取一个字符串

#endif
