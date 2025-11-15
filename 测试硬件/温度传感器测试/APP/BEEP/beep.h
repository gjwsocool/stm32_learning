// beep.h
#ifndef __BEEP_H_ 
#define __BEEP_H_

#include "stm32f10x.h"
#include "system.h"

extern void BEEP_Init(void);

extern void BEEP_On(void);
extern void BEEP_Off(void);

#define BEEP PBout(8)

#endif

