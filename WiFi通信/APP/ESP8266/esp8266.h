// esp8266.h
#ifndef __ESP8266_H_
#define __ESP8266_H_

#include "stm32f10x.h"
#include "system.h"
#include "usart3.h"

#define WIFI_RESET        PAout(15)

#define WIFI_Printf             UART3_Puts
#define WIFI_RxCounter          UART3_RxCounter
#define WIFI_RxBuffer           UART3_RxBuff
#define WIFI_RXBUFF_SIZE        UART3_RXBUFF_SIZE

// 定义连接信息 - 每个人不同
#define SSID         "CMCC"
#define PASSWD       "729831hh"
// 服务器的IP地址, 电脑启动NetAssist.exe, 启动服务器, 服务器的IP地址 
// 和wifi模块在同一个网段下 
#define ServerIP     "192.168.24.159"
#define ServerPort   8000

//////////////////////////////////////////////
extern void WIFI_ResetIO_Init(void);

extern u8   WIFI_Reset(int timeout); 
extern u8   WIFI_SendCmd(u8* cmd, int timeout);
extern u8   WIFI_JoinAP(int timeout);
extern u8   WIFI_Connect_Server(int timeout);
extern u8   WIFI_Connect_IOTServer(void); 

// 测试函数
extern void WIFI_Connect(void);
extern void WIFI_Send_Data(void); 
#endif