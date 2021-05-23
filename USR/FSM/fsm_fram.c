/*
状态机文件
状态机框架
*/

#include "header.h"
#include "fsm_fram.h"

const FSM_STAUS List_status[]={
						{fsm_id_selfCheck, 	0,		fsm_fun_selfCheck},
						{fsm_id_init, 		0,		fsm_fun_welcom}, 
						{fsm_id_choose, 	10,		fsm_fun_chooseGoods},
						{fsm_id_cashTrade,	0,		fsm_fun_cashTrade},
						{fsm_id_cardOlinePay,60,	fsm_fun_cardOnlinePay},
						{fm_card,			0,		NULL},
						{fm_sale,			0,		NULL},
						{fsm_id_lowMenu, 	0,		fsm_fun_lowerMenu},
						{fsm_id_seniorMenu,	0,		fsm_fun_seniorMenu},
						{fsm_id_vmInfo,		0,		fsm_fun_vmInfo},
						{fsm_id_vmSet,		0,		fsm_fun_vmSet},
						{fsm_id_payment,	0,		fsm_fun_payment},
						{fsm_id_temperatureSet,	0,		fsm_fun_temperatureSet},
						{fsm_id_peripheralSet,	0,		fsm_peripheralSet},
						{fsm_id_channelSet,		0,		fsm_fun_channelSet},
						{fsm_id_channelTest,	0,		fsm_fun_channleTest},
						{fsm_id_goodsSet,		0,		fsm_fun_goodsSet},
						{fsm_id_cardOnlineCheck, 6,		fsm_fun_cardOnlineCheck},
						{fsm_id_qrcode, 		0,		fsm_fun_qrcodePay},
						{fsm_id_vmSimpleInfo,	5,		fsm_fun_vmSimpleInfo},
						{fsm_id_driverSet,		0,		fsm_driverboardSet},
						{fsm_id_statistic,  	60, 	fsm_fun_statistic}
						};
						

						
						
const FSM_STAUS *cur_status = NULL;			
FSMMSG  fsmMsg;//用来记录上一个消息
FSMMSG* curMsg;//当前消息指针

//队列
#define FSM_QUEEN_SIZE 50	//状态机消息队列大小
OS_EVENT *fsmQueen;
void *fsmmsg[FSM_QUEEN_SIZE];


						
/*
状态机初始化
*/
/*
系统消息初始化
*/
static void fsm_init(void)
{
	//主要是初始化消息队列
	fsmQueen = OSQCreate(fsmmsg, FSM_QUEEN_SIZE);
}
						
/*
状态机切换
*/
void Load_status(uint8_t id)
{
	if(id != fm_null)
	{
		fsmMsg.type = MsgSYS;
		fsmMsg.stype = FSM_STATUS_EXIT;
		stopassistantming();
	    cur_status->fsm_fun(&fsmMsg);
	    
		cur_status=& List_status[id];
		fsmMsg.stype = FSM_STATUS_INIT;
		cur_status->fsm_fun(&fsmMsg);
	}
}

/*
状态机任务
*/
void FSM_RUN(void)
{
	uint8_t err;
	
	fsm_init();//状态机初始化
	
	cur_status = &List_status[fsm_id_selfCheck];
	fsmMsg.type = MsgSYS;
	fsmMsg.stype = 0;
	cur_status->fsm_fun(&fsmMsg); //调用消息处理函数
	while(1)
	{
		curMsg = WaitSYSMSG((cur_status->timeout)*OS_TICKS_PER_SEC, &err);//等待状态机消息
		if(err == OS_TIMEOUT)//超时
		{
			fsmMsg.type=MsgSYS;
			fsmMsg.stype=FSM_STATUS_TIMEOUT;
			cur_status->fsm_fun(&fsmMsg); 
		}
		else//处理得到消息
		{
			fsmMsg.type=curMsg->type;
			fsmMsg.stype=curMsg->stype;
			fsmMsg.value=curMsg->value;
			release_fsm_msg(curMsg);
			cur_status->fsm_fun(&fsmMsg);//调用消息处理函数
		}
	}	
}

