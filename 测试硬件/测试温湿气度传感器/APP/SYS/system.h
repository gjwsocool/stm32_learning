#ifndef __SYSTEM_H
#define __SYSTEM_H

#include "stm32f10x.h"

//定义位带地址换算公式
#define BITBAND(addr, bitnum)		((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))
#define MEM_ADDR(addr)					(*(unsigned long *)addr)
#define BIT_ADDR(addr, bitnum)	MEM_ADDR(BITBAND(addr, bitnum))
	
//定义输出寄存器和输入寄存器物理地址
#define GPIOA_ODR_ADDR					(GPIOA_BASE + 12)
#define GPIOB_ODR_ADDR					(GPIOB_BASE + 12)
#define GPIOC_ODR_ADDR					(GPIOC_BASE + 12)
#define GPIOD_ODR_ADDR					(GPIOD_BASE + 12)
#define GPIOE_ODR_ADDR					(GPIOE_BASE + 12)
#define GPIOF_ODR_ADDR					(GPIOF_BASE + 12)
#define GPIOG_ODR_ADDR					(GPIOG_BASE + 12)

#define GPIOA_IDR_ADDR					(GPIOA_BASE + 8)
#define GPIOB_IDR_ADDR					(GPIOB_BASE + 8)
#define GPIOC_IDR_ADDR					(GPIOC_BASE + 8)
#define GPIOD_IDR_ADDR					(GPIOD_BASE + 8)
#define GPIOE_IDR_ADDR					(GPIOE_BASE + 8)
#define GPIOF_IDR_ADDR					(GPIOF_BASE + 8)
#define GPIOG_IDR_ADDR					(GPIOG_BASE + 8)

//位带映射
#define PAout(n)								BIT_ADDR(GPIOA_ODR_ADDR, n)	//输出,结果：*(unsigned long *)位带别名地址
#define PAin(n)									BIT_ADDR(GPIOA_IDR_ADDR, n)	//输入,结果：*(unsigned long *)位带别名地址
#define PBout(n)								BIT_ADDR(GPIOB_ODR_ADDR, n)	//输出,结果：*(unsigned long *)位带别名地址
#define PBin(n)									BIT_ADDR(GPIOB_IDR_ADDR, n)	//输入,结果：*(unsigned long *)位带别名地址
#define PCout(n)								BIT_ADDR(GPIOC_ODR_ADDR, n)	//输出,结果：*(unsigned long *)位带别名地址
#define PCin(n)									BIT_ADDR(GPIOC_IDR_ADDR, n)	//输入,结果：*(unsigned long *)位带别名地址
#define PDout(n)								BIT_ADDR(GPIOD_ODR_ADDR, n)	//输出,结果：*(unsigned long *)位带别名地址
#define PDin(n)									BIT_ADDR(GPIOD_IDR_ADDR, n)	//输入,结果：*(unsigned long *)位带别名地址
#define PEout(n)								BIT_ADDR(GPIOE_ODR_ADDR, n)	//输出,结果：*(unsigned long *)位带别名地址
#define PEin(n)									BIT_ADDR(GPIOE_IDR_ADDR, n)	//输入,结果：*(unsigned long *)位带别名地址
#define PFout(n)								BIT_ADDR(GPIOF_ODR_ADDR, n)	//输出,结果：*(unsigned long *)位带别名地址
#define PFin(n)									BIT_ADDR(GPIOF_IDR_ADDR, n)	//输入,结果：*(unsigned long *)位带别名地址
#define PGout(n)								BIT_ADDR(GPIOG_ODR_ADDR, n)	//输出,结果：*(unsigned long *)位带别名地址
#define PGin(n)									BIT_ADDR(GPIOG_IDR_ADDR, n)	//输入,结果：*(unsigned long *)位带别名地址

#define ARRAY_SIZE(x)						(sizeof(x)/sizeof(x[0]))
#endif
