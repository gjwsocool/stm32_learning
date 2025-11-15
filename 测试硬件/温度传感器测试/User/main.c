// main.c
#include "stm32f10x.h"
#include "led.h"
#include "beep.h"
#include "system.h"
#include "Systick.h"
#include "key.h"
#include "exti.h"
#include "usart.h"
#include "string.h" // strcmp函数
#include "init.h"
#include "cmd.h"
#include "interrupt.h"


static cmd_t* pcmd; // 定义变量保存find_name的返回值 

int main(void) {
	// 优先级分组 |--|--|
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	// 所有的硬件初始化
	DEVICE_Init();
	
	while(1) {
		// 其他业务 
		if (UART_Send_Flag) {
			UART_Puts("received command:");
			// 上位机给下位机发送命令
			// buf="led on"/"led off"/"beep on"/"beep off"
			
			// 命令的匹配 
			pcmd = find_cmd(UART_RxBuff);
			if (pcmd != 0) // 匹配成功
				pcmd->callback();
			else
				UART_Puts("invalid command\n");
			
			UART_Send_Flag = 0; // 读取完成后标志清0 
		}
	}
	
}

