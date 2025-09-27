#ifndef __TIMER4_H
#define __TIMER4_H

#include "stm32f10x.h"
#include "system.h"

////开关灯周期T=(ARR+1)*(PSC+1)/72000000=5000*7200/72000000=0.5s=500ms
//#define ARR		4999
//#define PSC		7199

//开关灯周期T=(ARR+1)*(PSC+1)/72000000=5000*7200/72000000=0.1s=100ms
#define ARR		4999
#define PSC		1439

extern void TIMER4_Init(void);		//初始化定时器4函数
extern void TIMER4_Start(void);		//启动定时器4函数
extern void TIMER4_Stop(void);		//关闭定时器4函数

#endif
