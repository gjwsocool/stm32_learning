#include "timer3.h"
#include "mqtt.h"
#include "ds18b20.h"
#include "oled.h"
#include "stdio.h"

//定义定时器4初始化函数
void TIMER3_ENALBE_1S(void) {
	TIM_TimeBaseInitTypeDef TIMER3_Config;
	NVIC_InitTypeDef	NVIC_Config;
	
	//启动定时器TIMER3时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	//初始化定时器3TIMER3的工作参数
	TIMER3_Config.TIM_Period = ARR;		//初始化自动装载值
	TIMER3_Config.TIM_Prescaler = PSC;	//初始化分频系数
	TIMER3_Config.TIM_ClockDivision = TIM_CKD_DIV1;	//1分频,72MHz
	TIMER3_Config.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数模式
	TIM_TimeBaseInit(TIM3, &TIMER3_Config);	//初始化定时器3
	
	//使能TIMER3的溢出中断功能
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	//使能TIM3
	TIM_Cmd(TIM3, ENABLE);
	
	//初始化NVIC的定时器3通道的中断功能
	NVIC_Config.NVIC_IRQChannel = TIM3_IRQn;	//指定定时器3通道中断功能
	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 2;		//指定抢占优先级
	NVIC_Config.NVIC_IRQChannelSubPriority = 3; //指定子优先级
	NVIC_Config.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_Config);
}

//定义定时器4的中断处理函数
void TIM3_IRQHandler(void) {
	//定时器溢出产生中断，首先判断是否是溢出中断
	if(TIM_GetITStatus(TIM3, TIM_IT_Update)) {
		//发布温度
		MQTT_PublishTemp_Cmd();
		OLED_ShowFontHZ(16*1,0,0,16,1);	 
		OLED_ShowFontHZ(16*2,0,1,16,1);
		OLED_ShowFontHZ(16*3,0,2,16,1);
		OLED_ShowFontHZ(16*4,0,3,16,1);
		OLED_ShowFontHZ(16*5,0,4,16,1);
		OLED_ShowFontHZ(16*6,0,5,16,1);
		u8 msg[40] = {0};
		float temp = 0;
		temp = DS18B20_GetTemperture();
		sprintf((char *)msg, "Temperture:%.2f", temp);
		OLED_ShowString(0,32,msg,16);
		OLED_Refresh_Gram();  
		if(temp > 30)
			TIM_SetCompare2(TIM2,0);
		else
			TIM_SetCompare2(TIM2,100);
		//清除溢出中断到来标志位
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}
