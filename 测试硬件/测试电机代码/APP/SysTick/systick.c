#include "systick.h"

//指定微秒延时的计数次数,例如1us计数9次(时钟源为72MHz/8=9MHz)
static u32		fac_us = 0;
//指定毫秒延时的计数次数,例如1ms计数9000次(时钟源为72MHz/8=9MHz)
static u32	fac_ms = 0;

//定义SysTick初始化函数
void SysTick_Init(void)
{
	//1.指定SysTick的时钟源为9MHz
	SysTick->CTRL &= ~(1 << 2);
	
	//2.指定1微秒计数9次
	fac_us = SYSCLK/8; 
	
	//3.指定1毫秒计数9000次
	fac_ms = fac_us * 1000; 
}

//定义微秒级延时函数
void delay_us(u32 nus) {
	u32 temp;
	//1.指定计数的总次数
	SysTick->LOAD = nus * fac_us;
	//2.清空计数器
	SysTick->VAL = 0x00;
	//3.使能SysTick,开始计数
	SysTick->CTRL |= 1;
	//4.判断是否计数到0
	do  {
		temp = SysTick->CTRL;
	}while((temp & 0x01) && !(temp & (1 << 16)));
	//5.关闭SysTick
	SysTick->CTRL &= ~1;
	//6.清空计数器
	SysTick->VAL = 0x00;
}

//定义毫秒级延时函数
void delay_ms(u32 nms) {
	u32 temp;
	//1.指定计数的总次数
	SysTick-> LOAD = fac_ms * nms;
	//2.清空计数器
	SysTick->VAL = 0x00;
	//3.使能SysTick，开始计数
	SysTick->CTRL |= 1;
	//4.判断是否计数到0
	do {
		temp = SysTick->CTRL;
	}while((temp & 0x01) && !(temp & (1 << 16)));
	//5.关闭计数器
	SysTick->CTRL &= ~1;
	//6.清空计数器
	SysTick->VAL = 0x00;
}

