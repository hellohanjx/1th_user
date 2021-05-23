/*
��������
���������ԣ����̲��ԣ���������

ע�⣺���ļ��Ǳ�׼ģ��
*/

#include "header.h"


#include "stm32f10x_bkp.h"
volatile uint32_t lesstime = 0;
volatile uint32_t timerecord;

enum{TESTING, TEST_OK, TEST_ERR, STOP};
enum{T_COL, T_TRAY, T_WHOLE}; 

/*
��ʾ�������������״̬
�������ֻ��ʾ���ڵĵ��
*/
static void show_column_state(void)
{
	uint8_t i, state;
	
	lcd_show_num(g_trade.curDiverBoard.container + 1, pt_word.set[10].valx, pt_word.set[10].valy, pt_word.set[10].color, pt_word.set[10].background, *(pt_word.set[10].font), 1, 1);//�����
	lcd_show_num(g_trade.curDiverBoard.tray + 1, pt_word.set[11].valx, pt_word.set[11].valy, pt_word.set[11].color, pt_word.set[11].background, *(pt_word.set[11].font), 1, 1); //���̺�
	for(i = 0; i < 10; i++)//��������
	{
		e2p_get_channelEnable(g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + i, &state);
		lcd_show_num(state, i*(pt_word.set[12].font->size)/2, pt_word.set[12].valx, pt_word.set[12].color, pt_word.set[12].background, *(pt_word.set[12].font), 1, 1);//����ʹ��
	}
	for(i = 0; i < 10; i++)//����״̬
	{
		lcd_show_num(pt_driverBoard[g_trade.curDiverBoard.container]->channelState[g_trade.curDiverBoard.tray*10 + i], i*(pt_word.set[12].font->size)/2, pt_word.set[12].valy, pt_word.set[12].color, pt_word.set[12].background, *(pt_word.set[12].font), 1, 1);
	}
	
}

/*
��ǰ������˸
state �� 1����˸�� 0��ֹͣ��˸
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
���û������볤��
*/
static void set_column_len(void)
{
	uint8_t i, state;
	
	g_fsm.count = 0;
	
	if(g_vm.channelBit > 2)
		state = 3;			//�˴�����Ļ���λ��
	else
		state = 2;			//�˴�����Ļ���λ��
	input_len_set(state);		//�������볤��
	
	for(i = 0; i < state; i++)//��ʾռλ��
		input_char_get()[i] = '-';
	lcd_show_string(input_char_get(), pt_word.set[13].valx, pt_word.set[13].valy, pt_word.set[13].color, pt_word.set[13].background, *(pt_word.set[13].font));
	
}


/*
�������ý���״̬��
*/
uint8_t fsm_fun_channleTest(FSMMSG* cmsg)
{	
	switch (cmsg->type)
	{
		case MsgSYS:
			if(cmsg->stype == FSM_STATUS_INIT)
			{
				input_clear();		//����ؼ�����
				select_clear();		//��������ؼ�����
				if(g_fsm.shortcut == 1)
					lcd_clear();
				else
					LCD_CLEAR_SIDE;		//ˢ�ұ���
				g_fsm.mode = T_COL;		//0���������ԣ�1�����̲��ԣ�2��������
				g_fsm.status = STOP;//1:δ������2�����ڲ���
				g_fsm.count = 0;	//�������λ��
				g_fsm.num = pt_word.set[12].color;		//���������������˸���.1:��ͨ��ʾ��2������
				
				g_trade.curDiverBoard.container = 0;
				g_trade.curDiverBoard.tray = 0;
				g_trade.curDiverBoard.col = 0;
				
				lcd_show_character(&pt_word.set[10]);//"����"
				lcd_show_character(&pt_word.set[11]);//"����"
				lcd_show_character(&pt_word.set[12]);//"����"
				lcd_show_character(&pt_word.set[59]);//"ִ��

				show_column_state();
				
				lcd_show_character(&pt_word.set[13]);//"����"
				set_column_len();
			}
			else
			if(cmsg->stype == FSM_STATUS_EXIT)
			{
				input_clear();//����ؼ�����
				select_clear();//��������ؼ�����
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
				if(g_fsm.status != STOP)//���ڲ���
				{
					g_fsm.mode = 0;
					g_fsm.status = STOP;
					input_clear();
					set_column_len();
//					lcd_show_character(&pt_word.set[13]);//ֹͣ����
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
			if(cmsg->value == KEY_DOWN_VAL)//�·�
			{
				if(g_fsm.status != STOP)
					return 0;
				//������һ������
				input_clear();
				set_column_len();
				//������һ���ֵ�������
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
			if(cmsg->value == KEY_UP_VAL)//�Ϸ�
			{
				if(g_fsm.status != STOP)
					return 0;
				//������һ������
				input_clear();
				set_column_len();
				//������һ���ֵ�������
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
			if(cmsg->value == '#')//�������Ի������̲���
			{
				if(g_fsm.status != STOP)
					return 0;
				
				if(g_vm.channelBit > 2)
				{
					if(input_len_get() > 0)//���������˻���λ
					{
						if(input_len_get() == 2)//����������λ����ֻ���Դ�����
						{
							input_char_get()[input_len_get()] = '#';
							g_trade.curDiverBoard.col = 0;
							show_column_state();
							g_fsm.mode = T_TRAY;
							g_fsm.status = TESTING;
							show_column_fls(TRUE);	//��ǰ������˸
							//������ָ��
							apply_driverboard_msg(M_CHANNEL_TEST, g_trade.curDiverBoard.container, g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, NULL, NULL);
						}
						else
						if(input_len_get() == 1)//��������
						{
							input_char_get()[input_len_get()] = '#';
							show_column_state();
							g_trade.curDiverBoard.container = 0;
							g_trade.curDiverBoard.tray = 0;
							g_trade.curDiverBoard.col = 0;
							g_fsm.mode = T_WHOLE;
							g_fsm.status = TESTING;
							show_column_state();
							show_column_fls(TRUE);	//��ǰ������˸
							//������ָ��
							apply_driverboard_msg(M_CHANNEL_TEST, g_trade.curDiverBoard.container, g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, NULL, NULL);
						}
					}
				}
				else
				{
					if(input_len_get() == 1)//����������λ����ֻ���Դ�����
					{
						input_char_get()[input_len_get()] = '#';
						show_column_state();
						g_trade.curDiverBoard.col = 0;
						g_fsm.mode = T_TRAY;
						g_fsm.status = TESTING;
						show_column_fls(TRUE);	//��ǰ������˸
						//������ָ��
						apply_driverboard_msg(M_CHANNEL_TEST, g_trade.curDiverBoard.container, g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, NULL, NULL);
					}
					else
					if(input_len_get() == 0)//��������
					{
						input_char_get()[input_len_get()] = '#';
						show_column_state();
						g_trade.curDiverBoard.container = 0;
						g_trade.curDiverBoard.tray = 0;
						g_trade.curDiverBoard.col = 0;
						g_fsm.mode = T_WHOLE;
						g_fsm.status = TESTING;
						show_column_state();
						show_column_fls(TRUE);	//��ǰ������˸
						//������ָ��
						apply_driverboard_msg(M_CHANNEL_TEST, g_trade.curDiverBoard.container, g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, NULL, NULL);
					}
				}
				lcd_show_string(input_char_get(), pt_word.set[13].valx, pt_word.set[13].valy, pt_word.set[13].color, pt_word.set[13].background, *(pt_word.set[13].font));

			}
			
			else//ѡ�����������
			{
				if(g_fsm.status != STOP)
					return 0;
				if(g_vm.channelBit > 2)//3λ�����ϻ���λ��
				{
					switch(g_fsm.count)
					{
						case 0://����λ
							if(cmsg->value > '0' && cmsg->value <= g_vm.maxContainerNum + '0')
							{
								g_trade.curDiverBoard.container = cmsg->value - '1';
								input_num(cmsg->value);
								g_fsm.count = (g_fsm.count + 1)%3;
							}
						break;
						
						case 1://����λ
							if(cmsg->value > '0' && cmsg->value < '9')//������̺�Ϊ8
							{
								g_trade.curDiverBoard.tray = cmsg->value - '1';
								input_num(cmsg->value);
								g_fsm.count = (g_fsm.count + 1)%3;
							}
						break;
							
						case 2://����λ
							if(cmsg->value >= '0' && cmsg->value <= '9')
							{
								g_trade.curDiverBoard.col = cmsg->value - '0';
								input_num(cmsg->value);
								g_fsm.count = (g_fsm.count + 1)%3;
							}
						break;
					}
					
					if(input_len_get() == 3)//����������������
					{
						show_column_state();
						show_column_fls(TRUE);	//��ǰ������˸
						g_fsm.mode = T_COL;		//��������
						g_fsm.status = TESTING;	//���ڲ���
						//������ָ��
						apply_driverboard_msg(M_CHANNEL_TEST, g_trade.curDiverBoard.container, g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, NULL, NULL);
					}
					
				}
				else//3λ�����»���λ��
				{
					if(g_fsm.count == 0)//����λ
					{
						if(cmsg->value > '0' && cmsg->value < '9')//������̺�Ϊ8
						{
							g_trade.curDiverBoard.tray = cmsg->value - '1';
							input_num(cmsg->value);
							g_fsm.count = (g_fsm.count + 1)%2;
						}
					}
					else
					if(g_fsm.count == 1)//����λ
					{
						if(cmsg->value >= '0' && cmsg->value <= '9')
						{
							g_trade.curDiverBoard.col = cmsg->value - '0';
							input_num(cmsg->value);
							g_fsm.count = (g_fsm.count + 1)%2;
						}
					}
					
					if(input_len_get() == 2)//����������������
					{
						
						show_column_state();	//��ʾ��ǰ����ҳ
						show_column_fls(TRUE);	//��ǰ������˸
						g_fsm.mode = T_COL;		//��������
						g_fsm.status = TESTING; //���ڲ���
						//������ָ��
					if(lesstime == 0)
					lesstime = OSTimeGet();
						apply_driverboard_msg(M_CHANNEL_TEST, g_trade.curDiverBoard.container, g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, NULL, NULL);
					}
				}
				
				lcd_show_string(input_char_get(), pt_word.set[13].valx, pt_word.set[13].valy, pt_word.set[13].color, pt_word.set[13].background, *(pt_word.set[13].font));
			}
		break;
			
		case MsgCHAN://��������Ϣ
			if(cmsg->stype == M_CHANNEL_TEST)
			{
				if(cmsg->value == 1)
				{
					timerecord = OSTimeGet();
					timerecord = OSTimeGet() - lesstime;
					lesstime = 0;

//					if(OSTimeGet() - lesstime < 1800)//500ms�ڳɹ�
//						BKP_WriteBackupRegister(BKP_DR30,(BKP_ReadBackupRegister(BKP_DR30)+1));
//					else
//						BKP_WriteBackupRegister(BKP_DR31,(BKP_ReadBackupRegister(BKP_DR31)+1));
					
					
					
					if(g_fsm.status != STOP)//��ֹ���ת��ʱ��'*'���󣬵��ת���ָ����˹���״̬
						g_fsm.status = TEST_OK;//"��������"
					else
						show_column_fls(FALSE);//ֱ�Ӹ�����ʾ
				}
				else
				{
					if(g_fsm.status != STOP)
						g_fsm.status = TEST_ERR;//"�����쳣"
					else
						show_column_fls(FALSE);
				}
				//��ʾִ�н��
				lcd_show_num(cmsg->value, pt_word.set[59].valx, pt_word.set[59].valy, pt_word.set[59].color, pt_word.set[59].background, *(pt_word.set[59].font), 1 ,2);
			}
			else
			if(cmsg->stype == M_DRV_ERR)//�����������Ϣ
			{
				if(g_fsm.status != STOP)
					g_fsm.status = TEST_ERR;//"�����쳣"
				lcd_show_num(cmsg->value, pt_word.set[59].valx, pt_word.set[59].valy, pt_word.set[59].color, pt_word.set[59].background, *(pt_word.set[59].font), 1 ,2);
			}
		break;
			
		case MsgASSIS://��Ϣ����
			if(assistantstate)
			{
				if(g_fsm.status == STOP)
				{
					show_column_fls(FALSE);
				}
				else
				{
					show_column_fls(TRUE);
					if(g_fsm.status == TEST_ERR || g_fsm.status == TEST_OK)//һ�������������
					{
						OSTimeDly(OS_TICKS_PER_SEC);//���Լ��
						switch(g_fsm.mode)
						{
							case T_COL:
								g_fsm.status = TESTING;	//���ڲ���
									
						if(lesstime == 0)
						lesstime = OSTimeGet();

								apply_driverboard_msg(M_CHANNEL_TEST, g_trade.curDiverBoard.container, g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, NULL, NULL);
							break;
								
							case T_TRAY:
								show_column_fls(FALSE);//��ǰֹͣ��˸
								g_trade.curDiverBoard.col = (g_trade.curDiverBoard.col + 1) % 10;
								show_column_fls(TRUE);//
								g_fsm.status = TESTING;	//���ڲ���
								apply_driverboard_msg(M_CHANNEL_TEST, g_trade.curDiverBoard.container, g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col, NULL, NULL);
							break;
							
							case T_WHOLE:
								show_column_fls(FALSE);//��ǰֹͣ��˸
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
								g_fsm.status = TESTING;	//���ڲ���
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


