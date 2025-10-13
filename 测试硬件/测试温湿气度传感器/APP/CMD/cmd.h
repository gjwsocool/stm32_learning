#ifndef __CMD_H
#define __CMD_H

#include "stm32f10x.h"
#include "system.h"

/*声明函数指针变量数据类型*/
typedef void (*cb_t)(void);

/*声明描述命令的数据结构*/
typedef struct _cmd {
    char *name; //命令的名称
    cb_t call_back; //命令对应的执行函数
}cmd_t;

/*声明根据命令名称找命令对象的函数*/
//一旦找到,返回命令对象的首地址
extern cmd_t *find_cmd(char *name);

#endif
