// iic.h
#ifndef __IIC_H_  
#define __IIC_H_

#include "stm32f10x.h"
#include "system.h"

#define IIC_SCL		PBout(6)
#define IIC_SDA		PBout(7)
#define READ_SDA	PBin(7)

extern void IIC_Init(void);

extern void IIC_Start(void);
extern void IIC_Stop(void);

extern u8 	IIC_Wait_Ack(void);
extern void IIC_Ack(void);
extern void IIC_NAck(void);

extern void IIC_Send_Byte(u8 TxData);
extern u8   IIC_Read_Byte(u8 ack); 

#endif
