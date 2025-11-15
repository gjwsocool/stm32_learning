// cmd.c
#include "cmd.h"
#include "led.h"
#include "beep.h"
#include "string.h"
#include "init.h"
#include "at24c02.h"
#include "ds18b20.h"

cmd_t cmd[] = {
	{"led on",   	LED_On},
	{"led off",  	LED_Off},
	{"beep on",  	BEEP_On},
	{"beep off", 	BEEP_Off},
	{"EEPROM R", 	AT24C02_ReadOne},		// 单字节读取
	{"EEPROM W", 	AT24C02_WriteOne},	// 单字节写入
	{"EEPROM RS",	AT24C02_ReadMul},		// 多字节读取
	{"EEPROM WS", AT24C02_WriteMul},	// 多字节写入
	{"temp",      DS18B20_Test},			// 获取温度值命令
	{"rom",				DS18B20_ReadRom}		// 读取ROM值 
		// rom + temp
};


// 匹配的函数
cmd_t* find_cmd(char* name) {
	int i = 0; 
	for( i = 0; i < ARR_NUM(cmd); i++) {
		if (!strcmp(name, cmd[i].name))
			return &cmd[i];
	}

	return 0;
}


