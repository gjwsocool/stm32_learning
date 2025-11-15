// tim4.h
#ifndef __TIM4_H_ 
#define __TIM4_H_ 

#include "stm32f10x.h"

// ÖÜÆÚ : (PSC + 1) * (ARR + 1) / CLK 
// 				14400 * 5000 / 72000000 = 1s
#define  PSC  14399
#define  ARR  4999

extern void TIM4_Init(void);

// ²âÊÔº¯Êý
extern void TIM4_Start(void);
extern void TIM4_Stop(void);

#endif
