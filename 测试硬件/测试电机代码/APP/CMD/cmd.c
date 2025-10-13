#include "cmd.h"
#include "led.h"
#include "beep.h"
#include "string.h"
#include "at24c02.h"
#include "ds18b20.h"
#include "dma.h"
#include "timer4.h"
#include "esp8266.h"
#include "mqtt.h"
#include "run.h"
#include "oled.h"
#include "pwm.h"

/*定义初始化开灯和关灯命令对象*/
cmd_t cmd_tbl[] = {
    //初始化开灯命令对象
    {"led on", LED_On},
    //初始化关灯命令对象
    {"led off", LED_Off},
		{"beep on", BEEP_On},
		{"beep off", BEEP_Off},
		{"EEPROM R", AT24C02_ReadOne},
		{"EEPROM W", AT24C02_WriteOne},
		{"EEPROM RS", AT24C02_ReadMul},
		{"EEPROM WS", AT24C02_WriteMul},
		{"temp", DS18B20_Test},
		{"rom", DS18B20_ReadRom},
		{"dma tx", UART1DMA_Tx_Test},
		{"timer4 start", TIMER4_Start},
		{"timer4 stop", TIMER4_Stop},
		
		{"esp connect", WIFI_Connect},
		{"esp send", WIFI_Send_Data},
		
		{"mqtt c", MQTT_Connect_Cmd},
		{"mqtt sub", MQTT_Sub_Cmd},
		{"mqtt pub temp", MQTT_PublishTemp_Cmd},
		{"mqtt ping", MQTT_Ping_Cmd},
		
		{"run", EHOME_Run},
		
		{"oled test", OLED_Test},
		
		{"pwm test", PWM_Test},
		
		{"x6818 tcp test", X6818_TCPTest}
};
//记录结构体数组元素的个数
int cmd_num = 
    sizeof(cmd_tbl) / sizeof(cmd_tbl[0]);

/*定义找命令的函数*/
cmd_t *find_cmd(char *name){
    int i;
    for (i = 0; i < cmd_num; i++) {
        if (!strcmp(name, 
                    cmd_tbl[i].name))
        return &cmd_tbl[i];
    }
    return 0; //没有找到命令对象
}
