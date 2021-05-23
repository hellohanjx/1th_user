/*
货道测试
单货道测试，托盘测试，整机测试

注意：此文件非标准模板
*/

#include "header.h"


#include "stm32f10x_bkp.h"
volatile uint32_t lesstime = 0;
volatile uint32_t timerecord;

enum{TESTING, TEST_OK, TEST_ERR, STOP};
enum{T_COL, T_TRAY, T_WHOLE}; 

/*
显示货道设置与货道状态
这里可以只显示存在的电机
*/
static void show_column_state(void)
{
	uint8_t i, state;
	
	lcd_show_num(g_trade.curDiverBoard.container + 1, pt_word.set[10].valx, pt_word.set[10].valy, pt_word.set[10].color, pt_word.set[10].background, *(pt_word.set[10].font), 1, 1);//机柜号
	lcd_show_num(g_trade.curDiverBoard.tray + 1, pt_word.set[11].valx, pt_word.set[11].valy, pt_word.set[11].color, pt_word.set[11].background, *(pt_word.set[11].font), 1, 1); //托盘号
	for(i = 0; i < 10; i++)//货道设置
	{
		e2p_get_channelEnable(g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + i, &state);
		lcd_show_num(state, i*(pt_word.set[12].font->size)/2, pt_word.set[12].valx, pt_word.set[12].color, pt_word.set[12].background, *(pt_word.set[12].font), 1, 1);//货道使能
	}
	for(i = 0; i < 10; i++)//货道状态
	{
		lcd_show_num(pt_driverBoard[g_trade.curDiverBoard.container]->channelState[g_trade.curDiverBoard.tray*10 + i], i*(pt_word.set[12].font->size)/2, pt_word.set[12].valy, pt_word.set[12].color, pt_word.set[12].background, *(pt_word.set[12].font), 1, 1);
	}
	
}

/*
当前货道闪烁
state ： 1：闪烁； 0：停止闪烁
*/
static void show_column_fls(bool state)
{
	if(state == TRUE)
	{
		uint16_t background = g_fsm.num;
		if(g_fsm.num == pt_word.set[12].color)
			g_fsm.num = pt_word.set[12].background;
		else
			g_fsm.num = pt_word.set[12].color;
		lcd_show_num(pt_driverBoard[g_trade.curDiverBoard.container]->channelState[g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col], g_trade.curDiverBoard.col*(pt_word.set[12].font->size)/2, pt_word.set[12].valy, g_fsm.num, background, *(pt_word.set[12].font), 1, 1);
		assistanttiming(1);
	}
	else
	{
//		g_fsm.num = holder.foreground;
		lcd_show_num(pt_driverBoard[g_trade.curDiverBoard.container]->channelState[g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col], g_trade.curDiverBoard.col*(pt_word.set[12].font->size)/2, pt_word.set[12].valy, pt_word.set[12].color , pt_word.set[12].background, 
		*(pt_word.set[12].font), 1, 1);
	
	}
}

/*
设置货道输入长度
*/
static void set_column_len(void)
{
	uint8_t i, state;
	
	g_fsm.count = 0;
	
	if(g_vm.channelBit > 2)
		state = 3;			//此处输入的货道位数
	else
		state = 2;			//此处输入的货道位数
	input_len_set(state);		//设置输入长度
	
	for(i = 0; i < state; i++)//显示占位符
		input_char_get()[i] = '-';
	lcd_show_string(input_char_get(), pt_word.set[13].valx, pt_word.set[13].valy, pt_word.set[13].color, pt_word.set[13].background, *(pt_word.set[13].font));
	
}


/*
货道设置界面状态机
*/
uint8_t fsm_fun_channleTest(FSMMSG* cmsg)
{	
	switch (cmsg->type)
	{
		case MsgSYS:
			if(cmsg->stype == FSM_STATUS_INIT)
			{
				input_clear();		//输入控件清零
				select_clear();		//货道输入控件清零
				if(g_fsm.shortcut == 1)
					lcd_clear();
				else
					LCD_CLEAR_SIDE;		//刷右边栏
				g_fsm.mode = T_COL;		//0：货道测试；1：托盘测试；2整机测试
				g_fsm.status = STOP;//1:未动作；2：正在测试
				g_fsm.count = 0;	//输入货道位数
				g_fsm.num = pt_word.set[12].color;		//这个变量用来做闪烁标记.1:普通显示；2：高亮
				
				g_trade.curDiverBoard.container = 0;
				g_trade.curDiverBoard.tray = 0;
				g_trade.curDiverBoard.col = 0;
				
				lcd_show_character(&pt_word.set[10]);//"机柜"
				lcd_show_character(&pt_word.set[11]);//"托盘"
				lcd_show_character(&pt_word.set[12]);//"货道"
				lcd_show_character(&pt_word.set[59]);//"执行

				show_column_state();
				
				lcd_show_character(&pt_word.set[13]);//"输入"
				set_column_len();
			}
			else
			if(cmsg->stype == FSM_STATUS_EXIT)
			{
				input_clear();//输入控件清零
				select_clear();//货道输入控件清零
			}
		break;
		
		case MsgKEY:
			if(cmsg->value == DOOR_CLOSE_VAL)
			{
				Load_status(fsm_id_init);
			}
			else
			if(cmsg->value == '*')
			{
				if(g_fsm.status != STOP)//正在测试
				{
					g_fsm.mode = 0;
					g_fsm.status = STOP;
					input_clear();
					set_column_len();
//					lcd_show_character(&pt_word.set[13]);//停止测试
				} 
				else
				{
					if(g_fsm.shortcut == 1)
						Load_status(fsm_id_init);
					else
						Load_status(fsm_id_seniorMenu);
				}
			}
			else 
			if(cmsg->value == KEY_DOWN_VAL)//下翻
			{
				if(g_fsm.status != STOP)
					return 0;
				//翻到下一个托盘
				input_clear();
				set_column_len();
				//计算下一个轮到的托盘
				g_trade.curDiverBoard.col = 0;
				if(g_trade.curDiverBoard.tray == g_vm.maxtrayNum - 1 || g_trade.curDiverBoard.tray >= g_vm.maxtrayNum )
				{
					g_trade.curDiverBoard.tray = 0;
					g_trade.curDiverBoard.container++;
					if(g_trade.curDiverBoard.container >= g_vm.maxContainerNum )
					{
						g_trade.curDiverBoard.container = 0;
						g_trade.curDiverBoard.tray = 0;
					}
				}
				else
				{
					g_trade.curDiverBoard.tray++;
				}
				
				show_column_state();
				
			}
			else
			if(cmsg->value == KEY_UP_VAL)//上翻
			{
				if(g_fsm.status != STOP)
					return 0;
				//翻到上一个托盘
				input_clear();
				set_column_len();
				//计算下一个轮到的托盘
				g_trade.curDiverBoard.col = 0;
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
				
				show_column_state();
			}
			
			else
			if(cmsg->value == '#')//整机测试或者托盘测试
			{
				if(g_fsm.status != STOP)
					return 0;
				
				if(g_vm.channelBit > 2)
				{
					if(input_len_get() > 0)//至少输入了机柜位
					{
						if(input_len_get() == 2)//输入了托盘位，则只测试此托盘
						{
							input_char_get()[input_len_get()] = '#';
							g_trade.curDiverBoard.col = 0;
							show_column_state();
							g_fsm.mode = T_TRAY;
							g_fsm.status = TESTING;
							show_column_fls(TRUE);	//当前货道闪烁
							//发测试指令
							apply_driverboard_msg(M_CHANNEL_TEST, g_trade.curDiverBoard.container, g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, NULL, NULL);
						}
						else
						if(input_len_get() == 1)//整机测试
						{
							input_char_get()[input_len_get()] = '#';
							show_column_state();
							g_trade.curDiverBoard.container = 0;
							g_trade.curDiverBoard.tray = 0;
							g_trade.curDiverBoard.col = 0;
							g_fsm.mode = T_WHOLE;
							g_fsm.status = TESTING;
							show_column_state();
							show_column_fls(TRUE);	//当前货道闪烁
							//发测试指令
							apply_driverboard_msg(M_CHANNEL_TEST, g_trade.curDiverBoard.container, g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, NULL, NULL);
						}
					}
				}
				else
				{
					if(input_len_get() == 1)//输入了托盘位，则只测试此托盘
					{
						input_char_get()[input_len_get()] = '#';
						show_column_state();
						g_trade.curDiverBoard.col = 0;
						g_fsm.mode = T_TRAY;
						g_fsm.status = TESTING;
						show_column_fls(TRUE);	//当前货道闪烁
						//发测试指令
						apply_driverboard_msg(M_CHANNEL_TEST, g_trade.curDiverBoard.container, g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, NULL, NULL);
					}
					else
					if(input_len_get() == 0)//整机测试
					{
						input_char_get()[input_len_get()] = '#';
						show_column_state();
						g_trade.curDiverBoard.container = 0;
						g_trade.curDiverBoard.tray = 0;
						g_trade.curDiverBoard.col = 0;
						g_fsm.mode = T_WHOLE;
						g_fsm.status = TESTING;
						show_column_state();
						show_column_fls(TRUE);	//当前货道闪烁
						//发测试指令
						apply_driverboard_msg(M_CHANNEL_TEST, g_trade.curDiverBoard.container, g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, NULL, NULL);
					}
				}
				lcd_show_string(input_char_get(), pt_word.set[13].valx, pt_word.set[13].valy, pt_word.set[13].color, pt_word.set[13].background, *(pt_word.set[13].font));

			}
			
			else//选择货道或托盘
			{
				if(g_fsm.status != STOP)
					return 0;
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
						show_column_state();
						show_column_fls(TRUE);	//当前货道闪烁
						g_fsm.mode = T_COL;		//货道测试
						g_fsm.status = TESTING;	//正在测试
						//发测试指令
						apply_driverboard_msg(M_CHANNEL_TEST, g_trade.curDiverBoard.container, g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, NULL, NULL);
					}
					
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
						
						show_column_state();	//显示当前托盘页
						show_column_fls(TRUE);	//当前货道闪烁
						g_fsm.mode = T_COL;		//货道测试
						g_fsm.status = TESTING; //正在测试
						//发测试指令
					if(lesstime == 0)
					lesstime = OSTimeGet();
						apply_driverboard_msg(M_CHANNEL_TEST, g_trade.curDiverBoard.container, g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, NULL, NULL);
					}
				}
				
				lcd_show_string(input_char_get(), pt_word.set[13].valx, pt_word.set[13].valy, pt_word.set[13].color, pt_word.set[13].background, *(pt_word.set[13].font));
			}
		break;
			
		case MsgCHAN://驱动板消息
			if(cmsg->stype == M_CHANNEL_TEST)
			{
				if(cmsg->value == 1)
				{
					timerecord = OSTimeGet();
					timerecord = OSTimeGet() - lesstime;
					lesstime = 0;

//					if(OSTimeGet() - lesstime < 1800)//500ms内成功
//						BKP_WriteBackupRegister(BKP_DR30,(BKP_ReadBackupRegister(BKP_DR30)+1));
//					else
//						BKP_WriteBackupRegister(BKP_DR31,(BKP_ReadBackupRegister(BKP_DR31)+1));
					
					
					
					if(g_fsm.status != STOP)//防止电机转动时按'*'键后，电机转完又更新了工作状态
						g_fsm.status = TEST_OK;//"货道正常"
					else
						show_column_fls(FALSE);//直接更新显示
				}
				else
				{
					if(g_fsm.status != STOP)
						g_fsm.status = TEST_ERR;//"货道异常"
					else
						show_column_fls(FALSE);
				}
				//显示执行结果
				lcd_show_num(cmsg->value, pt_word.set[59].valx, pt_word.set[59].valy, pt_word.set[59].color, pt_word.set[59].background, *(pt_word.set[59].font), 1 ,2);
			}
			else
			if(cmsg->stype == M_DRV_ERR)//驱动板故障消息
			{
				if(g_fsm.status != STOP)
					g_fsm.status = TEST_ERR;//"货道异常"
				lcd_show_num(cmsg->value, pt_word.set[59].valx, pt_word.set[59].valy, pt_word.set[59].color, pt_word.set[59].background, *(pt_word.set[59].font), 1 ,2);
			}
		break;
			
		case MsgASSIS://消息助手
			if(assistantstate)
			{
				if(g_fsm.status == STOP)
				{
					show_column_fls(FALSE);
				}
				else
				{
					show_column_fls(TRUE);
					if(g_fsm.status == TEST_ERR || g_fsm.status == TEST_OK)//一个货道测试完成
					{
						OSTimeDly(OS_TICKS_PER_SEC);//测试间隔
						switch(g_fsm.mode)
						{
							case T_COL:
								g_fsm.status = TESTING;	//正在测试
									
						if(lesstime == 0)
						lesstime = OSTimeGet();

								apply_driverboard_msg(M_CHANNEL_TEST, g_trade.curDiverBoard.container, g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, NULL, NULL);
							break;
								
							case T_TRAY:
								show_column_fls(FALSE);//当前停止闪烁
								g_trade.curDiverBoard.col = (g_trade.curDiverBoard.col + 1) % 10;
								show_column_fls(TRUE);//
								g_fsm.status = TESTING;	//正在测试
								apply_driverboard_msg(M_CHANNEL_TEST, g_trade.curDiverBoard.container, g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, NULL, NULL);
							break;
							
							case T_WHOLE:
								show_column_fls(FALSE);//当前停止闪烁
								g_trade.curDiverBoard.col++;
								if(g_trade.curDiverBoard.col == 10)
								{
									g_trade.curDiverBoard.col = 0;
									if(g_trade.curDiverBoard.tray >= g_vm.maxtrayNum -1)
									{
										g_trade.curDiverBoard.tray = 0;
										g_trade.curDiverBoard.container++;
										show_column_state();
										if(g_trade.curDiverBoard.container >= g_vm.maxContainerNum )
										{
											g_trade.curDiverBoard.container = 0;
											show_column_state();
										}
									}
									else
									{
										g_trade.curDiverBoard.tray++;
										show_column_state();
									}
								}
								show_column_fls(TRUE);//
								g_fsm.status = TESTING;	//正在测试
								apply_driverboard_msg(M_CHANNEL_TEST, g_trade.curDiverBoard.container, g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, NULL, NULL);
							break;
							default:
								break;
						}
					}
				}
		  	}
		break;
	}
	return 0;
}


