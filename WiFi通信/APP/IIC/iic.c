// iic.c
#include "iic.h"
#include "Systick.h"

void IIC_Init(void){
	// 1.打开GPIOB控制器的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	// 2.配置SCL(PB6) - 推挽输出, 50MHz
	GPIO_InitTypeDef GPIO_Config;
	GPIO_Config.GPIO_Pin = GPIO_Pin_6;
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_Config);
	
	// 3.配置SDA(PB7) - 推挽输出, 50MHz
	GPIO_Config.GPIO_Pin = GPIO_Pin_7;
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_Config);
	// 4.拉高SCL/SDA
	IIC_SDA = 1;
	IIC_SCL = 1; 
}
// 配置SDA为推挽输出, 50MHz
static void SDA_OUT(void) {
	GPIO_InitTypeDef GPIO_Config;
	// 配置SDA(PB7) - 推挽输出, 50MHz
	GPIO_Config.GPIO_Pin = GPIO_Pin_7;
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_Config);
}
// 配置SDA为上拉输入
static void SDA_IN(void) {
	GPIO_InitTypeDef GPIO_Config;
	GPIO_Config.GPIO_Pin = GPIO_Pin_7;
	GPIO_Config.GPIO_Mode = GPIO_Mode_IPU; 
	GPIO_Init(GPIOB, &GPIO_Config);
}
/*
	1.配置SDA为输出模式 
	2.拉高SCL 
	3.拉高SDA
	4.保持 >=4.7us
	5.拉低SDA
	6.保持 >= 4us
*/
void IIC_Start(void){
	SDA_OUT();
	IIC_SCL = 1;
	IIC_SDA = 1;
	delay_us(6);
	IIC_SDA = 0;
	delay_us(6);
	// ---> 发送了开始信号, 后续数据传输 
	IIC_SCL = 0; 
}
/*
	1.配置SDA为输出模式 
	2.拉低SDA
	3.拉高SCL 
	4.保持 >= 4us
	5.拉高SDA
	6.保持 >= 4.7us
*/
void IIC_Stop(void){
	SDA_OUT();
	IIC_SDA = 0;
	IIC_SCL = 1;
	delay_us(6);
	IIC_SDA = 1;
	delay_us(6);
}
/*
	cpu 读取ack信号 - CPU读取一个低电平
	收到ack, 0; 没收到ack, 1;
	低放 - 外设放数据
	高取 - CPU读数据 
*/
u8 	 IIC_Wait_Ack(void){
	u32 tempTime = 0;
	// 低放 外设放数据
	IIC_SCL = 0;
	delay_us(6);
	SDA_IN(); // 配置为输入模式 
	
	// 高取 CPU读数据 
	IIC_SCL = 1;
	delay_us(6);
	
	// 收到了ack READ_SDA=0; 没收到ack, READ_SDA=1; 
	while (READ_SDA) {
		tempTime++;
		if (tempTime > 250){
			IIC_Stop();
			return 1;
		}
	}
	
	IIC_SCL = 0; // 继续下次传输
	return 0;
}

// 发送ack信号 - 发送1bit的低电平给外设
void IIC_Ack(void){
	// 低放 - CPU放数据 
	IIC_SCL = 0;
	SDA_OUT(); // SDA输出模式 
	IIC_SDA = 0; 
	delay_us(6);
	// -> 将数据(低电平-ack信号)放到了SDA上
	
	// 高取 - 外设读数据 
	IIC_SCL = 1;
	delay_us(6);
	
	IIC_SCL = 0; // 准备下一次数据传输
}
// 发送nack信号 - 发送1bit的高电平给外设
void IIC_NAck(void){
	// 低放 - CPU放数据 
	IIC_SCL = 0;
	SDA_OUT(); // SDA输出模式 
	IIC_SDA = 1; 
	delay_us(6);
	// -> 将数据(高电平-nack信号)放到了SDA上
	
	// 高取 - 外设读数据 
	IIC_SCL = 1;
	delay_us(6);
	
	IIC_SCL = 0; // 准备下一次数据传输
}	
//						76543210	位编号
// TxData =   cdefgh00;
//						10000000 &
//						c0000000 
void IIC_Send_Byte(u8 TxData){
	u8 i;
	SDA_OUT(); // 输出模式 
	IIC_SCL = 0; // 拉低 
	for(i = 0; i < 8; i++) {
		if (TxData & 0X80) // 非0
			IIC_SDA = 1;
		else
			IIC_SDA = 0;
		
		TxData <<= 1;
			
		delay_us(6); // 低电平的时钟周期 
		
		IIC_SCL = 1;
		delay_us(6); // 高电平的时钟周期 
		
		IIC_SCL = 0;
	}
}

// 返回值 : 返回读取到的1字节数据 
// 参数:
//		1, 回复ack; 0, 回复nack;
u8   IIC_Read_Byte(u8 ack){
	u8 i = 0, data = 0; // data保存读取到的数据 
	SDA_IN(); //输入模式 
	for( i = 0; i < 8; i++) {
		// 低放 - 外设放数据
		IIC_SCL = 0;
		delay_us(6);
	
		// 高取 - CPU获取数据 
		IIC_SCL = 1;
		data |= READ_SDA << (7-i); // 7 6 5 ... 0 |
		delay_us(6);
	}
	
	// 回复ack/nack
	if (!ack) 
		IIC_NAck();
	else 
		IIC_Ack();
	
	return data;
}


