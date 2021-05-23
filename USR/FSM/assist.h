/*
状态机文件

*/
#ifndef _ASSISIT_H_
#define _ASSISIT_H_

#include "stdint.h" 

extern volatile uint8_t assistantstate;

void assistant(void);//助手
void assistanttiming(uint16_t num);//秘书计时
void assistanttiminghm(uint16_t num);//秘书计时 50毫秒
void stopassistantming(void);//停止秘书计时

#endif
