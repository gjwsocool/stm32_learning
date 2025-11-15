// key.c
#include "key.h"
#include "Systick.h"

// KEY0 - PE4
// KEY_UP - PA0
void KEY_Init(void){
	// 1.打开GPIOE, GPIOA控制器时钟 APB2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	// 2.配置PE4为上拉输入 
	GPIO_InitTypeDef GPIO_Config;
	GPIO_Config.GPIO_Pin  = GPIO_Pin_4;
	GPIO_Config.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
	GPIO_Init(GPIOE, &GPIO_Config);
	// 2.2.配置PA0为下拉输入
	GPIO_Config.GPIO_Pin  = GPIO_Pin_0;
	GPIO_Config.GPIO_Mode = GPIO_Mode_IPD; // 下拉输入
	GPIO_Init(GPIOA, &GPIO_Config);
}

// 判断按键是否按下
// 有按键按下, 返回按键的键值
//		KEY0   - KEY0_PRESS
//		KEY_UP - KEY_UP_PRESS
// 没有按键按下: 返回0 
u8   KEY_Scan(void) {
	// 按键没有被按下, 0; 按键被按下, 1;
	static u8 ispressed = 0;
	
	if ((ispressed == 0) && ((KEY0 == 0) || (KEY_UP == 1)) ) {
		// 延时10ms, 软件去抖动
		delay_ms(10);
		
		// 一旦按键被按下, ispressed = 1;
		ispressed = 1;
		
		if (KEY0 == 0)
			return KEY0_PRESS; 
		else if (KEY_UP == 1)
			return KEY_UP_PRESS;
	}
	else if ((KEY0 == 1) && (KEY_UP == 0)) { // 按键松开 
		ispressed = 0;
	}
	
	return 0;
}


