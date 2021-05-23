/*
商品设置
*/

#include "header.h"

#define HOLDER_SIZE	4						//此菜单内容数量（添加新的菜单项要改变这个大小）


/*
定义一个结构体,是内容的集合
这样定义是为了内存的连贯性
只有内存是连续的，调用 holder容器方法才可行
（因为holder本身是依靠指针来定位content的，如果内存不连续，则指针指向未知地方）
*/
typedef struct AGGREGATE{
	CONTENT content[HOLDER_SIZE];//->>>>>>>>>添加新内容要注意这个数组的大小
}AGGREGATE;


/*
定义要用到的数据结构，都用指针，
这些指针使用的时候需要分配内存，
所以注意空间的申请与释放
*/
static AGGREGATE *pt_content;//指向内容的集合
static SETUP_CHANNEL *pt_channelSet;



/*
读取要显示的内容
*/
static void content_value_read(void)
{
	//读取货道价格，库存，商品id
	uint8_t i;
	
	if(g_vm.channelBit == 1) g_trade.curDiverBoard.col = 0;//如果是托盘出货则用每层0货道存储托盘库存
	
	e2p_get_channel((g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col), pt_channelSet);
	
	for(i = 0; i < holder.size; i++)
	{
		if(holder.contentList[i].inputContent == CONTENT_PRICE)//读价格
		{
			transfer_value_to_money(pt_channelSet->price, pt_content->content[i].val);//整体内容拷贝
			if(i < holder.shownumPage)
			{
				//先把前面显示的刷掉(这里就是相当于刷了11个字符的空格，所以内容要同一行的间距超过11个字符位)
				lcd_show_string(input_char_get() , pt_content->content[i].valueX, pt_content->content[i].valueY, holder.numcolor, holder.background, *(holder.font));
				//显示新值
				lcd_show_money(pt_channelSet->price , pt_content->content[i].valueX, pt_content->content[i].valueY, holder.foreground, holder.background, *(holder.font));
			}
		}
		else
		if(holder.contentList[i].inputContent == CONTENT_STORE)//读库存
		{
			transfer_value_to_char(pt_channelSet->store, pt_content->content[i].val);//整体内容拷贝
			if(i < holder.shownumPage)
			{
				//刷新此行显示
				lcd_show_string(input_char_get() , pt_content->content[i].valueX, pt_content->content[i].valueY, holder.foreground, holder.background, *(holder.font));
				//显示库存(按层卖时显示的设置库存)
				lcd_show_num(pt_channelSet->store, pt_content->content[i].valueX, pt_content->content[i].valueY, holder.foreground, holder.background, *(holder.font) ,0 ,10);
			
				if(g_vm.channelBit == 1)
				{
					uint8_t num;
					uint16_t val;
					//显示可卖库存
					lcd_show_num(bkp_trayStore_get(g_trade.curDiverBoard.tray), pt_content->content[i].valueX + 4*8, pt_content->content[i].valueY, holder.foreground, holder.background, *(holder.font) ,0 ,10);
					//显示实际库存
					bkp_trayStatistic_get(g_trade.curDiverBoard.tray, &num, &val);
					lcd_show_num(pt_channelSet->store - num, pt_content->content[i].valueX + 8*8, pt_content->content[i].valueY, holder.foreground, holder.background, *(holder.font) ,0 ,10);
				}
			
			}
		}
		else
		if(holder.contentList[i].inputContent == CONTENT_GOODSID)//读条码
		{
			char tmp[11];
			sprintf(tmp, "%06u", pt_channelSet->code);//固定6位长度
			strcpy(pt_content->content[i].val, tmp);
			if(i < holder.shownumPage)//如果不在一个屏幕内，则不显示此项
			{
				//刷新此行显示
				lcd_show_string(input_char_get() , pt_content->content[i].valueX, pt_content->content[i].valueY, holder.foreground, holder.background, *(holder.font));
				//显示新值
				lcd_show_string(pt_content->content[i].val, pt_content->content[i].valueX, pt_content->content[i].valueY, holder.foreground, holder.background, *(holder.font));
			}
		}
	}
	
}


/*
设置输入长度
*/
static void content_input_set(void)
{
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_CHANNEL)//货道输入
	{
		input_len_set(g_vm.channelBit);//输入长度
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_PRICE)//价格输入
	{
		input_len_set(g_vm.priceBit);//输入长度
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_GOODSID)//商品条码输入
	{
		input_len_set(g_vm.goodsIdBit);//输入长度
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_STORE)//库存输入
	{
		input_len_set(g_vm.storeBit);//输入长度
	}
	
}

/*
刷新当前选项
这个一般只用在不定长输入时
*/
static void cur_item_fls(void)
{
	content_item_fls(&holder);
	input_clear();
	content_input_set();
}

/*
输入保存
*/
static void content_input_save(void)
{
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_CHANNEL)//货道输入
	{

	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_PRICE)//价格输入
	{
		if(input_len_get() != 0 && input_value_get() != 0)//价格不许为0
		{
			//判断是否数据不同
			if(input_value_get() != pt_channelSet->price)
			{
				pt_channelSet->price = input_value_get();
				transfer_value_to_money(input_value_get() , pt_content->content[holder.curSelectId -1].val);
				if(g_vm.channelBit == 1) g_trade.curDiverBoard.col = 0;
				
				//计算价格和校验
				pt_channelSet->priceCHK = ((pt_channelSet->price >> 24)&0xff) + ((pt_channelSet->price >> 16)&0xff) + ((pt_channelSet->price >> 8)&0xff) + (pt_channelSet->price & 0xff);
				//保存价格
				e2p_set_price((g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col),  &pt_channelSet->price);
				//汇报后台
				report_channel_set(C_CHANNEL_PRICE, pt_channelSet->price);
			}
			//刷新当前项
			cur_item_fls();
			
			lcd_show_character(&pt_word.warring[0]);//设置成功
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//设置失败
		}
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_GOODSID)//商品条码输入
	{
		if(input_len_get() == g_vm.goodsIdBit)//商品条码长度符合
		{
			//然后判断是否数据不同
			if(input_value_get() != pt_channelSet->code)
			{
				input_char_get()[input_len_get()] = 0;
				strcpy(pt_content->content[holder.curSelectId -1].val, input_char_get());
				pt_channelSet->code = input_value_get();
				if(g_vm.channelBit == 1) g_trade.curDiverBoard.col = 0;
				//保存商品编号
				e2p_set_channelCode((g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col), &pt_channelSet->code);
				//汇报后台
				report_channel_set(C_CHANNEL_GOODSID, pt_channelSet->code);
			}
//			//刷新当前项
//			cur_item_fls();

			lcd_show_character(&pt_word.warring[0]);//设置成功
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//设置失败
		}
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_STORE)//库存输入
	{
		if(input_len_get() != 0 )
		{
			if(g_vm.channelBit > 1)//大于1位的货道号
			{
				if(pt_channelSet->spring >= input_value_get())
				{
					transfer_value_to_char(input_value_get(), pt_content->content[holder.curSelectId -1].val);
					pt_channelSet->store = input_value_get();
					e2p_set_store((g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col), &pt_channelSet->store);
					//汇报后台
					report_channel_set(C_CHANNEL_STORE, pt_channelSet->store);
					lcd_show_character(&pt_word.warring[0]);//"设置成功"
					//貨道從0開始启动
					loop_channel_set(g_trade.curDiverBoard.tray, 0);
					
					//刷新当前项
					cur_item_fls();
				}
				else
				{
					lcd_show_character(&pt_word.warring[1]);//"设置失败"
				}
			}
			else
			if(g_vm.channelBit == 1)//1位货道号，用于循环出货
			{
				uint8_t i, j;
				g_trade.curDiverBoard.col = 0;
				for(i = 0, j = 0; i < MAX_COLUMN_SIZE; i++)
				{
					e2p_get_channelEnable(g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + i, &pt_channelSet->type);
					if(pt_channelSet->type == 1)//正常货道
						j++;
				}
				if(j*pt_channelSet->spring >= input_value_get() )//最大库存 = 正常货道数*货道弹簧数
				{
					transfer_value_to_char(input_value_get(), pt_content->content[holder.curSelectId -1].val);
					pt_channelSet->store = input_value_get();
					//保存库存
					e2p_set_store((g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col), &pt_channelSet->store);
					
					bkp_trayStore_set(g_trade.curDiverBoard.tray, pt_channelSet->store);//更新可卖库存
					for(i =0; i < MAX_COLUMN_SIZE; i++)//清除此层货道统计
					{
						e2p_set_channelStatistic(g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + i, 0, 0);
					}
					bkp_trayStatistic_set(g_trade.curDiverBoard.tray, 0, 0);	//清除此层统计
					report_channel_set(C_CHANNEL_STORE, pt_channelSet->store);	//汇报后台
					lcd_show_character(&pt_word.warring[0]);					//设置成功
					loop_channel_set(g_trade.curDiverBoard.tray, 0);			//貨道從0開始启动
					//刷新当前项
					cur_item_fls();
					
					{
					uint8_t num;
					uint16_t val;
					//显示可卖库存
					lcd_show_num(bkp_trayStore_get(g_trade.curDiverBoard.tray), pt_content->content[holder.curSelectId -1].valueX + 4*8, pt_content->content[holder.curSelectId -1].valueY, holder.foreground, holder.background, *(holder.font) ,0 ,10);
					//显示实际库存
					bkp_trayStatistic_get(g_trade.curDiverBoard.tray, &num, &val);
					lcd_show_num(pt_channelSet->store - num, pt_content->content[holder.curSelectId -1].valueX + 8*8, pt_content->content[holder.curSelectId -1].valueY, holder.foreground, holder.background, *(holder.font) ,0 ,10);
					}
				}
				else
				{
					lcd_show_character(&pt_word.warring[1]);//设置失败
				}
			}
			else
			{
				lcd_show_character(&pt_word.warring[1]);//设置失败
			}
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//设置失败
		}
	}
}


/*
创建显示内容
后面改动显示内容顺序只要改下面这个函数
*/
static void content_creat(void)
{
	uint8_t err;
	/*
	第一步：内存分配
	*/
	pt_channelSet = _malloc(sizeof(SETUP_CHANNEL), &err);
	assert_param(pt_channelSet);
	
	pt_content = _malloc(sizeof(AGGREGATE), &err); 
	assert_param(pt_content);
	
	err = 0;
	/*
	第二步：内容初始化
	*/ 
	pt_content->content[0].item = &pt_word.set[14];//"商品:" 
	pt_content->content[0].valueX = pt_word.set[14].valx ; pt_content->content[0].valueY = pt_word.set[14].valy;
	pt_content->content[0].val[err] = '\0'; 
	pt_content->content[0].id = 0;
	pt_content->content[0].inputContent = CONTENT_CHANNEL;//输入内容是货道
	
	
	pt_content->content[1].item = &pt_word.set[15];//"库存:"; 
	pt_content->content[1].valueX = pt_word.set[15].valx ; pt_content->content[1].valueY = pt_word.set[15].valy;
	pt_content->content[1].val[err] = '\0';
	pt_content->content[1].id = 1;
	pt_content->content[1].inputContent = CONTENT_STORE; 
	
	
	pt_content->content[2].item = &pt_word.set[16];//"价格:"; 
	pt_content->content[2].valueX = pt_word.set[16].valx ; pt_content->content[2].valueY = pt_word.set[16].valy;
	pt_content->content[2].val[err] = '\0';
	pt_content->content[2].id = 2;
	pt_content->content[2].inputContent = CONTENT_PRICE; 

	pt_content->content[3].item = &pt_word.set[17];// "条码:";  
	pt_content->content[3].valueX = pt_word.set[17].valx ; pt_content->content[3].valueY = pt_word.set[17].valy;
	pt_content->content[3].val[err] = '\0'; 
	pt_content->content[3].id = 3;
	pt_content->content[3].inputContent = CONTENT_GOODSID;

//	pt_content->content[4].item = &pt_word.set[17];// "库存值:"
//	pt_content->content[4].valueX = pt_word.set[17].valx ; pt_content->content[3].valueY = pt_word.set[17].valy;
//	pt_content->content[4].val[err] = '\0'; 
//	pt_content->content[4].id = 4;

	/*
	第三步，添加到容器
	*/
	holder.size = HOLDER_SIZE;				//容器大小
	holder.font = G_HOLDER_FONT; 				//容器字体
	holder.shownumPage = G_HOLDER_ITEM_SIZE;	//一页显示多少个内容
	holder.zicolor = G_HOLDER_CUR_ZI_COLOR;	//当前选项数字颜色
	holder.numcolor = G_HOLDER_CUR_NAME_COLOR;//当前选项名称颜色
	holder.foreground = G_HOLDER_FOREGROUND;	//前景色
	holder.background = G_HOLDER_BACKGROUND;	//背景色
	holder.contentList = pt_content->content;		//菜单内容
	contant_load(&holder);					//载入容器
	
	content_input_set();	//设置输入长度
}

/*
释放显示内容
如果是_malloc的全局指针，则在这里释放指针内容
如果只是一次打印到屏幕上的内容，则空着就好
*/
static void content_free(void)
{
	_free(pt_content);
	_free(pt_channelSet);
}



/*
商品设置界面状态机
*/
uint8_t fsm_fun_goodsSet(FSMMSG* cmsg)
{
	switch (cmsg->type)
	{
		case MsgSYS:
			if(cmsg->stype == FSM_STATUS_INIT)
			{
				input_clear();//输入控件清零
				select_clear();//货道输入控件清零
				if(g_fsm.shortcut == 1)
					lcd_clear();
				else
					LCD_CLEAR_SIDE;////刷右边栏
				content_creat();//显示内容
			}
			else
			if(cmsg->stype == FSM_STATUS_EXIT)
			{
				content_free();
				input_clear();//输入控件清零
				select_clear();//货道输入控件清零
			}
		break;
		
		case MsgKEY:
			lcd_clear_character(&pt_word.warring[0]);//清除下边显示

			if(cmsg->value == DOOR_CLOSE_VAL)
			{
				Load_status(fsm_id_init);
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
			if(cmsg->value == KEY_DOWN_VAL)//下翻
			{
				if(select_state_get())
				{
					content_page_down(&holder);
					input_clear();
					content_input_set();
				}
			}
			else
			if(cmsg->value == KEY_UP_VAL)
			{
				if(select_state_get())//只有选好商品才允许下翻
				{
					/*
					这三个顺序不得打乱：1.选项变化，2.控件复位，3.当前输入内容长度设置
					*/
					content_page_up(&holder);
					input_clear();
					content_input_set();
				}
			}
			else
			if(cmsg->value == '#')//数据保存
			{
				if(holder.contentList[holder.curSelectId-1].inputContent != CONTENT_CHANNEL)//货道是不能保存的
				{
					content_input_save();
				}
			}
			else//这里也可以用输入内容来区分
			{
				if(holder.contentList[holder.curSelectId-1].inputContent == CONTENT_CHANNEL)//货道输入
				{
					select_goods(cmsg->value);
					transfer_value_to_char(g_trade.select.value, pt_content->content[0].val);
					lcd_show_string(select_char_get(),holder.contentList[holder.curSelectId-1].valueX, holder.contentList[holder.curSelectId-1].valueY, holder.numcolor, holder.background, *(holder.font));
					if(select_state_get())//如果选好货
					{
						content_value_read();//显示其他三项内容
					}
				}
				else
				if(holder.contentList[holder.curSelectId-1].inputContent == CONTENT_PRICE)//金额输入
				{
					input_money(cmsg->value);//输入金额
					lcd_show_string(input_char_get(), holder.contentList[holder.curSelectId-1].valueX, holder.contentList[holder.curSelectId-1].valueY, holder.numcolor, holder.background, *(holder.font));
				}
				else
				if(holder.contentList[holder.curSelectId-1].inputContent == CONTENT_GOODSID
				|| holder.contentList[holder.curSelectId-1].inputContent == CONTENT_STORE)//值输入
				{
					input_num(cmsg->value);
					lcd_show_string(input_char_get(),holder.contentList[holder.curSelectId-1].valueX, holder.contentList[holder.curSelectId-1].valueY, holder.numcolor, holder.background, *(holder.font));
				}
				
			}
		break;
	}
	return 0;
}


