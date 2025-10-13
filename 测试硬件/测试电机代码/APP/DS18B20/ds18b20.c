#include "ds18b20.h"
#include "systick.h"
#include "stdio.h"

//定义初始化函数
void DS18B20_Init(void) {
	GPIO_InitTypeDef GPIO_Config;
	
	//使能GPIOG时钟
	RCC_APB2PeriphClockCmd(DS18B20_PORT_RCC, ENABLE);
	
	//初始化GPIOG11端口
	GPIO_Config.GPIO_Pin = DS18B20_PIN;
	GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DS18B20_PORT, &GPIO_Config);
}

//定义配置GPIOG11为输入功能
void DS18B20_IO_IN(void) {
	GPIO_InitTypeDef GPIO_Config;
	
	GPIO_Config.GPIO_Pin = DS18B20_PIN;
	GPIO_Config.GPIO_Mode = GPIO_Mode_IPU; //上拉输入
	GPIO_Init(DS18B20_PORT, &GPIO_Config);
}

//定义配置GPIOG11为输出功能
void DS18B20_IO_OUT(void) {
	GPIO_InitTypeDef GPIO_Config;
	
	GPIO_Config.GPIO_Pin = DS18B20_PIN;
	GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DS18B20_PORT, &GPIO_Config);
}

//定义复位函数
void DS18B20_Reset(void) {
	DS18B20_IO_OUT();
	DS18B20_DQ_OUT = 0;
	delay_us(500);
	DS18B20_DQ_OUT = 1;
	delay_us(30);
	
	//检测是否握手成功
	u8 retry = 0;
	DS18B20_IO_IN();
	while(DS18B20_DQ_IN && retry < 240) {
		retry++;
		delay_us(1);
	}
	if(retry >= 240)
		printf("Reset Failed.\n");
	else {
		printf("Reset Successed.\n");
		retry = 0;
	}
}

//定义写1字节函数
void DS18B20_Write_Byte(u8 data) {
	u8 i;
	DS18B20_IO_OUT();
	for(i = 0; i < 8; i++) {
		if(data & 0x01) {
			DS18B20_DQ_OUT = 0;
			delay_us(2);
			DS18B20_DQ_OUT = 1;
			delay_us(60);
		} else {
			DS18B20_DQ_OUT = 0;
			delay_us(60);
			DS18B20_DQ_OUT = 1;
			delay_us(2);
		}
		data >>= 1;
	}
}

//定义读1字节数据
u8 DS18B20_Read_Byte(void) {
	u8 i;
	u8 data = 0;
	for(i = 0; i < 8; i++) {
		DS18B20_IO_OUT();
		DS18B20_DQ_OUT = 0;
		delay_us(2);
		DS18B20_IO_IN();
		delay_us(8);
		data |= DS18B20_DQ_IN << i;
		delay_us(50);
	}
	return data;
}

//分配存储ROM值内存
static u8 rom[8] = {0};

//定义获取ROM命令
void DS18B20_ReadRom(void) {
	int i;
	DS18B20_Reset();
	DS18B20_Write_Byte(0x33);	//read rom
	for(i = 0; i < 8; i++) {
			rom[i] = DS18B20_Read_Byte();
			printf("%#x ", rom[i]);
	}
	printf("\n");
}

//定义发送ROM匹配命令和发送ROM值函数
void DS18B20_MatchRom(void) {
	int i;
	DS18B20_Write_Byte(0x55); //match rom 
	for(i = 0; i < 8; i++) 
		DS18B20_Write_Byte(rom[i]);
}

//定义获取温度函数
float DS18B20_GetTemperture(void) {
	u16 temp = 0;
	u8 temp_lsb = 0, temp_msb = 0;
	float value = 0;
	
	#ifdef DS18B20_SKIP_ROM	//跳过匹配
	DS18B20_Reset();
	DS18B20_Write_Byte(0xCC);
	DS18B20_Write_Byte(0x44);
	
	DS18B20_Reset();
	DS18B20_Write_Byte(0xCC);
	DS18B20_Write_Byte(0xBE);
	#else	//匹配
	DS18B20_Reset();
	DS18B20_MatchRom();
	DS18B20_Write_Byte(0x44);
	
	DS18B20_Reset();
	DS18B20_MatchRom();
	DS18B20_Write_Byte(0xBE);
	#endif
	
	temp_lsb = DS18B20_Read_Byte();
	temp_msb = DS18B20_Read_Byte();
	
	temp = (temp_msb << 8) | temp_lsb;
	
	if((temp & 0xF800) == 0xF800) {
		temp = (~temp) + 1;
		value = temp * (0.0625);
	} else  {
		value = temp * 0.0625;
	}
	return value;
}

//定义温度传感器测试命令
void DS18B20_Test(void) {
	float temp = 0;
	temp = DS18B20_GetTemperture();
	if(temp < 0)
		printf("Current Temp: -");
	else
		printf("Current Temp: ");
	printf("%.2f\r\n", temp);
}




