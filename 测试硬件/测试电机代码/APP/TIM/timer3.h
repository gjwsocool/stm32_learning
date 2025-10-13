#ifndef __TIMER3_H
#define __TIMER3_H

#include "stm32f10x.h"
#include "system.h"

//每隔2秒触发一次
#define ARR		9999  
#define PSC		14399

//每隔0.5秒触发一次
//#define ARR		4999  
//#define PSC		7199

extern void TIMER3_ENALBE_1S(void);

#endif
