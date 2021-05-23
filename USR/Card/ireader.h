/*
读卡头驱动 头文件

*/

#ifndef _IREADER_H_
#define _IREADER_H_

#include "stdint.h"

/*
业务层寻卡指令
*/
enum {AB_NORMAL, NORMAL, INIT};//设备状态

uint8_t ireader_read(void);
uint8_t ireader_pay(uint32_t money);
void ireader_link(void);

#endif
