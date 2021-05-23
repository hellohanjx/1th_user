/*
调货检测头文件

*/
#ifndef _DROP_H_
#define _DROP_H_

#include "stdint.h"

#define  DROP_RESET 0xF0 //复位
#define  DROP_STATE 0xF1 //查询工作状态
#define  DROP_START 0xF2 //检测开始
#define  DROP_END   0xF3 //检测结束
#define  DROP_REQU  0xF4 //检测结果。
#define  DROP_DEBUG 0xF5 //工作详情


#define  MSG_DS_STATUS 0
#define  MSG_DS_REQU   1
#define  MSG_DS_CHECK  2
#define  MSG_DS_INIT   3
#define  MSG_DS_START  4

void DropInit(void);
void API_DROP_CHECK(uint8_t num);
void API_DROP_WORKSTART(uint8_t num);
void API_DROP_WORK_REQU(uint8_t num);
void API_DROP_WORK_END(uint8_t num);
void TASK_DROP_RUN(void);

#endif
