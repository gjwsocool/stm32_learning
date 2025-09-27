#include "esp8266.h"
#include "systick.h"
#include "stdio.h"
#include "string.h"
#include "mqtt.h"

//声明WIFI复位引脚初始化函数
void WIFI_ResetIO_Init(void) {
	GPIO_InitTypeDef GPIO_Config;
	
	//使能GPIOA和AFIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	//GPIOA15功能选择为普通GPIO
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
	//初始化GPIOA15复位引脚
	GPIO_Config.GPIO_Pin = GPIO_Pin_15;				//指定GPIOA15
	GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP;	//推挽输出
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz; //最大输出速率
	GPIO_Init(GPIOA, &GPIO_Config);
	
	//默认拉高不复位
	WIFI_RESET = 1;			
}

//定义WIFI复位函数
char WIFI_Reset(int timeout){
	WIFI_RESET = 0;	//拉低复位
	delay_ms(500);	//延时
	WIFI_RESET = 1;	//拉高禁止复位
	while(timeout--) {
		delay_ms(100);
		//判断WIFI模块是否复位成功，复位成功会返回ready
		if(strstr(WIFI_RxBuff, "ready"))
			break;
		printf("%d ", timeout);
	}
	printf("\r\n");
	if(timeout <= 0)
		return 1; //如果超时，说明复位失败
	return 0; //复位成功
}

//定义给WIFI发送AT指令命令
char WIFI_SendCmd(char *cmd, int timeout) {
	WIFI_RxCounter = 0;													//接收数据个数清0
	memset(WIFI_RxBuff, 0, WIFI_RXBUFF_SIZE);		//将接收缓冲区清空
	WIFI_Printf("%s\r\n", cmd);									//发送AT指令
	while(timeout--) {
		delay_ms(100);			
		//判断WIFI模块给的反馈结果是否正确
		if(strstr(WIFI_RxBuff, "OK"))
			break;
		printf("%d ", timeout);
	}
	printf("\r\n");		//打印输出结果，同步到串口终端上
	if(timeout <= 0)
		return 1; //说明接收WIFI反馈消息超时，没有能够收到OK信息，返回1，表示失败了
	return 0; //说明收到了WIFI的正常返回结果
}

//定义连接路由器函数
char WIFI_JoinAP(int timeout) {
	WIFI_RxCounter = 0;													//接收数据个数清0
	memset(WIFI_RxBuff, 0, WIFI_RXBUFF_SIZE);		//将接收缓冲区清空
	WIFI_Printf("AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PASSWD);	//给WIFI发送连接路由器的名称和密码
	while(timeout--) {
		delay_ms(1000);	 //切记延时加长点
		if(strstr(WIFI_RxBuff, "WIFI GOT IP"))
			break;
		printf("%d ", timeout);
	}
	printf("\r\n");
	if(timeout <= 0)
		return 1;
	return 0;
}

//定义连接服务器函数
char WIFI_Connect_Server(int timeout) {
	WIFI_RxCounter = 0;													//接收数据个数清0
	memset(WIFI_RxBuff, 0, WIFI_RXBUFF_SIZE);		//将接收缓冲区清空
	WIFI_Printf("AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", ServerIP, ServerPort);	//发送连接服务器AT指令
	while(timeout--) {
		delay_ms(100);
		if(strstr(WIFI_RxBuff, "CONNECT"))	//连接成功
			break;
		if(strstr(WIFI_RxBuff, "CLOSED"))		//表示服务器没有启动，没有找到服务器
			return 1;
		if(strstr(WIFI_RxBuff, "ALREADY CONNECTED")) //表示已经建立连接
			return 2;
		printf("%d ", timeout);
	}
	printf("\r\n");
	if(timeout <= 0)
		return 3;	//超时返回
	else {	//连接成功，开启数据传输
		printf("连接服务器成功,准备开始传输数据\r\n");
		WIFI_RxCounter = 0;													//接收数据个数清0
		memset(WIFI_RxBuff, 0, WIFI_RXBUFF_SIZE);		//将接收缓冲区清空
		WIFI_Printf("AT+CIPSEND\r\n");
		while(timeout--) {
			delay_ms(100);
			printf("%s\r\n", WIFI_RxBuff);
			if(strstr(WIFI_RxBuff, "\r\nOK\r\n\r\n>"))
				break;
			printf("%d ", timeout);
		}
		if(timeout <= 0)
			return 4;
	}
	return 0;
}

//定义WIFI操作的总操作函数
char WIFI_Connect_IOTServer(void) {
	printf("准备复位模块\r\n");
	if(WIFI_Reset(50)) {
		printf("复位失败,重试.\r\n");
		return 1;
	} else {
		printf("复位成功.\r\n");
	}
	
	printf("准备设置为STA模式\r\n");
	if(WIFI_SendCmd("AT+CWMODE=1", 50)) {
		printf("设置STA模式失败,重试.\r\n");
		return 2;
	} else {
		printf("设置STA模式成功.\r\n");
	}
	
	printf("准备取消自动连接\r\n");
	if(WIFI_SendCmd("AT+CWAUTOCONN=0", 50)) {
		printf("取消自动连接失败,重试.\r\n");
		return 3;
	} else {
		printf("取消自动连接成功.\r\n");
	}
	
	printf("准备连接路由器\r\n");
	if(WIFI_JoinAP(30)) {
		printf("连接路由器失败,重试\r\n");
		return 4;
	} else {
		printf("连接路由器成功.\r\n");
	}
	
	delay_ms(1000);	//等待稳定
	delay_ms(1000); //等待稳定
	
	printf("准备设置透传\r\n");
	if(WIFI_SendCmd("AT+CIPMODE=1", 50)) {
		printf("设置透传失败.\r\n");
		return 5;
	} else {
		printf("设置透传模式成功.\r\n");
	}
	
	printf("准备关闭多路连接\r\n");
	if(WIFI_SendCmd("AT+CIPMUX=0", 50)) {
		printf("关闭多路连接失败,重试\r\n");
		return 6;
	} else {
		printf("关闭多路连接成功\r\n");
	}
	
	printf("准备连接服务器\r\n");
	if(WIFI_Connect_Server(10)) {
		printf("连接服务器失败,重试\r\n");
		return 7;
	} else {
		printf("连接服务器成功，可以传输数据\r\n"); 
	}
	return 0;
}

//定义WIFI连接命令
void WIFI_Connect(void) {
	if(WIFI_Connect_IOTServer())
		WIFI_Connect(); //重新连接
}

//定义WIFI发送数据命令
void WIFI_Send_Data(void){
	char *msg = "hello,world";
	WIFI_Printf("%s", msg);
}

void X6818_TCPTest(void) {
	
}