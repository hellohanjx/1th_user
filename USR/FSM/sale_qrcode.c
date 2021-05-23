/*
��ά�����
*/
#include "header.h"

uint8_t fsm_fun_qrcodePay(FSMMSG * cmsg)
{
	switch (cmsg->type)
	{
		case MsgSYS :
			if(cmsg->stype == FSM_STATUS_INIT)
			{
				g_fsm.num = 30;
				assistanttiming(1);
				lcd_show_character(&pt_word.salling[1]);//�����ڳ���...��
				
				//��¼����ʱ��
				g_trade.saleCostTime = OSTimeGet();
				apply_driverboard_msg(M_CHANNEL_SALE, g_trade.curDiverBoard.container, g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, NULL, NULL);
				
				loop_channel_set(g_trade.curDiverBoard.tray , g_trade.curDiverBoard.col + 1);//�����´ο�ʼ������
				
				//�����
				g_trade.channel.store = g_trade.channel.store > 0 ? (--g_trade.channel.store) : (0);//��ȥ���
				if(g_vm.channelBit == 1){
					if(bkp_trayStore_get(g_trade.curDiverBoard.tray) > 0)
					bkp_trayStore_set(g_trade.curDiverBoard.tray, g_trade.channel.store);
				}else{
					e2p_set_store(g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, (uint16_t*)(&g_trade.channel.store));
				}
				tradenum_add();//����ʱ���ӽ��׺�
			}
		break;
			
		case MsgCHAN:
			if(cmsg->stype == M_CHANNEL_SALE)
			{
				if(cmsg->value == 1 || cmsg->value == 10)
				{ 
					if(OSTimeGet() - g_trade.saleCostTime < 2100)//������ʱС��1.8s����Ϊ����������
					{
						lcd_show_character(&pt_word.salling[3]);//����ʧ��
						//�㱨��̨
						send_qrcode_dat(QRCODE_RESULT, 0);
						report_trade_result(0, g_trade.qrcode.type - '0', cmsg->value);
					}
					else
					{
					
						lcd_show_character(&pt_word.salling[2]);//"�����ɹ�"
						//�㱨��̨
						send_qrcode_dat(QRCODE_RESULT, 1);
						report_trade_result(1, g_trade.qrcode.type - '0', cmsg->value);
						
						//ͳ�Ƽ�¼
						e2p_add_channelStatistic(g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, 1, g_trade.channel.price);
						bkp_trayStatistic_add(g_trade.curDiverBoard.tray, 1, g_trade.channel.price);
						bkp_wholeStatistic_add(1, g_trade.channel.price);
					}
				}
				else
				{
					lcd_show_character(&pt_word.salling[3]);//"����ʧ��"
					//�㱨��̨
					send_qrcode_dat(QRCODE_RESULT, 0);
					report_trade_result(0, g_trade.qrcode.type - '0', cmsg->value);
				}
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
					lcd_show_character(&pt_word.salling[3]);//"����ʧ��"
					//�㱨��̨
					send_qrcode_dat(QRCODE_RESULT, 0);
					report_trade_result(0, g_trade.qrcode.type - '0', cmsg->value);
					
					OSTimeDly(2*OS_TICKS_PER_SEC);
					Load_status(fsm_id_init);
				}
			}
		break;
	
	}
	return 0;
}
