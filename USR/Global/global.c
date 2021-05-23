/*
全局要用的变量或方法
*/
#include "global.h"
#include "log.h "
#include "bsp_bkp.h"
#include "stdlib.h"

//临时用
uint8_t CA306;
volatile uint8_t coin_level;			
uint32_t sysfactor = 100;




//开机检测
uint8_t check_w_power_on = 0;



volatile CLASS_TRADE g_trade;
volatile CLASS_VM_SET g_vm;
volatile CLASS_FSM_STATUS g_fsm;


/*
输入金额
说明：先输入的是高位
参数：key：键值
与输入数字的区别是增加了一些条件检测
使用此方法前，需要先清除干净
g_trade.inputNum.len :输入位数，每次设置啥就读啥的位数
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
		
		if(g_trade.inputNum.state == 0)//第一位输入
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
					if(!(g_trade.inputNum.state == 0 || g_trade.inputNum.state == g_trade.inputNum.backup - 1))//首位与末尾不允许输入小数点
					{
						g_trade.inputNum.numchar[g_trade.inputNum.state++] = key;
						
						for(i = g_trade.inputNum.state, j = 0; i < g_trade.inputNum.backup; i++, j++)//小数点后占位符清除
						{
							g_trade.inputNum.numchar[i] = ' ';
						}
						
						g_trade.inputNum.backup = g_trade.inputNum.state + g_vm.point;//更新输入小数点后的输入总长度（根据小数点位置计算）
						
						if(g_trade.inputNum.backup > g_vm.priceBit + 1)//限制总长度，不能超过价格位数+1（因为多了一个小数点）
							g_trade.inputNum.backup = g_vm.priceBit + 1;
						
						for(i = g_trade.inputNum.state, j = 0; i < g_trade.inputNum.backup; i++, j++)//小数点后占位符显示
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
				
				//计算金额（金额与小数点位置有关，是一个计算值）
				for(i  = 0, j = 1; i < g_vm.point; i++ )//放大系数
				{
					j *= 10;
				}
				for(i = 0, num = 0; i < g_trade.inputNum.backup && g_trade.inputNum.numchar[i] != '.'; i++)//找到小数点位置
				{
					;
				}
				if(i < g_trade.inputNum.backup - 1)
				{
					i++;
					for(num = 0; i < g_trade.inputNum.backup; i++)//计算小数点后的位数
					{
						if(g_trade.inputNum.numchar[i] <= '9' && g_trade.inputNum.numchar[i] >= '0')
						{
							num++;
						}
					}
					for(i = 0, k = 1; i < num; i++)//缩小系数
					{
						k *= 10;
					}

				}
				else
				{
					k = 1;
				}
				
				for(i = 0, val = 0; i < g_trade.inputNum.backup; i++)//输入值（纯数值）
				{
					if(g_trade.inputNum.numchar[i] <= '9' && g_trade.inputNum.numchar[i] >= '0')
					val = val*10 + g_trade.inputNum.numchar[i] - '0';
					
				}
				g_trade.inputNum.value = val*(j/k);
			
			}
		}
		else//重头开始输入
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

				//计算金额（金额与小数点位置有关，是一个计算值）
				for(i  = 0, j = 1; i < g_vm.point; i++ )//放大系数
				{
					j *= 10;
				}
				for(i = 0, num = 0; i < g_trade.inputNum.backup && g_trade.inputNum.numchar[i] != '.'; i++)//找到小数点位置
				{
					;
				}
				if(i < g_trade.inputNum.backup - 1)
				{
					i++;
					for(num = 0; i < g_trade.inputNum.backup; i++)//计算小数点后的位数
					{
						if(g_trade.inputNum.numchar[i] <= '9' && g_trade.inputNum.numchar[i] >= '0')
						{
							num++;
						}
					}
					for(i = 0, k = 1; i < num; i++)//缩小系数
					{
						k *= 10;
					}

				}
				else
				{
					k = 1;
				}
				
				for(i = 0, val = 0; i < g_trade.inputNum.backup; i++)//输入值（纯数值）
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
清除输入
这个和清理屏幕一样很常用
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
设置输入长度
*/
void input_len_set(uint8_t len)
{
	g_trade.inputNum.lenn = len;
}
/*
读取输入长度
*/
uint8_t input_len_get(void) 
{
	return g_trade.inputNum.state ;
}

/*
返回输入字符，
*/
char* input_char_get(void)
{
	return (char*)g_trade.inputNum.numchar;
}
/*
返回输入数值
*/
uint32_t input_value_get(void)
{
	return g_trade.inputNum.value;
}


/*
输入时间
*/
uint8_t input_time(uint8_t key)
{
	uint8_t i;
	if( (key >= '0' && key <= '2' && (g_trade.inputNum.state == 0 || g_trade.inputNum.state == g_trade.inputNum.lenn))							//第1位
	|| ( (key >= '0' && key <= '3') &&  g_trade.inputNum.state == 1) || (g_trade.inputNum.numchar[0] != '2' && g_trade.inputNum.state == 1)		//第2位
	|| ( (key >= '0' && key <= '5') && g_trade.inputNum.state == 2)		//第3位
	|| ( (key >= '0' && key <= '9') && g_trade.inputNum.state == 4) )	//第4位
	{
		g_trade.inputNum.lenn = 5;//输入长度
		
		if(g_trade.inputNum.state == 0)//输入第一个字符
		{
			for(i = 0; i < g_trade.inputNum.lenn; i++)//占位符处理
			{
				if(i == 2)
					g_trade.inputNum.numchar[i] = ':';//第三位是个冒号
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
			for(i = 0; i < g_trade.inputNum.state; i++)//占位符处理
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
功能：输入日期
说明：对于大小月日输入没有限制，对于2月日输入没有限制，对于三月
*/
uint8_t input_data(uint8_t key)
{
	uint8_t i;
	
	if((key >= '1' && key < '3' && (g_trade.inputNum.state == 0 || g_trade.inputNum.state == g_trade.inputNum.lenn))//年第 1 位
	|| (key >= '0' && key <= '9' && g_trade.inputNum.state == 1 && g_trade.inputNum.numchar[0] == '2')//年第 2 位
	|| (key == '9' && g_trade.inputNum.state == 1 && g_trade.inputNum.numchar[0] == '1')//年第 2 位(只能输入19xx)
	|| (key >= '0' && key <= '9' && g_trade.inputNum.state == 2)//年第 3 位
	|| (key >= '0' && key <= '9' && g_trade.inputNum.state == 3)//年第 4 位
	|| (key >= '0' && key < '2' && g_trade.inputNum.state == 4)//月第 1 位
	|| (key > '0' && key <= '9' && g_trade.inputNum.state == 6 && g_trade.inputNum.numchar[5] == '0')//月第 2 位
	|| (key >= '0' && key < '3' && g_trade.inputNum.state == 6 && g_trade.inputNum.numchar[5] == '1')//月第 2 位
	|| (key >= '0' && key < '4' && g_trade.inputNum.state == 7)//日第 1 位
	|| (key > '0' && key <= '9' && g_trade.inputNum.state == 9 && g_trade.inputNum.numchar[8] == '0' )//日第 2 位
	|| (key >= '0' && key <= '9' && g_trade.inputNum.state == 9 && (g_trade.inputNum.numchar[8] == '1' || g_trade.inputNum.numchar[8] == '2'))//日第 2 位
	|| (key >= '0' && key < '2' && g_trade.inputNum.state == 9 && g_trade.inputNum.numchar[8] == '3')//日第 2 位
	)
	{
		g_trade.inputNum.lenn = 10;//输入长度
		
		if(g_trade.inputNum.state == 0)//输入第一个字符
		{
			for(i = 0; i < g_trade.inputNum.lenn; i++)//占位符初始化
			{
				if(i == 4 || i == 7)
				{
					g_trade.inputNum.numchar[i] = '/';//分隔符
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
			for(i = 0; i < g_trade.inputNum.state; i++)//占位符处理
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
输入数字
说明：先输入的是高位
参数：key：键值
这是一个方法，传入指针操作，通用型
使用此方法前，需要先清除干净
*/
void input_num(uint8_t key)
{
	if(key >= '0' && key <= '9')
	{
		if(g_trade.inputNum.lenn == 0)
		{
			g_trade.inputNum.lenn = 1;
		}
		
		if(g_trade.inputNum.state == 0)//第一次输入
		{
			uint8_t i;
			for(i = 0; i < g_trade.inputNum.lenn; i++)
			g_trade.inputNum.numchar[i] = '-';//占位符
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
按键编码值转换为内部货道编码
*/
static void transfer_selsec_encode(uint16_t channel) 
{
	if(g_vm.channelBit == 1)//1位货道编码（只能表示托盘）
	{
		//这里还要加上轮询的货道号
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
	else//异常情况也按照2位货道来处理
//	if(g_vm.channelBit == 2)
	{
		g_trade.curDiverBoard.container = 0;
		g_trade.curDiverBoard.tray = (channel - 10) / 10;
		g_trade.curDiverBoard.col = (channel - 10) % 10;
	}
} 

/*
选择货道
参数：key ：键值
这个因为要显示*号，固定长度，所有是独有型
*/
void select_goods(uint8_t key)
{
	uint8_t i;
	if(g_trade.select.state == 0 || g_trade.select.state == g_vm.channelBit )//第一次按键或已经按到限定长度
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
				g_trade.select.value = atoi((const char*)g_trade.select.select);//字符串转为int型10进制的值
				transfer_selsec_encode(g_trade.select.value);
			}
		}
	}
	else//按键中间值
	{
		if(key >= '0' && key <= '9')
		{
			g_trade.select.select[g_trade.select.state] = key;
			g_trade.select.state++;
			if(g_trade.select.state == g_vm.channelBit)//输入完成
			{
				g_trade.select.value = atoi((const char*)g_trade.select.select);
				transfer_selsec_encode(g_trade.select.value);
			}
			
		}
	}
}
/*
返回货道选择状态
*/
bool select_state_get(void)
{
	if(g_trade.select.state == g_vm.channelBit)
		return TRUE;
	else
		return FALSE;
}

/*
返回货道字符串指针
*/
char *select_char_get(void)
{
	return (char*)g_trade.select.select;
}

/*
清除货道选择
*/
void select_clear(void)
{
	uint8_t i;
	g_trade.select.state = 0;
	g_trade.select.value = 0;
	for(i = 0; i < 11; i++)
	g_trade.select.select[i] = ' ';//这个地方可以改成*，根据channelBit
	
	g_trade.curDiverBoard.container = 0;
	g_trade.curDiverBoard.tray = 0;
	g_trade.curDiverBoard.col = 0;
}

/*
返回货道值，就是按键的10进制表示
*/
uint16_t select_value_get(void)
{
	return g_trade.select.value;
}

/*
状态机数据清零
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
实现断言
从串口1输出
*/ 
void assert_failed(uint8_t* file, uint32_t line)
{
	log_send(file, line);
}

/*
实现用户日志输出
从串口1输出
*/ 
void log_fail(uint8_t* file, uint32_t line)
{
	log_send(file, line);
}




