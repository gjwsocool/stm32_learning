#ifndef __OLED_H
#define __OLED_H

#include "stm32f10x.h"
#include "system.h"

//GND  接电源地
//VCC  接3.3v电源
//D0   接PD6（SCL）
//D1   接PD7（SDA）
//RES  接PD4
//DC   接PD5
//CS   接PD3 
#define OLED_CS   PDout(3)		
#define OLED_RST  PDout(4)
#define OLED_DC   PDout(5)
#define OLED_SCL  PDout(6)
#define OLED_SDA  PDout(7)

#define OLED_CMD  0	
#define OLED_DATA 1	    						  


extern void OLED_Init(void);
extern void OLED_WR_Byte(u8 data,u8 cmd);	       							   		    
extern void OLED_Refresh_Gram(void);
extern void OLED_Clear(void);
extern void OLED_DrawPoint(u8 x,u8 y,u8 t);
extern void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);
extern void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size); 
void OLED_ShowFontHZ(u8 x,u8 y,u8 pos,u8 size,u8 mode);

extern void OLED_Test(void);
#endif
