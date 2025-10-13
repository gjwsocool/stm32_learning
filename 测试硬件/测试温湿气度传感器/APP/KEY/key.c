#include "key.h"
#include "systick.h"

//定义初始化按键函数
void KEY_Init(void) {
	GPIO_InitTypeDef GPIO_Config;
	
	//1.使能GPIOE时钟和GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOA, ENABLE);
	
	//2.初始化GPIOE4,GPIOE3
	GPIO_Config.GPIO_Pin = KEY0_PIN|KEY1_PIN; 	//指定GPIOE3/4
	GPIO_Config.GPIO_Mode = GPIO_Mode_IPU; 			//指定上拉输入
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz; 	//指定输入最大时钟
	GPIO_Init(KEY_PORT, &GPIO_Config);
	
	//3.初始化GPIOA0
	GPIO_Config.GPIO_Pin = KEY_UP_PIN;					//指定GPIOA0
	GPIO_Config.GPIO_Mode = GPIO_Mode_IPD;			//指定下拉输入
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;	//指定输入最大时钟
	GPIO_Init(KEY_UP_PORT, &GPIO_Config);
}

//定义按键扫描函数
u8 KEY_Scan(void) {
	static u8 ispressed = 1; 									//ispressed=0:表示按键无操作,ispressed=1:表示按键有操作
	if((ispressed == 0) && (KEY0 == 0 || KEY1 == 0 || KEY_UP == 1)) {			//判断按键是否按下
			delay_ms(10);													//去除抖动
			ispressed = 1; 												//记录按键按下
			if(KEY0 == 0)
				return KEY0_PRESS;									//返回KEY0按键的键值		
			else if(KEY1 == 0)
				return KEY1_PRESS;									//返回KEY0按键的键值		
			else if(KEY_UP == 1)
				return KEY_UP_PRESS;								//返回KEY0按键的键值		
	} else if(KEY0 == 1 && KEY1 == 1 && KEY_UP == 0)											//松开,无操作
			ispressed = 0; 												
	return 0;
}
