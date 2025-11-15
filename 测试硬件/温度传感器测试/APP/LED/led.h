// led.h
#ifndef __LED_H_ 
#define __LED_H_

#include "stm32f10x.h"
#include "system.h"

extern void LED_Init(void);

extern void LED_On(void);
extern void LED_Off(void);

extern void delay(unsigned int n);

// 定义位带操作控制LED灯 
#define  	LED0		PBout(5)
#define   LED1    PEout(5)

#endif


