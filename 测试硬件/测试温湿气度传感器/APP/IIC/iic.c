#include "iic.h"
#include "systick.h"
#include "stdio.h"

//定义IIC初始化函数
void IIC_Init(void) {
	GPIO_InitTypeDef GPIO_Config;
	
	//使能SCL和SDA的GPIO时钟
	RCC_APB2PeriphClockCmd(IIC_SCL_PORT_RCC|IIC_SDA_PORT_RCC, ENABLE);
	
	//初始化SCL的引脚
	GPIO_Config.GPIO_Pin = IIC_SCL_PIN;
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(IIC_SCL_PORT, &GPIO_Config);

	//初始化SDA的引脚
	GPIO_Config.GPIO_Pin = IIC_SDA_PIN;
	GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IIC_SDA_PORT, &GPIO_Config);

	//默认拉高SCL和SDA
	IIC_SCL = 1;
	IIC_SDA = 1;
}

//定义配置SDA为输出函数
void SDA_OUT(void) {
	GPIO_InitTypeDef GPIO_Config;
	
	GPIO_Config.GPIO_Pin = IIC_SDA_PIN;
	GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IIC_SDA_PORT, &GPIO_Config);
}

//定义配置SDA为输入函数
void SDA_IN(void) {
	GPIO_InitTypeDef GPIO_Config;
	
	GPIO_Config.GPIO_Pin = IIC_SDA_PIN;
	GPIO_Config.GPIO_Mode = GPIO_Mode_IPU; //上拉输入
	GPIO_Init(IIC_SDA_PORT, &GPIO_Config);
}

//定义发送START信号函数
/*时序
SCL:
	1----------------
	0								|
SDA: 4.7us=tsu:sta
	1--------
					|  4us=thd:sta
	0				----------
*/
void IIC_Start(void){
		SDA_OUT();
		IIC_SCL = 1;
		IIC_SDA = 1;
		delay_us(5);
		IIC_SDA = 0;
		delay_us(6);
		IIC_SCL = 0; //准备开始传输数据了
}

//定义发送STOP信号函数
/*时序
SCL:
	1----------------
	0								
SDA: 									tbuf=4.7us
	1									---------
     tsu:stop=4us	  |
	0	----------------		
*/
void IIC_Stop(void) {
		SDA_OUT();
		IIC_SDA = 0;
		IIC_SCL = 1;
		delay_us(6);
		IIC_SDA = 1;
		delay_us(6);
}

//定义等待ACK信号函数
/*时序
SCL:								thigh=4
	1								 --------------
				tlow=4.7	|
	0-----------------								
SDA: 									
	1									
     
	0	-----------------------------		
*/
u8 IIC_Wait_Ack(void) {
	u8 tempTime = 0;
	
	IIC_SCL = 0;
	delay_us(6);
	SDA_IN();
	IIC_SCL = 1;
	delay_us(6);
	while(READ_SDA) {
		tempTime++;
		if(tempTime > 250) {
			IIC_Stop(); //没有收到ACK，停止数据传输
			printf("Not Receive ACK\n");
			return 1;
		}
	}
	IIC_SCL = 0; //继续开始传输数据了
	return 0;
}

//定义发送ACK信号函数
/*时序
SCL:
	1----------------
	0								
SDA: 									tbuf=4.7us
	1									---------
     tsu:stop=4us	  |
	0	----------------		
*/
void IIC_Ack(void) {
	IIC_SCL = 0;
	SDA_OUT();
	IIC_SDA = 0;
	delay_us(6);
	IIC_SCL = 1;
	delay_us(6);
	IIC_SCL = 0; 
}

//定义发送NACK信号函数
/*时序
SCL:
	1----------------
	0								
SDA: 									tbuf=4.7us
	1									---------
     tsu:stop=4us	  |
	0	----------------		
*/
void IIC_NAck(void) {
	IIC_SCL = 0;
	SDA_OUT();
	IIC_SDA = 1;
	delay_us(6);
	IIC_SCL = 1;
	delay_us(6);
	IIC_SCL = 0; 
}

//定义发送1字节数据时序函数
void IIC_Send_Byte(u8 TxData) {
	u8 i;
	SDA_OUT();
	IIC_SCL = 0;
	for(i = 0; i < 8; i++) {
		if(TxData & 0x80)
			IIC_SDA = 1;
		else
			IIC_SDA = 0;
		TxData <<= 1;
		delay_us(6);
		IIC_SCL = 1;
		delay_us(6);
		IIC_SCL = 0;	
	}
}

//定义读取1字节数据函数
u8 IIC_Read_Byte(u8 ack) {
	u8 i, data = 0;
	SDA_IN();
	for(i = 0; i < 8; i++) {
		IIC_SCL = 0;
		delay_us(6);
		IIC_SCL = 1;
		data |= READ_SDA << (7 - i);
		delay_us(6);
	}
	if(!ack)
		IIC_NAck();
	else
		IIC_Ack();
	return data;
}
