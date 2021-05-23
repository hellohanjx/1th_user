/*
调货检测

*/
#include "header.h"

//485掉货检测

FSMMSG fdropmsg;
OS_EVENT* dropqueue;
void *fdmsg[1];

void DropInit()
{
}

void setdropstate(uint8_t con,uint8_t num,uint8_t type)
{
	FSMMSG * dropmsg;
		dropmsg=apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
		assert_param(dropmsg);
		dropmsg->type=MsgDROP;
			dropmsg->stype=MSG_DS_INIT;
			dropmsg->stype=MSG_DS_STATUS;
		    {
				if(num==3)
					status_equip_msg(1,0xFF,3);//有隔断
				else
				if(num==1)
					status_equip_msg(1,0xFF,2);//正常
				else
				if(num==2)
					status_equip_msg(1,0xFF,0xFF);//无连接
				else
				if(num==4)
					status_equip_msg(1,0xFF,1);//故障
		   	}
		dropmsg->value=con;
		if(send_fsm_msg(dropmsg)!=TRUE)
			release_fsm_msg(dropmsg);
}

void dropinductor_CHECK(uint8_t num)
{
	
}

void dropinductor_work_start(uint8_t num)
{
	

}
void dropinductor_work_requ(uint8_t num)
{
	
}

void dropinductor_work_end(uint8_t num)
{
	
}

void TASK_DROP_RUN(void)
{
	
}

void API_DROP_CHECK(uint8_t num)
{
	FSMMSG* cmsg;
	cmsg=apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
	assert_param(cmsg);
	cmsg->type=num;
	cmsg->stype=DROP_STATE;
	if(OSQPost(dropqueue,cmsg)!=OS_NO_ERR)
		release_fsm_msg(cmsg);
}

void API_DROP_WORKSTART(uint8_t num)
{
	FSMMSG* cmsg;
	cmsg=apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
	assert_param(cmsg);
	cmsg->type=num;
	cmsg->stype=DROP_START;
	if(OSQPost(dropqueue,cmsg)!=OS_NO_ERR)
		release_fsm_msg(cmsg);
}

void API_DROP_WORK_REQU(uint8_t num)
{
	FSMMSG* cmsg;
	cmsg=apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
	assert_param(cmsg);
	cmsg->type=num;
	cmsg->stype=DROP_REQU;
	if(OSQPost(dropqueue,cmsg)!=OS_NO_ERR)
		release_fsm_msg(cmsg);
}

void API_DROP_WORK_END(uint8_t num)
{
	FSMMSG* cmsg;
	cmsg=apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
	assert_param(cmsg);
	cmsg->type=num;
	cmsg->stype=DROP_END;
	if(OSQPost(dropqueue,cmsg)!=OS_NO_ERR)
		release_fsm_msg(cmsg);
}
