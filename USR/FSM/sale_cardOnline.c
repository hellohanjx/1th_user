/*
���߿����׳���
*/
#include "header.h"

//======================
//�˺��������ȴ�����֧�����
//���������˳�
//======================
volatile uint32_t t_t;
uint8_t fsm_fun_cardOnlinePay(FSMMSG * cmsg)
{
	switch (cmsg->type)
	{
		case MsgSYS :
			if(cmsg->stype == FSM_STATUS_INIT)
			{
				g_fsm.num = 12;
				g_fsm.mode = 0;
				lcd_show_character(&pt_word.choose[10]);//"���"
				lcd_show_character(&pt_word.choose[11]);//"����"
				//��ʾ����
				lcd_show_string(cIC.PrintSnr, pt_word.choose[11].valx, pt_word.choose[11].valy, pt_word.choose[11].color, pt_word.choose[11].background, *(pt_word.choose[11].font));
				assistanttiming(1);
			}
		break;
		
		case MsgOnline://���߿����
			if(cmsg->stype == 0x31)//������ص�������֧��
			{
				if(cmsg->value == 0x30)//֧���ɹ�
				{
					g_fsm.mode = 1;
					g_fsm.num = 30;
					lcd_show_character(&pt_word.choose[13]);//"���"
					lcd_show_money(cIC.balance, pt_word.choose[13].valx, pt_word.choose[13].valy, pt_word.choose[13].color, 
									pt_word.choose[13].background, *(pt_word.choose[13].font));//��ʾ���
					lcd_show_character(&pt_word.salling[1]);//"���ڳ���"
					g_trade.saleCostTime = OSTimeGet();//��¼����ʱ��
					
					apply_driverboard_msg(M_CHANNEL_SALE, g_trade.curDiverBoard.container, 
						g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, NULL, NULL);//֪ͨ����
					
					loop_channel_set(g_trade.curDiverBoard.tray , g_trade.curDiverBoard.col + 1);//�����´γ���������
					
//					g_trade.channel.store = ( g_trade.channel.store > 0 ? (g_trade.channel.store--) : (0) );//��ȥ���
					if(g_trade.channel.store > 0)
						g_trade.channel.store -= 1;
					if(g_vm.channelBit == 1){//���̳���
						if(bkp_trayStore_get(g_trade.curDiverBoard.tray) > 0)
						bkp_trayStore_set(g_trade.curDiverBoard.tray, g_trade.channel.store);//�������̿������
					}
					else{//��������
						e2p_set_store(g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + 
										g_trade.curDiverBoard.col, (uint16_t*)(&g_trade.channel.store));//���»������
					}
				}
				else//֧��ʧ��
				{
					switch(cmsg->value)
					{
						case 0x31:lcd_show_character(&pt_word.choose[14]);break;//"�������ڲ�������"
						case 0x32:lcd_show_character(&pt_word.choose[15]);break;//" ��Ч���������� "
						case 0x33:lcd_show_character(&pt_word.choose[16]);break;//"�Ǳ�У����������"
						case 0x34:lcd_show_character(&pt_word.choose[17]);break;//"�������޲�������"
						case 0x35:lcd_show_character(&pt_word.choose[18]);break;//"�Ǳ�������������"
						case 0x36:lcd_show_character(&pt_word.choose[19]);break;//"���㲻������"
						case 0x37:lcd_show_character(&pt_word.choose[20]);	//" ��  ����Ʒͣ�� "
							lcd_show_num(select_value_get(),pt_word.choose[20].valx, pt_word.choose[20].valy, pt_word.choose[20].color, pt_word.choose[20].background, *(pt_word.choose[20].font) , 1, 1);
						break;
						//case 0x38:Display("  ��������Ա��  ",30, 400);break;
						case 0x50:lcd_show_character(&pt_word.choose[21]);break;//��ʱδ�յ��ظ�
						default:lcd_show_character(&pt_word.choose[22]);break;//"�����쳣��������"
					}
					//�㱨��̨
					send_online_cardRequestResult(ONLINE_SALE_FAIL);
					OSTimeDly(2*OS_TICKS_PER_SEC);
					Load_status(fsm_id_init);
				}
			}
			else//�����Ϣ��������֧�����
			{
				lcd_clear();
				lcd_show_character(&pt_word.choose[23]);//"֪ͨ����Ա"
//				//�㱨����ʧ��
				send_online_cardRequestResult(ONLINE_SALE_FAIL);
//				report_trade_result(0, SALE_TYPE_CARD, cmsg->value);
				OSTimeDly(2*OS_TICKS_PER_SEC);
				Load_status(fsm_id_init);
			}
		break;
			
		case MsgCHAN:
			if(cmsg->stype == M_CHANNEL_SALE)
			{
				t_t = OSTimeGet() - g_trade.saleCostTime;
				if(cmsg->value == 1 || cmsg->value == 10)
				{ 
					if(t_t < 2100)//������ʱС��1.8s����Ϊ����������
					{
						lcd_show_character(&pt_word.salling[3]);//����ʧ��
						//�㱨��̨
						send_online_cardRequestResult(ONLINE_SALE_FAIL);
						report_trade_result(0, SALE_TYPE_CARD, cmsg->value);
					}
					else
					{					
						if(cIC.balance < g_trade.channel.price)
							cIC.balance = 0;
						else
							cIC.balance -= g_trade.channel.price;
						lcd_show_money(cIC.balance, pt_word.choose[13].valx, pt_word.choose[13].valy, pt_word.choose[13].color, pt_word.choose[13].background, *(pt_word.choose[13].font));
						lcd_show_character(&pt_word.salling[2]);//"�����ɹ�"
						//�㱨��̨
						send_online_cardRequestResult(ONLINE_SALE_OK);
						report_trade_result(1, SALE_TYPE_CARD, cmsg->value);
						
						//ͳ�Ƽ�¼
						e2p_add_channelStatistic(g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, 1, g_trade.channel.price);
						bkp_trayStatistic_add(g_trade.curDiverBoard.tray, 1, g_trade.channel.price);
						bkp_wholeStatistic_add(1, g_trade.channel.price);
					}
				}
				else
				{
					lcd_show_character(&pt_word.salling[3]);//����ʧ��
					//�㱨��̨
					send_online_cardRequestResult(ONLINE_SALE_FAIL);
					report_trade_result(0, SALE_TYPE_CARD, cmsg->value);
				}
				
				lcd_show_num(cmsg->value,14*8, 6, pt_word.choose[20].color, pt_word.choose[20].background, *(pt_word.choose[20].font) , 1, 2);
				OSTimeDly(OS_TICKS_PER_SEC*2);
				Load_status(fsm_id_init);
			}
		break;
	
		case MsgASSIS:
			if(assistantstate)
			{
				if(g_fsm.num)
				{
					g_fsm.num--;
					//��ʾ����ʱ
					lcd_show_num(g_fsm.num, pt_word.salling[3].valx, pt_word.salling[3].valy, pt_word.salling[3].color, pt_word.salling[3].background, *(pt_word.salling[3].font), 1, 2);
					assistanttiming(1);
				}
				else
				{
					send_online_cardRequestResult(ONLINE_SALE_FAIL);//ˢ��ʧ��
					if(g_fsm.mode == 1)//�յ�֧���ɹ����أ�ʧ��ԭ���������ն�����
					{
						lcd_show_character(&pt_word.salling[3]);
						report_trade_result(0, SALE_TYPE_CARD, cmsg->value);//����ʧ��
					}
					else//û���յ�֧���ɹ����أ�ʧ��ԭ��������ƽ̨����
					{
						lcd_show_character(&pt_word.choose[22]);
					}
					
					OSTimeDly(2*OS_TICKS_PER_SEC);
					Load_status(fsm_id_init);
				}
			}
		break;
	
	}
	return 0;
}
