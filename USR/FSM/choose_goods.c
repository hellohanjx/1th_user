/*
选货界面
*/


#include "header.h"


/*
功能：查询货道状态
返回值：1：货道正常；0：无正常货道；2：货道禁能；3：货道无库存销售；4：价格错误；5：库存错误
*/
uint8_t get_channel_state(void)
{
	uint8_t i = 0, rs = FALSE;
	
	if(g_vm.channelBit == 1)//1位货道
	{
		uint8_t state;
		SETUP_CHANNEL channel;
		
		//价格判断
		if(!e2p_get_price(g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 , &(channel.price)))
		{
			return 4;
		}
		
		if(g_vm.channelBit == 1)//1位货道号，循环出货
		{
			g_trade.channel.store = bkp_trayStore_get(g_trade.curDiverBoard.tray);
		}
		else
		{
			//库存判断
			if(!e2p_get_store(g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 , &(channel.store)))
			{
				return 5;
			}
			g_trade.channel.store = channel.store;
		}
		
		g_trade.channel.price = channel.price;
		
		
		rs = FALSE;
		for(i = 0; i < 10; i++, g_trade.curDiverBoard.col++)//查询10个货道
		{
			g_trade.curDiverBoard.col %= 10;
			e2p_get_channelEnable(g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, &state);
			if(state == 1)//货道使能
			{
				drv_cmd_channleCheck(g_trade.curDiverBoard.container, g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col);
				if(pt_driverBoard[g_trade.curDiverBoard.container]->channelState[g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col] == 1)
				{
					if(g_trade.channel.store > 0)
					{
						rs = TRUE;
					}
					else
					{
						rs = 3;
					}
					break;
				}
				else
				{
					rs = 0;
					if(g_trade.channel.store > 0)
					{
						g_trade.channel.store--;
					}
					else
					{
						rs = 3;
						break;
					}
				}
			}
			else//货道非使能
			{
				rs = 2;
				if(g_trade.channel.store > 0)
				{
//					g_trade.channel.store--;
				}
				else
				{
					rs = 3;
					break;
				}
			}
		}
	}
	else//2位以上货道
	{
		SETUP_CHANNEL channel;
		//价格判断
		if(!e2p_get_price(g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col  , &(channel.price)))
		{
			return 4;
		}
		//库存判断
		if(!e2p_get_store(g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, &(channel.store)))
		{
			return 5;
		}
		//货道使能
		e2p_get_channelEnable(g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, &(channel.type));
		
		g_trade.channel.price = channel.price;
		g_trade.channel.store = channel.store;
		if(channel.type == 1)//货道使能
		{
			if(channel.store != 0)
			{
				drv_cmd_channleCheck(g_trade.curDiverBoard.container, g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col);
				if(pt_driverBoard[g_trade.curDiverBoard.container]->channelState[g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col] == 1)
				{
					rs = TRUE;
				}
				else
				{
					rs = FALSE;
				}
			}
			else
			{
				rs = 3;
			}
		}
		else
		{
			rs = 2;
		}
	}
	return rs;
}

/*
显示货道信息
*/
static void show_channel_state(void)
{
	uint8_t rs;
	if(!(g_trade.curDiverBoard.container > g_vm.maxContainerNum - 1))//有这个驱动板
	{
		if(pt_driverBoard[g_trade.curDiverBoard.container]->state == 1)//驱动板正常
		{
			rs = get_channel_state();//获取货道状态
			
			if(rs == 1)//找到正常货道
			{
				if(g_vm.linkState == 1)//连网
				{
					send_icreader_msg(ICREADER_FIND, NULL); 
					if(g_vm.applyEnable == 1)//开启二维码支付
					{
						creat_ercode(G_ERCODE_X, G_ERCODE_Y, G_ERCODE_SCALE);//显示二维码
						if(cIC.state == 1)
						{
							lcd_show_character(&pt_word.choose[9]);//"请刷卡或扫码"
						}
						else
						{
							lcd_show_character(&pt_word.choose[25]);//"请扫码支付"
						}
						g_fsm.num = 100;
						assistanttiminghm(10);
						g_fsm.count = g_fsm.num;//记录当前读秒，提供给二维码使用
					}
					else//未开启二维码支付
					{
						if(cIC.state == 1)
						{
							lcd_show_character(&pt_word.choose[8]);//"请刷卡购物"
						}
						else
						{
							lcd_show_character(&pt_word.choose[24]);//"暂停服务"
						}
						g_fsm.num = 10;
						assistanttiminghm(10);
					}
				}
				else//未连网
				{
					lcd_show_character(&pt_word.choose[7]);
					g_fsm.num = 10;
					assistanttiminghm(10);
				}
			}
			else
			if(rs == 2)//货道/托盘禁能
			{
				lcd_show_character(&pt_word.choose[4]);  
			}
			else
			if(rs == 3)//商品售空
			{
				lcd_show_character(&pt_word.choose[5]);  
			}
			else
			if(rs == 0)//货道故障
			{
				lcd_show_character(&pt_word.choose[6]);
			}
			else
			if(rs == 4)//价格错误
			{
				lcd_show_character(&pt_word.choose[26]);
				//这里可以发一条信息到平台用来分析
			}
			else
			if(rs == 5)//库存错误
			{
				lcd_show_character(&pt_word.choose[27]);
				//这里可以发一条信息到平台用来分析
			}
		}
		else//驱动板异常
		{
			lcd_show_character(&pt_word.choose[12]);
		}
		//显示金额与库存
		lcd_show_money(g_trade.channel.price, pt_word.choose[1].valx, pt_word.choose[1].valy, pt_word.choose[1].color, pt_word.choose[1].background, *(pt_word.choose[1].font));
		lcd_show_num(g_trade.channel.store, pt_word.choose[2].valx, pt_word.choose[2].valy, pt_word.choose[2].color, pt_word.choose[2].background, *(pt_word.choose[2].font), 0, 10);
	}
	else
	{
		lcd_show_character(&pt_word.choose[3]);//"无此货道"
	}
}


/*
选货状态机
*/
uint8_t fsm_fun_chooseGoods(FSMMSG* cmsg)
{
	switch (cmsg->type)
	{
		case MsgSYS:
			if(cmsg->stype == FSM_STATUS_INIT)
			{
				lcd_clear();
				
				//商品
				lcd_show_character(&pt_word.choose[0]);
				lcd_show_string((const char*)g_trade.select.select, pt_word.choose[0].valx, pt_word.choose[0].valy, pt_word.choose[0].color, pt_word.choose[0].background, *(pt_word.choose[0].font)); 
				//价格
				lcd_show_character(&pt_word.choose[1]);
				lcd_show_money(g_trade.channel.price, pt_word.choose[1].valx, pt_word.choose[1].valy, pt_word.choose[1].color, pt_word.choose[1].background, *(pt_word.choose[1].font));
				//库存
				lcd_show_character(&pt_word.choose[2]);
				lcd_show_num(g_trade.channel.store, pt_word.choose[2].valx, pt_word.choose[2].valy, pt_word.choose[2].color, pt_word.choose[2].background, *(pt_word.choose[2].font), 0, 10); 
				
				if(select_state_get())//已完成选货
				{
					if(g_vm.applyEnable == 1)//二维码支付开启
					{
						g_fsm.status++;
					}
					show_channel_state();
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
				if(g_vm.applyEnable == 1)//如果开启了二维码则需要清除
				delete_ercode(G_ERCODE_X, G_ERCODE_Y, G_ERCODE_SCALE);
			}
		break;
			
		case MsgKEY:
			if(cmsg->value == '*')
			{
				if(g_vm.applyEnable == 1)
				{
					send_qrcode_dat(QRCODE_RESULT, 0);//二维码取消
				}
				Load_status(fsm_id_init);
			}
			else
			if(cmsg->value >= '0' && cmsg->value <= '9')
			{
				/*
				有二维码生成，不允许重新选货
				*/
				if(g_fsm.status == 0)
				{
					select_goods(cmsg->value);
					lcd_show_string((const char*)g_trade.select.select, pt_word.choose[0].valx, pt_word.choose[0].valy, pt_word.choose[0].color, pt_word.choose[0].background, *(pt_word.choose[0].font)); 
					if(select_state_get())
					{
						g_fsm.num = 10;
						show_channel_state();
					}
				}
			}
		break;
		
		case MsgCOIN:

		break;
		
		case MsgBILL:

		break;

		case MsgCARD:
			if(cmsg->value == TRUE)
			{
				if(select_state_get())
				{
					BEEP(ON);
					OSTimeDly(OS_TICKS_PER_SEC/10);
					BEEP(OFF);
					
					tradenum_add();
					send_online_cardRequest(ONLINE_PAY);//在线支付申请
					Load_status(fsm_id_cardOlinePay);
				}
				else
				{
					send_icreader_msg(ICREADER_FIND, NULL); 
				}
			}
			else
			{
				send_icreader_msg(ICREADER_FIND, NULL); 
			}
		break;
			
		case MsgQrcode:
			if(cmsg->value == 0x31)
			{
				Load_status(fsm_id_qrcode);
			}
			else
			if(cmsg->value == 0x32)
			{
				Load_status(fsm_id_init);
			}
			else
			{
				g_fsm.mode = 1;//允许继续发送二维码查询
			}
		break;
		
		case MsgASSIS:
			if(assistantstate)
			{
				if(g_fsm.num)
				{
					/*
					5秒后发起二维码支付查询
					*/
					if(g_fsm.count - g_fsm.num >= 5 && g_fsm.mode == 0 && g_vm.applyEnable == 1)//0表示初始状态
					{
						g_fsm.count = g_fsm.num;
						g_fsm.mode = 1;
						send_qrcode_dat(QRCODE_CHECK, 0);
					}
					else
					if(g_fsm.count - g_fsm.num >= 2 && g_fsm.mode == 1 && g_vm.applyEnable == 1)//1表示允许继续发送二维码查询
					{
						g_fsm.count = g_fsm.num;
						g_fsm.mode = 2;//停止发送二维码查询
						send_qrcode_dat(QRCODE_CHECK, 0);
					}
					g_fsm.num--;
					lcd_show_num(g_fsm.num, pt_word.choose[8].valx, pt_word.choose[8].valy, pt_word.choose[8].color, pt_word.choose[8].background, *(pt_word.choose[8].font), 1, 2);
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
	
