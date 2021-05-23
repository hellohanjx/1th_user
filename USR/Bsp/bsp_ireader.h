/*
状态机文件

*/
/*
¶Á¿¨Æ÷×ÜÏßÍ·ÎÄ¼þ
*/
#ifndef _BSP_IREADER_H_
#define _BSP_IREADER_H_

#include "stdint.h"

void bsp_ireader_init(void);
uint8_t wait_ireader_bus(void);
uint8_t lock_ireader_bus(void);
uint8_t unlock_ireader_bus(void);

typedef void (*ICREADER_RECV_DEALWITH)(uint8_t*, uint16_t);
uint8_t* cmd_icreader_send(uint8_t *dat, uint16_t size, ICREADER_RECV_DEALWITH callback)  ;

#endif
