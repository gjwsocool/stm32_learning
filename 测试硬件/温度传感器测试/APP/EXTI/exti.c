 // exti.c
 #include "exti.h"
 #include "Systick.h"
 #include "led.h"
 #include "key.h"
 #include "beep.h"
 
 // KEY_UP - PA0 - 上升沿触发中断 
 // 按下KEY_UP, 触发中断信号给CPU核 
void My_EXTI_Init(void){
	// 1.配置PA0 下拉输入 + 打开GPIOA控制器时钟 √
	// 1.2.配置PE4 上拉输入 + 打开GPIOE时钟 
	
	// 2.将PA0 映射到EXTI0硬件线, PE4 -> EXTI4
	// 2.1.打开AFIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	// 2.2.映射 
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4);
	
	// 3.EXTI控制器的配置 
	EXTI_InitTypeDef EXTI_Config;
	EXTI_Config.EXTI_Line    = EXTI_Line0; 					// EXTI0中断线 
	EXTI_Config.EXTI_Mode    = EXTI_Mode_Interrupt; // 中断模式
	EXTI_Config.EXTI_Trigger = EXTI_Trigger_Rising; // 上升沿
	EXTI_Config.EXTI_LineCmd = ENABLE; 						  // 使能
	EXTI_Init(&EXTI_Config);
	
	// EXTI4 : 下降沿触发中断信号
	EXTI_Config.EXTI_Line    = EXTI_Line4; 					// EXTI4中断线 
	EXTI_Config.EXTI_Mode    = EXTI_Mode_Interrupt; // 中断模式
	EXTI_Config.EXTI_Trigger = EXTI_Trigger_Falling;// 下降沿触发
	EXTI_Config.EXTI_LineCmd = ENABLE; 						  // 使能
	EXTI_Init(&EXTI_Config);
	
	// 4.NVIC控制器的配置 
	NVIC_InitTypeDef NVIC_Config;
	NVIC_Config.NVIC_IRQChannel                   = EXTI0_IRQn; // EXTI0触发中断信号
	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_Config.NVIC_IRQChannelSubPriority        = 2;
	NVIC_Config.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_Config);
	
	NVIC_Config.NVIC_IRQChannel                   = EXTI4_IRQn; // EXTI4触发中断信号
	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Config.NVIC_IRQChannelSubPriority        = 1;
	NVIC_Config.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_Config);
}
 
 // 中断处理函数 
 // 开灯 / 关灯; 延时函数
void EXTI0_IRQHandler(void) {
	// 判断EXTI0的中断是否到来 
	if (EXTI_GetITStatus(EXTI_Line0) == SET) {
		// 业务逻辑 
		delay_ms(10);
		if (KEY_UP == 1) 
			LED0 = !LED0;
	}
	// 清除中断到来位
	EXTI_ClearITPendingBit(EXTI_Line0);
} 

// EXTI4中断处理函数
void EXTI4_IRQHandler(void) {
	// 判断EXTI4的中断是否到来 
	if (EXTI_GetITStatus(EXTI_Line4) == SET) {
		// 清除中断到来位
		EXTI_ClearITPendingBit(EXTI_Line4);
		// 业务逻辑
		delay_ms(10);
		if (KEY0 == 0) 
			BEEP = !BEEP; 
	}
	
}
 