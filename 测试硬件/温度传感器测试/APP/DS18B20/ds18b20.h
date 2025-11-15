// ds18b20.h
#ifndef __DS18B20_H_ 
#define __DS18B20_H_

#include "stm32f10x.h"
#include "system.h"

#define DS18B20_IO_OUT	PGout(11)
#define DS18B20_IO_IN		PGin(11)

extern void DS18B20_Init(void); 

extern void DS18B20_Reset(void); // 初始化
extern void DS18B20_Write_Byte(u8 data); // 发送单字节 
extern u8   DS18B20_Read_Byte(void); // 读取单字节 

extern float DS18B20_GetTemperature(void); // 获取温度值 
// 测试函数
extern void  DS18B20_Test(void); 

// 读取ROM值 
extern void DS18B20_ReadRom(void); 

#endif
