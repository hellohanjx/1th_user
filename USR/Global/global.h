/*
全局要用的变量或方法
*/
#ifndef _GOLBAL_H_
#define _GOLBAL_H_

#include "stdint.h"
#include "msg.h" 
#include "main.h"


#define MAX_H_SIZE 	8	//托盘数
#define MAX_L_SIZE  10	//货道数

//断言声明
void assert_failed(uint8_t* file, uint32_t line);
//用户日志输出
void log_fail(uint8_t* file, uint32_t line);

/*
交易行为描述
*/
typedef struct CLASS_TRADE 
{
	//当前机柜描述
	struct {
		uint8_t  container; //当前货箱
		uint8_t  tray;    	//当前托盘
		uint8_t  col;    	//当前货道（这个用backup区来存储）
	}curDiverBoard;
	
	//当前选择描述
	struct {
		char   select[11];	//第11字节用来做停止字符
		uint8_t	state;  	//当前的选择状态 0 未选，1指定了托盘，2指定了货道
		uint32_t value;  	//当前选择的值
	}select;
	
	struct {
		uint8_t state;		//输入状态（其实是用来记录输入位数）
		char numchar[12];	//为了显示,因为如果输入金额小数点会多一位，所以这里允许12字节长度
		uint32_t value;		//输入值（输入几位计算几位）
		uint8_t lenn;		//输入长度(读设置,每次设置状态时读)
		uint8_t backup;		//备份
		bool flag;			//一个状态标志
	}inputNum;
	
	//货道信息描述
	struct{
		uint32_t price; //价格（动态读设置）
		uint16_t store;	//库存（动态读设置）
	}channel;
	
	//二维码相关
	struct{
		uint8_t token[18];
		char *timeStamp;//时间戳
		uint8_t type;//支付类型 0x32支付宝，0x33微信
		uint8_t stamp[14];//记录时间戳
	}qrcode;
	
	uint8_t businessmode; //0现金 1 IC卡 2 手机支付
	uint32_t saleCostTime;//出货耗费时间
}CLASS_TRADE;

/*
状态参数，用于控制状态的行为
*/
typedef struct CLASS_FSM_STATUS  
{
	uint8_t mode; 	//指示目标状态进入某种模式
	uint8_t status; //用于记录状态内部处理时产生的中间态
	uint8_t count; 	//用于状态内部计数，
	uint8_t shortcut; //快捷方式标志
	uint16_t num;	//状态时间计数

}CLASS_FSM_STATUS;



/*
描述售货机整机参数
这里的参数都是开机就读取的
*/
typedef struct CLASS_VM_SET
{
	uint8_t DT;	//强制购买
	uint8_t comDtu;//
	
	uint8_t language;	//语言
	uint8_t point;		//小数点位数（开机读读设置,各种输入长度，以及金额输入长度）
	uint8_t priceBit;	//价格位数
	uint8_t channelBit;	//货道位数
	uint8_t storeBit;	//库存位数
	uint8_t goodsIdBit;	//条码位数
	uint8_t keyState;	//键盘触发方式切换
	uint8_t door;		//门碰 
	
	uint8_t linkState;	//连网状态
	uint8_t linkNum;	//通讯流水号
	
	uint8_t storeEnable;//库存使能
	uint8_t applyEnable;//二维码支付开关
	
	uint8_t maxChannelNum;	//每托盘货道数
	uint8_t maxtrayNum;		//托盘数
	uint8_t maxContainerNum;//机柜数目
}CLASS_VM_SET;


/*
提示信息结构体
实现定时清除
认为提示只是一行
并且回复底色是背景色
*/
typedef struct CLASS_PROMPT
{
	uint8_t fontsize;//单字符一边点阵数
	uint16_t x;
	uint16_t y;
	uint32_t len;
}CLASS_PROMPT;




/*
描述消息助手
*/
typedef struct CLASS_ASSISIT{
	uint8_t  state;
	uint16_t value;

}CLASS_ASSISIT;



//暂时用的(这些后面都要替换掉)
extern uint8_t CA306;
extern volatile uint8_t coin_level;			//硬币器层级控制
extern uint32_t sysfactor;
extern uint8_t check_w_power_on;







/*
全局变量声明
*/
extern volatile CLASS_VM_SET g_vm;	//售货机参数
extern volatile CLASS_FSM_STATUS g_fsm;	//状态机状态
extern volatile CLASS_TRADE g_trade;//交易相关

/*
声明
*/
	
void tradenumadd(void);
void select_goods(uint8_t key);
char *select_char_get(void);
bool select_state_get(void);
void select_clear(void);
uint16_t select_value_get(void);

void input_num(uint8_t key);
void input_money(uint8_t key);
void input_clear(void);
void input_len_set(uint8_t len);
char* input_char_get(void);
uint8_t input_time(uint8_t key);
uint8_t input_len_get(void);
void fsm_value_clear(void);

uint32_t input_value_get(void);
uint8_t input_data(uint8_t key);

#endif
