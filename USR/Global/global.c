/*
ȫ��Ҫ�õı����򷽷�
*/
#include "global.h"
#include "log.h "
#include "bsp_bkp.h"
#include "stdlib.h"

//��ʱ��
uint8_t CA306;
volatile uint8_t coin_level;			
uint32_t sysfactor = 100;




//�������
uint8_t check_w_power_on = 0;



volatile CLASS_TRADE g_trade;
volatile CLASS_VM_SET g_vm;
volatile CLASS_FSM_STATUS g_fsm;


/*
������
˵������������Ǹ�λ
������key����ֵ
���������ֵ�������������һЩ�������
ʹ�ô˷���ǰ����Ҫ������ɾ�
g_trade.inputNum.len :����λ����ÿ������ɶ�Ͷ�ɶ��λ��
*/
void input_money(uint8_t key)
{
	
	uint8_t i;
	uint32_t j,num,k,val;
	
	if(g_trade.inputNum.backup == 0)
		g_trade.inputNum.backup = g_vm.priceBit;
	
	
	if((key >= '0' && key <= '9') || key == '.')
	{
		if(g_trade.inputNum.backup == 0)g_trade.inputNum.backup = 1;
		
		if(g_trade.inputNum.state == 0)//��һλ����
		{
			for(i = 0; i < g_trade.inputNum.backup; i++)
				g_trade.inputNum.numchar[i] = '-';
			
		}
			
		if(g_trade.inputNum.state < g_trade.inputNum.backup)
		{
			if(key == '.')
			{
				if(g_trade.inputNum.flag == FALSE)
				{
					if(!(g_trade.inputNum.state == 0 || g_trade.inputNum.state == g_trade.inputNum.backup - 1))//��λ��ĩβ����������С����
					{
						g_trade.inputNum.numchar[g_trade.inputNum.state++] = key;
						
						for(i = g_trade.inputNum.state, j = 0; i < g_trade.inputNum.backup; i++, j++)//С�����ռλ�����
						{
							g_trade.inputNum.numchar[i] = ' ';
						}
						
						g_trade.inputNum.backup = g_trade.inputNum.state + g_vm.point;//��������С�����������ܳ��ȣ�����С����λ�ü��㣩
						
						if(g_trade.inputNum.backup > g_vm.priceBit + 1)//�����ܳ��ȣ����ܳ����۸�λ��+1����Ϊ����һ��С���㣩
							g_trade.inputNum.backup = g_vm.priceBit + 1;
						
						for(i = g_trade.inputNum.state, j = 0; i < g_trade.inputNum.backup; i++, j++)//С�����ռλ����ʾ
						{
							g_trade.inputNum.numchar[i] = '-';
						}
						
						g_trade.inputNum.flag = TRUE;
					}
				}
			}
			else
			{
				g_trade.inputNum.numchar[g_trade.inputNum.state++] = key;
				
				//����������С����λ���йأ���һ������ֵ��
				for(i  = 0, j = 1; i < g_vm.point; i++ )//�Ŵ�ϵ��
				{
					j *= 10;
				}
				for(i = 0, num = 0; i < g_trade.inputNum.backup && g_trade.inputNum.numchar[i] != '.'; i++)//�ҵ�С����λ��
				{
					;
				}
				if(i < g_trade.inputNum.backup - 1)
				{
					i++;
					for(num = 0; i < g_trade.inputNum.backup; i++)//����С������λ��
					{
						if(g_trade.inputNum.numchar[i] <= '9' && g_trade.inputNum.numchar[i] >= '0')
						{
							num++;
						}
					}
					for(i = 0, k = 1; i < num; i++)//��Сϵ��
					{
						k *= 10;
					}

				}
				else
				{
					k = 1;
				}
				
				for(i = 0, val = 0; i < g_trade.inputNum.backup; i++)//����ֵ������ֵ��
				{
					if(g_trade.inputNum.numchar[i] <= '9' && g_trade.inputNum.numchar[i] >= '0')
					val = val*10 + g_trade.inputNum.numchar[i] - '0';
					
				}
				g_trade.inputNum.value = val*(j/k);
			
			}
		}
		else//��ͷ��ʼ����
		{
			if(key >= '0' && key <= '9')
			{
				g_trade.inputNum.backup = g_vm.priceBit ;//= g_trade.inputNum.backup;
				if(g_trade.inputNum.backup == 0)
					g_trade.inputNum.backup = 1;
				
				for(i = 0; i < (g_vm.priceBit + 1); i++)
				{
					g_trade.inputNum.numchar[i] = ' ';
				}
				
				for(i = 0; i < g_trade.inputNum.backup; i++)
				{
					g_trade.inputNum.numchar[i] = '-';
				}
				
				g_trade.inputNum.state = 0;
				g_trade.inputNum.value = 0;
				g_trade.inputNum.flag = FALSE;
				g_trade.inputNum.numchar[g_trade.inputNum.state++] = key;

				//����������С����λ���йأ���һ������ֵ��
				for(i  = 0, j = 1; i < g_vm.point; i++ )//�Ŵ�ϵ��
				{
					j *= 10;
				}
				for(i = 0, num = 0; i < g_trade.inputNum.backup && g_trade.inputNum.numchar[i] != '.'; i++)//�ҵ�С����λ��
				{
					;
				}
				if(i < g_trade.inputNum.backup - 1)
				{
					i++;
					for(num = 0; i < g_trade.inputNum.backup; i++)//����С������λ��
					{
						if(g_trade.inputNum.numchar[i] <= '9' && g_trade.inputNum.numchar[i] >= '0')
						{
							num++;
						}
					}
					for(i = 0, k = 1; i < num; i++)//��Сϵ��
					{
						k *= 10;
					}

				}
				else
				{
					k = 1;
				}
				
				for(i = 0, val = 0; i < g_trade.inputNum.backup; i++)//����ֵ������ֵ��
				{
					if(g_trade.inputNum.numchar[i] <= '9' && g_trade.inputNum.numchar[i] >= '0')
					val = val*10 + g_trade.inputNum.numchar[i] - '0';
					
				}
				g_trade.inputNum.value = val*(j/k);
			}
		}
	}
}

/*
�������
�����������Ļһ���ܳ���
*/
void input_clear(void)
{
	uint8_t i;
	for(i = 0; i < 12; i++)
		g_trade.inputNum.numchar[i] = ' ';
	g_trade.inputNum.value = 0;
	g_trade.inputNum.state = 0;
	g_trade.inputNum.flag = FALSE;
	g_trade.inputNum.lenn = 0;
	g_trade.inputNum.backup = 0;
}
/*
�������볤��
*/
void input_len_set(uint8_t len)
{
	g_trade.inputNum.lenn = len;
}
/*
��ȡ���볤��
*/
uint8_t input_len_get(void) 
{
	return g_trade.inputNum.state ;
}

/*
���������ַ���
*/
char* input_char_get(void)
{
	return (char*)g_trade.inputNum.numchar;
}
/*
����������ֵ
*/
uint32_t input_value_get(void)
{
	return g_trade.inputNum.value;
}


/*
����ʱ��
*/
uint8_t input_time(uint8_t key)
{
	uint8_t i;
	if( (key >= '0' && key <= '2' && (g_trade.inputNum.state == 0 || g_trade.inputNum.state == g_trade.inputNum.lenn))							//��1λ
	|| ( (key >= '0' && key <= '3') &&  g_trade.inputNum.state == 1) || (g_trade.inputNum.numchar[0] != '2' && g_trade.inputNum.state == 1)		//��2λ
	|| ( (key >= '0' && key <= '5') && g_trade.inputNum.state == 2)		//��3λ
	|| ( (key >= '0' && key <= '9') && g_trade.inputNum.state == 4) )	//��4λ
	{
		g_trade.inputNum.lenn = 5;//���볤��
		
		if(g_trade.inputNum.state == 0)//�����һ���ַ�
		{
			for(i = 0; i < g_trade.inputNum.lenn; i++)//ռλ������
			{
				if(i == 2)
					g_trade.inputNum.numchar[i] = ':';//����λ�Ǹ�ð��
				else
					g_trade.inputNum.numchar[i] = '-';
			}
		}
		
		if(g_trade.inputNum.state < g_trade.inputNum.lenn)
		{
			if(g_trade.inputNum.state == 2)
			{
				g_trade.inputNum.state++;
			}
			g_trade.inputNum.numchar[g_trade.inputNum.state++] = key;
			g_trade.inputNum.value = g_trade.inputNum.value*10 + key - '0';
		}
		else
		{
			for(i = 0; i < g_trade.inputNum.state; i++)//ռλ������
			{
				if(i == 2)
					g_trade.inputNum.numchar[i] = ':';
				else
					g_trade.inputNum.numchar[i] = '-';
			}
			g_trade.inputNum.state = 0;
			g_trade.inputNum.value = 0;
			g_trade.inputNum.value = g_trade.inputNum.value*10 + key - '0';
			g_trade.inputNum.numchar[g_trade.inputNum.state++] = key;
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*
���ܣ���������
˵�������ڴ�С��������û�����ƣ�����2��������û�����ƣ���������
*/
uint8_t input_data(uint8_t key)
{
	uint8_t i;
	
	if((key >= '1' && key < '3' && (g_trade.inputNum.state == 0 || g_trade.inputNum.state == g_trade.inputNum.lenn))//��� 1 λ
	|| (key >= '0' && key <= '9' && g_trade.inputNum.state == 1 && g_trade.inputNum.numchar[0] == '2')//��� 2 λ
	|| (key == '9' && g_trade.inputNum.state == 1 && g_trade.inputNum.numchar[0] == '1')//��� 2 λ(ֻ������19xx)
	|| (key >= '0' && key <= '9' && g_trade.inputNum.state == 2)//��� 3 λ
	|| (key >= '0' && key <= '9' && g_trade.inputNum.state == 3)//��� 4 λ
	|| (key >= '0' && key < '2' && g_trade.inputNum.state == 4)//�µ� 1 λ
	|| (key > '0' && key <= '9' && g_trade.inputNum.state == 6 && g_trade.inputNum.numchar[5] == '0')//�µ� 2 λ
	|| (key >= '0' && key < '3' && g_trade.inputNum.state == 6 && g_trade.inputNum.numchar[5] == '1')//�µ� 2 λ
	|| (key >= '0' && key < '4' && g_trade.inputNum.state == 7)//�յ� 1 λ
	|| (key > '0' && key <= '9' && g_trade.inputNum.state == 9 && g_trade.inputNum.numchar[8] == '0' )//�յ� 2 λ
	|| (key >= '0' && key <= '9' && g_trade.inputNum.state == 9 && (g_trade.inputNum.numchar[8] == '1' || g_trade.inputNum.numchar[8] == '2'))//�յ� 2 λ
	|| (key >= '0' && key < '2' && g_trade.inputNum.state == 9 && g_trade.inputNum.numchar[8] == '3')//�յ� 2 λ
	)
	{
		g_trade.inputNum.lenn = 10;//���볤��
		
		if(g_trade.inputNum.state == 0)//�����һ���ַ�
		{
			for(i = 0; i < g_trade.inputNum.lenn; i++)//ռλ����ʼ��
			{
				if(i == 4 || i == 7)
				{
					g_trade.inputNum.numchar[i] = '/';//�ָ���
				}
				else
				{
					g_trade.inputNum.numchar[i] = '-';
				}
			}
		}
		
		if(g_trade.inputNum.state < g_trade.inputNum.lenn)
		{
			if(g_trade.inputNum.state == 4 || g_trade.inputNum.state == 7)
			{
				g_trade.inputNum.state++;
			}
			g_trade.inputNum.numchar[g_trade.inputNum.state++] = key;
			g_trade.inputNum.value = g_trade.inputNum.value*10 + key - '0';
		}
		else
		{
			for(i = 0; i < g_trade.inputNum.state; i++)//ռλ������
			{
				if(i == 4 || i == 7)
				{
					g_trade.inputNum.numchar[i] = '/';
				}
				else
				{
					g_trade.inputNum.numchar[i] = '-';
				}
			}
			g_trade.inputNum.state = 0;
			g_trade.inputNum.value = 0;
			g_trade.inputNum.value = g_trade.inputNum.value*10 + key - '0';
			g_trade.inputNum.numchar[g_trade.inputNum.state++] = key;
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
/*
��������
˵������������Ǹ�λ
������key����ֵ
����һ������������ָ�������ͨ����
ʹ�ô˷���ǰ����Ҫ������ɾ�
*/
void input_num(uint8_t key)
{
	if(key >= '0' && key <= '9')
	{
		if(g_trade.inputNum.lenn == 0)
		{
			g_trade.inputNum.lenn = 1;
		}
		
		if(g_trade.inputNum.state == 0)//��һ������
		{
			uint8_t i;
			for(i = 0; i < g_trade.inputNum.lenn; i++)
			g_trade.inputNum.numchar[i] = '-';//ռλ��
		}
		
		if(g_trade.inputNum.state < g_trade.inputNum.lenn)
		{
			g_trade.inputNum.numchar[g_trade.inputNum.state++] = key;
			g_trade.inputNum.value = g_trade.inputNum.value*10 + key - '0';
		}
		else
		{
			uint8_t i;
			for(i = 0; i < g_trade.inputNum.state; i++)
				g_trade.inputNum.numchar[i] = '-';

			g_trade.inputNum.state = 0;
			g_trade.inputNum.value = 0;
			g_trade.inputNum.value = g_trade.inputNum.value*10 + key - '0';
			g_trade.inputNum.numchar[g_trade.inputNum.state++] = key;
		}
	}
}


/*
��������ֵת��Ϊ�ڲ���������
*/
static void transfer_selsec_encode(uint16_t channel) 
{
	if(g_vm.channelBit == 1)//1λ�������루ֻ�ܱ�ʾ���̣�
	{
		//���ﻹҪ������ѯ�Ļ�����
		g_trade.curDiverBoard.container = 0;
		g_trade.curDiverBoard.tray = channel - 1;
		g_trade.curDiverBoard.col = loop_channel_get(g_trade.curDiverBoard.tray);
	}
	else
	if(g_vm.channelBit == 3)
	{
		g_trade.curDiverBoard.container = (channel-110) / 100; 
		g_trade.curDiverBoard.tray = (channel - 110) %100 / 10;
		g_trade.curDiverBoard.col = (channel - 110) %100 % 10;
	}
	else//�쳣���Ҳ����2λ����������
//	if(g_vm.channelBit == 2)
	{
		g_trade.curDiverBoard.container = 0;
		g_trade.curDiverBoard.tray = (channel - 10) / 10;
		g_trade.curDiverBoard.col = (channel - 10) % 10;
	}
} 

/*
ѡ�����
������key ����ֵ
�����ΪҪ��ʾ*�ţ��̶����ȣ������Ƕ�����
*/
void select_goods(uint8_t key)
{
	uint8_t i;
	if(g_trade.select.state == 0 || g_trade.select.state == g_vm.channelBit )//��һ�ΰ������Ѿ������޶�����
	{
		if(key > '0' && key <= ('0' + g_vm.maxtrayNum))
		{
			g_trade.select.state = 0;
			g_trade.select.select[g_trade.select.state++] = key;
			for(i = 1; i < g_vm.channelBit; i++)
			{
				g_trade.select.select[i] = '*';
			}
			g_trade.select.select[i] = '\0';
			
			if(g_vm.channelBit == g_trade.select.state)
			{
				g_trade.select.value = atoi((const char*)g_trade.select.select);//�ַ���תΪint��10���Ƶ�ֵ
				transfer_selsec_encode(g_trade.select.value);
			}
		}
	}
	else//�����м�ֵ
	{
		if(key >= '0' && key <= '9')
		{
			g_trade.select.select[g_trade.select.state] = key;
			g_trade.select.state++;
			if(g_trade.select.state == g_vm.channelBit)//�������
			{
				g_trade.select.value = atoi((const char*)g_trade.select.select);
				transfer_selsec_encode(g_trade.select.value);
			}
			
		}
	}
}
/*
���ػ���ѡ��״̬
*/
bool select_state_get(void)
{
	if(g_trade.select.state == g_vm.channelBit)
		return TRUE;
	else
		return FALSE;
}

/*
���ػ����ַ���ָ��
*/
char *select_char_get(void)
{
	return (char*)g_trade.select.select;
}

/*
�������ѡ��
*/
void select_clear(void)
{
	uint8_t i;
	g_trade.select.state = 0;
	g_trade.select.value = 0;
	for(i = 0; i < 11; i++)
	g_trade.select.select[i] = ' ';//����ط����Ըĳ�*������channelBit
	
	g_trade.curDiverBoard.container = 0;
	g_trade.curDiverBoard.tray = 0;
	g_trade.curDiverBoard.col = 0;
}

/*
���ػ���ֵ�����ǰ�����10���Ʊ�ʾ
*/
uint16_t select_value_get(void)
{
	return g_trade.select.value;
}

/*
״̬����������
*/
void fsm_value_clear(void)
{
	g_fsm.count = 0;
	g_fsm.mode = 0;
	g_fsm.num = 0;
	g_fsm.shortcut = 0;
	g_fsm.status = 0;
	
}

/*
ʵ�ֶ���
�Ӵ���1���
*/ 
void assert_failed(uint8_t* file, uint32_t line)
{
	log_send(file, line);
}

/*
ʵ���û���־���
�Ӵ���1���
*/ 
void log_fail(uint8_t* file, uint32_t line)
{
	log_send(file, line);
}




