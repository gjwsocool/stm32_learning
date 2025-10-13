#include "BEEP.h"

//定义初始化函数
void BEEP_Init(void) {
	GPIO_InitTypeDef GPIO_Config;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);				//必须先使能GPIOB时钟，否则寄存器无法进行操作
	
	//指定GPIOB8工作特性
	GPIO_Config.GPIO_Pin = GPIO_Pin_8;													//指定GPIOB8
	GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP; 									//通用推挽输出模式
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz; 									//指定输出最大速度为50MHz
	GPIO_Init(GPIOB, &GPIO_Config);															//初始化GPIOB8
	
	//默认关闭蜂鸣器，输出低电平
	GPIO_ResetBits(GPIOB, GPIO_Pin_8);
}

//定义开蜂鸣器函数
void BEEP_On(void) {
	GPIO_SetBits(GPIOB, GPIO_Pin_8);
}

//定义关蜂鸣器函数
void BEEP_Off(void) {
	GPIO_ResetBits(GPIOB, GPIO_Pin_8);
}












