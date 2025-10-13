#include "mqtt.h"
#include "string.h"
#include "stdio.h"
#include "uart.h"
#include "utils_hmac.h"
#include "esp8266.h"
#include "systick.h"
#include "ds18b20.h"
#include "led.h"
#include "uart3.h"
#include "key.h"
#include "beep.h"

unsigned char MQTT_RxDataBuf[RXBUFF_SIZE]; //分配接收缓冲区，暂存从服务器发送来的数据
unsigned char MQTT_TxDataBuf[TXBUFF_SIZE]; //分配接收缓冲区，暂存向服务器发送的数据
unsigned char MQTT_CmdBuf[TXBUFF_SIZE];		 //分配缓冲区，暂存发送的命令信息

char Clientid[128];		//存放客户端ID的缓冲区
int Clientid_len;			//存放客户端ID的长度

char Username[128];		//存放用户名的缓冲区
int Username_len;			//存放用户名长度

char Passwd[128];			//存放密码的缓冲区
int Passwd_len;				//存放密码长度

char ServerIP[128];		//存放服务器IP地址,此信息会给ESP8266使用，所以别忘记修改ESP8266.c/.h
int ServerPort;				//存放端口号

int Fixed_len;				//存放固定报头长度
int Variable_len;			//存放可变报头长度
int Payload_len;			//存放有效载荷长度
unsigned char temp_buff[TXBUFF_SIZE];	//临时缓冲区，构建报文

//初始化登录阿里云服务器参数，得到客户端id，用户名和密码
void AliIoT_Parameter_Init(void) {
	char temp[128];		//临时缓冲区，用于加密时使用,存放加密对象
	
	memset(Clientid, 0, 128);	//将存放客户端id的缓冲区清0
	sprintf(Clientid, "%s|securemode=3,signmethod=hmacsha1|", DEVICENAME); //构建登录使用的客户端ID信息存入缓冲区Clientid
	Clientid_len = strlen(Clientid);	//计算客户端id的长度
	
	memset(Username, 0, 128);	
	sprintf(Username, "%s&%s", DEVICENAME, PRODUCTKEY); //构造登录使用的用户名
	Username_len = strlen(Username);
	
	memset(temp, 0, 128);
	sprintf(temp, "clientId%sdeviceName%sproductKey%s", DEVICENAME, DEVICENAME, PRODUCTKEY); //构造加密对象
	//利用hmacsha1加密算法并且结合devicesecre秘钥对构造好的加密对象temp进行加密，得到的passwd保存到缓冲区Passwd中
	utils_hmac_sha1(temp, strlen(temp), Passwd, DEVICESECRE, DEVICESECRE_LEN);
	Passwd_len = strlen(Passwd);
	
	memset(ServerIP, 0, 128);
	sprintf(ServerIP, "%s.iot-as-mqtt.cn-shanghai.aliyuncs.com", PRODUCTKEY); //构造服务器IP地址
	ServerPort = 1883; //构造端口号
	
	printf("SIP&Port:%s:%d\r\n", ServerIP, ServerPort);
	printf("Clientid:%s\r\n", Clientid);
	printf("Username:%s\r\n", Username);
	printf("Passward:%s\r\n", Passwd);
}

//定义构造连接报文函数
void MQTT_ConnectPack(void) {
	int temp, Remaining_len; //记录剩余长度
	
	//为了构造固定报头中的剩余长度信息
	Fixed_len = 1; //固定报头总长度暂且定义为1
	Variable_len = 10;	//连接报文中可变报头长度=10
	Payload_len = 2 + Clientid_len + 2 + Username_len + 2 + Passwd_len;	//连接报文中的负载长度
	Remaining_len = Variable_len + Payload_len;	//剩余长度=可变报头长度+负载长度
	
	//构造固定报头第1个字节为:0x10(byte1)
	temp_buff[0] = 0x10;		

	//构造固定报头中的剩余长度(byte2....)
	do {
		temp = Remaining_len%128;	
		Remaining_len = Remaining_len / 128;
		//如果存在高字节，则将低字节的bit7置1，表示有进位
		if(Remaining_len > 0)	
			temp |= 0x80;
		temp_buff[Fixed_len] = temp;	//将剩余长度的低字节放到报文中
		Fixed_len++;	//更新固定报头总长度：byte1+byte2+byte3+....+byten											   
	}while(Remaining_len > 0);

	//构造可变报头
	temp_buff[Fixed_len + 0] = 0x00;	//可变报头第1字节:固定0x00
	temp_buff[Fixed_len + 1] = 0x04;	//可变报头第2字节：固定0x04
	temp_buff[Fixed_len + 2] = 0x4D;	//可变报头第3字节：固定0x4D
	temp_buff[Fixed_len + 3] = 0x51;	//可变报头第4字节：固定0x51
	temp_buff[Fixed_len + 4] = 0x54;	//可变报头第5字节:固定0x54
	temp_buff[Fixed_len + 5] = 0x54;	//可变报头第6字节：固定0x54
	temp_buff[Fixed_len + 6] = 0x04;	//可变报头第7字节：固定0x04
	temp_buff[Fixed_len + 7] = 0xC2;	//可变报头第8字节：使用用户名和密码校验，不用遗嘱，不保留会话
	temp_buff[Fixed_len + 8] = 0x00;	//可变报头第9字节：保持连接时间高字节
	temp_buff[Fixed_len + 9] = 0x64;	//可变报头第10字节：保持连接时间低字节，100s
	
	//构造有效载荷
	temp_buff[Fixed_len + 10] = Clientid_len / 256; //添加clientid长度的高字节
	temp_buff[Fixed_len + 11] = Clientid_len % 256; //添加clientid长度的低字节
	memcpy(&temp_buff[Fixed_len + 12], Clientid, Clientid_len); //添加clientid信息
	
	temp_buff[Fixed_len + 12 + Clientid_len] = Username_len / 256; //添加用户名长度的高字节
	temp_buff[Fixed_len + 13 + Clientid_len] = Username_len % 256; //添加用户名长度的低字节
	memcpy(&temp_buff[Fixed_len + 14 + Clientid_len], Username, Username_len); //添加用户名信息
	
	temp_buff[Fixed_len+14+Clientid_len+Username_len] = Passwd_len / 256; //添加密码长度的高字节
	temp_buff[Fixed_len+15+Clientid_len+Username_len] = Passwd_len % 256; //添加密码长度的低字节
	memcpy(&temp_buff[Fixed_len+16+Clientid_len+Username_len], Passwd, Passwd_len); //添加密码信息
	
	//将构造好的连接报文数据添加到发送缓冲区中
	int total_size = Fixed_len + Variable_len + Payload_len;
	MQTT_TxDataBuf[0] = total_size >> 8; //发送缓冲区第1字节存储报文长度的高字节
	MQTT_TxDataBuf[1] = total_size & 0xFF; //发送缓冲区第2字节存储报文长度的低字节
	memcpy(&MQTT_TxDataBuf[2], temp_buff, total_size); //将连接报文拷贝到发送缓冲区中，从第3个字节开始存储
	
	//恭喜：至此连接报文构造完毕！
}

//定义构造订阅报文函数
void MQTT_Subscribe(char *topic_name, int QoS) {
	int temp, Remaining_len;
	
	Fixed_len = 1; //订阅报文的固定报头长度暂定为1
	Variable_len = 2; //订阅报文的可变报头长度2字节
	Payload_len = 2 + strlen(topic_name) + 1; //订阅报文有效载荷长度=2(topic_name长度)+topic_name字符串长度+1字节服务等级
	Remaining_len = Variable_len + Payload_len;	//剩余长度=可变报头长度+负载长度
	
	//构造订阅报文固定报头的低1字节数据
	temp_buff[0] = 0x82;	//第1字节固定为0x82
	
	//构造固定报头中的剩余长度(byte2....)
	do {
		temp = Remaining_len%128;	
		Remaining_len = Remaining_len / 128;
		//如果存在高字节，则将低字节的bit7置1，表示有进位
		if(Remaining_len > 0)	
			temp |= 0x80;
		temp_buff[Fixed_len] = temp;	//将剩余长度的低字节放到报文中
		Fixed_len++;	//更新固定报头总长度：byte1+byte2+byte3+....+byten											   
	}while(Remaining_len > 0);
	
	//构造可变报头
	temp_buff[Fixed_len + 0] = 0x00;	//报文标识符高字节
	temp_buff[Fixed_len + 1] = 0x0A;	//报文标识符低字节
	
	//构造有效载荷
	temp_buff[Fixed_len + 2] = strlen(topic_name)/256; //订阅主题名称长度的高字节
	temp_buff[Fixed_len + 3] = strlen(topic_name)%256; //订阅主题名称长度的低字节
	memcpy(&temp_buff[Fixed_len + 4], topic_name, strlen(topic_name)); //添加主题名称
	temp_buff[Fixed_len+4+strlen(topic_name)] = QoS; //添加服务等级
	
	//将构造好的连接报文数据添加到发送缓冲区中
	int total_size = Fixed_len + Variable_len + Payload_len;
	MQTT_TxDataBuf[0] = total_size >> 8; //发送缓冲区第1字节存储报文长度的高字节
	MQTT_TxDataBuf[1] = total_size & 0xFF; //发送缓冲区第2字节存储报文长度的低字节
	memcpy(&MQTT_TxDataBuf[2], temp_buff, total_size); //将连接报文拷贝到发送缓冲区中，从第3个字节开始存储
	
	//恭喜：至此订阅报文构造完毕！
}

//定义构造发布消息
void MQTT_Publish(char *topic, char *data, int data_len) {
	int temp, Remaining_len;
	
	Fixed_len = 1;	//发布消息固定报头长度暂定为1
	Variable_len = 2 + strlen(topic); //可变报头长度=2字节topic长度+topic字符串长度，不用报文标识符
	Payload_len = data_len;	//有效载荷就是数据长度
	Remaining_len = Variable_len + Payload_len; //剩余长度 = 可变报头长度+有效载荷长度
	
	//构造发布报文的第1个字节固定0x30
	temp_buff[0] = 0x30;
	
	//构造发布报文的剩余长度信息
	do {
		temp = Remaining_len % 128;
		Remaining_len = Remaining_len / 128;
		if(Remaining_len > 0)
			temp |= 0x80;
		temp_buff[Fixed_len] = temp;
		Fixed_len++;
	}while(Remaining_len > 0);

	//构造发布报文的可变报文信息
	temp_buff[Fixed_len + 0] = strlen(topic)/256;	//添加发布主题长度的高字节
	temp_buff[Fixed_len + 1] = strlen(topic)%256; //添加发布主题长度的低字节
	memcpy(&temp_buff[Fixed_len + 2], topic, strlen(topic)); //添加主题字符串
	
	//构造发布报文的有效载荷信息
	memcpy(&temp_buff[Fixed_len+2+strlen(topic)], data, data_len); //添加要发布的数据信息
	
	//将构造好的连接报文数据添加到发送缓冲区中
	int total_size = Fixed_len + Variable_len + Payload_len;
	MQTT_TxDataBuf[0] = total_size >> 8; //发送缓冲区第1字节存储报文长度的高字节
	MQTT_TxDataBuf[1] = total_size & 0xFF; //发送缓冲区第2字节存储报文长度的低字节
	memcpy(&MQTT_TxDataBuf[2], temp_buff, total_size); //将连接报文拷贝到发送缓冲区中，从第3个字节开始存储
	
	//恭喜：至此发布报文构造完毕！
}

//定义构造心跳报文
void MQTT_PingREQ(void) {
	temp_buff[0] = 0xC0; //固定
	temp_buff[1] = 0x00; //固定
	
	//将构造好的连接报文数据添加到发送缓冲区中
	int total_size = 2;
	MQTT_TxDataBuf[0] = total_size >> 8; //发送缓冲区第1字节存储报文长度的高字节
	MQTT_TxDataBuf[1] = total_size & 0xFF; //发送缓冲区第2字节存储报文长度的低字节
	memcpy(&MQTT_TxDataBuf[2], temp_buff, total_size); //将连接报文拷贝到发送缓冲区中，从第3个字节开始存储
}

//定义处理从服务器上获取的推送消息，将推送消息中的有效载荷提取上来
/*例如：
固定头 剩余长度  主题长度  主题名字(/sys/a1npjYXsMPJ/ehome/thing/service/property/set)
30     95 01     00 31     2F 73 79 73 2F 61 31 6E 70 6A 59 58 73 4D 50 4A 2F 65 68 6F 6D 65 2F 74 68 69 6E 67 2F 73 
65 72 76 69 63 65 2F 70 72 6F 70 65 72 74 79 2F 73 65 74 
主题推送来的消息内容：{"method":"thing.service.property.set","id":"1225891422","params":{"bed_rom":1},"version":"1.0.0"}
7B 22 6D 65 74 68 6F 64 22 3A 22 74 68 69 6E 67 2E 73 65 72 76 69 63 65 2E 70 72 6F 70 65 72 74 79 2E 73 
65 74 22 2C 22 69 64 22 3A 22 31 32 32 35 38 39 31 34 32 32 22 2C 22 70 61 72 61 6D 73 22 3A 7B 22 62 65 
64 5F 72 6F 6D 22 3A 31 7D 2C 22 76 65 72 73 69 6F 6E 22 3A 22 31 2E 30 2E 30 22 7D
*/

void MQTT_Push_Handle(unsigned char *redata) {
	unsigned char *data = redata; //data指向接收的消息
	int Remaining_len = 0; //保存推送消息中的剩余长度信息
	int index = 1; //剩余长度的起始位置
	int temp; //暂存剩余长度中的某个字节数据
	int mul = 1; //倍数：*1/ *128/ *128*128/ ...
	int total_len = UART3_RxCounter; //获取总长度
	int topic_len_msb; //主题长度的高字节
	int topic_len_lsb; //主题长度的低字节
	int topic_len;		 //主题长度
	int payload_index; //有效载荷的起始偏移量
	int payload_len;	 //有效载荷的长度
	
	//换算剩余长度
	do {
		temp = data[index]; 
		Remaining_len += (temp & 0x7F) * mul;
		mul *= 128;
		index++;
	}while((temp & 0x80) != 0);
	
	//此时index指向主题长度的首地址,换算主题长度
	topic_len_msb = data[index++];
	topic_len_lsb = data[index++];
	topic_len = topic_len_msb << 8 | topic_len_lsb;
	
	//换算有效载荷长度和起始偏移量
	payload_index = index + topic_len;
	payload_len = Remaining_len - topic_len;
	
	printf("total_len = %d, Remaining_len = %d, topic_len = %d, payload_len = %d\n",
																	total_len, Remaining_len, topic_len, payload_len);
	
	//从接收的数据中获取有效载荷拷贝到MQTT_CmdBuf缓冲区，将来用于解析发送过来的开关灯命令
	//例如：MQTT_CmdBuf="{"method":"thing.service.property.set","id":"1225891422","params":{"bed_rom":1},"version":"1.0.0"}"
	memcpy(MQTT_CmdBuf, &data[payload_index], payload_len);
	MQTT_CmdBuf[payload_len - 2] = '\0'; //对方发送的数据"xxxxx\r\n\0"
}

//定义建立连接命令
void MQTT_Connect_Cmd(void) {
	//1.第一步：构造连接报文
	MQTT_ConnectPack();	
	//2.第二步：发送连接报文
	MQTT_TxData(MQTT_TxDataBuf);
	//3.延时等待服务器反馈
	delay_ms(1000);
	//4.判断服务器的反馈消息,如果是CONNECT ACK消息并且反馈的消息是连接成功
	if((MQTT_RxDataBuf[0] == 0x20) && (MQTT_RxDataBuf[3] == 0x00))
		printf("CONNECT Successfully!\r\n");
}

//定义订阅消息命令函数
void MQTT_Sub_Cmd(void) {
	//1.第一步：构造订阅报文,采用QOS0等级
	MQTT_Subscribe(SUB_TOPIC_NAME, 0);
	
	//2.第二步：发送订阅报文
	MQTT_TxData(MQTT_TxDataBuf);
	
	//3.等待服务器反馈
	delay_ms(1000);
	
	//4.判断是否订阅成功
	if((MQTT_RxDataBuf[0] == 0x90) && (MQTT_RxDataBuf[4] == 0x01))
		printf("Subscribe Successfully！\r\n");
}

//定义发布温度信息命令函数
void MQTT_PublishTemp_Cmd(void) {
	char payload[256];	//存储有效载荷信息
	u8 tempvalue; //存储温度值
	tempvalue = DS18B20_GetTemperture(); //获取温度值
	printf("Current temperture is : %d\n", tempvalue);
	//构造有效载荷
	sprintf(payload, "{\"method\":\"thing.event.property.post\","
										"\"params\":{\"CurrentTemperature\":%d,\"is_thief\":%d}}", 
										tempvalue, KEY_UP);
	//构造发布消息报文
	MQTT_Publish(PUB_TOPIC_NAME, payload, strlen(payload));
	//发送发布消息
	MQTT_TxData(MQTT_TxDataBuf);
}

//定义发送心跳包命令函数
void MQTT_Ping_Cmd(void) {
	//构造心跳报文
	MQTT_PingREQ();
	//发布心跳报文
	MQTT_TxData(MQTT_TxDataBuf);
}

//定义发布LED开关状态的命令函数
void MQTT_PublishLedState_Cmd(void) {
	char payload[256];
	//构造有效载荷,万千不要上传id和version信息，会导致信息紊乱
	sprintf(payload, "{\"params\":{\"bed_rom\":%d,\"food_rom\":%d},"
										"\"method\":\"thing.event.property.post\"}", !LED1, !LED2);
		//构造发布消息报文
	MQTT_Publish(PUB_TOPIC_NAME, payload, strlen(payload));
	//发送发布消息
	MQTT_TxData(MQTT_TxDataBuf);
}

//定义推送的开关灯命令处理函数,注意：必须每次发布消息就需要重新判断一次
void MQTT_Ctrl_LedState(void) {
	if(strstr((char *)MQTT_CmdBuf,"\"params\":{\"bed_rom\":1}")) {
		LED1 = 0; //硬件打开灯
		MQTT_PublishLedState_Cmd(); //发布灯的状态
	}
	else if(strstr((char *)MQTT_CmdBuf,"\"params\":{\"bed_rom\":0}")) {
		LED1 = 1;
		MQTT_PublishLedState_Cmd(); //发布灯的状态
	}
	if(strstr((char *)MQTT_CmdBuf,"\"params\":{\"food_rom\":1}")) {
		LED2 = 0;
		MQTT_PublishLedState_Cmd(); //发布灯的状态
	}
	else if(strstr((char *)MQTT_CmdBuf,"\"params\":{\"food_rom\":0}")) {
		LED2 = 1;
		MQTT_PublishLedState_Cmd(); //发布灯的状态
	}
} //问：MQTT_CmdBuf接收的推送消息代码是通过UART3串口中断来完成，此时编写UART3串口中断

//定义发布LED开关状态的命令函数
void MQTT_PublishBeepState_Cmd(void) {
	char payload[256];
	//构造有效载荷,万千不要上传id和version信息，会导致信息紊乱
	sprintf(payload, "{\"params\":{\"beep_ctrl\":%d},\"method\":\"thing.event.property.post\"}", BEEP);
		//构造发布消息报文
	MQTT_Publish(PUB_TOPIC_NAME, payload, strlen(payload));
	//发送发布消息
	MQTT_TxData(MQTT_TxDataBuf);
}

//定义推送的开关灯命令处理函数,注意：必须每次发布消息就需要重新判断一次
void MQTT_Ctrl_BeepState(void) {
	if(strstr((char *)MQTT_CmdBuf,"\"params\":{\"beep_ctrl\":1}")) {
		BEEP = 1; //硬件打开蜂鸣器
		MQTT_PublishBeepState_Cmd(); //发布灯的状态
	}
	else if(strstr((char *)MQTT_CmdBuf,"\"params\":{\"beep_ctrl\":0}")) {
		BEEP = 0; //硬件关闭蜂鸣器
		MQTT_PublishBeepState_Cmd(); //发布灯的状态
	}
} //问：MQTT_CmdBuf接收的推送消息代码是通过UART3串口中断来完成，此时编写UART3串口中断








