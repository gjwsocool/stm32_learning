// esp8266.c
#include "esp8266.h"
#include "Systick.h"
#include "string.h" // strstr
#include "stdio.h" 

// 初始化PA15引脚
void WIFI_ResetIO_Init(void){
	// 1.打开GPIOA控制器的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	// 2.配置PA15作为普通引脚使用 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
	// 2.配置PA15 推挽输出, 50MHz
	GPIO_InitTypeDef GPIO_Config;
	GPIO_Config.GPIO_Pin = GPIO_Pin_15;
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Config.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_Config);
	
	// 3.默认将其拉高 (没有复位)
	WIFI_RESET = 1;
}

// 参数 : 延时时间 
u8   WIFI_Reset(int timeout){
	WIFI_RESET = 0; // 复位 
	delay_ms(500);
	WIFI_RESET = 1; // 取消复位 
	
	while (timeout--) {
		delay_ms(100); 
		if (strstr((char*)WIFI_RxBuffer, "ready"))
			break;
		printf("%d ", timeout);
	}
	printf("\n");
	
	if (timeout <= 0)
		return 1; // 复位失败
	else
		return 0; // 复位成功
}

// a1 : 要发送的命令
// a2 : 延时时间
u8   WIFI_SendCmd(u8* cmd, int timeout){
	WIFI_RxCounter = 0;
	memset(WIFI_RxBuffer, 0, WIFI_RXBUFF_SIZE);
	
	// 将AT指令发送给WiFi模块
	WIFI_Printf("%s\r\n", cmd);
	while (timeout--) {
		delay_ms(100); 
		if (strstr((char*)WIFI_RxBuffer, "OK"))
			break;
		printf("%d ", timeout);
	}
	printf("\n");
	
	if (timeout <= 0)
		return 1; // 复位失败
	else
		return 0; // 复位成功
}

// 加入AP热点 
// AT+CWJAP="zjyab","123456abc"
u8   WIFI_JoinAP(int timeout){
	WIFI_RxCounter = 0;
	memset(WIFI_RxBuffer, 0, WIFI_RXBUFF_SIZE);
	
	// 将AT指令发送给WiFi模块
	WIFI_Printf("AT+CWJAP=\"%s\",\"%s\"\r\n",
																		SSID, PASSWD);
	while (timeout--) {
		delay_ms(1000); 
		if (strstr((char*)WIFI_RxBuffer, "WIFI GOT IP"))
			break;
		printf("%d ", timeout);
	}
	printf("\n");
	
	if (timeout <= 0)
		return 1; // 复位失败
	else
		return 0; // 复位成功
}

// AT+CIPSTART="TCP","IP地址",8080
u8   WIFI_Connect_Server(int timeout){
	int timeout2 = timeout; // 备份timeout, 为了第二条AT指令使用
	WIFI_RxCounter = 0;
	memset(WIFI_RxBuffer, 0, WIFI_RXBUFF_SIZE);
	
	// 将AT指令发送给WiFi模块
	WIFI_Printf("AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",
														ServerIP, ServerPort);
	while (timeout--) {
		delay_ms(100); 
		if (strstr((char*)WIFI_RxBuffer, "CONNECT"))
			break;
		if (strstr((char*)WIFI_RxBuffer, "CLOSED"))
			return 1; // 返回1, 服务器关闭, 错误
		printf("%d ", timeout);
	}
	printf("\n");
	
	if (timeout <= 0)
		return 2; // 返回2, 超时结束, 错误
	else // 连接服务器成功
	{
		printf("connect server success(8266)\n");
		
		// 发送 AT+CIPSEND 指令 
		WIFI_RxCounter = 0;
		memset(WIFI_RxBuffer, 0, WIFI_RXBUFF_SIZE);
		
		// 将AT指令发送给WiFi模块
		WIFI_Printf("AT+CIPSEND\r\n");
		while (timeout2--) {
			delay_ms(100); 
			if (strstr((char*)WIFI_RxBuffer, "OK"))
				break;
			printf("%d ", timeout2);
		}
		printf("\n");
		
		if (timeout2 <= 0)
			return 3; // 返回3, 发送失败
	}
	
	return 0; // 返回0, 成功
}
u8   WIFI_Connect_IOTServer(void){
	// 1.复位
	if (WIFI_Reset(50)) {
		printf("reset failed\n");
		return 1;
	}
	printf("reset success\n");
	
	// 2.设置STA模式
	if (WIFI_SendCmd("AT+CWMODE=1", 50)) {
		printf("set sta mode failed\n");
		return 2;
	}
	printf("set sta mode success\n");
	
	// 3.设置上电不自动连接
	if (WIFI_SendCmd("AT+CWAUTOCONN=0", 50)) {
		printf("cancel auto conn failed\n");
		return 3;
	}
	printf("cancel auto conn success\n");
	
	// 4.连接ap热点
	if (WIFI_JoinAP(30)) {
		printf("connect router failed\n");
		return 4;
	}
	delay_ms(1000);
	delay_ms(1000);
	printf("connect router success\n");
	
	// 5.设置透传模式
	if (WIFI_SendCmd("AT+CIPMODE=1", 50)) {
		printf("set direct mode failed\n");
		return 5;
	}
	printf("set direct mode success\n");
	
	// 6.设置单连接
	if (WIFI_SendCmd("AT+CIPMUX=0", 50)) {
		printf("set single mode failed\n");
		return 6;
	}
	printf("set single mode success\n");
	// 7.连接服务器+通信
	if (WIFI_Connect_Server(10)) {
		printf("connect server failed\n");
		return 7;
	}
	printf("connect server success(7)\n");
	
	return 0; 
}

// 测试函数
void WIFI_Connect(void){
	if (WIFI_Connect_IOTServer())
		WIFI_Connect();
}
void WIFI_Send_Data(void){
	char* msg = "hello, esp8266\n";
	WIFI_Printf("%s", msg);
}