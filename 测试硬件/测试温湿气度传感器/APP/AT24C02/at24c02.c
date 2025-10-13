#include "at24c02.h"
#include "iic.h"
#include "stdio.h"
#include "systick.h"

//定义AT24C02初始化函数
void AT24C02_Init(void) {
	IIC_Init();	//初始化IIC
}

//定义读1字节数据函数
//时序：ST->0x50<<1|0->(ACK)->ReadAddr->(ACK)->ST->0x50<<1|1->(ACK)->(DATA)->NACK->SP
u8 AT24C02_ReadByte(u16 ReadAddr) {
	u8 temp = 0;
	IIC_Start();
	IIC_Send_Byte(AT24C02_ID << 1 | 0);
	IIC_Wait_Ack();
	IIC_Send_Byte(ReadAddr);
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte(AT24C02_ID << 1 | 1);
	IIC_Wait_Ack();
	temp = IIC_Read_Byte(0);	//0:发送NACK, 1:发送ACK
	IIC_Stop();
	return temp;
}

//定义写1字节数据函数
//时序：ST->0x50<<1|0->(ACK)->WriteAddr->(ACK)->Data->(ACK)->SP
void AT24C02_WriteByte(u16 WriteAddr, u8 Data) {
	IIC_Start();
	IIC_Send_Byte(AT24C02_ID << 1 | 0);
	IIC_Wait_Ack();
	IIC_Send_Byte(WriteAddr);
	IIC_Wait_Ack();
	IIC_Send_Byte(Data);
	IIC_Wait_Ack();
	IIC_Stop();
	delay_ms(10);
}

//定义连续读取多字节函数
//时序：ST->0x50<<1|0->(ACK)->ReadAddr->(ACK)->ST->0x50<<1|1->(ACK)->(DATA1)->ACK->(DATA2)->ACK->...->(DATAn)->NACK->SP
void AT24C02_ReadBlockData(u16 ReadAddr, u8 *pBuffer, u16 Len) {
	IIC_Start();
	IIC_Send_Byte(AT24C02_ID << 1 | 0);
	IIC_Wait_Ack();
	IIC_Send_Byte(ReadAddr);
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte(AT24C02_ID << 1 | 1);
	IIC_Wait_Ack();
	while(Len) {
		if(Len==1)
			*pBuffer = IIC_Read_Byte(0); //发送NACK
		else
			*pBuffer = IIC_Read_Byte(1); //发送ACK
		Len--;
		pBuffer++;
	}
	IIC_Stop();
}

//定义连续写入多字节数据
//时序：ST->0x50<<1|0->(ACK)->WriteAddr->(ACK)->Data1->(ACK)->DATA2->(ACK)->...->DATAn->(ACK)->SP
//注意：AT24C02如果连续写入，只能一次写1页，如果地址超过范围了，会重新覆盖roll back
//所以如果超过了一页，需要重新开始传输
void AT24C02_WriteBlockData(u16 WriteAddr, u8 *pBuffer, u16 Len) {
	IIC_Start();
	IIC_Send_Byte(AT24C02_ID << 1 | 0);
	IIC_Wait_Ack();
	IIC_Send_Byte(WriteAddr);
	IIC_Wait_Ack();
	
	while(Len--) {
		IIC_Send_Byte(*pBuffer);
		IIC_Wait_Ack();
		pBuffer++;
		
		WriteAddr++; //虽然内部地址++,但是软件也需要++,主要用来判断是否超过一页
		
		if(WriteAddr % 8 == 0) { //新的一页写入数据
			IIC_Stop();
			delay_ms(5);		//页写需要5ms周期
			//重新开始写入下一页数据
			IIC_Start();
			IIC_Send_Byte(AT24C02_ID << 1 | 0);
			IIC_Wait_Ack();
			IIC_Send_Byte(WriteAddr);
			IIC_Wait_Ack();
		}
		
	}
	
	IIC_Stop();
	delay_ms(5);		//页写需要5ms周期
}

//定义读1字节命令
void AT24C02_ReadOne(void) {
	printf("Read Data is: %d\n", AT24C02_ReadByte(0));
}

//定义写1字节命令
void AT24C02_WriteOne(void) {
	AT24C02_WriteByte(0, 0xaa);
}

//定义连续读多字节数据命令
void AT24C02_ReadMul(void) {
	/*
	u8 Data[255] = {0};
	AT24C02_ReadBlockData(0x00, Data, ARRAY_SIZE(Data));
	int i;
	for(i = 0; i < ARRAY_SIZE(Data); i++)
		printf("Addr[%d]: Data[%d]\n", i, Data[i]);
	printf("Read Datas Over.\n");
	*/
	
	u8 Data[20] = {0};
	AT24C02_ReadBlockData(0x00, Data, ARRAY_SIZE(Data));
	int i;
	for(i = 0; i < ARRAY_SIZE(Data); i++)
		printf("Addr[%d]: Data[%#x]\n", i, Data[i]);
}

//定义连续写入多字节数据命令
void AT24C02_WriteMul(void) {
	/*
	int i;
	u8 Data[255];
	for(i = 0; i < 255; i++) 
		Data[i] = i;
	
	AT24C02_WriteBlockData(0x00, Data, ARRAY_SIZE(Data));
	printf("Write Datas Over.\n");
	*/
	int i;
	u8 Data[9];
	for(i = 0; i < 9; i++) 
		Data[i] = i+1;
	
	AT24C02_WriteBlockData(0x00, Data, ARRAY_SIZE(Data));
}
