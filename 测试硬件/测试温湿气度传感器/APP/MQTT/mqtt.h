#ifndef __MQTT_H
#define __MQTT_H

#include "stm32f10x.h"
#include "system.h"
#include "uart3.h"

#define RXBUFF_SIZE				300			//接收缓冲区长度
#define TXBUFF_SIZE				300			//发送缓冲区长度

#define MQTT_TxData(x)		UART3_TxData(x)			//通过串口3将MQTT数据发送出去

#define PRODUCTKEY				"a1zowAcMv7r"				//产品ID
#define PRODUCTKEY_LEN		strlen(PRODUCTKEY)	//产品ID的长度
#define DEVICENAME				"ehome"							//设备名称
#define DEVICENAME_LEN		strlen(DEVICENAME)	//设备名称长度
#define DEVICESECRE				"6a8f655867b06bda1db9d51d8f2e613b"	//秘钥
#define DEVICESECRE_LEN		strlen(DEVICESECRE)	//秘钥长度
#define  SUB_TOPIC_NAME         "/sys/a1zowAcMv7r/ehome/thing/service/property/set"   //订阅主题的名称 
#define  PUB_TOPIC_NAME         "/sys/a1zowAcMv7r/ehome/thing/event/property/post"    //发布主题的名称

extern unsigned char MQTT_RxDataBuf[RXBUFF_SIZE];	//存放从服务器发送来的报文数据，缓冲区第一个字节存放数据长度
extern unsigned char MQTT_TxDataBuf[TXBUFF_SIZE];	//存放发往服务器的数据,缓冲区第一个字节存放数据长度
extern unsigned char MQTT_CmdBuf[TXBUFF_SIZE];		//存放服务器发送来的推送消息中的有效载荷信息,

extern char Clientid[128];	//存放客户端ID的缓冲区
extern int Clientid_len;		//客户端ID的长度
extern char Username[128];	//存放用户名的缓冲区
extern int Username_len;		//存放用户名的长度
extern char Passwd[128];		//存放密码缓冲区
extern int Passwd_len;			//存放密码的长度
extern char ServerIP[128];	//存放服务器IP地址
extern int ServerPort;			//存放端口号

extern void AliIoT_Parameter_Init(void); //初始化连接阿里云的登录信息:客户端ID，用户名，密码
extern void MQTT_ConnectPack(void);	//构造连接报文
extern void MQTT_Subscribe(char *topic_name, int QoS); //构造订阅报文，第一个参数表示订阅的主题名称，第二个参数服务等级
extern void MQTT_Publish(char *topic, char *data, int data_len); //构造发布报文,主题名称,发布的数据信息,数据长度
extern void MQTT_PingREQ(void);	//构造心跳报文
extern void MQTT_Push_Handle(unsigned char *redata);	//处理推送报文

extern void MQTT_Connect_Cmd(void);	//建立连接命令函数
extern void MQTT_Sub_Cmd(void);	//发布订阅命令函数
extern void MQTT_PublishTemp_Cmd(void); //发布温度命令
extern void MQTT_Ping_Cmd(void); //发送心跳包命令

extern void MQTT_PublishLedState_Cmd(void); //发布灯的状态命令
extern void MQTT_Ctrl_LedState(void); //处理开关灯命令函数

extern void MQTT_PublishBeepState_Cmd(void); //发布灯的状态命令
extern void MQTT_Ctrl_BeepState(void); //处理开关灯命令函数
#endif
