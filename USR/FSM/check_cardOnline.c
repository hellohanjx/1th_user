/*
���߿���ѯ����
*/

#include "header.h"

uint8_t fsm_fun_cardOnlineCheck(FSMMSG *cmsg) //��ѯ��
{
	switch (cmsg->type)
	{
		case MsgSYS:
			if(cmsg->stype == FSM_STATUS_INIT)
			{
				lcd_clear(); 
				lcd_show_character(&pt_word.standby[4]);//����
				lcd_show_character(&pt_word.standby[5]);//���
				lcd_show_character(&pt_word.standby[6]);//�޹�
				g_fsm.num = 10;
				g_fsm.mode = 0;
				if(g_vm.linkState == 1)
				{
					if(cIC.state == 1)
					{
						lcd_show_character(&pt_word.standby[7]);//��ˢ����ѯ
						send_icreader_msg(ICREADER_FIND, NULL); //Ѱ��
						//��ʾ����ʱ
						lcd_show_num(g_fsm.num, pt_word.standby[7].valx, pt_word.standby[7].valy, pt_word.standby[7].color, pt_word.standby[7].background, *(pt_word.standby[7].font), 1, 2);
						assistanttiming(1);
					}
					else
					{
						lcd_show_character(&pt_word.choose[24]);//��ˢ����ѯ
						OSTimeDly(OS_TICKS_PER_SEC*2);
						Load_status(fsm_id_init);
					}
				}
				else
				{
					lcd_show_character(&pt_word.standby[8]);//����δ����
					OSTimeDly(OS_TICKS_PER_SEC*2);
					Load_status(fsm_id_init);
				}
			} 
			else
			if(cmsg->stype == FSM_STATUS_TIMEOUT)
			{
				Load_status(fsm_id_init);
			}
			else
			if(cmsg->stype == FSM_STATUS_EXIT)
			{
				
				
			}
		break;
		
		case MsgKEY:
			if(cmsg->value == '*')
			{
				if(g_fsm.mode == 0)
				{
					Load_status(fsm_id_init);
				}
			}
		break;
			
		case MsgCARD:
			if(cmsg->value == TRUE)
			{
					BEEP(ON);
					OSTimeDly(OS_TICKS_PER_SEC/10);
					BEEP(OFF);
				
				g_fsm.mode = 1;
				send_online_cardRequest(ONLINE_CHECK);//����ѯ
				//��ʾ����
				lcd_show_string(cIC.PrintSnr, pt_word.standby[4].valx, pt_word.standby[4].valy, pt_word.standby[4].color, pt_word.standby[4].background, *(pt_word.standby[4].font));
			}
			else
			{
				send_icreader_msg(ICREADER_FIND, NULL); //Ѱ��
			}
		break;
		
		case MsgOnline:
			if(cmsg->stype == 0x32)//��ѯ����
			{
				if(cmsg->value == 0x30)//��ѯ�ɹ�
				{
					//��ʾ���
					lcd_show_money(cIC.balance, pt_word.standby[5].valx, pt_word.standby[5].valy, pt_word.standby[5].color, pt_word.standby[5].background, *(pt_word.standby[5].font));
					if(cIC.daymaxpay == '0')
					{
						lcd_show_character(&pt_word.standby[9]);//"����"
					}
					else 
					{
						//��ʾ�޹�����
						lcd_show_num(cIC.daymaxpay - '0', pt_word.standby[6].valx, pt_word.standby[6].valy, pt_word.standby[6].color, pt_word.standby[6].background, *(pt_word.standby[6].font), 0 ,10);
						lcd_show_character(&pt_word.standby[10]);//'��'
					}
					OSTimeDly(OS_TICKS_PER_SEC*3);
					Load_status(fsm_id_init);
				}
				else
				{
					g_fsm.mode = 0;//����"*"���˳�
					switch(cmsg->value)
					{
						case 0x31:
							lcd_show_character(&pt_word.standby[11]);
							break;
						case 0x32:
							lcd_show_character(&pt_word.standby[12]);
							break;
						case 0x33:
							lcd_show_character(&pt_word.standby[13]);
							break;
						case 0x35:
							lcd_show_character(&pt_word.standby[14]);
							break;
						case 0x36:
							lcd_show_character(&pt_word.standby[15]);
							break;
						case 0x37:
							//��ͨ����Ա��
							cIC.user = 1;
							g_vm.keyState = 1;//�����๦�ܼ�
							g_fsm.shortcut = 1;//��ݷ�ʽ����
							Load_status(fsm_id_lowMenu);
							break;
						case 0x38:
							cIC.user = 2;
							g_vm.keyState = 1;//�����๦�ܼ�
							//��������Ա��
							Load_status(fsm_id_seniorMenu);
							break;
						default:
							lcd_show_character(&pt_word.standby[16]);
							break;
					}
				}
			}
		break;
			
		case MsgASSIS:
			if(assistantstate)
			{
				if(g_fsm.num > 0)
				{
					g_fsm.num--;
					//��ʾ����ʱ
					lcd_show_num(g_fsm.num, pt_word.standby[7].valx, pt_word.standby[7].valy, pt_word.standby[7].color, pt_word.standby[7].background, *(pt_word.standby[7].font), 1, 2);
					assistanttiming(1);
				}
				else
				{
					Load_status(fsm_id_init);
				}
			}
		break;
	}
	return 0;
}




