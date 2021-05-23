/*
读卡设备框架头文件

*/

#ifndef _CARD_FRAM_H_
#define _CARD_FRAM_H_

#include "stdint.h"
#include "ucos_ii.h" 

extern OS_EVENT * sem_icreader_recv;//读卡头接收数据信号量
void icreader_state_change(uint8_t state);



#endif
