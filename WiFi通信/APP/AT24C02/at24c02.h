// at24c02.h
#ifndef __AT24C02_H_ 
#define __AT24C02_H_

#include "stm32f10x.h"

#define AT24C02_ID	(0X50)

extern void AT24C02_Init(void);

extern u8   AT24C02_ReadByte(u16 ReadAddr);
extern void AT24C02_WriteByte(u16 WriteAddr, u8 data);
extern void AT24C02_ReadBlockData(u16 ReadAddr, u8* pBuffer, u16 Len);
extern void AT24C02_WriteBlockData(u16 WriteAddr, u8* pBuffer, u16 Len);

// 4¸ö²âÊÔº¯Êý
extern void AT24C02_ReadOne(void);
extern void AT24C02_WriteOne(void);
extern void AT24C02_ReadMul(void);
extern void AT24C02_WriteMul(void);

#endif
