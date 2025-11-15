// tim4.c
#include "tim4.h"
#include "led.h"

// 使用定时器4
void TIM4_Init(void){
	// 1.打开定时器4时钟 APB1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	// 2.配置TIM4 : ARR / PSC / 不分频 / 向上计数
	TIM_TimeBaseInitTypeDef TIM4_Config;
	TIM4_Config.TIM_Period    = ARR;
	TIM4_Config.TIM_Prescaler = PSC;
	TIM4_Config.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM4_Config.TIM_CounterMode   = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM4, &TIM4_Config);
	
	// 3.配置定时器4支持中断
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	
	// 4.配置NVIC支持定时器4的中断
	NVIC_InitTypeDef NVIC_Config;
	NVIC_Config.NVIC_IRQChannel = TIM4_IRQn; 
	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Config.NVIC_IRQChannelSubPriority = 1;
	NVIC_Config.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_Config);
}
void TIM4_Start(void){
	TIM_Cmd(TIM4, ENABLE);
}
void TIM4_Stop(void){
	TIM_Cmd(TIM4, DISABLE);
}
// 中断处理函数 
void  TIM4_IRQHandler(void) {
	// 判断是否update中断
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) {
		// 清除中断到来位
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		LED0 = !LED0;
	}
}

