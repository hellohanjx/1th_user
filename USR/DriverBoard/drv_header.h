/*
驱动板对外公开头文件

*/

#ifndef _DRV_HEADER_H_
#define _DRV_HEADER_H_

#include "stdint.h"

/*
定义驱动板
*/
#define CHANNEL_SIZE	80	//机柜货道总数
#define EX_SIZE			6	//控制回路总数
#define TEMP_SIZE		2	//测温回路总数
#define VER_SIZE		11	//版本号长度

/*
定义驱动板外部回路
*/
#define EX_COLD   		0
#define EX_WIND   		1
#define	EX_LIGHT		2
#define EX_DISINFECT  	3	//消毒
#define EX_BACKUP1  	4	//备份1，暂定为玻璃加热
#define EX_BACKUP2 		5	//备份2

/*
驱动板描述
*/
typedef struct DRIVERBOARD{
 	uint8_t state; 						//驱动板状态 ，0没有连接，1已经连接，5未檢查
 	uint8_t exState[EX_SIZE];     		//6路输出通道状态
 	uint8_t channelState[CHANNEL_SIZE];	//货道状态
	char	version[VER_SIZE];			//版本号
	
	struct{
		uint8_t state[TEMP_SIZE];	//温感探头状态
		int value[TEMP_SIZE];		//温度
		uint8_t stoptimer;		//压缩机定时停机（保护压缩机）
	}temp;
 	
	struct {
		uint8_t count;         	//过流允许次数
		uint8_t value;         	//过流保护阀值
	}overload;
}DRIVERBOARD;

/* 
定义驱动板消息
作用：用来与业务层交互
说明：温度指令与回路控制指令并不提供给状态机
*/
#define M_CHANNEL_SALE    0
#define M_CHANNEL_TEST    1
#define M_CHANNEL_STATE   3
#define M_CHANNEL_CHECK   2
#define M_CHANNEL_INIT    4
#define M_DRV_CHECK		5	
#define M_DRV_INIT   	6
#define M_DRV_RESET		7
#define M_DRV_ERR		10//驱动板故障

/*
定义电机状态
*/
#define MOTO_OK			1	//正常
#define MOTO_PLACE_ERR	2	//不在卡位
#define MOTO_OVER		3	//过流
#define MOTO_START_TIMEOUT	4	//超时未启动
#define MOTO_STOP_TIMEOUT	5	//超时未停止
#define MOTO_LACK			6	//没有电机
#define MOTO_FORBID			7	//电机禁能
#define MOTO_LINK			8	//电机联动

/*
对外接口函数
*/
void drv_task(void);		//驱动板主任务
void drv_self_task(void);	//驱动板自动运行任务
uint8_t apply_driverboard_msg(uint8_t cmd, uint8_t id, uint8_t channel, uint16_t extend, uint8_t *dat);
extern volatile DRIVERBOARD *pt_driverBoard[10];//声明驱动板指针
uint8_t drv_cmd_channleCheck(uint8_t addr, uint8_t channel);
uint8_t drv_cmd_reset(uint8_t addr);

uint8_t	drv_cmd_getParam(uint8_t addr , uint16_t *dat);
uint8_t	drv_cmd_setParam(uint8_t addr , uint16_t *dat);
uint8_t	drv_cmd_resetParam(uint8_t addr);

#endif
