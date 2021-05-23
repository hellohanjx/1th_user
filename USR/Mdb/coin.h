/*
硬币器头文件

*/
#ifndef COIN_H
#define COIN_H

#include "stdint.h"
#include "mdb_header.h"

#define COIN_NO_RESPENSE  10   //硬币器超时时限
#define COIN_NRLIMT 2000      //超时总时限 2S
//定义硬币器的命令
#define COIN_RESET      0x08  //硬币器复位命令
#define COIN_SETUP      0x09  //硬币器参数查询
#define COIN_POLL       0x0B  //硬币器巡查命令
#define COIN_TUBESTATUS 0x0A  //硬币器储币管状态
#define COIN_TYPE       0x0C  //硬币器接受币种
#define COIN_DISPENSE   0x0D  //硬币器退币命令
#define COIN_EXP		0x0F  //硬币器扩展指令

#define M_CS_ACCEPT      0      //接收硬币
#define M_CS_DISPENSED   1      //层级3的退币信息
#define M_CS_PAYOUT      2      //退币命令
#define M_CS_OTHER       3      //其他类型
#define M_CS_POLL        4
#define M_CS_TYPE        5      //TYPE命令
#define M_CS_STATUS      6      //硬币器状态改变
#define M_CS_INIT        7

#ifndef COINHOPPER//这与硬币接收重复定义
  #define M_CS_EN					8	//使能
  #define M_CS_UN					9	//禁能
  #define M_CS_MANUAL_DIS  			10  //人工退币
  #define M_CS_PAYRESULT_LEV3 		11  //层级3以上的退币
#endif
  


typedef struct CoinChanger
{
	uint8_t   level;             //层级
	uint16_t  BCD;              //货币代码
	uint8_t   factor;      //比例因子
	uint8_t   point   ;          //小数点位置
	uint16_t  path   ;       //路径选择
	uint8_t   par_value[16] ;     //硬币面值
	uint16_t  full;         //储币管满
	uint8_t   store[16];        //储币管中硬币数目。
	uint16_t  allvalue;     //储币管中硬币总金额。
	uint8_t   lowest;
	uint8_t   highest;
	uint16_t  extenderr[24];
	uint8_t   changetype;//找零方式
	uint8_t 	diagnostic;//扩展统计方式
	uint8_t	fill;//补币扩展功能(是能后可以在0F 06后poll不返回接收硬币消息)
	uint8_t	ftl;//文件传输层
}CoinChanger;





extern mdbListNode coinequip;
extern CoinChanger   coinchanger;
extern uint32_t coinfactor;

void CoinChangerInit(void);
uint8_t CheckDispense(uint32_t money); //检查是否可以找零
uint8_t DISPENSE(void);//对外部程序提供的退币接口
void savecoinaccept(uint16_t num);
void savedispense(uint16_t num,uint8_t count);
uint8_t CE_COIN(uint8_t type);
uint8_t enloughcoin(void);
uint8_t NewCheckDispense(uint32_t money,uint32_t* real);
uint8_t NewDISPENSE(uint32_t money);//退币
#endif
