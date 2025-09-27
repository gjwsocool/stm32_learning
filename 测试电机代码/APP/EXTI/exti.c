#include "exti.h"
#include "led.h"
#include "systick.h"
#include "key.h"
#include "beep.h"

//定义按键中断初始化函数
void My_EXTI_Init(void) {
	//用于配置NVIC控制器
	NVIC_InitTypeDef NVIC_Config;
	//用于配置EXTI控制器
	EXTI_InitTypeDef EXTI_Config;
	
	//使能AFIO(复用功能选择)控制器时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	//将KEY0按键的引脚GPIOE4作为外部中断输入源:GPIOE4->EXTI4
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4);
	//将KEY1按键的引脚GPIOE3作为外部中断输入源:GPIOE3->EXTI3
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource3);
	//将KEY_UP按键的引脚GPIOA0作为外部中断输入源：GPIOA0->EXTI0
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
	
	//配置EXTI控制器
	EXTI_Config.EXTI_Line = EXTI_Line4|EXTI_Line3;		//指定EXTI4/EXTI3作为外部中断输入源
	EXTI_Config.EXTI_Mode = EXTI_Mode_Interrupt; //指定硬件外设产生中断
	EXTI_Config.EXTI_Trigger = EXTI_Trigger_Falling;	//指定下降沿触发中断
	EXTI_Config.EXTI_LineCmd = ENABLE;	//使能EXTI4中断源
	EXTI_Init(&EXTI_Config);
	
	//配置EXTI控制器
	EXTI_Config.EXTI_Line = EXTI_Line0;		//指定EXTI0作为外部中断输入源
	EXTI_Config.EXTI_Mode = EXTI_Mode_Interrupt; //指定硬件外设产生中断
	EXTI_Config.EXTI_Trigger = EXTI_Trigger_Rising;	//指定上升沿沿触发中断
	EXTI_Config.EXTI_LineCmd = ENABLE;	//使能EXTI0中断源
	EXTI_Init(&EXTI_Config);
	
	//配置NVIC控制器
	NVIC_Config.NVIC_IRQChannel = EXTI4_IRQn; 	//指定EXTI4中断通道
	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 2; //指定抢占优先级
	NVIC_Config.NVIC_IRQChannelSubPriority = 0; //指定子优先级
	NVIC_Config.NVIC_IRQChannelCmd = ENABLE; //使能EXTI4中断通道
	NVIC_Init(&NVIC_Config);	//一旦初始化完毕，静静等待按键KEY0硬件触发中断信号
	
	NVIC_Config.NVIC_IRQChannel = EXTI3_IRQn; 	//指定EXTI3中断通道
	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 2; //指定抢占优先级
	NVIC_Config.NVIC_IRQChannelSubPriority = 1; //指定子优先级
	NVIC_Config.NVIC_IRQChannelCmd = ENABLE; //使能EXTI3中断通道
	NVIC_Init(&NVIC_Config);	//一旦初始化完毕，静静等待按键KEY1硬件触发中断信号
	
	NVIC_Config.NVIC_IRQChannel = EXTI0_IRQn; 	//指定EXTI0中断通道
	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 2; //指定抢占优先级
	NVIC_Config.NVIC_IRQChannelSubPriority = 3; //指定子优先级
	NVIC_Config.NVIC_IRQChannelCmd = ENABLE; //使能EXTI0中断通道
	NVIC_Init(&NVIC_Config);	//一旦初始化完毕，静静等待按键KEY_UP硬件触发中断信号
}

//定义EXT4外部中断4中断处理函数
void EXTI4_IRQHandler(void){
	//首先判断是否是EXTI4中断源给的中断信号
	if(EXTI_GetITStatus(EXTI_Line4) == 1) {
		//去抖动
		delay_ms(10);
		//如果是KEY0按键产生的中断，实现开关灯操作
		if(KEY0 == 0)
			LED2 = !LED2; 
	}
	//清除中断到来的标志位为0，否则一直产生中断
	EXTI_ClearITPendingBit(EXTI_Line4);
}

//定义EXT3外部中断3中断处理函数
void EXTI3_IRQHandler(void){
	//首先判断是否是EXTI3中断源给的中断信号
	if(EXTI_GetITStatus(EXTI_Line3) == 1) {
		//去抖动
		delay_ms(10);
		//如果是KEY1按键产生的中断，实现开蜂鸣器操作
		//if(KEY1 == 0)
			//BEEP = 1; 
	}
	//清除中断到来的标志位为0，否则一直产生中断
	EXTI_ClearITPendingBit(EXTI_Line3);
}

//定义EXT0外部中断0中断处理函数
void EXTI0_IRQHandler(void){
	//首先判断是否是EXTI0中断源给的中断信号
	if(EXTI_GetITStatus(EXTI_Line0) == 1) {
		//去抖动
		delay_ms(10);
		//如果是KEY_UP按键产生的中断，实现关蜂鸣器操作
		//if(KEY_UP == 1)
			//BEEP = 0;
	}
	//清除中断到来的标志位为0，否则一直产生中断
	EXTI_ClearITPendingBit(EXTI_Line0);
}
