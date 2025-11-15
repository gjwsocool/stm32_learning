// key.h
#ifndef __KEY_H_
#define __KEY_H_

#include "stm32f10x.h"
#include "system.h"

// KEY0 - PE4
#define KEY0					PEin(4)	// u8 val = KEY0; 
#define KEY0_PRESS		1				// key0键值 
// KEY_UP - PA0 
#define KEY_UP				PAin(0)
#define KEY_UP_PRESS	2				// key_up键值 

extern void KEY_Init(void); 
// 判断按键是否按下
// 有按键按下, 返回按键的键值
//		KEY0 - KEY0_PRESS
// 没有按键按下: 返回0 
extern u8   KEY_Scan(void); 

#endif
