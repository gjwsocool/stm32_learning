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

static cmd_t *pcmd; //指向匹配的命令对象 

int main(void) {
	//初始化硬件
	DEVICE_Init();
	
	//中断优先级分组 分2组
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  
	
	//初始化系统主程序
	//EHOME_Run();
	
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
		
		//循环判断从上位机接收到的推送消息并且解析
		if(UART3_RxCounter != 0) {
			if(MQTT_RxDataBuf[0] == 0x30) { //判断是否是推送消息
				MQTT_Push_Handle(MQTT_RxDataBuf); //获取推送消息中的有效载荷，将有效载荷拷贝到MQTT_CmdBuf中将来解析
				printf("Push message: %s\n", MQTT_CmdBuf);
				MQTT_Ctrl_LedState(); //解析推送的开关灯命令
				MQTT_Ctrl_BeepState(); //解析推送的蜂鸣器开关命令
			}
			
			//判断是否是心跳反馈消息
			if(MQTT_RxDataBuf[0] == 0xD0) {
				printf("PING RESPONE.\n");
			}
			UART3_RxCounter = 0; //重新接收下一个推送消息
		}
	}
}
