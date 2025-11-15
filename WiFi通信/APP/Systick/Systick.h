// Systick.h
#ifndef __SYSTICK_H_ 
#define __SYSTICK_H_

#include "stm32f10x.h"

extern void Systick_Init(void); 

extern void delay_us(u32 n);  // —” ±nŒ¢√Î
extern void delay_ms(u32 n); 	// —” ±n∫¡√Î

#endif

