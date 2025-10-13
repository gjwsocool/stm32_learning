#ifndef __INIT_H
#define __INIT_H

#include "stm32f10x.h"
#include "system.h"

//声明初始化函数
extern void DEVICE_Init(void);

//声明函数指针数据类型并且取别名
typedef void (*PINIT_T)(void);
#endif
