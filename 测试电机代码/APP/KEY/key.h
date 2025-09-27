#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"
#include "system.h"

#define KEY0_PIN			GPIO_Pin_4			//定义KEY0按键的管脚
#define KEY1_PIN			GPIO_Pin_3			//定义KEY1按键的管脚
#define KEY_UP_PIN		GPIO_Pin_0			//定义KEY_UP按键的管脚

#define KEY_PORT			GPIOE						//定义KEY0按键所在端口
#define KEY_UP_PORT		GPIOA						//定义KEY_UP按键所在端口

#define KEY0					PEin(4)					//定义KEY0引脚位带信息
#define KEY1					PEin(3)
#define KEY_UP				PAin(0)

#define KEY0_PRESS		1								//定义KEY0按键的键值
#define KEY1_PRESS		2								//定义KEY1按键的键值
#define KEY_UP_PRESS	3								//定义KEY_UP按键的键值

extern void KEY_Init(void);						//定义按键初始化函数
extern u8	KEY_Scan(void);							//定义按键扫描函数

#endif
