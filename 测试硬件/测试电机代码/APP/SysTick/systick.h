#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"
#include "system.h"

#define 	SYSCLK		(72)					//指定SysTick外部时钟源72MHz
	
extern void SysTick_Init(void);		//初始化SysTick
extern void delay_ms(u32 nms);		//毫秒级延时函数
extern void delay_us(u32 nus);		//微秒级延时函数

#endif
