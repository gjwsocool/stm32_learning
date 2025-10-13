#include "oled.h"
#include "systick.h"
#include "oledfont.h"  
#include "ds18b20.h"
#include "stdio.h"
#include "key.h"
#include "led.h"
#include "beep.h"

//向SSD1106写入一个字节。
//data:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(u8 data,u8 cmd)
{	
	u8 i;			  
	OLED_DC=cmd;  			  
	OLED_CS=0;
	for(i=0;i<8;i++)
	{			  
		OLED_CLK=0;
		if(data&0x80)
			OLED_DATA=1;
		else 
		  OLED_DATA=0;
		OLED_CLK=1;
		data<<=1;   
	}				 		  
	OLED_CS=1;  	  
} 

void OLED_Init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7; 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOD, &GPIO_InitStructure);	 
 	GPIO_SetBits(GPIOD,GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);	//拉高电平

	OLED_RST=1;
	delay_ms(100);
	OLED_RST=0;
	delay_ms(100);
	OLED_RST=1; 
			
	OLED_WR_Byte(0xAE,OLED_CMD_F);//关闭显示
     
  OLED_WR_Byte(0x00,OLED_CMD_F);//X轴低位，起始X轴为0
  OLED_WR_Byte(0x10,OLED_CMD_F);//X轴高位
  OLED_WR_Byte(0x40,OLED_CMD_F);//Y轴，可设区间[0x40,0x7F]，设置为0了
     
 
  OLED_WR_Byte(0xA1,OLED_CMD_F);//设置X轴扫描方向，0xa0左右反置 ，0xa1正常（左边为0列）
  OLED_WR_Byte(0xC8,OLED_CMD_F);//设置Y轴扫描方向，0xc0上下反置 ，0xc8正常（上边为0行）
  OLED_WR_Byte(0xA6,OLED_CMD_F);//位值表示的意义，0xa6表示正常，1为点亮，0为关闭，0xa7显示效果相反
     
 
  OLED_WR_Byte(0x81,OLED_CMD_F);//命令头，调节亮度,对比度,变化很小，但是仔细可以观察出来
  OLED_WR_Byte(0xFF,OLED_CMD_F);//可设置区间[0x00,0xFF]
     
  OLED_WR_Byte(0xA8,OLED_CMD_F);//命令头，设置多路复用率(1 to 64)
  OLED_WR_Byte(0x3f,OLED_CMD_F);//--1/64 duty
     
  OLED_WR_Byte(0xD3,OLED_CMD_F);//命令头，设置显示偏移移位映射RAM计数器(0x00~0x3F)
  OLED_WR_Byte(0x00,OLED_CMD_F);//不偏移
     
  OLED_WR_Byte(0xd5,OLED_CMD_F);//命令头，设置显示时钟分频比/振荡器频率
  OLED_WR_Byte(0x80,OLED_CMD_F);//设置分割比率，设置时钟为100帧/秒
     
  OLED_WR_Byte(0xD9,OLED_CMD_F);//命令头，--set pre-charge period
  OLED_WR_Byte(0xF1,OLED_CMD_F);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
     
  OLED_WR_Byte(0xDA,OLED_CMD_F);//命令头，--set com pins hardware configuration
  OLED_WR_Byte(0x12,OLED_CMD_F);
     
  OLED_WR_Byte(0xDB,OLED_CMD_F);//命令头，--set vcomh
  OLED_WR_Byte(0x40,OLED_CMD_F);//Set VCOM Deselect Level
     
  OLED_WR_Byte(0x20,OLED_CMD_F);//命令头，设置寻址模式
  OLED_WR_Byte(0x10,OLED_CMD_F);//页面寻址模式(重置) (0x00/0x01/0x02)
     
  OLED_WR_Byte(0x8D,OLED_CMD_F);//命令头，--set Charge Pump enable/disable
  OLED_WR_Byte(0x14,OLED_CMD_F);//--set(0x10) disable
     
  OLED_WR_Byte(0xA4,OLED_CMD_F);//恢复到RAM内容显示(重置)    						   
	OLED_WR_Byte(0xAF,OLED_CMD_F); //开启显示	  
	
	OLED_Clear(0x00);   
	//OLED_Draw_Chinese(0, 4);
}
  		
void OLED_Draw_Point(u8 page, u8 col, u8 data) {
	OLED_WR_Byte (0xb0+page,OLED_CMD_F);
	OLED_WR_Byte (0x00+(col&0x0F),OLED_CMD_F); 
	OLED_WR_Byte (0x10+((col>>4)&0x0F),OLED_CMD_F);
	OLED_WR_Byte(data,OLED_DATA_F);
}

void OLED_Clear(u8 color)  {  
	unsigned char i,n;		    
	for(i=0;i<8;i++)  {  
		OLED_WR_Byte (0xb0+i,OLED_CMD_F);    	//从0~7页依次写入
		OLED_WR_Byte (0x00,OLED_CMD_F);      	//列低地址
		OLED_WR_Byte (0x10,OLED_CMD_F);      	//列高地址  
		for(n=0;n<128;n++)
			OLED_WR_Byte(color,OLED_DATA_F); 				//清屏
	}
}

void OLED_Draw_Char(u8 page, u8 col, u8 data) {
	unsigned char n;
	unsigned char ch;
	unsigned int index = data - 32; //获取字符在字符集中的偏移量
	
	for(n = 0; n < 8; n++) {
			ch = ascii_1608[index][2*n];
			OLED_Draw_Point(page, col + n, ch);
			ch = ascii_1608[index][2*n+1];
			OLED_Draw_Point(page+1, col + n, ch);
	}
}

void OLED_Draw_String(u8 page, u8 col, u8 *pstr) {
	while(*pstr) {
		OLED_Draw_Char(page, col, *pstr);
		col += 8;
		pstr++;
	}
}

void OLED_Draw_Chinese(u8 page, u8 col) {
	unsigned char n;
	unsigned char ch;
	unsigned char *p = (unsigned char *)FontHzk;
	int size = sizeof(FontHzk) / sizeof(FontHzk[0][0]);
	
	for(n = 0; n < size/2; n++) {
			ch = p[2*n];
			OLED_Draw_Point(page, col + n, ch);
			ch = p[2*n+1];
			OLED_Draw_Point(page+1, col + n, ch);
	}
}

/*******************************************************************************************************/
#define CMD_GET_TEMP		1
#define CMD_CTRL_LED		2
#define CMD_CTRL_BEEP		3

int show_temp_flag = 0;			//0:不允许定时器3显示温度只采集，1：允许定时器显示温度,具体代码参见timer3.c
														//中的中断处理函数

//检测业务类型
void menu(void) {
		u8 cmd;
		cmd = show_main_menu();
		switch(cmd) {
			case CMD_GET_TEMP:	
				show_get_temp_menu();		//获取显示当前温度
				break;
			case CMD_CTRL_LED:
				show_led_ctrl_menu(); 	//显示LED操作界面
				break;
			case CMD_CTRL_BEEP:
				show_beep_ctrl_menu();	//显示BEEP操作界面
				break;
		}
}

//灯控函数
void show_led_ctrl_menu(void) {
		OLED_Clear(0x00); 
		OLED_Draw_String(0, 0, "1.LED ON");
		OLED_Draw_String(2, 0, "2.LED OFF");	
		for(;;) {
			u8 key;
			key=KEY_Scan();
			if(key==KEY_UP_PRESS) {
				menu();
			}
			else if(key==KEY1_PRESS)
				LED1 = 0;
			else if(key == KEY0_PRESS) 
				LED1 = 1;
		}
}

//蜂鸣器控制函数
void show_beep_ctrl_menu(void) {
		OLED_Clear(0x00); 
		OLED_Draw_String(0, 0, "1.BEEP ON");
		OLED_Draw_String(2, 0, "2.BEEP OFF");	
		for(;;) {
			u8 key;
			key=KEY_Scan();
			if(key==KEY_UP_PRESS) {
				menu();
			}
			else if(key==KEY1_PRESS)
				BEEP = 1;
			else if(key == KEY0_PRESS) 
				BEEP = 0;
		}
}

//温度显示函数,这个需要配合定时器3，定会器负责采集温度，这个函数负责是否显示温度
void show_get_temp_menu(void) {
		OLED_Clear(0x00); 
		show_temp_flag = 1; //让定时器3开始显示温度
		for(;;) {
			u8 key;
			key=KEY_Scan();
			if(key==KEY_UP_PRESS) {
				show_temp_flag = 0; //禁止定时器显示温度
				menu();
			}
		}
}

//主页面显示函数
u8 show_main_menu(void) {
		u8 key;
		OLED_Clear(0x00);   
		OLED_Draw_String(0, 0, "1.GET  TEMP");
		OLED_Draw_String(2, 0, "2.CTRL LED");
		OLED_Draw_String(4, 0, "3.CTRL BEEP");
		for(;;) {
			key=KEY_Scan();
			if(key==KEY_UP_PRESS)
				return CMD_GET_TEMP;
			else if(key==KEY1_PRESS)
				return CMD_CTRL_LED;
			else if(key == KEY0_PRESS) 
				return CMD_CTRL_BEEP;
		}
}

//测试命令对应的，上位机发送oled_test命令给下位机，执行此函数，正式开启图形界面操作
void OLED_Test(void) {
	OLED_Draw_Chinese(2, 4);
	for(;;) {
			u8 key;
			key=KEY_Scan();
			if(key==KEY_UP_PRESS)
				break;
	}
	menu();
}