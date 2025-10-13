#include "string.h"
#include "init.h"
#include "cmd.h"
#include "interrupt.h"
#include "uart.h"
#include "dma.h"
#include "stdio.h"
#include "esp8266.h"
#include "mqtt.h"
#include "uart3.h"
#include "run.h"
#include "oled.h"
#include "uart2.h"
#include "modbus.h"

static cmd_t *pcmd; //指向匹配的命令对象 

int main(void) {
	//初始化硬件
	DEVICE_Init();
	
	//中断优先级分组 分2组
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  
	
	//初始化系统主程序
	//EHOME_Run();
	//OLED_Test();
	//Modbus_03_Master();
	//主程序循环开关灯
	while(1){
		/* 不采用DMA接收数据
		//如果命令接收完毕,开始解析命令
		if(UART_Send_Flag) {
			//调试打印接收到的命令信息
			UART_Puts("Receive command is:");
			UART_Puts(UART_RxBuff);
			UART_Puts("\n");
			//解析命令
			pcmd = find_cmd((char *)UART_RxBuff);
			if(pcmd != 0)
				pcmd->call_back();
			else
				UART_Puts("invalid command!\n");
			//重新等待下一个命令到来
			UART_Send_Flag = 0;
		}
		*/
		//采用DMA接收数据
		if(UART1DMA_RxCounter != 0) {
			UART_Puts(UART1DMA_RxBuff);
			pcmd = find_cmd((char *)UART1DMA_RxBuff);
			if(pcmd != 0)
				pcmd->call_back();
			else
				UART_Puts("invalid command!\n");
			UART1DMA_RxCounter = 0;
			printf("\n");
		}
		
		//采用DMA接收UART2数据并且处理上位机主机发送来的请求
//		if(UART2_RxCounter != 0) {
//			Modbus_Handle_Slave();
//			UART2_RxCounter = 0;
//		}
		
		//采用DMA接收UART2数据并且处理从机TH01S温湿度传感器的响应
//		if(UART2_RxCounter != 0) {
//			Modbus_Handle_Master();
//			UART2_RxCounter = 0;
//		}
	}
}
