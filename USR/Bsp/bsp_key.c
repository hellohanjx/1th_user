/*
��������

*/



/*
Լ������
KEY1~KEY4 �����У�3.3v����
key4 ��һ��
key3 �ڶ���
key2 ������
key1 ������

KEY5~KEY8 ������
key5 ��һ��
key6 �ڶ���
key7 ������
key8 ��4��
*/
//12������
//1 2 3
//4 5 6
//7 8 9
//* 0 #
//===============================
#include "header.h"
#include "stm32_lib.h"
#include "bsp_key.h" 

static const uint8_t keyValue[4][4]={
	{'1','2','3','*'},
	{'4','5','6','0'},
	{'7','8','9','#'}, 
	{'*','0','#','#'},
};


static const uint16_t pin[]={GPIO_Pin_10, GPIO_Pin_9, GPIO_Pin_8, GPIO_Pin_7};

static void key_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
//	//WAKEUP PA0
//	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//DOOR PA1
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;//����ĳ��������ֹ����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//USER1 PE2
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	//USER2,3,4 PB7,PB6,PB5
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7|GPIO_Pin_6|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//��������� KEY5,KEY6,KEY7,KEY8.
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF,&GPIO_InitStructure);
  
	//��������� KEY1,KEY2,KEY3,KEY4
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
}


/*
��ɨ��
*/
static uint8_t keyBoardHscan(void)
{
	uint8_t inputData;
	inputData = GPIO_ReadInputData(GPIOC);
	inputData &= 0x0F;
	switch(inputData)
	{
		case ____XXX_:
			return 0;
		case ____XX_X:
			return 1;
		case ____X_XX:
			return 2;
		case _____XXX:
			return 3;
		case ____XX__:
			return 4;
		case ____X__X:
			return 5;
		case ____X_X_:
			return 6;
		case _____XX_:
			return 7;
		default:
			return 0xFF;
	}
}


/*
���尴����Ϣ����
*/
static void send_key_msg(uint8_t key)
{
	FSMMSG* msg = apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
	assert_param(msg);
	if(msg != 0)
	{
		msg->type = MsgKEY;
		msg->value = key;
		BEEP(ON);
		OSTimeDly(OS_TICKS_PER_SEC/10);
		BEEP(OFF);
		if(!send_fsm_msg(msg))
		release_fsm_msg(msg);
	}
}

/*
���尴��ɨ��
*/
static void scan_board_key(void)
{
	uint8_t key =  0xff;
	
	if(USER_1 == PRESS)
	{
		key = PRESS ;
		OSTimeDly(60);//������
		if(key == USER_1)
		{
			send_key_msg(USER1_VAL);
			while(USER_1 == PRESS)
			{
				OSTimeDly(60);
			}
		}
	}
	else
	if(USER_2 == PRESS)
	{
		key = PRESS ;
		OSTimeDly(60);//������
		if(key == USER_2)
		{
			send_key_msg(USER2_VAL);
			while(USER_2 == PRESS)
			{
				OSTimeDly(60);
			}
		}
	}
	else
	if(USER_3 == PRESS)
	{
		key = PRESS ;
		OSTimeDly(60);//������
		if(key == USER_3)
		{
			send_key_msg(USER3_VAL);
			while(USER_3 == PRESS)
			{
				OSTimeDly(60);
			}
		}
	}
	else
	if(USER_4 == PRESS)
	{
		key = PRESS ;
		OSTimeDly(60);//������
		if(key == USER_4)
		{
			send_key_msg(USER4_VAL);
			while(USER_4 == PRESS)
			{
				OSTimeDly(60);
			}
		}
	}
//	else
//	if(WAKE_UP == PRESS)
//	{
//		key = PRESS ;
//		OSTimeDly(60);//������
//		if(key == WAKE_UP)
//		{
//			send_key_msg(WAKEUP_VAL);
//			while(WAKE_UP == PRESS)
//			{
//				OSTimeDly(60);
//			}
//		}
//	}
}





/*
��ϼ���ֵ��ֵ
*/
static uint8_t check_key_value(uint8_t key1, uint8_t key2)
{
	uint8_t key;

	#ifdef JY_MILK	//����ţ����ϰ�������
		if((key1 == '3' && key2 == '8') || ( key2 == '3' && key1 == '8'))//����ΪС����λ
		{
			key = '.';
		}
		else
		if((key1 == '2' && key2 == '6') || (key2 == '2' && key1 == '6'))//����Ϊ�·�
		{
			key = KEY_DOWN_VAL;
		}
		else
		if((key1 == '1' && key2 == '5') || (key2 == '1' && key1 == '5'))//�����Ϸ�
		{
			key = KEY_UP_VAL;
		}
		else
		if((key1 == '3' && key2 == '7') || (key2 == '3' && key1 == '7'))//�����Ϸ�
		{
			key = '9';
		}
		else
		if((key1 == '4' && key2 == '8') || (key2 == '4' && key1 == '8'))//�����Ϸ�
		{
			key = '0';
		}
	
	#else
		if((key1 == '8' && key2 == '6') || ( key2 == '8' && key1 == '6'))//����ΪС����λ
		{
			key = '.';
		}
		else
		if((key1 == '5' && key2 == '7') || (key2 == '5' && key1 == '7'))//����Ϊ�·�
		{
			key = KEY_DOWN_VAL;
		}
		else
		if((key1 == '2' && key2 == '4') || (key2 == '2' && key1 == '4'))//�����Ϸ�
		{
			key = KEY_UP_VAL;
		}
	
	#endif
	
	return key;
}

/*
���������ʵ���Է�Ϊ���ִ�����ʽ
�ͻ�����ʱΪ����죬�ǰ��´���
����Ա����ʱ��Ϊ��ʹ����ϼ����ǵ��𴥷�

˵������ϼ�Ŀǰ֧�ֲ�ͬ�в�ͬ�еĴ���
����������ͬ�в�ͬ�еĴ������Ƚϸ��ӣ���ʱ����

*/
void Key_Run(void)
{
	bool lock_key ,lock_group;	//��ֵ����
	bool groupFlag ;			//��ϼ�������־
	uint8_t key_row, key_line;	//��¼��ʱֵ
	uint8_t curRow, curLine ; 	//������¼���µļ�ֵ
	uint8_t curRow2,curLine2 ;	//��¼��ϼ���ֵ
	g_vm.door = 0xff;	//������ʼֵ
	
	key_Configuration();//��ʼ��
	
	key_line = curRow = curLine = curRow2 = curLine2 = 0xff;
	lock_key = lock_group = groupFlag = FALSE;
	for(key_row = 0;key_row < 4; key_row++ )
	{
		GPIO_WriteBit(GPIOF,pin[key_row],Bit_SET);//KET5~KEY8 ����
	}
	
	
	while(1)
	{
		if(g_vm.keyState)//���𴥷���ʽ��֧�ֶ๦�ܰ���
		{
			for(key_row = 0; key_row < 4; key_row++)
			{
				GPIO_WriteBit(GPIOF, pin[key_row],Bit_RESET);//KEY5~KEY8 ��������
				curRow = key_row; 	//��¼��ǰ���͵���
				OSTimeDly(10);		//�ȴ���ƽƽ��
				curLine = key_line = keyBoardHscan();	//��¼�а���
						
				if(key_line != 0xFF)			//�а��¶���
				{
					OSTimeDly(20);			//ȥ���������Ե��ڰ�����������
					key_line = keyBoardHscan();
					if(curLine == key_line )//�������ǰ�水�µļ�
					{
						lock_key = TRUE;
						while(lock_key)//��ס�����������
						{	
							if( keyBoardHscan() == 0xff)//�����ɿ���
							{
								if(!groupFlag)//���û�д�������ϼ�
								{
									FSMMSG* key=apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
									assert_param(key);
									if(key != 0)
									{
										key->type = MsgKEY;
										key->value = keyValue[curRow][curLine];
										BEEP(ON);
										OSTimeDly(OS_TICKS_PER_SEC/10);
										BEEP(OFF);
										if(!send_fsm_msg(key))
										release_fsm_msg(key);
									}
								}
								lock_key = FALSE;
								groupFlag = FALSE;
							}
							else//��һ�����µļ�û���ɿ�
							{
								//��ϼ���ƣ�����ɨ������������,������ϼ������ǲ�ͬ�в�ͬ��
								GPIO_WriteBit(GPIOF,pin[curRow],Bit_SET);//���ߵ�ǰ��
								OSTimeDly(10);//�ȴ���ƽƽ��

								for(key_row = 0; key_row < 4; key_row++)
								{
									if(key_row != curRow)
									{
										GPIO_WriteBit(GPIOF,pin[key_row],Bit_RESET);//KEY5~KEY8 ʣ��3����������
										curRow2 = key_row;
										OSTimeDly(10);	//�ȴ���ƽƽ��
										curLine2 = key_line = keyBoardHscan();	
										
										if(key_line != 0xff && curLine2 != curLine)			//�а��¶���
										{
											OSTimeDly(20);	//ȥ���������Ե��ڰ�����������
											
											key_line = keyBoardHscan();
											if(curLine2 == key_line )//�������ǰ�水�µļ�
											{
												//���ﴦ��ʽ�ǰ��¾ʹ����������ǵ����ٴ���
												FSMMSG* key=apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
												assert_param(key);
												if(key != 0)
												{
													key->type = MsgKEY;
													
													/*
													��ϼ�ֵ
													*/
													key->value = check_key_value(keyValue[curRow][curLine], keyValue[curRow2][curLine2]);
													
													BEEP(ON);
													OSTimeDly(OS_TICKS_PER_SEC/10);
													BEEP(OFF);
													if(!send_fsm_msg(key))
													release_fsm_msg(key);
													groupFlag = TRUE;//��������ϼ�
												}
												
												lock_group = TRUE;
												while(lock_group)//��ס�ڶ�����������
												{
													if( keyBoardHscan() == 0xff)//�����ɿ���
													{
														lock_group = FALSE;
													}
													else
													{
//														if(key_free > 50*120)
//															break;//�ػ�����ֹ����
														OSTimeDly(20);//�������ɨ���Щ
													}
												}
											}
										}
										GPIO_WriteBit(GPIOF,pin[curRow2],Bit_SET);
									}
								}
								GPIO_WriteBit(GPIOF,pin[curRow],Bit_RESET);//��ǰ�лظ����ͣ����ǻظ�ǰһ����ʱ״̬
								OSTimeDly(10);
							}
						}
					}
				}
				GPIO_WriteBit(GPIOF,pin[curRow],Bit_SET);//���͵�������
			}	
		}
		else//���´�����ʽ����֧�ֶ๦�ܰ�����
		{
			for(key_row = 0; key_row < 4; key_row++)
			{
				GPIO_WriteBit(GPIOF,pin[key_row],Bit_RESET);//KEY5~KEY8 ��������
				OSTimeDly(10);	//�ȴ���ƽƽ��
				curRow = key_row; 	//��¼��ǰ���͵���
				curLine = key_line = keyBoardHscan();	//��¼�а���
				if(key_line != 0xff)			//�а��¶���
				{
					OSTimeDly(20);				//ȥ���������Ե��ڰ�����������
					key_line = keyBoardHscan();
					if(curLine == key_line )//�������ǰ�水�µļ�
					{
						FSMMSG* key=apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
						assert_param(key);
						if(key != 0)
						{
							key->type = MsgKEY;
							key->value = keyValue[curRow][curLine];
							BEEP(ON);
							OSTimeDly(OS_TICKS_PER_SEC/10);
							BEEP(OFF);
							if(!send_fsm_msg(key))
							release_fsm_msg(key);
						}
						lock_key = TRUE;
						while(lock_key)//��ס����������������ſ�������ɨ��
						{
							if(keyBoardHscan() == 0xff)//��������
							{
								lock_key = FALSE;
							}
							else
							{
								OSTimeDly(20);
							}
						}
					}
				}
				GPIO_WriteBit(GPIOF,pin[key_row],Bit_SET);//����������
			}
		}
		
		/*
		��ѯ���尴��
		*/
		scan_board_key();
		
		/*
		������
		���������⵽��������Ҫ��¼����״̬
		*/
		if(g_vm.door != DOOR)//����״̬�ı� 
		{
			
			if(g_vm.door == 0xff)//����������Ϣ
			{
				g_vm.door = DOOR; 
			}
			else
			{
				uint8_t state;
				state = DOOR; //��¼��ǰ״̬
				OSTimeDly(60);//������
				if(state == DOOR)
				{
					FSMMSG* key=apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
					assert_param(key);
					g_vm.door = DOOR; 
					if(key != 0)
					{
						key->type = MsgKEY;
						if(g_vm.door == 1)//�ſ�
						key->value = DOOR_OPEN_VAL;
						else//�Ź�
						key->value = DOOR_CLOSE_VAL;
						
						BEEP(ON);
						OSTimeDly(OS_TICKS_PER_SEC/10);
						BEEP(OFF);
						/*
						����0��ʾ�أ�1��ʾ��
						*/
						report_state_change(STATE_DOOR, 1, g_vm.door);//�㱨��������״̬�ı�
						if(!send_fsm_msg(key))
						release_fsm_msg(key);
					}
				}
			}
		}
		
		OSTimeDly(10);
	}
}


