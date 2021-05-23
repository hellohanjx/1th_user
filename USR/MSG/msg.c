/*
状态机文件

*/
#include "header.h"  

extern OS_EVENT *fsmQueen;


//extern OS_EVENT* sem_memBlock_lock;//内存分配锁定信号量

/*
申请一个标准消息
类型为 FSMMSG
*/
FSMMSG* apply_fsm_msg(uint16_t size)
{
	FSMMSG* msg = 0;
	uint8_t err;
//		OSSemPend(sem_memBlock_lock, 0, &rs);

	msg = (FSMMSG*)_malloc(sizeof(FSMMSG), &err);
	
	log_output(msg);

	if(msg != 0 )
	{
		msg->dat = 0;//防止野指针
		if(size)
		{
			msg->exLen = size;
			msg->dat = _malloc(size, &err);
			log_output(msg->dat );
		}
//		OSSemPost(sem_memBlock_lock);
		return msg;
	}
//			OSSemPost(sem_memBlock_lock);

	return 0;
}


/*
释放一个消息
*/
void release_fsm_msg(FSMMSG * msg)
{
	_free(msg->dat);
	_free(msg);
}

/*
发送一条信息到队列后
*/
uint8_t send_fsm_msg(FSMMSG *msg)
{
	uint8_t tmp = 0xDD,i=0; 
	do{
		tmp = OSQPost(fsmQueen,msg);
	}while(tmp != OS_NO_ERR && i++<10);
	return (tmp+1);
}


/*
发送一条信息到队列前
*/
uint8_t SendFSYSMSG(FSMMSG *msg)
{
	return OSQPostFront(fsmQueen,msg);
}


/*
等待消息
*/
FSMMSG * WaitSYSMSG(uint16_t timeout, uint8_t *err)
{
	return (FSMMSG *) OSQPend(fsmQueen,timeout,err);
}


