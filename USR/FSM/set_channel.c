/*
货道设置
设置弹簧圈数，货道使能
*/

#include "header.h"

#define HOLDER_SIZE	3						//此菜单内容数量（添加新的菜单项要改变这个大小）


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
	//读取货道使能
	uint8_t i;
	for(i = 0; i < holder.size; i++)
	{
		if(holder.contentList[i].inputContent == CONTENT_CHANNEL)
		{
			
		}
		else
		if(holder.contentList[i].inputContent == CONTENT_SPRING)//弹簧
		{
			//刷屏
			lcd_show_string(input_char_get(), pt_content->content[i].valueX, pt_content->content[i].valueY, holder.foreground, holder.background, *(holder.font));
			//读取货道弹簧
			if(g_vm.channelBit == 1)
				e2p_get_spring((g_trade.curDiverBoard.container *100 + g_trade.curDiverBoard.tray*10 ), &pt_channelSet->spring);
			else
				e2p_get_spring((g_trade.curDiverBoard.container *100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col), &pt_channelSet->spring);
			transfer_value_to_char(pt_channelSet->spring, pt_content->content[i].val);
			lcd_show_num(pt_channelSet->spring, pt_content->content[i].valueX, pt_content->content[i].valueY, holder.foreground, holder.background, *(holder.font), 0, 10);
		}
		else
		if(holder.contentList[i].inputContent == CONTENT_CHANNLTYPE)//使能
		{
			uint32_t fac, tray_state ;
			char tmp[10];
			//刷屏
			lcd_show_string(input_char_get(), pt_content->content[i].valueX, pt_content->content[i].valueY, holder.foreground, holder.background, *(holder.font));
			if(g_vm.channelBit == 1)//如果输入的是托盘
			{
				uint8_t j;
				for(j = 10, fac = 1, tray_state = 0;j != 0; j--)//读取本托盘所有货道状态
				{
					e2p_get_channelEnable( (g_trade.curDiverBoard.container *100 + g_trade.curDiverBoard.tray*10 + (j-1)), &pt_channelSet->type);
					if(j != MAX_COLUMN_SIZE)
						fac *= 10;
					tray_state += fac*pt_channelSet->type;
					sprintf(tmp, "%010u", tray_state);
					memcpy(pt_content->content[i].val, tmp,10);
					lcd_show_string(tmp, pt_content->content[i].valueX, pt_content->content[i].valueY, holder.foreground, holder.background, *(holder.font));	
					//汇报给平台
				}
			}
			else//按货道设置
			{
				e2p_get_channelEnable( (g_trade.curDiverBoard.container *100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col), &pt_channelSet->type);
				transfer_value_to_char(pt_channelSet->type, pt_content->content[i].val);
				lcd_show_num(pt_channelSet->type, pt_content->content[i].valueX, pt_content->content[i].valueY, holder.foreground, holder.background, *(holder.font), 0, 10);	
			
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
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_STORE)//库存/弹簧圈数输入
	{
		input_len_set(g_vm.storeBit);//输入长度
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_SPRING)//库存/弹簧圈数输入
	{
		input_len_set(g_vm.storeBit);//输入长度
	}	
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_VMID)
	{
		input_len_set(10);//机器号10位
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_PASSWORD)
	{
		input_len_set(6);//连网密码6位
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_LINKENABLE)
	{
		input_len_set(1);//连网使能1位
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_LANGUAGE)
	{
		input_len_set(1);//语言1位
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_DRVBIT)
	{
		input_len_set(1);//机柜数量1位
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_POINTBIT)
	{
		input_len_set(1);//小数点1位
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_CHANNELBIT)
	{
		input_len_set(1);//货道位数1位
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_PRICEBIT)
	{
		input_len_set(1);//价格位数1位
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_STOREBIT)
	{
		input_len_set(1);//库存位数1位
	}	
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_CHANNLTYPE)//禁能/使能输入
	{
		if(g_vm.channelBit == 1)
		input_len_set(10);
		else
		input_len_set(1);
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
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_SPRING)//弹簧输入
	{
		if(input_len_get() != 0)
		{
			if(input_value_get() != pt_channelSet->spring)
			{
				transfer_value_to_char(input_value_get(), pt_content->content[holder.curSelectId -1].val);
				pt_channelSet->spring = input_value_get();
				if(g_vm.channelBit == 1)
					e2p_set_spring((g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 ), &pt_channelSet->spring);
				else
					e2p_set_spring((g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col), &pt_channelSet->spring);
				
				//汇报后台
				report_channel_set(C_CHANNEL_SPRING, pt_channelSet->spring);
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
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_CHANNLTYPE)//货道使能输入
	{
		if( (g_vm.channelBit !=1 && input_value_get() < 2 && input_len_get() != 0))//保存一个货道的输入
		{
			if(input_value_get() != pt_channelSet->type)
			{
				transfer_value_to_char(input_value_get(), pt_content->content[holder.curSelectId -1].val);
				pt_channelSet->type = input_value_get();
				e2p_set_channelEnable((g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col), &pt_channelSet->type);
				//汇报后台
				report_channel_set(C_CHANNEL_ENABLE, pt_channelSet->type);
			}
			lcd_show_character(&pt_word.warring[0]);//设置成功
		}
		else
		if(g_vm.channelBit == 1 && input_len_get() == 10)//保存一个托盘的输入
		{
			for(g_trade.curDiverBoard.col = 0; g_trade.curDiverBoard.col < 10; g_trade.curDiverBoard.col++)
			{
				pt_channelSet->type = input_char_get()[g_trade.curDiverBoard.col] - '0';
				pt_content->content[holder.curSelectId -1].val[g_trade.curDiverBoard.col] =  input_char_get()[g_trade.curDiverBoard.col];
				e2p_set_channelEnable((g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col), &pt_channelSet->type);
				//汇报后台
				report_channel_set(C_CHANNEL_ENABLE, pt_channelSet->type);
			}
			lcd_show_character(&pt_word.warring[0]);//设置成功
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//设置失败

		}		
	}
}


/*
创建显示内容
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
	
	
	/*
	第二步：内容初始化
	*/
	pt_content->content[0].item = &pt_word.set[50];//"货道:";
	pt_content->content[0].valueX = pt_word.set[50].valx ; pt_content->content[0].valueY = pt_word.set[50].valy;
	pt_content->content[0].val[0] = '\0';
	pt_content->content[0].id = 0;
	pt_content->content[0].inputContent = CONTENT_CHANNEL;
	
	
	pt_content->content[1].item = &pt_word.set[51];//"弹簧圈数:"; 
	pt_content->content[1].valueX = pt_word.set[51].valx ; pt_content->content[1].valueY = pt_word.set[51].valy;
	pt_content->content[1].val[0] = '\0';
	pt_content->content[1].id = 1;
	pt_content->content[1].inputContent = CONTENT_SPRING;//输入类型
	
	
	pt_content->content[2].item = &pt_word.set[52];//"状态(0禁能1使能):"
	pt_content->content[2].valueX = pt_word.set[52].valx ; pt_content->content[2].valueY = pt_word.set[52].valy;
	pt_content->content[2].val[0] = '\0';
	pt_content->content[2].id = 2;
	pt_content->content[2].inputContent = CONTENT_CHANNLTYPE;


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
	holder.contentList = pt_content->content;
	contant_load(&holder);					//载入容器
	
	content_input_set();//设置输入长度
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
货道设置界面状态机
*/
uint8_t fsm_fun_channelSet(FSMMSG* cmsg)
{
	switch (cmsg->type)
	{
		case MsgSYS:
			if(cmsg->stype == FSM_STATUS_INIT)
			{
				input_clear();//输入控件清零
				select_clear();//货道输入控件清零
				
				//刷右边栏
				LCD_CLEAR_SIDE;//刷屏
				//content_value_read();
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
			lcd_clear_character(&pt_word.warring[0]);//清除下边提示
			if(cmsg->value == DOOR_CLOSE_VAL)
			{
				Load_status(fsm_id_init);
			}
			else
			if(cmsg->value == '*')
			{
				Load_status(fsm_id_seniorMenu);
			}
			else 
			if(cmsg->value == KEY_DOWN_VAL)//下翻
			{
				//if( (input_len_get() == 2 && g_vm.channelBit < 3) || (input_len_get() == 2 && g_vm.channelBit == 3) )
				if(select_state_get())
				{
					content_page_down(&holder);
					input_clear();
					content_input_set();
				}
			}
			else
			if(cmsg->value == KEY_UP_VAL)//上翻
			{
//				if( (input_len_get() == 2 && g_vm.channelBit < 3) || (input_len_get() == 2 && g_vm.channelBit == 3) )
				if(select_state_get())
				{
					/*
					这三个顺序不得打乱：1.选项变化，2.控件复位，3.输入内容长度设置
					*/
					content_page_up(&holder);
					input_clear();
					content_input_set();
				}
			}
			else
			if(cmsg->value == '#')//数据保存
			{
				if(holder.curSelectId != 1)
				content_input_save();
			}
			else
			{
				if(holder.contentList[holder.curSelectId-1].inputContent == CONTENT_CHANNEL)//货道输入
				{
					select_goods(cmsg->value);
					transfer_value_to_char(g_trade.select.value, pt_content->content[0].val);
//					show_string(select_char_get(),holder.contentList[holder.curSelectId-1].valueX, holder.contentList[holder.curSelectId-1].valueY);
					lcd_show_string(select_char_get(),holder.contentList[holder.curSelectId-1].valueX, holder.contentList[holder.curSelectId-1].valueY, holder.numcolor, holder.background, *(holder.font));
					if(select_state_get())//如果选好货
					{
						content_value_read();//显示其他三项内容
					}
				}
				else
				if(holder.contentList[holder.curSelectId-1].inputContent == CONTENT_PRICE)//金额输入
				{
				}
				else
				if(holder.contentList[holder.curSelectId-1].inputContent == CONTENT_DRVBIT
				|| holder.contentList[holder.curSelectId-1].inputContent == CONTENT_LINKENABLE	
				|| holder.contentList[holder.curSelectId-1].inputContent == CONTENT_CHANNELBIT
				|| holder.contentList[holder.curSelectId-1].inputContent == CONTENT_GOODSID
				|| holder.contentList[holder.curSelectId-1].inputContent == CONTENT_LANGUAGE
				|| holder.contentList[holder.curSelectId-1].inputContent == CONTENT_PASSWORD
				|| holder.contentList[holder.curSelectId-1].inputContent == CONTENT_POINTBIT
				|| holder.contentList[holder.curSelectId-1].inputContent == CONTENT_STORE
				|| holder.contentList[holder.curSelectId-1].inputContent == CONTENT_SPRING
				|| holder.contentList[holder.curSelectId-1].inputContent == CONTENT_STOREBIT
				|| holder.contentList[holder.curSelectId-1].inputContent == CONTENT_VMID
				|| holder.contentList[holder.curSelectId-1].inputContent == CONTENT_PRICEBIT)//值输入
				{
					input_num(cmsg->value);
					//显示输入数值
					lcd_show_string(input_char_get(),holder.contentList[holder.curSelectId-1].valueX, holder.contentList[holder.curSelectId-1].valueY,  holder.numcolor, holder.background, *(holder.font));
				}
				else
				if(holder.contentList[holder.curSelectId-1].inputContent == CONTENT_CHANNLTYPE)//货道使能输入
				{
					if(cmsg->value < '2')
					{
						input_num(cmsg->value);
						//显示货道使能数值
						lcd_show_string(input_char_get(),holder.contentList[holder.curSelectId-1].valueX, holder.contentList[holder.curSelectId-1].valueY, holder.numcolor, holder.background, *(holder.font));
					}
				}
			}
		break;
	}
	return 0;
}


