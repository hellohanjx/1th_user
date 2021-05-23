/*
状态机文件

*/
#ifndef _MSH_H_
#define _MSH_H_

#include "stdint.h"

//定义消息类型
#define MsgSYS    0    //状态机消息
#define MsgKEY    1    //按键消息
#define MsgCOIN   2    //硬币器消息
#define MsgBILL   3    //纸币器消息
#define MsgCARD   4    //读卡器消息
#define MsgPHONE  5    //手机卡读卡器消息
#define MsgCHAN   6    //出货机构消息
#define MsgEVN    7    //环境系统消息
#define MsgCOMIT  8    //通讯模块消息。
#define MsgEVENT  9    //事件
#define MsgASSIS  10   //秘书消息。
#define MsgDROP   11   //调货检测消息
#define MsgLIFT   12   //升降机信息
#define MsgPC     13   //上位机消息
#define MsgHT     14   //后台消息
#define MsgSCAN   15	//网络消费信息
#define MsgLftEx   16   //升降机拓展消息
#define MsgVoice   17   //语音模块信息
#define MsgSensor  18	//人体感应器
#define MsgQkPass	19	//quick pass 设备
#define MsgRemote	20	//远程控制消息
#define MsgOnline	21	//在线卡
#define MsgQrcode	22	//二维码


#define FSMMSG_SIZE_DEFAULT	0//默认消息大小
/*
定义系统消息结构
*/
typedef struct FSMMSG	
{
	uint8_t   type; 	//消息类型
	uint8_t   stype; 	//消息子类型
	uint32_t  value; 	//消息的值
	uint16_t  exLen;	//附加数据长度
	void  *dat;			//附加数据指针
}FSMMSG;



void SYSMsgInit(void); //消息系统初始化
FSMMSG* apply_fsm_msg(uint16_t size); //申请一个消息。
void  release_fsm_msg(FSMMSG * msg);//释放一个消息
uint8_t send_fsm_msg(FSMMSG *msg);  //发送一个系统消息。
uint8_t SendFSYSMSG(FSMMSG *msg);  //发送一个系统消息，并插到队列最前面。
FSMMSG * WaitSYSMSG(uint16_t timeout,uint8_t *err);//等待一个系统消息


#endif
