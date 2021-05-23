#ifndef SETUP_H_
#define SETUP_H_

#include "stdint.h"
#include "stddef.h"

#define EEPROM_PAGE_SIZE   16  //规定页大小为16字节，EEPROM的页大小为128字节 ，但是规定为16字节不会影响使用。
#define EEPROM_PAGE(page)  (EEPROM_PAGE_SIZE* (page))
#define DATA_VERSION 		1   //当前EEPROM中的数据版本。
#define EEPROM_VALID  		0xAAAB		//标志
#define EEPROM_SPACE_ADDR	0xFFFF		//空间尾地址



//第0页 EEPROM数据信息
#define SETUP_FLAG_ADDR 		EEPROM_PAGE(0)   //eeprom 数据是否有效标志
typedef struct SETUP_FLAG
{
	uint16_t validFlag;
	uint16_t version;
}SETUP_FLAG;



//第1页 机器基本信息
#define SETUP_DEVICE_ADDR 		EEPROM_PAGE(1)
typedef struct SETUP_DEVICE
{
	uint32_t facId; 	//出厂ID
	uint32_t usrId;  	//运营商自设ID
	uint32_t data; 		//出厂日期
}SETUP_DEVICE;

//第2页 售货机全局设置
#define SETUP_COMMON_ADDR 			EEPROM_PAGE(2)
typedef struct SETUP_COMMON 
{
	uint32_t factor; 		//金额数值缩小系数。point 10的指数 小数点为2 该值为100
	uint8_t point; 			//小数点位数
	uint8_t language;		//语言
	uint8_t drvnum;			//机柜数目
	uint8_t traynum;		//托盘数目
	uint8_t screenHorizontal; //是否横屏
	//各种设置输入位数
	uint8_t numStore;	//库存/容量/弹簧输入位数
	uint8_t numPrice;	//价格位数
	uint8_t numChannel;	//货道位数
	uint8_t numGoodsId;//条码位数
}SETUP_COMMON;


//第3页 非现金支付设置
#define SETUP_CASHLESS_ADDR  		EEPROM_PAGE(3)
typedef struct SETUP_CASHLESS
{
	uint32_t port;			//刷卡接口号
	uint8_t applyEnable;	//支付宝使能
	
	
}SETUP_CASHLESS;

//第4页 现金支付设置
#define SETUP_CASH_ADDR			EEPROM_PAGE(4)	
typedef struct SETUP_CASH
{
	uint8_t coinLever;		//硬币器层级
	
}SETUP_CASH;

/*
第5页 输出回路设置
*/
#define SETUP_EX1_ADDR		EEPROM_PAGE(5)
typedef struct SETUP_EX1
{
	uint16_t starttime_1;	//第1控制回路：压缩机
	uint16_t stoptime_1;
	
	uint16_t starttime_2;	//第2控制回路：风机
	uint16_t stoptime_2;
	
	uint16_t starttime_3;	//第3控制回路：照明
	uint16_t stoptime_3;
	
	uint8_t tempTarget;		//目标温度
	uint8_t workMode;		//工作方式
}SETUP_EX1;

/*
第6页 输出回路设置
*/
#define SETUP_EX2_ADDR		EEPROM_PAGE(6)
typedef struct SETUP_EX2 	
{
	uint16_t starttime_4;	//第4控制回路：消毒
	uint16_t stoptime_4;
	
	uint16_t starttime_5;	//第5控制回路：备用1
	uint16_t stoptime_5;
	
	uint16_t starttime_6;	//第6控制回路：备用2
	uint16_t stoptime_6;
}SETUP_EX2;

/*
第7页 通讯
*/
#define SETUP_COMMUCATION_ADDR 	EEPROM_PAGE(7)
typedef struct SETUP_COMMUCATION
{
	uint32_t password;		//通讯密码
	uint16_t port;  		//服务器端口
	uint8_t enable;  		//通讯使能
	uint8_t type; 			//通讯方式
}SETUP_COMMUCATION;


	




/*
第8页 货道设置  
每个机柜80个货道
最多支持9个机柜

结构体大小计算
1、每个成员的偏移量都必须是当前成员所占内存大小的整数倍如果不是编译器会在成员之间加上填充字节。
2、当所有成员大小计算完毕后，编译器判断当前结构体大小是否是结构体中最宽的成员变量大小的整数倍 如果不是会在最后一个成员后做字节填充。
*/
#define SETUP_CHANNEL_ADDR	EEPROM_PAGE(8)
typedef  struct SETUP_CHANNEL		//
{
	uint32_t price;		//价格
	uint32_t code;		//商品编码
	uint16_t store;		//库存
	uint16_t storeBkp;	//库存的备份
	uint16_t spring;	//弹簧
	uint8_t priceCHK;	//价格校验(价格每字节和校验)
	uint8_t type;		//使能，联动
}SETUP_CHANNEL;
/*
每个元素偏移地址
这些偏移地址是相对于结构体首地址的偏移地址
就是元素在内存中的实际位置
*/
#define CHANNEL_PRICEBKP_OFFSET		offsetof(SETUP_CHANNEL, priceCHK)
#define CHANNEL_STORE_OFFSET		offsetof(SETUP_CHANNEL, store)
#define CHANNEL_STOREBKP_OFFSET		offsetof(SETUP_CHANNEL, storeBkp)
#define CHANNEL_SPRING_OFFSET		offsetof(SETUP_CHANNEL, spring)
#define CHANNEL_ENABLE_OFFSET		offsetof(SETUP_CHANNEL, type)
#define CHANNEL_CODE_OFFSET			offsetof(SETUP_CHANNEL, code)



/*
货道统计
统计每个货道销售金额与销售数量
*/
#define SETUP_COLUMN_ADDR	EEPROM_PAGE(728)
typedef  struct SETUP_COLUMN
{
	uint32_t num;		//数量
	uint32_t val;		//金额
	uint8_t state;		//货道状态
}SETUP_COLUMN;

#define STATISC_NUM_OFFSET		offsetof(SETUP_COLUMN, num)
#define STATISC_VAL_OFFSET		offsetof(SETUP_COLUMN, val)
#define CHANNEL_STATE_OFFSET	offsetof(SETUP_COLUMN, state)


void e2p_poweron_init(void);
void e2p_set_ex2(SETUP_EX2* pt);
void e2p_set_ex1(SETUP_EX1* pt);
void e2p_set_cash(SETUP_CASH* pt);
void e2p_set_cashless(SETUP_CASHLESS* pt);
void e2p_set_common(SETUP_COMMON* pt);
void e2p_set_vminfo(SETUP_DEVICE *pt);
void e2p_set_commucation(SETUP_COMMUCATION* pt);

void e2p_get_commucation(SETUP_COMMUCATION* pt);
void e2p_get_ex2(SETUP_EX2* pt);
void e2p_get_ex1(SETUP_EX1* pt);
void e2p_get_cash(SETUP_CASH* pt);
void e2p_get_cashless(SETUP_CASHLESS* pt);
void e2p_get_common(SETUP_COMMON* pt);
void e2p_get_vminfo(SETUP_DEVICE *pt);


void e2p_get_global(void);
void e2p_get_channel(uint16_t channel, SETUP_CHANNEL* pt);
bool e2p_get_price(uint16_t channel, uint32_t *dat);
bool e2p_get_channelEnable(uint16_t channel, uint8_t *dat);
bool e2p_get_store(uint16_t channel, uint16_t* dat);
bool e2p_get_spring(uint16_t channel, uint16_t* dat);
bool e2p_get_channelCode(uint16_t channel, uint32_t* dat);

bool e2p_set_channel(uint16_t channel, SETUP_CHANNEL* pt);
bool e2p_set_price(uint16_t channel, uint32_t *dat);
bool e2p_set_channelEnable(uint16_t channel, uint8_t *dat);
bool e2p_set_store(uint16_t channel, uint16_t* dat);
bool e2p_set_spring(uint16_t channel, uint16_t* dat);
bool e2p_set_channelCode(uint16_t channel, uint32_t* dat);

void e2p_reset_all(void);

bool e2p_set_storeBkp(uint16_t channel, uint16_t* dat);
bool e2p_get_storeBkp(uint16_t channel, uint16_t* dat);

bool e2p_add_channelStatistic(uint16_t channel, uint32_t num, uint32_t val);
void e2p_reset_channelStatistic(void);
bool e2p_get_channelStatistic(uint16_t channel, uint32_t* num, uint32_t *val);
bool e2p_set_channelStatistic(uint16_t channel, uint32_t num, uint32_t val);

//货道状态（中吉兴元无信号线电机）
bool e2p_set_channelState(uint16_t channel, uint8_t *dat);
bool e2p_get_channelState(uint16_t channel, uint8_t *dat);



#endif
