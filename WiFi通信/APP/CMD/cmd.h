// cmd.h
#ifndef __CMD_H_ 
#define __CMD_H_

#include "stm32f10x.h"

typedef void (*cb_t)(void); 

// 定义命令数据类型
typedef struct {
	char* name; 
	cb_t  callback;
}cmd_t;

// 匹配的函数
extern cmd_t* find_cmd(char* name); 

#endif
