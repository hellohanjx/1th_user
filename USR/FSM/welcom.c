/*
״̬���ļ�
��ӭ���ٽ���
*/
#include "header.h"

/*
����״̬
*/
static void standby_status(void)
{
	if(pt_driverBoard[0]->state == 1)//�����������
	{
		if(cIC.state == 1)
		{
			if(g_vm.linkState == 1)
			{
				lcd_show_character(&pt_word.standby[0]);//��ӭˢ������
				lcd_show_character(&pt_word.standby[20]);//"��ѡ����Ʒ"
			}
			else
			{
				lcd_show_character(&pt_word.standby[1]);//��������޷�����
				lcd_show_character(&pt_word.standby[17]);//
			}
		}
		else
		{
			lcd_show_character(&pt_word.standby[2]);//ˢ�������޷�����
			lcd_show_character(&pt_word.standby[18]);//
		}
	}
	else
	{
		lcd_show_character(&pt_word.standby[3]);//���������޷�����
		lcd_show_character(&pt_word.standby[19]);//
	}
	
	
	/*
	��ʾһЩͼ��
	*/
	//����
	if(g_vm.linkState == 1)
	{
		lcd_show_character(&pt_word.warring[9]); //��̖�D��
	}
	else
	{
		lcd_clear_character(&pt_word.warring[9]);
	}
	//����
	if(g_vm.door == 1)
	{
		lcd_show_character(&pt_word.warring[10]); //�T�D��
	}
	else
	{
		lcd_clear_character(&pt_word.warring[10]); //���ͼ��
	}
	//������
	if(pt_driverBoard[0]->state == 1)
	{
		lcd_show_character(&pt_word.warring[11]); //�C���D��
	}
	else
	{
		lcd_clear_character(&pt_word.warring[11]); //���ͼ��
	}
	//����ͼ��
	if( pt_driverBoard[0]->exState[EX_DISINFECT] == 1)
	{
		lcd_show_character(&pt_word.warring[20]);
	}
	else//����ͼ��
	if(pt_driverBoard[0]->exState[EX_COLD] == 1 )
	{
		lcd_show_character(&pt_word.warring[16]);
	}
	else
	{
		lcd_clear_character(&pt_word.warring[16]); //���ͼ��
	}
	//����ͼ��
	if(pt_driverBoard[0]->exState[EX_LIGHT] == 1)
	{
		lcd_show_character(&pt_word.warring[17]);
	}
	else
	{
		lcd_clear_character(&pt_word.warring[17]); //���ͼ��
	}

//	//����ͼ��
//	if(pt_driverBoard[0]->exState[EX_DISINFECT] == 1)
//	{
//		lcd_show_character(&pt_word.warring[18]);
//	}
//	else
//	{
//		lcd_clear_character(&pt_word.warring[18]); //���ͼ��
//	}

	//�����ӟ�ͼ��
	if(pt_driverBoard[0]->exState[EX_BACKUP1] == 1)
	{
		lcd_show_character(&pt_word.warring[21]);
	}
	else
	{
		lcd_clear_character(&pt_word.warring[21]); //���ͼ��
	}

}
uint8_t  jj;
uint8_t fsm_fun_welcom(FSMMSG* cmsg)
{
	switch (cmsg->type)
	{
		case MsgSYS:
			if(cmsg->stype == FSM_STATUS_INIT)
			{
				if(jj == 0)
				{
				Dispen_Bill(1000);
					jj++;
				}
				if(billequip.eqiu_status < 1)
					CE_BILL(1);
				if(coinequip.eqiu_status < 1)
					CE_COIN(1);
				
				g_trade.channel.price = 0;
				g_vm.keyState = 0;
				lcd_clear();
				fsm_value_clear();
				select_clear();
				input_clear();
				stopassistantming();
				standby_status();
				assistanttiming(2);
				lcd_show_time(&pt_word.warring[7]); 

//				lcd_show_character(&pt_word.warring[21]);
				//������
//				lcd_show_num(tradenum_get(), 0, 350, WHITE, BLUE, font48, 0, 10); 
//				lcd_show_num(mailnum_get(), 200, 350, WHITE, BLUE, font48, 0, 10); 
			}
		break;
			
		case MsgKEY:
			if(cmsg->value == USER1_VAL)//�߼��˵�
			{
				if(g_vm.door == 1)//�ſ��ſ��Խ���˵�
				{
					g_vm.keyState = 1;
					Load_status(fsm_id_seniorMenu);
				}
			}
			else
			if(cmsg->value == USER2_VAL)//��Ʒ����
			{
				if(g_vm.door == 1)
				{
					g_vm.keyState = 1;
					g_fsm.shortcut = 1;
					Load_status(fsm_id_goodsSet);
				}
			}
			else
			if(cmsg->value == USER3_VAL)//��������
			{
				if(g_vm.door == 1)
				{
					g_vm.keyState = 1;
					g_fsm.shortcut = 1;
					Load_status(fsm_id_channelTest);
				}
			}
			else
			if(cmsg->value == USER4_VAL)//ͳ��
			{
				if(g_vm.door == 1)
				{
					g_vm.keyState = 1;
					g_fsm.shortcut = 1;
					Load_status(fsm_id_statistic);
				}
			}
			else
			if(cmsg->value == '*')
			{
				Load_status(fsm_id_vmSimpleInfo);
			}
			else
			if(cmsg->value == '#')
			{
				//�л���ˢ����ѯ
				Load_status(fsm_id_cardOnlineCheck);
			}
			else
			{
				if(cmsg->value > '0' && cmsg->value <= ('0' + g_vm.maxtrayNum))
				{
					select_goods(cmsg->value); 
					Load_status(fsm_id_choose);
				}
			}				
		break;
		
		case MsgCOIN:
			if(cmsg->stype == M_CS_ACCEPT){//????
//				tradenumadd();
//				savecoinaccept(cmsg->value);
				g_trade.businessmode=0;
				Load_status(fsm_id_cashTrade);
			}
		break;
			
		case MsgBILL:
			if(cmsg->stype == M_BS_AESC)//������
			{
				ESCROW(1);
				jj = 0;
			}
			else
			if((cmsg->stype == M_BS_ABOX) || (cmsg->stype == M_BS_RECY))//����
			{
				
				Load_status(fsm_id_cashTrade);
			}
			else
			if(cmsg->stype == M_BS_AOUT)
			{
				jj = 1;
			}
		break;
			
		case MsgCARD:

		break;
						
		case MsgASSIS:
			if(assistantstate)
			{
//				uint8_t i;
//				for( i =0; i < 100; i++)
//				{
//					
//					report_temp_value(0, 1, 31);
//					
//				}
				
				//������
//				lcd_show_num(tradenum_get(), 0, 350, WHITE, BLUE, font48, 0, 10); 
//				lcd_show_num(mailnum_get(), 200, 350, WHITE, BLUE, font48, 0, 10); 
//				lcd_show_num(flashqueue_head_get(), 400, 350, WHITE, BLUE, font48, 0, 10); 
//				lcd_show_num(flashqueue_rear_get(), 600, 350, WHITE, BLUE, font48, 0, 10); 
//				lcd_show_num(mailusedsec_get(), 200, 300, WHITE, BLUE, font48, 0, 10); 
				
				
				lcd_show_time(&pt_word.warring[7]);
				standby_status();
				assistanttiming(2);
		  	}
		break;
	}
	return 0;
}
