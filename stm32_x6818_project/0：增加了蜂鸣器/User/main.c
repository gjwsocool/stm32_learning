#include "uart.h"				//声明串口1
#include "string.h"			//声明字符串操作库函数
#include "init.h"
#include "cmd.h"
#include "interrupt.h"
#include "dma.h"
#include "esp8266.h"
#include "stdio.h"
#include "msg.h"
#include "led.h"
#include "ds18b20.h"
#include "FreeRTOS.h"
#include "beep.h"
#include "task.h"

#define BUF_LEN			100
//分配内存暂存从上位机接收到的字符串
static u8 buf[BUF_LEN];
//暂存匹配成功的命令结构体首地址
static cmd_t *pcmd;

void vTaskForTemp(void *arg) {
	while(1) {
		const TickType_t xTicksToWait = pdMS_TO_TICKS(5000);
		
		temp_req_t temp_req;
		temp_req.msgh.msgid = STM32_TO_X6818_TEMP_REQ;
		temp_req.temp = DS18B20_GetTemperture();
		UART3_TxData((unsigned char *)&temp_req, sizeof(temp_req));
		printf("**** temp = %.2f ******\n", temp_req.temp);
		
		vTaskDelay(xTicksToWait);
	}
}

void vTaskForMUL(void* arg){
	while(1) {
		const TickType_t xTicksToWait = pdMS_TO_TICKS(50);
		
		if(WIFI_RxCounter != 0) {
			msghead_t msgh;
			memcpy(&msgh, WIFI_RxBuff, sizeof(msgh));
			printf("*****stm32 get REQ msgid = %d ********\n", msgh.msgid);
			switch(msgh.msgid){
				case X6818_TO_STM32_LED_REQ:{
					  led_req_t led_req;
				   	memcpy(&led_req, WIFI_RxBuff, sizeof(led_req));
				  	if(led_req.index == 1 && led_req.cmd == 1)
						LED1 = 0;
					else if(led_req.index == 1 && led_req.cmd == 0)
						LED1 = 1;
					else if(led_req.index == 2 && led_req.cmd == 1)
						LED2 = 0;
					else if(led_req.index == 2 && led_req.cmd == 0)
						LED2 = 1;
					
					led_rsp_t led_rsp;
					led_rsp.msgh.msgid = STM32_TO_X6818_LED_RSP;
					led_rsp.success = 1;
					UART3_TxData((unsigned char *)&led_rsp, sizeof(led_rsp));
				}break;
				
				case X6818_TO_STM32_BEEP_REQ:{
					beep_req_t beepReq;
				memcpy(&beepReq,WIFI_RxBuff,sizeof(beepReq));
				//控制蜂鸣器
				if(beepReq.cmd == 0){
					BEEP=0;
				}else if(beepReq.cmd == 1){
					BEEP=1;
				}
				/*发送反馈消息给x6818*/
				beep_rsp_t beepRsp;
				beepRsp.msgh.msgid=STM32_TO_X6818_BEEP_RSP;
			    beepRsp.success=1;
				UART3_TxData((unsigned char*)&beepRsp,sizeof(beepRsp));
				} break;
			}
						
			WIFI_RxCounter = 0;	//等待接收下一个数据
			memset(WIFI_RxBuff, 0, WIFI_RXBUFF_SIZE); //情况接收缓冲区准备存储下一个数据
		}
		vTaskDelay(xTicksToWait);
	}
}

int main(void) {
	//硬件初始化
	DEVICE_Init();
	
	//指定中断优先级分组，分2组,表示抢占式优先级和子优先级的级别都是2，值分别是0~3
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	//读取ROM值
	DS18B20_ReadRom();
	
	//连接X6818服务器
	WIFI_Connect();
	
	//下位机循环给上位机发送字符串	
	//创建任务1
	xTaskCreate(vTaskForTemp,"Temp", 100, NULL, 1, NULL);
	xTaskCreate(vTaskForMUL,"MUL" ,  200, NULL, 2, NULL);
	//启动调度器，任务开始执行
	vTaskStartScheduler();

}

