#include "pwm.h"
#include "systick.h"
#include "key.h"
#include "stdio.h"
#include "led.h"

void TIM2_CH2_PWM_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseConfig;
	TIM_OCInitTypeDef TIM_OCConfig;
	GPIO_InitTypeDef GPIO_Config;
	
	/* 开启时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	/*  配置GPIO的模式和IO口 */
	GPIO_Config.GPIO_Pin=GPIO_Pin_3; //PB3，连接驱动模块L298N的ENA调速引脚
	GPIO_Config.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Config.GPIO_Mode=GPIO_Mode_AF_PP;//复用推挽输出，不再通过GPIO控制而是通过定时器控制器来操作
	GPIO_Init(GPIOB,&GPIO_Config);
	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2,ENABLE);//改变指定管脚的映射	PB3
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure); //GPIOB1连接L298N的ENB引脚
	
	//指定GPIOB4工作特性->连接到L298N的IN1
	GPIO_Config.GPIO_Pin = GPIO_Pin_4;													//指定GPIOB4
	GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP; 									//通用推挽输出模式
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz; 									//指定输出最大速度为50MHz
	GPIO_Init(GPIOB, &GPIO_Config);															//初始化GPIOB4
	
		//指定GPIOB7工作特性->连接到L298N的IN2
	GPIO_Config.GPIO_Pin = GPIO_Pin_7;													//指定GPIOB7
	GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP; 									//通用推挽输出模式
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz; 									//指定输出最大速度为50MHz
	GPIO_Init(GPIOB, &GPIO_Config);															//初始化GPIOB7
	
		//指定GPIOB0工作特性->连接到L298N的IN3
	GPIO_Config.GPIO_Pin = GPIO_Pin_0;													
	GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP; 									
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz; 									
	GPIO_Init(GPIOB, &GPIO_Config);																		
	
	//指定GPIOB2工作特性->连接到L298N的IN4
	GPIO_Config.GPIO_Pin = GPIO_Pin_2;													
	GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP; 									
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz; 									
	GPIO_Init(GPIOB, &GPIO_Config);															
	
	TIM_TimeBaseConfig.TIM_Period=PWM_ARR;   //自动装载值
	TIM_TimeBaseConfig.TIM_Prescaler=PWM_PSC; //分频系数
	TIM_TimeBaseConfig.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseConfig.TIM_CounterMode=TIM_CounterMode_Up; //设置向上计数模式
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseConfig);	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseConfig);
	
	TIM_OCConfig.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCConfig.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OCConfig.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OC2Init(TIM2,&TIM_OCConfig); //输出比较通道2初始化
	TIM_OC4Init(TIM3,&TIM_OCConfig);
	
	TIM_OC4PreloadConfig(TIM3,TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable); //使能TIMx在 CCR2 上的预装载寄存器
	TIM_ARRPreloadConfig(TIM2,ENABLE);//使能预装载寄存器
	TIM_ARRPreloadConfig(TIM3,ENABLE);//使能预装载寄存器
	
	TIM_Cmd(TIM2,ENABLE); //使能定时器
	TIM_Cmd(TIM3,ENABLE);
	TIM_SetCompare2(TIM2,0);
	TIM_SetCompare4(TIM3,0);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_4);
	GPIO_ResetBits(GPIOB, GPIO_Pin_7);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_0);
	GPIO_ResetBits(GPIOB, GPIO_Pin_2);
}

void PWM_Test(void) {
	int ccr=50;
	u8 key=0;
	u8 dir = 0;
	
	while(1) {
		//通过按键调速
		key=KEY_Scan();
		if(key==KEY_UP_PRESS)
		{
			ccr-=2;
			if(ccr<50)ccr=50;
		}
		else if(key==KEY1_PRESS)
		{
			ccr+=2;
			if(ccr>90)ccr=90;
		} else if(key == KEY0_PRESS) {
			dir = !dir;
			TIM_SetCompare2(TIM2, 50);	
			TIM_SetCompare4(TIM3, 50);
			delay_ms(500);	
			if(dir == 0) {
					GPIO_SetBits(GPIOB, GPIO_Pin_4);
					GPIO_ResetBits(GPIOB, GPIO_Pin_7);
					GPIO_SetBits(GPIOB, GPIO_Pin_0);
					GPIO_ResetBits(GPIOB, GPIO_Pin_2);
			} else {
					GPIO_SetBits(GPIOB, GPIO_Pin_7);
					GPIO_ResetBits(GPIOB, GPIO_Pin_4);
					GPIO_SetBits(GPIOB, GPIO_Pin_2);
					GPIO_ResetBits(GPIOB, GPIO_Pin_0);
			}
		}
		
		delay_ms(50);	 

		printf("ccr = %d\n", ccr);
		TIM_SetCompare2(TIM2,ccr);
		TIM_SetCompare4(TIM3,ccr);		
	}
}

