#ifndef __DS18B20_H
#define __DS18B20_H

#include "stm32f10x.h"
#include "system.h"

//定义DS18B20数据线引脚信息
#define 	DS18B20_PORT			GPIOG
#define 	DS18B20_PIN				GPIO_Pin_11
#define 	DS18B20_PORT_RCC	RCC_APB2Periph_GPIOG

//声明数据线操作函数
#define DS18B20_DQ_OUT			PGout(11)
#define DS18B20_DQ_IN				PGin(11)

extern void DS18B20_Init(void);	//初始化函数
extern void DS18B20_Reset(void); //复位函数
extern void DS18B20_Write_Byte(u8 data); //写1字节
extern u8 DS18B20_Read_Byte(void);//读1字节
extern float DS18B20_GetTemperture(void); //获取温度
extern void DS18B20_Test(void);	//测试命令

//#define DS18B20_SKIP_ROM		

extern void DS18B20_ReadRom(void);	//获取ROM测试命令
#endif


