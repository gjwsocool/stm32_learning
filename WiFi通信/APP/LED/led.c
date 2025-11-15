// led.c
#include "led.h"
#include "system.h"


void LED_Init(void){
	// 1.打开GPIOB控制器的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	// 1.2.打开GPIOE控制器的时钟 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	
	// 2.配置PB5为推挽输出, 50MHz
	GPIO_InitTypeDef GPIO_Config;
	GPIO_Config.GPIO_Pin   = GPIO_Pin_5;       // 第5个引脚 
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz; // 50MHz
	GPIO_Config.GPIO_Mode  = GPIO_Mode_Out_PP; // 推挽输出
	GPIO_Init(GPIOB, &GPIO_Config);
	// 2.2.配置PE5为推挽输出, 50MHz
	GPIO_Config.GPIO_Pin   = GPIO_Pin_5;       // 第5个引脚 
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz; // 50MHz
	GPIO_Config.GPIO_Mode  = GPIO_Mode_Out_PP; // 推挽输出
	GPIO_Init(GPIOE, &GPIO_Config);
	
	// 3.PB5输出高电平 
	GPIO_SetBits(GPIOB, GPIO_Pin_5);
	// 3.2.PE5输出高电平 
	GPIO_SetBits(GPIOE, GPIO_Pin_5);
}

void LED_On(void){
	GPIO_ResetBits(GPIOB, GPIO_Pin_5);
	GPIO_ResetBits(GPIOE, GPIO_Pin_5);
}
void LED_Off(void){
	GPIO_SetBits(GPIOB, GPIO_Pin_5);
	GPIO_SetBits(GPIOE, GPIO_Pin_5);
}
void delay(unsigned int n){
	while (n--); 
}
