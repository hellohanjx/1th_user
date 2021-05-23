/*
纸币器头文件
*/

#ifndef BILL_H
#define BILL_H

#include "stdint.h"
#include "mdb_header.h"

#define BILL_NO_RESPENSE 10		
#define BILL_NRLIMT      2000

//定义纸币器的命令

#define BILL_RESET      0x30     //纸币器复位
#define BILL_SETUP      0x31     //获得纸币器的设置
#define BILL_SECURITY   0x32     //安全模式
#define BILL_POLL       0x33     //POLL纸币器
#define BILL_TYPE       0x34     //控制禁使能
#define BILL_ESCROW     0x35	 //压钞动作
#define BILL_STACKER    0x36	 //检测钞箱是否已满
#define BILL_EXP		0x37



#define M_BS_ABOX       0        //接收到纸币到钞箱
#define M_BS_AESC       1        //接收纸币到暂存区
#define M_BS_ESCROW     2         //暂存纸币操作
#define M_BS_OTHER      3         //其他
#define M_BS_POLL       4
#define M_BS_TYPE       5        
#define M_BS_STATUS     6       //纸币器状态改变
#define M_BS_INIT       7		//初始化
#define M_BS_AOUT       8
#define M_BS_RECY		9		//找零箱
#define M_BS_DISTYPE	10		//禁能的纸币类型

#define M_BS_ATRS       11       //找零箱到钱箱 
#define M_BS_PAYOUT		12		//找零
#define M_BS_PAYRESULT	13		//找零结果
#define M_BS_RESET		14		//纸币器复位
#define	M_BS_RECYINFO	15		//退币异常

typedef struct BillValtdator
{
	uint8_t    level;//层级
	uint16_t   BCD;	//货币代码
	uint16_t   factor;
	uint8_t    point;
	uint16_t   capacity;
	uint16_t   security;
	uint8_t    escrow;//是否有暂存功能
	uint8_t    par_value[16];
	uint8_t    full;
	uint16_t   stock;
	uint16_t   escvalue;//暂存箱中的金额
	uint8_t    error[16];
	uint8_t   changetype;//找零方式
	uint8_t 	diagnostic;//扩展统计方式
	uint8_t	fill;//补币扩展功能(是能后可以在0F 06后poll不返回接收硬币消息)
	uint8_t	ftl;//文件传输层
	uint16_t  par_enable;
	
	uint8_t 	recycler;	//支持找零箱功能
	uint16_t  recyvalue;	//进入找零箱中的现值
	uint32_t  alldisvalue;//找零箱中总金额
	uint8_t 	recy_value[16];//可找零币种面值
	uint8_t	disfull[16];//找零箱已满
	uint8_t	disParNum[16];//找零箱各种币种数
	uint8_t   lowest;		//找零的最小面额
	uint8_t 	highest;	//找零的最大面额
	uint16_t	recypath;	//找零箱可找类型(路径)
}BillValtdator;

extern mdbListNode billequip;
extern BillValtdator billvaltdator;
extern uint32_t billfactor;
void BillValtdatorInit(void);
void savebillaccept(uint16_t num);
uint8_t ESCROW(uint8_t direction);
uint8_t CE_BILL(uint8_t type);
void savebillaccept(uint16_t num);
uint8_t Check_Dispen_Bill(uint32_t money,uint32_t* jackson);
uint8_t Dispen_Bill(uint32_t money);
uint8_t get_dis_money(uint32_t *money,uint32_t *num,uint32_t *type);
uint8_t exit_bill_onlyone(void);
#endif
