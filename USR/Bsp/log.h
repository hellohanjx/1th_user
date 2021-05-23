/*
log日志接口头文件

*/
#ifndef _LOG_H_
#define _LOG_H_

#include "stdint.h"

void log_init(void);
void log_send(uint8_t *dat, uint16_t line);

#endif
