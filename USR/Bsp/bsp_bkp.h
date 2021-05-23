/*
bkp 使用头文件
*/

#ifndef _BSP_BKP_H_
#define _BSP_BKP_H_

#include "stdint.h"

void bsp_bkp_init(void);
uint32_t credit_get(void);
void credit_set(uint32_t money);
uint32_t mailcount_get(void);
void mailcount_set(uint32_t num);
uint32_t mailnum_get(void);
void mailnum_set(uint32_t num);
uint32_t flashqueue_head_get(void);
void flashqueue_head_set(uint32_t num);
uint32_t flashqueue_rear_get(void);
void flashqueue_rear_set(uint32_t num);
uint32_t mailusedsec_get(void);
void mailusedsec_set(uint32_t num);
uint32_t nochanger_get(void);
void nochanger_set(uint32_t num);
void tradenum_set(uint32_t num);
uint32_t tradenum_get(void);
void tradenum_add(void); 
void loop_channel_set(uint8_t id, uint8_t num);
void loop_channel_add(uint8_t id);
uint8_t loop_channel_get(uint8_t id);

void bkp_trayStore_reset(void);
void bkp_trayStore_set(uint8_t tray, uint8_t num);
uint8_t bkp_trayStore_get(uint8_t tray);

void bkp_trayStatistic_reset(void);
void bkp_trayStatistic_add(uint8_t tray, uint8_t num, uint16_t money );
void bkp_trayStatistic_get(uint8_t tray, uint8_t *num, uint16_t *money );
void bkp_trayStatistic_set(uint8_t tray, uint8_t num, uint16_t money );

void bkp_wholeStatistic_reset(void);
void bkp_wholeStatistic_add(uint8_t num, uint16_t money );
void bkp_wholeStatistic_get(uint32_t *num, uint32_t *money );



#endif


