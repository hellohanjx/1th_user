/*
状态机文件

*/
#include "header.h"

volatile uint8_t  assistantstate=0;
volatile uint16_t atiming=0;


void assistanttiming(uint16_t num)//秘书计时（基准1s）
{
	assistantstate = 1 ;
	atiming = num*100+2;
}

void assistanttiminghm(uint16_t num)//秘书计时（基准10ms）
{
	assistantstate = 1;
	atiming = num+2;
}
void stopassistantming(void)//停止秘書計時 注意該函數有缺陷，在極地的概率下會導致執行了該函數后仍然會發送MsgASSIS消息
{
	assistantstate = 0; //所以需要一個由状态机本身来改变的变量来避免这种事情发生。当该值为0时，不处理该类消息。
	atiming = 1;
}


 

void assistant(void)//助手
{
	FSMMSG* assistmsg;	
	atiming=0;
	while(1)
	{
		//LookForMaxMinPrice();
		if(atiming > 0)
		atiming--;
		if(atiming == 2)
		{
			atiming = 0;
			assistmsg = apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
			log_output(assistmsg);
			if(assistmsg != 0)
			{
				assistmsg->type=MsgASSIS;
				if(send_fsm_msg(assistmsg)!=TRUE)
					release_fsm_msg(assistmsg);
			}
		}
		
		OSTimeDly(10);//等待10ms
	}
}
