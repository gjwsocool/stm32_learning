#include "init.h"
#include "uart.h"
#include "led.h"
#include "beep.h"
#include "key.h"
#include "systick.h"
#include "exti.h"
#include "at24c02.h"
#include "ds18b20.h"
#include "dma.h"
#include "timer4.h"

#include "uart3.h"
#include "esp8266.h"

#include "mqtt.h"

#include "oled.h"

#include "pwm.h"

//定义初始化函数指针数组保存各个硬件初始化函数
static PINIT_T init_func[] = {
	SysTick_Init,	//滴答定时器初始化
	LED_Init, 		//LED初始化
	//UART_Init, 		//UART初始化
	UART_IDLEInit,	//UART初始化,IDLE中断
	BEEP_Init,		//蜂鸣器初始化
	KEY_Init,			//按键初始化
	My_EXTI_Init,	//外部中断初始化
	AT24C02_Init,	//初始化IIC
	DS18B20_Init, //初始化DS18B20
	UART1DMA_Init, //初始化UART1的DMA
	TIMER4_Init,		//初始化定时器4
	
	UART3_Init, 		//初始化UART3相关内容
	WIFI_ResetIO_Init, //初始化WIFI复位引脚
	
	AliIoT_Parameter_Init, //初始化MQTT
	
	OLED_Init, //初始化OLED
	TIM2_CH2_PWM_Init, //初始化PWM
	0
};

//定义硬件初始化总入口函数
void DEVICE_Init(void) {
	PINIT_T *pfunc = init_func;
	//遍历所有硬件初始化函数挨个调用
	for(; *pfunc; pfunc++)
		(*pfunc)();
}
