#include "led.h"

//定义初始化函数
void LED_Init(void) {
	GPIO_InitTypeDef GPIO_Config;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);				//必须先使能GPIOB时钟，否则寄存器无法进行操作
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);				//必须先使能GPIOE时钟，否则寄存器无法进行操作
	
	//指定GPIOB5工作特性
	GPIO_Config.GPIO_Pin = GPIO_Pin_5;													//指定GPIOB5
	GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP; 									//通用推挽输出模式
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz; 									//指定输出最大速度为50MHz
	GPIO_Init(GPIOB, &GPIO_Config);															//初始化GPIOB5
	
	//指定GPIOE5工作特性 
	GPIO_Config.GPIO_Pin = GPIO_Pin_5; 													//指定GPIOE5
	GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP;										//指定通用推挽输出模式
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;									//指定输出最大速度是50MHz
	GPIO_Init(GPIOE, &GPIO_Config);															//初始化GPIOE5
	
	//默认关灯,输出高电平
	//GPIO_SetBits(GPIOB, GPIO_Pin_5);
	GPIO_SetBits(GPIOE, GPIO_Pin_5);
}

//定义开灯函数
void LED_On(void) {
	GPIO_ResetBits(GPIOB, GPIO_Pin_5);
	GPIO_ResetBits(GPIOE, GPIO_Pin_5);
}

//定义关灯函数
void LED_Off(void) {
	GPIO_SetBits(GPIOB, GPIO_Pin_5);
	GPIO_SetBits(GPIOE, GPIO_Pin_5);
}

//定义延时函数
void Delay(int n) {
	while(n--);
}












