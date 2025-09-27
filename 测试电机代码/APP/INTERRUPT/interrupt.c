#include "interrupt.h"
#include "dma.h"
#include "uart3.h"
#include "mqtt.h"
#include "string.h"

//定义串口接收的数据长度
u32 UART_RxCounter = 0;
//定义暂存串口接收数据的缓冲区
u8 UART_RxBuff[UART_RXBUFF_SIZE];
//定义是否可以发送给上位机接收到的命令标志：
//0：命令还没有接收完毕，1：表示命令接收完毕，可以发送给上位机了
u8 UART_Send_Flag = 0;

//定义USART1串口中断处理函数
void USART1_IRQHandler(void) {
		u8 c;	
		//判断是否是接收到数据产生的中断
		if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
			//清中断
			USART_ClearITPendingBit(USART1, USART_IT_RXNE);
			//读取接收到的数据
			c = USART_ReceiveData(USART1); 
			//将读取到的数据暂存到缓冲区中
			UART_RxBuff[UART_RxCounter++] = c;
			if(c == '\n') {
				//注意上位机SSCOM发送来的字符串是:"led on\r\n\0"
				UART_RxBuff[UART_RxCounter-2] = '\0';	
				//接收计数清零,准备接收下一个命令，然后重新放到数组中
				UART_RxCounter = 0;	
				//表示一帧数据接收完毕,可以发送给上位机数据了
				UART_Send_Flag = 1;
			}
		}
		
		//只有一帧数据传输完毕，中间没有数据传输了，产生一次中断
		if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) {
			//清除USART_IT_IDLE中断标志
			//USART_ClearITPendingBit();//此函数不能清除IDLE中断
			//根据芯片手册 P541,BIT4,USART_SR,读取这两个寄存器可以清除IDLE中断
			USART1->SR; //步骤1
			USART1->DR; //步骤2
			//获取此次串口1接收的数据量 = 总的DMA缓存大小 - 剩余DMA缓存大小
			UART1DMA_RxCounter = UART1DMA_RXBUFF_SIZE - DMA_GetCurrDataCounter(DMA1_Channel5);
			//上位机发送的字符串是：“led on\r\n\0”
			UART1DMA_RxBuff[UART1DMA_RxCounter - 2] = '\0';
			//重新开启DMA下一次接收数据功能
			//正常模式下：需要先禁止DMA，然后设置搬移的数据长度，然后启动DMA
			DMA_Cmd(DMA1_Channel5, DISABLE);
			DMA_SetCurrDataCounter(DMA1_Channel5, UART1DMA_RXBUFF_SIZE);
			DMA_Cmd(DMA1_Channel5, ENABLE);
		}
}

//定义DMA1通道4的发送数据完成中断函数
void DMA1_Channel4_IRQHandler(void) {
		//首先判断中断类型是否是传输完成中断
		if(DMA_GetITStatus(DMA1_IT_TC4) != RESET) {
			//清除DMA1通道4中断到来的标志位
			DMA_ClearITPendingBit(DMA1_IT_TC4);
			//关闭DMA1通道4
			DMA_Cmd(DMA1_Channel4, DISABLE);
			//标记DMA传输完成,通知测试，DMA传输完成了
			DMA_TcFlags = 1;
		}
}

//定义串口3接收中断
void USART3_IRQHandler(void) {
	//判断是否是UART3的接收空闲中断	
	if((USART_GetITStatus(USART3, USART_IT_IDLE)) != RESET) {
		//清除中断到来的标志位
		USART3->SR;
		USART3->DR;
		
		//获取本次串口3接收数据大小
		UART3_RxCounter = UART3_RXBUFF_SIZE - DMA_GetCurrDataCounter(DMA1_Channel3);
		//对于MQTT协议，新添加拷贝接收到的数据到MQTT缓冲区中的操作,每当来数据时，MQTT_RXDATABUF
		//就会接收到服务器发送的推送消息，然后再main函数中不断解析判断推动的消息
		memcpy(MQTT_RxDataBuf, UART3_RxBuff, UART3_RxCounter);
		
		DMA_Cmd(DMA1_Channel3, DISABLE); //先禁止DMA1通道3
		DMA1_Channel3->CNDTR = UART3_RXBUFF_SIZE; //重新设置接收的数据长度
		DMA_Cmd(DMA1_Channel3, ENABLE);	 //使能DMA1通道3
	}
}

//定义DMA1通道2串口发送数据完成中断
void DMA1_Channel2_IRQHandler(void) {
	//判断是否是DMA1通道2串口发送数据完毕
	if(DMA_GetITStatus(DMA1_IT_TC2) != RESET) {
		//清除DMA1通道2发送完成标志
		DMA_ClearITPendingBit(DMA1_IT_TC2);
		//关闭DMA1通道2
		DMA_Cmd(DMA1_Channel2, DISABLE);
	}
}
