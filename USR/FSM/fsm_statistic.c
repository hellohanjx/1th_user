/*
统计查询界面
*/
#include "header.h"

uint8_t fsm_fun_statistic(FSMMSG* cmsg)
{
	uint32_t num, val;
	switch(cmsg->type)
	{
		case MsgSYS:
			if(cmsg->stype == FSM_STATUS_INIT)
			{
				g_fsm.mode = 0;
				input_clear();
				select_clear();
				lcd_clear();
				if(g_vm.channelBit > 2)
				{
					input_len_set(3);
				}
				else{
					input_len_set(2);
				}
				lcd_show_character(&pt_word.set[62]);//"货道"
				lcd_show_character(&pt_word.set[63]);//"销售数量"
				lcd_show_character(&pt_word.set[64]);//"销售金额"
			}
			else
			if(cmsg->stype == FSM_STATUS_TIMEOUT)
			{
				Load_status(fsm_id_init);
			}
		break;
		
		case MsgKEY:
			if(g_fsm.mode == 0)
			{
				if(cmsg->value >= '0' && cmsg->value <= '9')
				{
					if(g_vm.channelBit > 2)//3位及以上货道位数
					{
						switch(g_fsm.count)
						{
							case 0://机柜位
								if(cmsg->value > '0' && cmsg->value <= g_vm.maxContainerNum + '0')
								{
									g_trade.curDiverBoard.container = cmsg->value - '1';
									input_num(cmsg->value);
									g_fsm.count = (g_fsm.count + 1)%3;
								}
							break;
							
							case 1://托盘位
								if(cmsg->value > '0' && cmsg->value < '9')//最大托盘号为8
								{
									g_trade.curDiverBoard.tray = cmsg->value - '1';
									input_num(cmsg->value);
									g_fsm.count = (g_fsm.count + 1)%3;
								}
							break;
								
							case 2://货道位
								if(cmsg->value >= '0' && cmsg->value <= '9')
								{
									g_trade.curDiverBoard.col = cmsg->value - '0';
									input_num(cmsg->value);
									g_fsm.count = (g_fsm.count + 1)%3;
								}
							break;
						}
						
						if(input_len_get() == 3)//输入了完整货道号
						{
							//显示统计信息
							e2p_get_channelStatistic(g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, &num, &val);
							//清除残留显示
							lcd_show_string("         ", pt_word.set[63].valx, pt_word.set[63].valy, pt_word.set[63].color, pt_word.set[63].background, *(pt_word.set[63].font));
							lcd_show_string("         ", pt_word.set[64].valx, pt_word.set[64].valy, pt_word.set[64].color, pt_word.set[64].background, *(pt_word.set[64].font));
							//显示数量与金额
							lcd_show_num(num, pt_word.set[63].valx, pt_word.set[63].valy, pt_word.set[63].color, pt_word.set[63].background, *(pt_word.set[63].font), 0, 10);
							lcd_show_money(val, pt_word.set[64].valx, pt_word.set[64].valy, pt_word.set[64].color, pt_word.set[64].background, *(pt_word.set[64].font));
						}
						lcd_show_string(input_char_get(), pt_word.set[62].valx, pt_word.set[62].valy, pt_word.set[62].color, pt_word.set[62].background, *(pt_word.set[62].font));
					}
					else//3位及以下货道位数
					{
						if(g_fsm.count == 0)//托盘位
						{
							if(cmsg->value > '0' && cmsg->value < '9')//最大托盘号为8
							{
								g_trade.curDiverBoard.tray = cmsg->value - '1';
								input_num(cmsg->value);
								g_fsm.count = (g_fsm.count + 1)%2;
							}
						}
						else
						if(g_fsm.count == 1)//货道位
						{
							if(cmsg->value >= '0' && cmsg->value <= '9')
							{
								g_trade.curDiverBoard.col = cmsg->value - '0';
								input_num(cmsg->value);
								g_fsm.count = (g_fsm.count + 1)%2;
							}
						}
						
						if(input_len_get() == 2)//输入了完整货道号
						{
							//显示统计信息
							e2p_get_channelStatistic(g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, &num, &val);
							//清除残留显示
							lcd_show_string("         ", pt_word.set[63].valx, pt_word.set[63].valy, pt_word.set[63].color, pt_word.set[63].background, *(pt_word.set[63].font));
							lcd_show_string("         ", pt_word.set[64].valx, pt_word.set[64].valy, pt_word.set[64].color, pt_word.set[64].background, *(pt_word.set[64].font));
							lcd_show_num(num, pt_word.set[63].valx, pt_word.set[63].valy, pt_word.set[63].color, pt_word.set[63].background, *(pt_word.set[63].font), 0, 10);
							lcd_show_money(val, pt_word.set[64].valx, pt_word.set[64].valy, pt_word.set[64].color, pt_word.set[64].background, *(pt_word.set[64].font));
						}
						lcd_show_string(input_char_get(), pt_word.set[62].valx, pt_word.set[62].valy, pt_word.set[62].color, pt_word.set[62].background, *(pt_word.set[62].font));
					}
				}
				else
				if(cmsg->value == '#')
				{
					if(g_vm.channelBit > 2)
					{
						if(input_len_get() > 0)//至少输入了机柜位
						{
							if(input_len_get() == 2)//输入了托盘位，整层统计
							{
								uint8_t num;
								uint16_t val;
								bkp_trayStatistic_get(g_trade.curDiverBoard.tray, &num, &val );
								//清除残留显示
								lcd_show_string("         ", pt_word.set[63].valx, pt_word.set[63].valy, pt_word.set[63].color, pt_word.set[63].background, *(pt_word.set[63].font));
								lcd_show_string("         ", pt_word.set[64].valx, pt_word.set[64].valy, pt_word.set[64].color, pt_word.set[64].background, *(pt_word.set[64].font));
								lcd_show_num(num, pt_word.set[63].valx, pt_word.set[63].valy, pt_word.set[63].color, pt_word.set[63].background, *(pt_word.set[63].font), 0, 10);
								lcd_show_money(val, pt_word.set[64].valx, pt_word.set[64].valy, pt_word.set[64].color, pt_word.set[64].background, *(pt_word.set[64].font));
								
								input_char_get()[input_len_get()] = '#';
							}
							else
							if(input_len_get() == 1)//整机统计
							{
								uint32_t num, val;
								bkp_wholeStatistic_get(&num, &val );
								//清除残留显示
								lcd_show_string("         ", pt_word.set[63].valx, pt_word.set[63].valy, pt_word.set[63].color, pt_word.set[63].background, *(pt_word.set[63].font));
								lcd_show_string("         ", pt_word.set[64].valx, pt_word.set[64].valy, pt_word.set[64].color, pt_word.set[64].background, *(pt_word.set[64].font));
								lcd_show_num(num, pt_word.set[63].valx, pt_word.set[63].valy, pt_word.set[63].color, pt_word.set[63].background, *(pt_word.set[63].font), 0, 10);
								lcd_show_money(val, pt_word.set[64].valx, pt_word.set[64].valy, pt_word.set[64].color, pt_word.set[64].background, *(pt_word.set[64].font));
							
								input_char_get()[input_len_get()] = '#';
							}
						}
						lcd_show_string(input_char_get(), pt_word.set[62].valx, pt_word.set[62].valy, pt_word.set[62].color, pt_word.set[62].background, *(pt_word.set[62].font));
					}
					else//2位及以下货道位
					{
						if(input_len_get() == 1)//输入了托盘位，显示整层统计数
						{
							uint8_t num;
							uint16_t val;
							bkp_trayStatistic_get(g_trade.curDiverBoard.tray, &num, &val );
							//清除残留显示
							lcd_show_string("         ", pt_word.set[63].valx, pt_word.set[63].valy, pt_word.set[63].color, pt_word.set[63].background, *(pt_word.set[63].font));
							lcd_show_string("         ", pt_word.set[64].valx, pt_word.set[64].valy, pt_word.set[64].color, pt_word.set[64].background, *(pt_word.set[64].font));
							lcd_show_num(num, pt_word.set[63].valx, pt_word.set[63].valy, pt_word.set[63].color, pt_word.set[63].background, *(pt_word.set[63].font), 0, 10);
							lcd_show_money(val, pt_word.set[64].valx, pt_word.set[64].valy, pt_word.set[64].color, pt_word.set[64].background, *(pt_word.set[64].font));
							
							input_char_get()[input_len_get()] = '#';
						}
						else
						if(input_len_get() == 0)//整机测试，显示整机统计数
						{
							uint32_t num, val;
							bkp_wholeStatistic_get(&num, &val );
							//清除残留显示
							lcd_show_string("         ", pt_word.set[63].valx, pt_word.set[63].valy, pt_word.set[63].color, pt_word.set[63].background, *(pt_word.set[63].font));
							lcd_show_string("         ", pt_word.set[64].valx, pt_word.set[64].valy, pt_word.set[64].color, pt_word.set[64].background, *(pt_word.set[64].font));
							lcd_show_num(num, pt_word.set[63].valx, pt_word.set[63].valy, pt_word.set[63].color, pt_word.set[63].background, *(pt_word.set[63].font), 0, 10);
							lcd_show_money(val, pt_word.set[64].valx, pt_word.set[64].valy, pt_word.set[64].color, pt_word.set[64].background, *(pt_word.set[64].font));
							
							input_char_get()[input_len_get()] = '#';
						}
						lcd_show_string(input_char_get(), pt_word.set[62].valx, pt_word.set[62].valy, pt_word.set[62].color, pt_word.set[62].background, *(pt_word.set[62].font));
					}
					input_clear();
					select_clear();
					g_fsm.count = 0;
					
					if(g_vm.channelBit > 2)
					{
						input_len_set(3);
					}
					else{
						input_len_set(2);
					}
				}
				else
				if(cmsg->value == '*')
				{
					if(g_fsm.shortcut == 1)
					{
						Load_status(fsm_id_init);//返回初始界面
					}
					else
					{
						Load_status(fsm_id_seniorMenu);//进入高权限菜单
					}
				}
				else
				if(cmsg->value == '.')
				{
					//复位
					g_fsm.mode = 1;
					lcd_clear();
					lcd_show_character(&pt_word.set[65]);//“清除统计数据"
					lcd_show_character(&pt_word.set[66]);//"1.确认"
				}
				else
				if(cmsg->value == KEY_UP_VAL)//上翻，查询上一个货道
				{
					if(g_trade.curDiverBoard.col == 0)
					{
						if(g_trade.curDiverBoard.tray == 0)
						{
							g_trade.curDiverBoard.tray = g_vm.maxtrayNum - 1;
							if(g_trade.curDiverBoard.container == 0)
							{
								g_trade.curDiverBoard.container = g_vm.maxContainerNum - 1;
							}
							else
							{
								g_trade.curDiverBoard.container --;
							}
						}
						else
						{
							g_trade.curDiverBoard.tray--;
						}
						g_trade.curDiverBoard.col = 9;	
					}
					else
					{
						g_trade.curDiverBoard.col--;
					}
					if(g_vm.channelBit == 1)
					{
						input_char_get()[0] = g_trade.curDiverBoard.tray + '1';
						input_char_get()[1] = g_trade.curDiverBoard.col + '0';
					}
					else
					{
						input_char_get()[0] = g_trade.curDiverBoard.container + '1';
						input_char_get()[1] = g_trade.curDiverBoard.tray + '1';
						input_char_get()[2] = g_trade.curDiverBoard.col + '0';
					}
					lcd_show_string(input_char_get(), pt_word.set[62].valx, pt_word.set[62].valy, pt_word.set[62].color, pt_word.set[62].background, *(pt_word.set[62].font));
					e2p_get_channelStatistic(g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, &num, &val);
					//清除残留显示
					lcd_show_string("         ", pt_word.set[63].valx, pt_word.set[63].valy, pt_word.set[63].color, pt_word.set[63].background, *(pt_word.set[63].font));
					lcd_show_string("         ", pt_word.set[64].valx, pt_word.set[64].valy, pt_word.set[64].color, pt_word.set[64].background, *(pt_word.set[64].font));
					lcd_show_num(num, pt_word.set[63].valx, pt_word.set[63].valy, pt_word.set[63].color, pt_word.set[63].background, *(pt_word.set[63].font), 0, 10);
					lcd_show_money(val, pt_word.set[64].valx, pt_word.set[64].valy, pt_word.set[64].color, pt_word.set[64].background, *(pt_word.set[64].font));
					
				}
				else
				if(cmsg->value == KEY_DOWN_VAL)//下翻，查询下一个货道
				{
					g_trade.curDiverBoard.col++;
					if(g_trade.curDiverBoard.col == MAX_COLUMN_SIZE)
					{
						g_trade.curDiverBoard.col = 0;
						if(g_trade.curDiverBoard.tray >= g_vm.maxtrayNum -1)
						{
							g_trade.curDiverBoard.tray = 0;
							g_trade.curDiverBoard.container++;
							if(g_trade.curDiverBoard.container >= g_vm.maxContainerNum )
							{
								g_trade.curDiverBoard.container = 0;
							}
						}
						else
						{
							g_trade.curDiverBoard.tray++;
						}
					}
					if(g_vm.channelBit == 1)
					{
						input_char_get()[0] = g_trade.curDiverBoard.tray + '1';
						input_char_get()[1] = g_trade.curDiverBoard.col + '0';
					}
					else
					{
						input_char_get()[0] = g_trade.curDiverBoard.container + '1';
						input_char_get()[1] = g_trade.curDiverBoard.tray + '1';
						input_char_get()[2] = g_trade.curDiverBoard.col + '0';
					}
					lcd_show_string(input_char_get(), pt_word.set[62].valx, pt_word.set[62].valy, pt_word.set[62].color, pt_word.set[62].background, *(pt_word.set[62].font));
					e2p_get_channelStatistic(g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, &num, &val);
					//清除残留显示
					lcd_show_string("         ", pt_word.set[63].valx, pt_word.set[63].valy, pt_word.set[63].color, pt_word.set[63].background, *(pt_word.set[63].font));
					lcd_show_string("         ", pt_word.set[64].valx, pt_word.set[64].valy, pt_word.set[64].color, pt_word.set[64].background, *(pt_word.set[64].font));
					lcd_show_num(num, pt_word.set[63].valx, pt_word.set[63].valy, pt_word.set[63].color, pt_word.set[63].background, *(pt_word.set[63].font), 0, 10);
					lcd_show_money(val, pt_word.set[64].valx, pt_word.set[64].valy, pt_word.set[64].color, pt_word.set[64].background, *(pt_word.set[64].font));
				}
			}
			else
			if(g_fsm.mode == 1)
			{
				if(cmsg->value == '1')
				{
					g_fsm.mode++;
					lcd_show_character(&pt_word.set[68]);//"正在清除"
					lcd_clear_character(&pt_word.set[66]);
					bkp_wholeStatistic_reset();//清除整机统计
					//清除成功
					lcd_show_character(&pt_word.set[67]);//"清除完成"
					OSTimeDly(OS_TICKS_PER_SEC);
					Load_status(fsm_id_statistic);
				}
				else
				if(cmsg->value == '*')
				{
					Load_status(fsm_id_statistic);
				}
			}
		break;
	}
	return 0;
}

