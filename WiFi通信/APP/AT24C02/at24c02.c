// at24c02.c
#include "iic.h"
#include "at24c02.h"
#include "Systick.h"
#include "stdio.h"  // printf函数

void AT24C02_Init(void){
	IIC_Init();
}

// 读取单字节 
// 参数 : 要读取的寄存器地址
// 返回值 : 返回读取的数据
u8   AT24C02_ReadByte(u16 ReadAddr){
	// 1.发送开始信号
	IIC_Start();
	// 2.发送写设备地址
	IIC_Send_Byte(AT24C02_ID << 1 | 0);
	// 3.读取ack
	IIC_Wait_Ack();
	// 4.发送要读取的寄存器地址
	IIC_Send_Byte(ReadAddr);
	// 5.读取ack
	IIC_Wait_Ack();
	// 6.发送开始信号
	IIC_Start();
	// 7.发送读设备地址
	IIC_Send_Byte(AT24C02_ID << 1 | 1);
	// 8.读取ack
	IIC_Wait_Ack();
	// 9.读取1字节数据 + 回复nack
	u8 temp = IIC_Read_Byte(0); 
	// 10.发送结束信号
	IIC_Stop();
	
	return temp; 
}
// arg1 : 要写入的寄存器地址 
// arg2 : 要写入的数据 
void AT24C02_WriteByte(u16 WriteAddr, u8 data){
	// 1.发送开始信号
	IIC_Start();
	// 2.发送写设备地址
	IIC_Send_Byte(AT24C02_ID << 1 | 0);
	// 3.读取ack
	IIC_Wait_Ack();
	// 4.发送要写入的寄存器地址
	IIC_Send_Byte(WriteAddr);
	// 5.读取ack
	IIC_Wait_Ack();
	// 6.发送要写入的数据
	IIC_Send_Byte(data);
	// 7.读取ack
	IIC_Wait_Ack();
	// 8.发送结束信号
	IIC_Stop();
}
// arg1: 要读取的多个寄存器中的第一个寄存器地址 
// arg2: 存储数据的首地址 
// arg3: 要读取的数据个数 (3)
// 05 06 07 
// char buf[128]; char* pBuffer = buf; 
void AT24C02_ReadBlockData(u16 ReadAddr, u8* pBuffer, u16 Len){
	// 1.发送开始信号
	IIC_Start();
	// 2.发送写设备地址
	IIC_Send_Byte(AT24C02_ID << 1 | 0);
	// 3.读取ack
	IIC_Wait_Ack();
	// 4.发送要读取的寄存器地址
	IIC_Send_Byte(ReadAddr);
	// 5.读取ack
	IIC_Wait_Ack();
	// 6.发送开始信号
	IIC_Start();
	// 7.发送读设备地址
	IIC_Send_Byte(AT24C02_ID << 1 | 1);
	// 8.读取ack
	IIC_Wait_Ack();
	// 9.连续读取多个字节数据
	// 读3个数 -> 3 2 1
	while (Len) {
		if (Len == 1)
			*pBuffer = IIC_Read_Byte(0); // 读取1字节, 回复nack
		else
			*pBuffer = IIC_Read_Byte(1); // 读取1字节, 回复ack
		pBuffer++;
		Len--;
	}
	// 10.发送停止信号
	IIC_Stop();
}
// arg1: 要写入的多个寄存器中的第一个寄存器地址 
// arg2: 要发送数据的首地址 
// arg3: 要发送的数据个数 (3)
// 05 06 07 
// char buf[128]; char* pBuffer = buf; 
void AT24C02_WriteBlockData(u16 WriteAddr, u8* pBuffer, u16 Len){
	// 1.发送开始信号
	IIC_Start();
	// 2.发送写设备地址
	IIC_Send_Byte(AT24C02_ID << 1 | 0);
	// 3.读取ack
	IIC_Wait_Ack();
	// 4.发送要写入的寄存器地址
	IIC_Send_Byte(WriteAddr);
	// 5.读取ack
	IIC_Wait_Ack();
	// 6.循环发送多字节数据
	while(Len--) {
		// 第一个字节永远不跨页 
		IIC_Send_Byte(*pBuffer);
		IIC_Wait_Ack();
		pBuffer++;
		WriteAddr++;
		if (WriteAddr % 8 == 0) { // 跨页
			IIC_Stop(); 
			delay_ms(5); // 页写入时间
			// --------> 本次传输结束
			IIC_Start();
			IIC_Send_Byte(AT24C02_ID << 1 | 0);
			IIC_Wait_Ack();
			IIC_Send_Byte(WriteAddr);
			IIC_Wait_Ack();
		}
	}
	// 7.发送结束信号
	IIC_Stop();
	delay_ms(5);
}

// 4个测试函数
void AT24C02_ReadOne(void){
	printf("READ DATA: %#x\n", AT24C02_ReadByte(0X00));
}
void AT24C02_WriteOne(void){
	AT24C02_WriteByte(0X00, 0XAA);// [00] = 0XAA;
}
void AT24C02_ReadMul(void){
	u8 data[5] = {};
	AT24C02_ReadBlockData(0X00, data, 5);
	u8 i;
	for(i = 0; i < 5; i++)
		printf("addr[%d], data[%#X]\n", i, data[i]);
}
void AT24C02_WriteMul(void){
	u8 data[5] = {1,2,3,4,5};
	// 1 2 3 4 5 -> 寄存器地址: 00 01 02 03 04
	AT24C02_WriteBlockData(0X00, data, 5);
}


