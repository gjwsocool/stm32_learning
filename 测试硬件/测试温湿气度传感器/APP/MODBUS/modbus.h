#ifndef __MODBUS_H
#define __MODBUS_H

//包含总头文件
#include "stm32f10x.h"
#include "system.h"

//指定当前开发板作为从机的地址
#define SlaveID  0x01
#define HoldRegStartAddr    0x0000																						//保持寄存器起始地址
#define HoldRegCount        8																									//保持寄存器数量
#define HoldMaxValue        10000            																	//寄存器最大值

//声明错误码
extern unsigned char Err;
//声明起始寄存器地址
extern unsigned short StartRegAddr;

//声明作为从机解析接收数据函数
extern void Modbus_Handle_Slave(void);

//声明作为从机处理功能码03函数
extern void Modbus_03_Slave(void);

//声明作为从机处理功能码06函数
extern void Modbus_06_Slave(void);

//声明作为从机处理功能码16函数
extern void Modbus_16_Slave(void);

//声明CRC16校验函数
extern unsigned short Get_CRC16(unsigned char *ptr, int len);

//声明作为主机处理功能码03函数
extern void Modbus_03_Master(void);

//声明作为主机处理功能码06函数
extern void Modbus_06_Master(void);

//声明作为主机处理接收到的数据函数
extern void Modbus_Handle_Master(void);

#define TH10S_ID_1		0x01		//指定TH10S温湿度传感器的地址
#define TH10S_ID_2		0x02		//指定TH10S温湿度传感器的地址
#endif
