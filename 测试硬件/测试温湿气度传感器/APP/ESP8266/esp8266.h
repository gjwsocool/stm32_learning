#ifndef __ESP8266_H
#define __ESP8266_H

#include "stm32f10x.h"
#include "system.h"
#include "uart3.h"

#define WIFI_RESET			PAout(15)					//指定ESP8266复位引脚的位带信息

#define WIFI_Printf				UART3_Puts				//WiFi发送数据函数
#define WIFI_RxCounter		UART3_RxCounter		//记录WIFI接收数据的个数
#define WIFI_RxBuff				UART3_RxBuff			//指定WIFI接收数据缓冲区首地址
#define WIFI_RXBUFF_SIZE	UART3_RXBUFF_SIZE	//指定WIFI接收缓冲区的大小

#define SSID			"youcw123"										//指定路由器的SSID名称
#define PASSWD		"asd.1234"								//指定路由器密码
//#define ServerIP	"192.168.95.134"							//指定服务器IP地址
//#define ServerPort 8080											//指定端口号

extern void WIFI_ResetIO_Init(void);					//声明WIFI复位引脚初始化函数
extern char WIFI_SendCmd(char *cmd, int timeout);	//声明给WIFI发送AT指令函数
extern char WIFI_Reset(int timeout);	//声明WIFI复位函数
extern char WIFI_JoinAP(int timeout);	//声明连接路由器函数
extern char WIFI_Connect_Server(int timeout); //声明连接服务器函数
extern char WIFI_Connect_IOTServer(void);		//声明连接服务器总入口函数
extern void WIFI_Connect(void); //声明连接服务器命令
extern void WIFI_Send_Data(void); //声明发送数据命令

extern void X6818_TCPTest(void);
#endif
