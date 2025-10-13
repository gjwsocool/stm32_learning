#include "run.h"
#include "esp8266.h"
#include "mqtt.h"
#include "ds18b20.h"
#include "timer3.h"
#include "systick.h"

//定义总的启动入口函数
void EHOME_Run(void) {
	//第一步：先获取DS18B20 ROM值
	DS18B20_ReadRom();
	//第二步：连接路由器
	WIFI_Connect();
	//第三步：连接阿里服务器
	MQTT_Connect_Cmd();
	//第四步：发布订阅消息
	MQTT_Sub_Cmd();
	//第五步：发布灯的开关状态和蜂鸣器开关状态消息
	MQTT_PublishLedState_Cmd();
	delay_ms(1000); //务必延时，否则连接不上
	delay_ms(1000); //务必延时，否则连接不上
	MQTT_PublishBeepState_Cmd();
	//第六步：启动定时器，每隔1秒发布一次温度值
	TIMER3_ENALBE_1S();
}
