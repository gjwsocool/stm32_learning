// beep.c
#include "beep.h"

// PB8 
void BEEP_Init(void){
	// 1.打开GPIOB控制器的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	// 2.配置PB8 推挽输出, 50MHz
	GPIO_InitTypeDef GPIO_Config;
	GPIO_Config.GPIO_Pin   = GPIO_Pin_8;
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Config.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_Config);
	
	// 3.默认输出低电平 
	GPIO_ResetBits(GPIOB, GPIO_Pin_8);
}
void BEEP_On(void){
	GPIO_SetBits(GPIOB, GPIO_Pin_8);
}
void BEEP_Off(void){
	GPIO_ResetBits(GPIOB, GPIO_Pin_8);
}
