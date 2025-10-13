#ifndef __BEEP_H
#define __BEEP_H

#include "stm32f10x.h"

//声明操作函数
extern void BEEP_Init(void);
extern void BEEP_On(void);
extern void BEEP_Off(void);

#define  BEEP		PBout(8)

#endif 
