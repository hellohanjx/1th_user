/*
读卡器设备架构

*/
#include "header.h" 
#include "ireader.h" 
#include "card_reader_fram.h"

#define IREADER_QUEEN_SIZE	5	//读卡器队列大小
/*
定义全局变量
*/
IC cIC;
static void *icmsg[IREADER_QUEEN_SIZE];
static OS_EVENT* icreaderQueen;  	//IC卡命令消息队列

/*
读卡器对业务层接口。
业务层调用，发送信息到读卡器设备，异步执行
说明，消息类型直接复用状态机的消息类型，里面数据都不用重新填写（小技巧）
*/
uint8_t send_icreader_msg(uint8_t cmdtype, uint8_t value)
{
	FSMMSG *imsg;
	imsg = apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
	log_output(imsg);
	if(imsg != 0)
	{
		imsg->type = MsgCARD;
		imsg->stype = cmdtype;
		imsg->value = value;
		if(OSQPost(icreaderQueen, imsg)!=OS_NO_ERR){
			release_fsm_msg(imsg);
			return FALSE;
		}
		return TRUE;
	}
	return 2; 
}

/*
读卡器状态汇报
*/
void icreader_state_change(uint8_t state)
{
	if(state != cIC.state)
	{
		if(cIC.state == INIT)//初始状态（此状态为开机，不汇报，因为有开机汇报包）
		{
			FSMMSG* imsg;
			//发送消息
			imsg = apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
			log_output(imsg);//日志输出
			cIC.state = state;
			if(imsg!= 0)
			{
				imsg->type = MsgCARD;
				imsg->stype = ICREADER_INIT;
				if(!send_fsm_msg(imsg))
				release_fsm_msg(imsg);
			}
			/***********
			这个赋值的前后顺序与状态机的优先级有关，
			如果状态机的优先级任务高于当前任务，则需要赋值在
			send_fsm_msg() 之前，否则消息发送后，状态机立即抢占
			cpu资源，导致下面这句话没有来得及执行
			***********/
			cIC.state = state;
		}
		else//这里不用发消息，因为有全局变量
		{
			cIC.state = state;
			report_state_change(STATE_PAY, EQU_ICCARD, state);//汇报状态改变
		}
	}
}

/*
设备初始化
*/
static void ireader_init(void)
{
	//设备初始化
	icreaderQueen = OSQCreate(icmsg,IREADER_QUEEN_SIZE);//创建队列
	sem_icreader_recv = OSSemCreate(0); //创建接收信号量
	cIC.state = INIT;
	ireader_link();//连机查询
}

/*
读卡设备主任务
*/
void ireader_task(void)
{
	FSMMSG* imsg;
	uint8_t err;
	ireader_init();
	
	while(1)
	{
		/*
		那个简易刷卡头,主动推送消息
		*/
//		if(wait_ireader_bus() == OS_NO_ERR)
//		{
//			imsg = apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
//			assert_param(imsg);
//			imsg->type = MsgCARD;
//			imsg->value = 1;
//			if(!send_fsm_msg(imsg))
//			release_fsm_msg(imsg);
//		}
		
		/*
		一问一答的刷卡头
		*/
		imsg = (FSMMSG*)OSQPend(icreaderQueen, OS_TICKS_PER_SEC*10, &err);//队列等待
		if(err == OS_NO_ERR)
		{
			switch(imsg->stype)
			{
				case ICREADER_FIND:
					imsg->value =  ireader_read();
					if(!send_fsm_msg(imsg))
					release_fsm_msg(imsg);
				break;
				
				case ICREADER_PAY:
					imsg->value =  ireader_pay(imsg->value);
					if(!send_fsm_msg(imsg))
					release_fsm_msg(imsg);
				break;
			}
		}
		else
		{
			ireader_link();//联机
		}
	}
}
