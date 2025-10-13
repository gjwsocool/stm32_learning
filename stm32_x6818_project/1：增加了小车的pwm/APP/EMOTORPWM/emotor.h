#ifndef __EMOTOR_H
#define __EMOTOR_H

/*2个重要文件*/
//stm32f10x.h---官方库头文件，告诉编译器STM32外设和寄存器怎么用
//system.h---用户自定义的系统配置文件，存放宏定义和常用函数声明
#include "stm32f10x.h"
#include "system.h"

//配置PWM的输出频率为:20KHZ
//PSC分频:让计数器慢多少倍
//F(pwm)=f(clk)/(arr+1)*(psc+1)   ---fclk=72MHZ
//20 000 =72 000 000 /100*(x+1)
#define PWM_ARR  99
#define PWM_PSC  35

extern int ccr;
extern void TIM2_CH2_EMOTOR_Init(void);

#endif