#ifndef __DS18B20_H
#define __DS18B20_H

//包含总头文件
#include "stm32f10x.h"
#include "system.h"

//定义DS18B20数据线引脚的信息
#define DS18B20_PORT							GPIOB									//GPIO的G组
#define DS18B20_PIN								GPIO_Pin_4	
#define DS18B20_PORT_RCC					RCC_APB2Periph_GPIOB		//GPIOG时钟

//定义数据线GPIOG11输入和输出的位带操作
#define DS18B20_DQ_OUT						PBout(4)
#define DS18B20_DQ_IN							PBin(4)

extern void DS18B20_Init(void);						//初始化DS18B20
extern void DS18B20_Reset(void);					//复位函数
extern void DS18B20_Write_Byte(u8 data);	//发送1字节数据
extern u8 DS18B20_Read_Byte(void);				//读1字节数据
extern float DS18B20_GetTemperture(void);	//获取温度

extern void DS18B20_ReadRom(void);				//读取ROM值的命令处理函数
extern void DS18B20_Test(void);						//测试命令处理函数

#define DS18B20_SKIP_ROM									//定义此宏表示采用SKIP方式,否则采用Match
#endif
