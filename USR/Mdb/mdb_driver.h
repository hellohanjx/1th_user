/*
mdb驱动头文件
*/

#ifndef MDB_DRVIER_H_
#define MDB_DRVIER_H_

#include "stdint.h"

void uart4_init(void);
uint16_t* mdb_send_cmd(uint8_t *cmd, uint8_t n, uint8_t ret, uint16_t timeout, uint8_t *rs, uint16_t *rx_len);

#endif

