/*
在线卡交易出货
*/
#include "header.h"

//======================
//此函数用来等待网络支付结果
//不允许按键退出
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
				lcd_show_character(&pt_word.choose[10]);//"余额"
				lcd_show_character(&pt_word.choose[11]);//"卡号"
				//显示卡号
				lcd_show_string(cIC.PrintSnr, pt_word.choose[11].valx, pt_word.choose[11].valy, pt_word.choose[11].color, pt_word.choose[11].background, *(pt_word.choose[11].font));
				assistanttiming(1);
			}
		break;
		
		case MsgOnline://在线卡结果
			if(cmsg->stype == 0x31)//如果返回的是申请支付
			{
				if(cmsg->value == 0x30)//支付成功
				{
					g_fsm.mode = 1;
					g_fsm.num = 30;
					lcd_show_character(&pt_word.choose[13]);//"余额"
					lcd_show_money(cIC.balance, pt_word.choose[13].valx, pt_word.choose[13].valy, pt_word.choose[13].color, 
									pt_word.choose[13].background, *(pt_word.choose[13].font));//显示余额
					lcd_show_character(&pt_word.salling[1]);//"正在出货"
					g_trade.saleCostTime = OSTimeGet();//记录出货时间
					
					apply_driverboard_msg(M_CHANNEL_SALE, g_trade.curDiverBoard.container, 
						g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, NULL, NULL);//通知出货
					
					loop_channel_set(g_trade.curDiverBoard.tray , g_trade.curDiverBoard.col + 1);//更新下次出货货道号
					
//					g_trade.channel.store = ( g_trade.channel.store > 0 ? (g_trade.channel.store--) : (0) );//减去库存
					if(g_trade.channel.store > 0)
						g_trade.channel.store -= 1;
					if(g_vm.channelBit == 1){//托盘出货
						if(bkp_trayStore_get(g_trade.curDiverBoard.tray) > 0)
						bkp_trayStore_set(g_trade.curDiverBoard.tray, g_trade.channel.store);//更新托盘可卖库存
					}
					else{//货道出货
						e2p_set_store(g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + 
										g_trade.curDiverBoard.col, (uint16_t*)(&g_trade.channel.store));//更新货道库存
					}
				}
				else//支付失败
				{
					switch(cmsg->value)
					{
						case 0x31:lcd_show_character(&pt_word.choose[14]);break;//"卡不存在不能消费"
						case 0x32:lcd_show_character(&pt_word.choose[15]);break;//" 无效卡不能消费 "
						case 0x33:lcd_show_character(&pt_word.choose[16]);break;//"非本校卡不能消费"
						case 0x34:lcd_show_character(&pt_word.choose[17]);break;//"次数超限不能消费"
						case 0x35:lcd_show_character(&pt_word.choose[18]);break;//"非本机卡不能消费"
						case 0x36:lcd_show_character(&pt_word.choose[19]);break;//"余额不足不能消费"
						case 0x37:lcd_show_character(&pt_word.choose[20]);	//" 第  层商品停售 "
							lcd_show_num(select_value_get(),pt_word.choose[20].valx, pt_word.choose[20].valy, pt_word.choose[20].color, pt_word.choose[20].background, *(pt_word.choose[20].font) , 1, 1);
						break;
						//case 0x38:Display("  超级管理员卡  ",30, 400);break;
						case 0x50:lcd_show_character(&pt_word.choose[21]);break;//超时未收到回复
						default:lcd_show_character(&pt_word.choose[22]);break;//"处理异常不能消费"
					}
					//汇报后台
					send_online_cardRequestResult(ONLINE_SALE_FAIL);
					OSTimeDly(2*OS_TICKS_PER_SEC);
					Load_status(fsm_id_init);
				}
			}
			else//如果消息不是申请支付类的
			{
				lcd_clear();
				lcd_show_character(&pt_word.choose[23]);//"通知管理员"
//				//汇报出货失败
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
					if(t_t < 2100)//出货耗时小于1.8s，认为出货不正常
					{
						lcd_show_character(&pt_word.salling[3]);//出货失败
						//汇报后台
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
						lcd_show_character(&pt_word.salling[2]);//"出货成功"
						//汇报后台
						send_online_cardRequestResult(ONLINE_SALE_OK);
						report_trade_result(1, SALE_TYPE_CARD, cmsg->value);
						
						//统计记录
						e2p_add_channelStatistic(g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, 1, g_trade.channel.price);
						bkp_trayStatistic_add(g_trade.curDiverBoard.tray, 1, g_trade.channel.price);
						bkp_wholeStatistic_add(1, g_trade.channel.price);
					}
				}
				else
				{
					lcd_show_character(&pt_word.salling[3]);//出货失败
					//汇报后台
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
					//显示倒计时
					lcd_show_num(g_fsm.num, pt_word.salling[3].valx, pt_word.salling[3].valy, pt_word.salling[3].color, pt_word.salling[3].background, *(pt_word.salling[3].font), 1, 2);
					assistanttiming(1);
				}
				else
				{
					send_online_cardRequestResult(ONLINE_SALE_FAIL);//刷卡失败
					if(g_fsm.mode == 1)//收到支付成功返回，失败原因是由于终端问题
					{
						lcd_show_character(&pt_word.salling[3]);
						report_trade_result(0, SALE_TYPE_CARD, cmsg->value);//出货失败
					}
					else//没有收到支付成功返回，失败原因是网络平台问题
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
