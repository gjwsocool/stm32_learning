#ifndef __AT24C02_H
#define __AT24C02_H

#include "stm32f10x.h"
#include "system.h"

#define AT24C02_ID	(0x50)											//指定AT24C02的设备地址

extern void AT24C02_Init(void);									//初始化AT24C02函数
extern u8 AT24C02_ReadByte(u16 ReadAddr);				//读取1字节数据函数
extern void AT24C02_WriteByte(u16 WriteAddr, u8 data);	//写1字节数据函数
extern void AT24C02_ReadBlockData(u16 ReadAddr, u8 *pBuffer, u16 Len);	//从指定的地址连续读取多字节数据
extern void AT24C02_WriteBlockData(u16 WriteAddr, u8 *pBuffer, u16 Len);//向指定的地址连续写入多字节数据

extern void AT24C02_ReadOne(void);		//读1字节命令
extern void AT24C02_WriteOne(void);		//写1字节命令
extern void AT24C02_ReadMul(void);		//读多字节命令
extern void AT24C02_WriteMul(void);		//写多字节命令
#endif
