// init.c
#include "init.h"
#include "led.h"
#include "beep.h"
#include "Systick.h"
#include "key.h"
#include "exti.h"
#include "usart.h"
#include "at24c02.h"
#include "ds18b20.h"
#include "dma.h"
#include "tim4.h"
#include "usart3.h"
#include "esp8266.h"

// 定义函数指针数组 
static PINIT_T init_func[] = {
	LED_Init,			// led初始化
	BEEP_Init,		// BEEP初始化
	Systick_Init,	// Systick初始化
	KEY_Init, 		// KEY 初始化
	My_EXTI_Init, // EXTI初始化 
	// UART_Init, 		// 串口初始化
	UARTIDLE_Init, // 串口初始化 + IDLE + DMA
	AT24C02_Init, // AT24C02初始化
	DS18B20_Init,	// DS18B20初始化
	My_DMA_Init,		// DMA初始化
	TIM4_Init,				// 定时器4初始化
	UART3_Init, 			// 串口3 + GPIO + DMA初始化
	WIFI_ResetIO_Init, // PA15引脚初始化
};

void DEVICE_Init(void) {
	int i = 0;
	for( i = 0; i < ARR_NUM(init_func); i++) {
		PINIT_T p;
		p = init_func[i];
		p();
	}
}