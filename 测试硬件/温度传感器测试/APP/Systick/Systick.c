// Systick.c
#include "Systick.h"

static u32 fac_us = 0;
static u32 fac_ms = 0;

void Systick_Init(void){
	// 1.CTRL[2] = 0, 时钟源=9MHz
	SysTick->CTRL &= ~(1 << 2);
	// 2.1us计数 
	fac_us = 9;
	// 3.1ms计数
	fac_ms = 9000;
}

void delay_us(u32 n){
	u32 temp; // 存储CTRL的值 
	// 设置初始值 
	SysTick->LOAD = n * fac_us;
	// 延时开始前, VAL=0
	SysTick->VAL = 0;
	
	// 启动定时器
	SysTick->CTRL |= (1 << 0);
	// 启动后, 初始值自动加载到VAL中, 每个时钟周期减1, 直到减到0, 延时时间到期
	// 这段时间, 循环的判断是否减到0
	// 判断的依据 CTRL[16] =1,时间到期, 结束循环; =0,不到期, 循环等待;
	do {
		temp = SysTick->CTRL; 
	}while((temp & (1 << 0)) && !(temp & (1 << 16)));
	// 结束循环, 时间到期
	// 关闭定时器
	SysTick->CTRL &= ~(1 << 0);
	
	// 延时结束后, VAL=0
	SysTick->VAL = 0;
}


void delay_ms(u32 n){
	u32 temp; // 存储CTRL的值 
	// 设置初始值 
	SysTick->LOAD = n * fac_ms;
	// 延时开始前, VAL=0
	SysTick->VAL = 0;
	
	// 启动定时器
	SysTick->CTRL |= (1 << 0);
	// 启动后, 初始值自动加载到VAL中, 每个时钟周期减1, 直到减到0, 延时时间到期
	// 这段时间, 循环的判断是否减到0
	// 判断的依据 CTRL[16] =1,时间到期, 结束循环; =0,不到期, 循环等待;
	do {
		temp = SysTick->CTRL; 
	}while((temp & (1 << 0)) && !(temp & (1 << 16)));
	// 结束循环, 时间到期
	// 关闭定时器
	SysTick->CTRL &= ~(1 << 0);
	
	// 延时结束后, VAL=0
	SysTick->VAL = 0;
}


