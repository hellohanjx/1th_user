/*
驱动板头文件

*/

#include "stdint.h"
#include "drv_header.h"
#include "ucos_ii.h"

#ifndef dri_H
#define dri_H

/*
设备状态
*/
enum{AB_NORMANL, NORMAL, INIT};

/*
对外接口,其实是文件夹中文件间的内部接口
至于温度啥的控制都是通过读取设置自动运行的
*/
uint8_t drv_cmd_channleCheck(uint8_t addr, uint8_t channel);
uint8_t drv_cmd_sale(uint8_t addr, uint8_t channel);
uint8_t	drv_cmd_channelReset(uint8_t addr, uint8_t channel);
uint8_t	drv_cmd_channelCheckall(uint8_t addr);
uint8_t	drv_cmd_channelResetall(uint8_t addr);
uint8_t	drv_cmd_openEX(uint8_t addr, uint8_t exchannel );
uint8_t drv_cmd_closeEX(uint8_t addr, uint8_t exchannel );
uint8_t drv_cmd_reset(uint8_t addr);
uint8_t	drv_cmd_checkEX(uint8_t addr, uint8_t exchannel );
uint8_t	drv_cmd_checkVER(uint8_t addr );
uint8_t	drv_cmd_checkTemp(uint8_t addr, uint8_t id );



#endif
