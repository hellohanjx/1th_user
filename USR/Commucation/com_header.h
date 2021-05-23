/*
通讯对外接口头文件

*/

#ifndef _COM_HEADER_H_
#define _COM_HEADER_H_

#include "stdint.h"

//通讯接收数据处理回调函数，发送可以和接收放在一个文件中
typedef uint8_t (*COM_RECV_CALLBACK)(uint8_t *recv, uint16_t recv_len,uint8_t *dat);
/*
信封类型
里面就保存两个指针
一个是回调函数指针
一个是数据指针
*/
typedef struct MAIL
{
	 COM_RECV_CALLBACK com_call_back;
	 uint8_t *addr;
}MAIL;


void commucation_task(void);//通讯主任务
MAIL* Apply_for_mail(uint16_t size);

//外部可调用本模块的这些东西
uint8_t send_moneydelete_msg(uint8_t type,uint32_t money);
uint8_t send_moneyaccept_msg(uint8_t stype,uint16_t value,uint8_t num);
uint8_t status_equip_msg(uint8_t id,uint8_t sid,uint8_t state); 

void send_online_cardRequest(uint8_t type);
void send_online_cardRequestResult(uint8_t type);

void report_temp_value(uint8_t id, uint8_t state, uint32_t value);
void report_channel_set(uint8_t type, uint32_t value);
void report_trade_result(uint8_t num,uint8_t payType, uint8_t saleResult);
void report_device_set(uint8_t type, int value);
void report_state_change(uint32_t equType, uint32_t id, uint32_t state);

void send_qrcode_dat(uint8_t type, uint8_t rs);



/*
提供给业务层的宏
*/
/*
0x11-0x30 包
设置类；整机相关
后面新增的在下面增加类
*/

#define PARAM_TEMPTARGET		1	//设置目标温度
#define PARAM_SLAEMODE			2	//购买方式
#define	PARAM_PAR_VALUE			3	//接收现金面额
#define	PARAM_TEMPETURE_MODE	4	//温控方式
#define	PARAM_SCAN_SWITCH		5	//扫码开关
#define	PARAM_CARD_INTERFACE	6	//刷卡接口
#define	PARAM_GETGOODS_INTERFACE	7//取货接口
#define	PARAM_LANGUAGE			8	//系统语言
#define	PARAM_CONTAINER_BIT		9	//机柜数量
#define	PARAM_POINT_BIT			10	//小数位数
#define	PARAM_PRICE_BIT			11	//价格位数
#define	PARAM_INPUT_BIT			12	//输入位数
#define	PARAM_CHANNEL_BIT		13	//货道位数
#define	PARAM_TRAT_SIZE			14	//托盘数量



/*
0x11-0x31
设置类，货道相关
*/
#define C_CHANNEL_GOODSID		0X31		//设置商品编码
#define	C_CHANNEL_PRICE			0X32		//设置商品价格
#define	C_CHANNEL_STORE			0X33		//设置商品库存
#define C_CHANNEL_SPRING		0X34		//设置商品容量(弹簧圈数)
#define C_CHANNEL_LINK			0X35		//设置货道联动
#define C_CHANNEL_ENABLE		0X36		//使能/禁能货道
#define C_CHANNEL_DROP			0X37		//掉货检测

#define WHOLE	3
#define LAYER	2
#define COL		1


/*
0x12-0x32
定时发送
*/
#define	C_INFO_TEMP		1	//温度
#define C_INFO_HUMODOTY	2	//湿度
#define C_INFO_POWER	3	//功耗




/*
0x12-0x36
状态改变时发送
*/
//设备类型
#define	STATE_CHANNEL		1	//货道
#define	STATE_PAY			2	//支付设备
#define	STATE_TEMP			3	//温控	
#define	STATE_DOOR			4	//门碰
#define	STATE_DRV			5	//驱动板
#define	STATE_DROP			6	//掉货检测
#define STATE_COLD			7	//制冷开关
#define STATE_WIND			8	//风机开关
#define STATE_LAMP			9	//照明
#define STATE_DISINFACT		10	//消毒灯
#define STATE_GLASS			11	//玻璃加热
#define STATE_BACKUP2		12	//备用2
//设备编码
#define EQU_COIN	1	//硬币器
#define EQU_BILL	2	//纸币器
#define EQU_ICCARD	3	//读卡器
#define EQU_DRV		4	//驱动板






/*
0x17-0x41
*/
#define ONLINE_PAY			0x31	//申请扣款
#define ONLINE_CHECK		0x32	//查余额
#define ONLINE_SALE_OK		0x33	//出货成功
#define ONLINE_SALE_FAIL	0x34	//出货失败
#define ONLINE_RECHARGER	0x35	//充值上报


/*
0x17- 0x30
*/
#define QRCODE_CHECK	0x31	//查询二维码支付结果
#define QRCODE_RESULT	0x32	//汇报二维码出货结果



/*
0x14-0x36
*/
#define	SALE_TYPE_CASH		1
#define	SALE_TYPE_APPLY		2
#define	SALE_TYPE_WEI		3
#define	SALE_TYPE_UNION		4
#define	SALE_TYPE_CODE		5
#define	SALE_TYPE_CARD		6





#endif
