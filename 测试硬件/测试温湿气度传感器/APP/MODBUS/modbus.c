#include "modbus.h"
#include "uart2.h"
#include "stdio.h"
#include "ds18b20.h"
#include "at24c02.h"
#include "oled.h"
#include "systick.h"

//定义CRC16校验
unsigned short Get_CRC16(unsigned char *ptr, int len) {
		uint8_t i;
    uint16_t crc = ~0x00;

    if((ptr == 0) || (len == 0xFFFF)) return crc;

    while(len--){
        crc ^= *ptr++;
        for(i = 0; i < 8; i++){
            if(crc & 0x01){
                crc >>= 1;
                crc ^= 0xA001;
            }
            else{
                crc >>= 1;
            }
        }
    }
    return crc;
}

//定义作为主机处理从机返回的数据信息
void Modbus_Handle_Master(void) {
	switch(UART2_RxBuff[1]) {
		case 0x03: 
		{
			if(UART2_RxBuff[0] == 1) {
				float temp = ((UART2_RxBuff[3] << 8)|UART2_RxBuff[4])/10.0;
				float humi = ((UART2_RxBuff[5] << 8)|UART2_RxBuff[6])/10.0;
				printf("Slave %d: Temp = %.1f, Humi = %.1f\n", UART2_RxBuff[0], temp, humi);
			}
			break;
		}
		case 0x06:
		{
			int i;
			for(i = 0; i < 8; i++)
				printf("%#2x ", UART2_RxBuff[i]);
			printf("\n");
			break;
		}
	}
}

/*
定义作为主机处理功能码03函数,例如：获取485接口的温湿度传感器数据信息
读取温度湿度：01 03 00 00 00 02 C4 0B （地址为1，读温度湿度值）
返回：01 03 04 00 B8 03 3F 3A F6 （对应温度：18.4℃，湿度83.1%）
*/
void Modbus_03_Master(void) {
	while(1) {
		unsigned short CRC16 = 0;
		UART2_TxBuff[0] = TH10S_ID_1;
		UART2_TxBuff[1] = 0x03;
		UART2_TxBuff[2] = 0x00;
		UART2_TxBuff[3] = 0x00;
		UART2_TxBuff[4] = 0x00;
		UART2_TxBuff[5] = 0x02;
		CRC16 = Get_CRC16(UART2_TxBuff, 6);
		UART2_TxBuff[6] = CRC16 & 0xFF;
		UART2_TxBuff[7] = (CRC16 >> 8) & 0xFF;
		UART2_TxData(UART2_TxBuff, 8);
		while(1) {
			if(UART2_RxCounter != 0) {
				Modbus_Handle_Master();
				UART2_RxCounter = 0;
				break;
			}
		}
		delay_ms(1000);
	}
}

/*
定义作为主机处理功能码06函数,例如：
写地址：01 06 00 64 00 03 49 D4   （已知原地址为01，改成02）
*/
void Modbus_06_Master(void) {
		unsigned short CRC16 = 0;
		UART2_TxBuff[0] = TH10S_ID_1;
		UART2_TxBuff[1] = 0x06;
		UART2_TxBuff[2] = 0x00;
		UART2_TxBuff[3] = 0x64;
		UART2_TxBuff[4] = 0x00;
		UART2_TxBuff[5] = 0x02;
		CRC16 = Get_CRC16(UART2_TxBuff, 6);
		UART2_TxBuff[6] = CRC16 & 0xFF;
		UART2_TxBuff[7] = (CRC16 >> 8) & 0xFF;
		UART2_TxData(UART2_TxBuff, 8);
		while(1) {
			if(UART2_RxCounter != 0) {
				Modbus_Handle_Master();
				break;
			}
		}
}








