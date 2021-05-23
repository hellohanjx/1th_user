/*
状态机文件
状态机框架
*/

#ifndef _FSM_FRAME_H_
#define _FSM_FRAME_H_



#include "stdint.h"
#include "msg.h"

#define FSM_STATUS_INIT      0
#define FSM_STATUS_TIMEOUT   1
#define FSM_STATUS_EXIT      2

/*
状态机序号
version_1;20151219
*/
#define  	fm_null         	255  	//空状态
#define  	fsm_id_selfCheck    0  		//开机自检     
#define		fsm_id_init			1		//待机		
#define 	fsm_id_choose		2	//选货
#define		fsm_id_cashTrade	3	//现金
#define		fsm_id_cardOlinePay	4	//非现金
#define		fm_card				5	//刷卡（非离线卡）
#define		fm_sale				6	//出货
#define 	fsm_id_lowMenu			7	//低权限菜单
#define 	fsm_id_seniorMenu		8	//高级菜单
#define 	fsm_id_vmInfo			9	//机器信息
#define		fsm_id_vmSet			10	//机器设置
#define 	fsm_id_payment			11	//支付设置
#define		fsm_id_temperatureSet			12	//温度设置
#define		fsm_id_peripheralSet		13	//控制回路设置
#define		fsm_id_channelSet		14	//货道设置
#define		fsm_id_channelTest		15	//货道测试
#define		fsm_id_goodsSet		16	//商品设置
#define		fsm_id_cardOnlineCheck	17	//在线卡查询
#define		fsm_id_qrcode			18	//扫码支付
#define		fsm_id_vmSimpleInfo		19	//快捷键查询信息
#define		fsm_id_driverSet		20	//驱动板参数设置
#define 	fsm_id_statistic		21	//统计

void Load_status(uint8_t id);	//状态装载和转换函数
void FSM_RUN(void); 			//状态机运行函数

typedef uint8_t (*FSM_MSG_dealwith)(FSMMSG*);//函数指针

//状态机结构体
typedef struct FSM_STAUS{
   uint8_t  id;					//状态机序号
   uint16_t timeout;            //最长等待时间 （秒）
   FSM_MSG_dealwith   fsm_fun;	//状态的消息处理函数
}FSM_STAUS;

/**********************************************
状态机消息函数
**********************************************/
uint8_t fsm_fun_selfCheck(FSMMSG* msg);//开机自检测
uint8_t fsm_fun_welcom(FSMMSG* cmsg);
uint8_t fsm_fun_chooseGoods(FSMMSG* cmsg);
uint8_t fsm_fun_lowerMenu(FSMMSG* cmsg);
uint8_t fsm_fun_seniorMenu(FSMMSG* cmsg);
uint8_t fsm_fun_vmSet(FSMMSG* cmsg);
uint8_t fsm_fun_payment(FSMMSG* cmsg);
uint8_t fsm_fun_goodsSet(FSMMSG* cmsg);
uint8_t fsm_fun_channleTest(FSMMSG* cmsg);
uint8_t fsm_fun_temperatureSet(FSMMSG* cmsg);
uint8_t fsm_peripheralSet(FSMMSG* cmsg);
uint8_t fsm_fun_channelSet(FSMMSG* cmsg);
uint8_t fsm_fun_cardOnlinePay(FSMMSG * cmsg);
uint8_t fsm_fun_cardOnlineCheck(FSMMSG *cmsg);
uint8_t fsm_fun_qrcodePay(FSMMSG * cmsg);
uint8_t fsm_fun_vmInfo(FSMMSG* cmsg);
uint8_t fsm_fun_vmSimpleInfo(FSMMSG* cmsg);
uint8_t fsm_driverboardSet(FSMMSG* cmsg);
uint8_t fsm_fun_cashTrade(FSMMSG* cmsg);
uint8_t fsm_fun_statistic(FSMMSG* cmsg);



#endif
