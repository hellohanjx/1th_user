/*
状态机文件

*/

#ifndef _COMMUCATION_H_
#define _COMMUCATION_H_

#include "stdint.h"

void bsp_commucation_init(void);
void com_send(uint16_t size);
uint8_t *commucation_recv_get(void);
uint8_t *commucation_send_get(void);
uint32_t commucation_rxlen_get(void);
typedef void (*COMMUCATION_RECV_DEALWITH)(uint8_t*, uint16_t);
void commucation_send(uint8_t *dat, uint16_t size, COMMUCATION_RECV_DEALWITH callback)  ;

#endif
