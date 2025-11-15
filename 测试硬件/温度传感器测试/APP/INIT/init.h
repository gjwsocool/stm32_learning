// init.h
#ifndef __INIT_H_ 
#define __INIT_H_

#include "stm32f10x.h"

// 定义函数指针数据类型 
typedef void (*PINIT_T)(void); 

#define ARR_NUM(x)	(sizeof(x) / sizeof(x[0]))

extern void DEVICE_Init(void); 

#endif
