#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"
#include "system.h"

//声明操作函数
extern void LED_Init(void);
extern void LED_On(void);
extern void LED_Off(void);
extern void Delay(int n);

//定义LED0,LED1位带别名
#define LED1		PBout(5)
#define LED2		PEout(5)
#endif 


