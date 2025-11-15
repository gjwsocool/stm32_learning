// ds18b20.c
#include "ds18b20.h"
#include "Systick.h"
#include "stdio.h"

// 初始化
void DS18B20_Init(void){
	// 1.打开GPIOG控制器时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
	
	// 2.配置PG11 推挽输出, 50MHz
	GPIO_InitTypeDef GPIO_Config;
	GPIO_Config.GPIO_Pin = GPIO_Pin_11;
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Config.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOG, &GPIO_Config);
} 
// 配置PG11, 推挽输出,50MHz
static void DS18B20_OUT(void){
	GPIO_InitTypeDef GPIO_Config;
	GPIO_Config.GPIO_Pin = GPIO_Pin_11;
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Config.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOG, &GPIO_Config);
}
// 配置PG11, 上拉输入
static void DS18B20_IN(void){
	GPIO_InitTypeDef GPIO_Config;
	GPIO_Config.GPIO_Pin = GPIO_Pin_11;
	GPIO_Config.GPIO_Mode  = GPIO_Mode_IPU; 
	GPIO_Init(GPIOG, &GPIO_Config);
}

// 初始化
void DS18B20_Reset(void){
	u8 tempTime = 0;
	// 1.拉低PG11, >= 480us
	DS18B20_OUT();
	DS18B20_IO_OUT = 0; 
	delay_us(500);
	
	// 2.拉高PG11, 15-60us
	DS18B20_IO_OUT = 1;
	delay_us(30);
	
	// 3.若DS18B20在线, 拉低PG11, 60-240us
	DS18B20_IN();
	while (DS18B20_IO_IN && tempTime < 240) {
		tempTime++;
		delay_us(1);
	}
	
	if (tempTime >= 240) 
		printf("DS18B20 Reset Failed\n");
	else {
		printf("DS18B20 Reset Success\n");
		tempTime = 0;
	}
} 

// 发送单字节 - 从低位发送 
void DS18B20_Write_Byte(u8 data){
	u8 i = 0;
	DS18B20_OUT();
	for( i = 0; i < 8; i++) {
		if (data & 0x01) { // 1bit 1
			// 1.拉低PG11, >1us
			DS18B20_IO_OUT = 0; 
			delay_us(2);
			// 2.拉高PG11, >45us
			DS18B20_IO_OUT = 1;
			delay_us(60);
		} else {	// 1bit 0 
			// 1.拉低PG11, 60-120us
			DS18B20_IO_OUT = 0; 
			delay_us(60);
			// 2.拉高PG11, >1us
			DS18B20_IO_OUT = 1;
			delay_us(2);
		}
		
		data >>= 1;
	}
} 
// 读取单字节 
u8   DS18B20_Read_Byte(void){
	u8 i = 0, data = 0; // 暂存读取的数据 
	
	for(i = 0; i < 8; i++) {
		// 1.拉低PG11, >1us - 输出模式
		DS18B20_OUT(); 
		DS18B20_IO_OUT = 0;
		delay_us(2);
		
		// 2.输入模式 
		DS18B20_IN(); 
		delay_us(8);
		// 读取PG11的高低电平 
		data |= DS18B20_IO_IN << i;
		delay_us(50);
	}
	
	return data;
} 

// +++++++++++++++++++++++++++++++++++++
// 存储读取到的ROM值 
static  u8 rom[8] = {}; 

// 读取ROM值 
void DS18B20_ReadRom(void) {
	// 1.初始化
	DS18B20_Reset();
	// 2.发送READ ROM - 0X33
	DS18B20_Write_Byte(0X33);
	// 3.循环读取8字节数据
	u8 i = 0;
	for(i = 0; i < 8; i++) {
		rom[i] = DS18B20_Read_Byte();
		printf("%#x ", rom[i]);
	}
	printf("\n");
} 

// match rom
static void DS18B20_MatchRom(void) {
	// 1.发送MATCH ROM - 0X55
	DS18B20_Write_Byte(0X55);
	// 2.发送ROM值 
	u8 i = 0;
	for (i = 0; i < 8; i++) 
		DS18B20_Write_Byte(rom[i]); 
}

// 获取温度值 
float DS18B20_GetTemperature(void){
	u8 temp_lsb = 0, temp_msb = 0;
	u16 temp = 0;
	float value; // 暂存温度值
	// 1.初始化 - 2.MATCH ROM - 3.发送CONVERT T - 0X44
	DS18B20_Reset();
	DS18B20_MatchRom();
	DS18B20_Write_Byte(0X44);
	
	// 4.延时>=750ms
	delay_ms(800);
	
	// 5.初始化 - 6.MATCH ROM - 7.发送READ SCRATCHPAD - 0XBE
	DS18B20_Reset();
	DS18B20_MatchRom();
	DS18B20_Write_Byte(0XBE);
	
	// 8.连续读取2个字节的数据
	temp_lsb = DS18B20_Read_Byte();	// byte0
	temp_msb = DS18B20_Read_Byte();	// byte1
	temp = temp_msb << 8 | temp_lsb; 
	
	// 9.温度转换
	if ((temp & 0XF800) == 0XF800) {	 // 负数, 零下温度
		temp = (~temp) + 1;
		value = temp * (-0.0625);
	} else { // 正数, 零上温度 
		value = temp  * 0.0625; 
	}
	
	return value; 
} 
// 测试函数
void  DS18B20_Test(void){
	float temp = DS18B20_GetTemperature();
	printf("curret temp: %.3f\n", temp);
} 



