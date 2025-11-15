// usart.c
#include "usart.h"
#include "stdio.h"

void UART_Init(void){
	// 1.打开GPIOA/USART1控制器的时钟 - APB2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	// 2.配置PA9 - 推挽复用输出, 50MHz
	GPIO_InitTypeDef GPIO_Config;
	GPIO_Config.GPIO_Pin = GPIO_Pin_9;
	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Config.GPIO_Mode  = GPIO_Mode_AF_PP; 
	GPIO_Init(GPIOA, &GPIO_Config);
	
	// 3.配置PA10 - 浮空输入
	GPIO_Config.GPIO_Pin  = GPIO_Pin_10;
	GPIO_Config.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_Init(GPIOA, &GPIO_Config);
	
	// 4.配置工作参数: 115200 8n1 TX/RX 不要硬件流控制
	USART_InitTypeDef UART_Config;
	UART_Config.USART_BaudRate   = 115200; // 波特率 
	UART_Config.USART_WordLength = USART_WordLength_8b; // 数据位8位
	UART_Config.USART_Parity     = USART_Parity_No; // 不校验
	UART_Config.USART_StopBits   = USART_StopBits_1; // 停止位1位
	// 发送 + 接收模式
	UART_Config.USART_Mode			 = USART_Mode_Tx | USART_Mode_Rx; 
	// 不要硬件流控制
	UART_Config.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART1, &UART_Config);
	
	// 5.使能串口1
	USART_Cmd(USART1, ENABLE);
	
	// 6.配置NVIC支持串口1的中断 抢0响2
	NVIC_InitTypeDef NVIC_Config;
	NVIC_Config.NVIC_IRQChannel 									= USART1_IRQn; // 串口1的中断
	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Config.NVIC_IRQChannelSubPriority        = 2;
	NVIC_Config.NVIC_IRQChannelCmd 								= ENABLE;
	NVIC_Init(&NVIC_Config);
	
	// 7.配置串口1支持RXNE中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

// 参数 : 要发送的字符 
void UART_Putc(char c){
	// 放入c到DR中
	// TC, SET, 可以放入; RESET, 不能放入 
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	// 将字符c放到了串口1的DR中 
	USART_SendData(USART1, c); 
}
// 发送一个字符串
// char* pstr = "hello";
//							   ^-pstr
void UART_Puts(char* pstr){
	while (*pstr) {
		UART_Putc(*pstr);
		pstr++;
	}
}
// 接收一个字符 
char UART_Getc(void) {
	// RXNE SET, 可以读取; RESET, 等待;
	while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
	
	return (char)USART_ReceiveData(USART1);
}
// 获取一个字符串
// char buf[len];
// "hello" -> "hello\r\n" -> "hello"
//						"hello\0\n"
void UART_Gets(char* buf, u32 len) {
	int i = 0;
	for(i = 0; i < (len - 1); i++) {
		buf[i] = UART_Getc();
		if (buf[i] == '\n')
			break;
	}
	buf[i - 1] = '\0';
} 

// 功能 : printf函数重定向
// printf函数多次调用该函数, 每次调用都会将一个字符给参数1
// 参数1 : 依次获取printf函数的一个字符
int fputc(int c, FILE* stream) {
	
	// 判断缓冲区是否为空
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);

	// 每次获取字符发送到串口1的DR中
	USART_SendData(USART1, (u8)c);
	
	return c;
}









