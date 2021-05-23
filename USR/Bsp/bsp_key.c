/*
键盘驱动

*/



/*
约定规则：
KEY1~KEY4 键盘列，3.3v供电
key4 第一列
key3 第二列
key2 第三列
key1 第四列

KEY5~KEY8 键盘行
key5 第一行
key6 第二行
key7 第三行
key8 第4行
*/
//12键键盘
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
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;//这里改成输出，防止串扰
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
	
	//键盘输出脚 KEY5,KEY6,KEY7,KEY8.
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF,&GPIO_InitStructure);
  
	//键盘输出脚 KEY1,KEY2,KEY3,KEY4
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
}


/*
列扫描
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
主板按键消息发送
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
主板按键扫描
*/
static void scan_board_key(void)
{
	uint8_t key =  0xff;
	
	if(USER_1 == PRESS)
	{
		key = PRESS ;
		OSTimeDly(60);//防抖动
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
		OSTimeDly(60);//防抖动
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
		OSTimeDly(60);//防抖动
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
		OSTimeDly(60);//防抖动
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
//		OSTimeDly(60);//防抖动
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
组合键键值赋值
*/
static uint8_t check_key_value(uint8_t key1, uint8_t key2)
{
	uint8_t key;

	#ifdef JY_MILK	//均亚牛奶组合按键定义
		if((key1 == '3' && key2 == '8') || ( key2 == '3' && key1 == '8'))//定义为小数点位
		{
			key = '.';
		}
		else
		if((key1 == '2' && key2 == '6') || (key2 == '2' && key1 == '6'))//定义为下翻
		{
			key = KEY_DOWN_VAL;
		}
		else
		if((key1 == '1' && key2 == '5') || (key2 == '1' && key1 == '5'))//定义上翻
		{
			key = KEY_UP_VAL;
		}
		else
		if((key1 == '3' && key2 == '7') || (key2 == '3' && key1 == '7'))//定义上翻
		{
			key = '9';
		}
		else
		if((key1 == '4' && key2 == '8') || (key2 == '4' && key1 == '8'))//定义上翻
		{
			key = '0';
		}
	
	#else
		if((key1 == '8' && key2 == '6') || ( key2 == '8' && key1 == '6'))//定义为小数点位
		{
			key = '.';
		}
		else
		if((key1 == '5' && key2 == '7') || (key2 == '5' && key1 == '7'))//定义为下翻
		{
			key = KEY_DOWN_VAL;
		}
		else
		if((key1 == '2' && key2 == '4') || (key2 == '2' && key1 == '4'))//定义上翻
		{
			key = KEY_UP_VAL;
		}
	
	#endif
	
	return key;
}

/*
这个任务其实可以分为两种触发方式
客户购买时为了体检，是按下触发
管理员设置时，为了使用组合键，是弹起触发

说明：组合键目前支持不同行不同列的触发
最多可以做成同行不同列的触发，比较复杂，暂时不做

*/
void Key_Run(void)
{
	bool lock_key ,lock_group;	//键值锁定
	bool groupFlag ;			//组合键触发标志
	uint8_t key_row, key_line;	//记录临时值
	uint8_t curRow, curLine ; 	//用来记录按下的键值
	uint8_t curRow2,curLine2 ;	//记录组合键键值
	g_vm.door = 0xff;	//门碰初始值
	
	key_Configuration();//初始化
	
	key_line = curRow = curLine = curRow2 = curLine2 = 0xff;
	lock_key = lock_group = groupFlag = FALSE;
	for(key_row = 0;key_row < 4; key_row++ )
	{
		GPIO_WriteBit(GPIOF,pin[key_row],Bit_SET);//KET5~KEY8 拉高
	}
	
	
	while(1)
	{
		if(g_vm.keyState)//弹起触发方式，支持多功能按键
		{
			for(key_row = 0; key_row < 4; key_row++)
			{
				GPIO_WriteBit(GPIOF, pin[key_row],Bit_RESET);//KEY5~KEY8 逐行拉低
				curRow = key_row; 	//记录当前拉低的行
				OSTimeDly(10);		//等待电平平稳
				curLine = key_line = keyBoardHscan();	//记录列按键
						
				if(key_line != 0xFF)			//有按下动作
				{
					OSTimeDly(20);			//去抖动，可以调节按键的灵敏度
					key_line = keyBoardHscan();
					if(curLine == key_line )//如果还是前面按下的键
					{
						lock_key = TRUE;
						while(lock_key)//锁住这个按键动作
						{	
							if( keyBoardHscan() == 0xff)//按键松开了
							{
								if(!groupFlag)//如果没有触发了组合键
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
							else//第一个按下的键没有松开
							{
								//组合键设计：继续扫描其他的三行,所以组合键必须是不同行不同列
								GPIO_WriteBit(GPIOF,pin[curRow],Bit_SET);//拉高当前行
								OSTimeDly(10);//等待电平平稳

								for(key_row = 0; key_row < 4; key_row++)
								{
									if(key_row != curRow)
									{
										GPIO_WriteBit(GPIOF,pin[key_row],Bit_RESET);//KEY5~KEY8 剩下3行逐行拉低
										curRow2 = key_row;
										OSTimeDly(10);	//等待电平平稳
										curLine2 = key_line = keyBoardHscan();	
										
										if(key_line != 0xff && curLine2 != curLine)			//有按下动作
										{
											OSTimeDly(20);	//去抖动，可以调节按键的灵敏度
											
											key_line = keyBoardHscan();
											if(curLine2 == key_line )//如果还是前面按下的键
											{
												//这里处理方式是按下就触发，而不是弹起再触发
												FSMMSG* key=apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
												assert_param(key);
												if(key != 0)
												{
													key->type = MsgKEY;
													
													/*
													组合键值
													*/
													key->value = check_key_value(keyValue[curRow][curLine], keyValue[curRow2][curLine2]);
													
													BEEP(ON);
													OSTimeDly(OS_TICKS_PER_SEC/10);
													BEEP(OFF);
													if(!send_fsm_msg(key))
													release_fsm_msg(key);
													groupFlag = TRUE;//触发了组合键
												}
												
												lock_group = TRUE;
												while(lock_group)//锁住第二个按键动作
												{
													if( keyBoardHscan() == 0xff)//按键松开了
													{
														lock_group = FALSE;
													}
													else
													{
//														if(key_free > 50*120)
//															break;//守护，防止死锁
														OSTimeDly(20);//这里可以扫描快些
													}
												}
											}
										}
										GPIO_WriteBit(GPIOF,pin[curRow2],Bit_SET);
									}
								}
								GPIO_WriteBit(GPIOF,pin[curRow],Bit_RESET);//当前行回复拉低，就是回复前一个键时状态
								OSTimeDly(10);
							}
						}
					}
				}
				GPIO_WriteBit(GPIOF,pin[curRow],Bit_SET);//拉低的行拉高
			}	
		}
		else//按下触发方式（不支持多功能按键）
		{
			for(key_row = 0; key_row < 4; key_row++)
			{
				GPIO_WriteBit(GPIOF,pin[key_row],Bit_RESET);//KEY5~KEY8 逐行拉低
				OSTimeDly(10);	//等待电平平稳
				curRow = key_row; 	//记录当前拉低的行
				curLine = key_line = keyBoardHscan();	//记录列按键
				if(key_line != 0xff)			//有按下动作
				{
					OSTimeDly(20);				//去抖动，可以调节按键的灵敏度
					key_line = keyBoardHscan();
					if(curLine == key_line )//如果还是前面按下的键
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
						while(lock_key)//锁住这个按键动作，不放开不重新扫描
						{
							if(keyBoardHscan() == 0xff)//按键弹起
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
				GPIO_WriteBit(GPIOF,pin[key_row],Bit_SET);//检测的行拉高
			}
		}
		
		/*
		查询主板按键
		*/
		scan_board_key();
		
		/*
		查门碰
		门碰必须检测到，所以需要记录门碰状态
		*/
		if(g_vm.door != DOOR)//门碰状态改变 
		{
			
			if(g_vm.door == 0xff)//开机不发消息
			{
				g_vm.door = DOOR; 
			}
			else
			{
				uint8_t state;
				state = DOOR; //记录当前状态
				OSTimeDly(60);//防抖动
				if(state == DOOR)
				{
					FSMMSG* key=apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
					assert_param(key);
					g_vm.door = DOOR; 
					if(key != 0)
					{
						key->type = MsgKEY;
						if(g_vm.door == 1)//门开
						key->value = DOOR_OPEN_VAL;
						else//门关
						key->value = DOOR_CLOSE_VAL;
						
						BEEP(ON);
						OSTimeDly(OS_TICKS_PER_SEC/10);
						BEEP(OFF);
						/*
						门碰0表示关，1表示开
						*/
						report_state_change(STATE_DOOR, 1, g_vm.door);//汇报主柜门碰状态改变
						if(!send_fsm_msg(key))
						release_fsm_msg(key);
					}
				}
			}
		}
		
		OSTimeDly(10);
	}
}


