/*
状态机文件

*/
#include "header.h" 

uint8_t start;


void start_server(void)//开启服务
{

	//DisplayNUM(check_w_power_on,0,0,3,0);
	if(check_w_power_on >= 4)
	{		
		check_w_power_on=0xFF;		
		OSTimeDly(OS_TICKS_PER_SEC);
		if(credit_get() != 0)//吞币
		{
			send_moneydelete_msg(1,1);
			credit_set(0);
		}
		Load_status(fsm_id_init);
	}
}

//开机自检
uint8_t fsm_fun_selfCheck(FSMMSG* cmsg)
{
	switch (cmsg->type)
	{
		case MsgSYS:
			if(cmsg->stype == FSM_STATUS_INIT)
			{
				lcd_clear();
				
				if(g_vm.language > 1 )
				{
					g_vm.language = 0;
				}
				lcd_show_character(&pt_word.start[0]); 
				assistanttiminghm(20);
			}
		break;
			
		case MsgCOIN:
			if(cmsg->stype == M_CS_INIT)
			{
				if(coinequip.eqiu_status < 2)
					lcd_show_character(&pt_word.start[2]);
				else
					lcd_show_character(&pt_word.start[4]);
				
				check_w_power_on ++;
				start_server();
			}
		break;
			
		case MsgBILL:
			if(cmsg->stype == M_BS_INIT)
			{
				if(billequip.eqiu_status<2)
					lcd_show_character(&pt_word.start[1]);
				else
					lcd_show_character(&pt_word.start[3]);
				
				check_w_power_on ++;
				start_server();
				
			}
		break;
			
		case MsgDROP:
			if(cmsg->stype == MSG_DS_INIT)
			{
				check_w_power_on++;
				start_server();	
			}
		break;
			
		case MsgCARD:
			if(cmsg->stype == ICREADER_INIT)
			{
				if(cIC.state == 1)
					lcd_show_character(&pt_word.start[7]);
				else
					lcd_show_character(&pt_word.start[8]);
				
				check_w_power_on ++;
				start_server();
			}
		break;
			
		case MsgCHAN:
			if(cmsg->stype == M_DRV_INIT)
			{
				if(pt_driverBoard[0] != 0)
				{
					if(pt_driverBoard[0]->state == 1)
					{
						lcd_show_character(&pt_word.start[5]);
					}
					else
					{
						lcd_show_character(&pt_word.start[6]);
					}
				}
				check_w_power_on ++;
				start_server();
			}
		break;
			
		case MsgASSIS:
			if(assistantstate)
			{
				//DisplayLT(">>>>>>          ",6,start,0);
				start=(start+1)%16;
				assistanttiminghm(100);
		  	}
		break;
	}
	return 0;
}
	
