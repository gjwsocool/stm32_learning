#include "emotor.h"

//默认ccr为60
int ccr= 60;
/*
    @：电压决定转速，电流决定转向，通过pwm的占空比来模拟交流电
	@：推挽输出是：强拉高强拉低，没有电阻
	@：预装载寄存器是防止速度突变的关键机制，就是建立了一个缓冲，让周期边界改变
	@: 定时的“时基配置”管"时间"，输出“比较配置”管"动作"
	@：TimeBaseInit定时器心脏，OCxIni输出手脚PWM，心脏没初始化，手脚就不能动。
	@：ARR寄存器预装载，保证周期更新平滑；CCR寄存器预装载，保证占空比更新平滑
*/
void TIM2_CH2_EMOTOR_Init(void){
	/*gpio引脚配置+定时器本身配置+定时器输出比较配置 定义*/
	GPIO_InitTypeDef GPIO_Config;
	TIM_TimeBaseInitTypeDef Time_BaseConfig;
	TIM_OCInitTypeDef  Time_OCConfig;
	
	/*使能时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	/*配置6个引脚，2个使能，4个其他*/
	//1:ENA<->PB3 
	GPIO_Config.GPIO_Pin=GPIO_Pin_3;
	GPIO_Config.GPIO_Mode=GPIO_Mode_AF_PP;//复用推挽
	GPIO_Config.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_Config);
	//重映射PB3引脚为TIM2
	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2,ENABLE);
	//2:ENB<->PB1	
	GPIO_Config.GPIO_Pin=GPIO_Pin_1;
	GPIO_Config.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_Config.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_Config);
	//3:B4,B7,B0,B2<->普通引脚
	GPIO_Config.GPIO_Pin=GPIO_Pin_4;
	GPIO_Config.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Config.GPIO_Speed=GPIO_Speed_50MHz;//普通输出
	GPIO_Init(GPIOB,&GPIO_Config);
	
	GPIO_Config.GPIO_Pin=GPIO_Pin_7;
	GPIO_Config.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Config.GPIO_Speed=GPIO_Speed_50MHz;//普通输出
	GPIO_Init(GPIOB,&GPIO_Config);
	
	GPIO_Config.GPIO_Pin=GPIO_Pin_0;
	GPIO_Config.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Config.GPIO_Speed=GPIO_Speed_50MHz;//普通输出
	GPIO_Init(GPIOB,&GPIO_Config);
	
	GPIO_Config.GPIO_Pin=GPIO_Pin_2;
	GPIO_Config.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Config.GPIO_Speed=GPIO_Speed_50MHz;//普通输出
	GPIO_Init(GPIOB,&GPIO_Config);
	
	/*定时器base和oc*/
	/*时钟配置需要：计数终点，计数速度，计数方向，计数分割*/
	Time_BaseConfig.TIM_Period=PWM_ARR;//自动装载值--终点
	Time_BaseConfig.TIM_Prescaler=PWM_PSC;//预分频--降速
	Time_BaseConfig.TIM_ClockDivision=TIM_CKD_DIV1;//不时钟分频
    Time_BaseConfig.TIM_CounterMode=TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInit(TIM2,&Time_BaseConfig);
	TIM_TimeBaseInit(TIM3,&Time_BaseConfig);
	//配置输出通道
	/*高低电平的顺序01或10 + 极性输出高低电平+ 内部状态使能*/
	Time_OCConfig.TIM_OCMode=TIM_OCMode_PWM1;
	Time_OCConfig.TIM_OCPolarity=TIM_OCPolarity_High;
	Time_OCConfig.TIM_OutputState=TIM_OutputState_Enable;
	//OCx--x是具体通道数
	TIM_OC2Init(TIM2,&Time_OCConfig);
	TIM_OC4Init(TIM3,&Time_OCConfig);
	
	/*AR自动加载R和CRR比较预加载寄存器*/
	//TIM3通道4，TIM2通道2
	TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM2,ENABLE);//只需要开启关闭
	TIM_OC4PreloadConfig(TIM3,TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM3,ENABLE);
	/*定时器生效的使能*/
	TIM_Cmd(TIM2,ENABLE);
	TIM_Cmd(TIM3,ENABLE);
	
	/*先让电平不转*/
	TIM_SetCompare2(TIM2,0);
	TIM_SetCompare4(TIM3,0);
	/*初始：pin4 1 pin7 0 pin0 1 pin2 0*/
	GPIO_SetBits(GPIOB,GPIO_Pin_4);
	GPIO_ResetBits(GPIOB,GPIO_Pin_7);
	GPIO_SetBits(GPIOB,GPIO_Pin_0);
    GPIO_ResetBits(GPIOB,GPIO_Pin_2);
	
}