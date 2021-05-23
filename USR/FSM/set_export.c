/*
其他回路设置
*/

#include "header.h"

#define HOLDER_SIZE	9						//此菜单内容数量（添加新的菜单项要改变这个大小）


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
static AGGREGATE *pt_content ;//指向内容的集合
static SETUP_EX1 *pt_ex1Set ;
static SETUP_EX2 *pt_ex2Set ;


/*
读取要显示的内容
*/
static void content_value_read(void)
{
	//读取货回路设置
	e2p_get_ex1(pt_ex1Set);	
	e2p_get_ex2(pt_ex2Set);	
}



/*
设置输入长度
*/
static void content_input_set(void)
{
	switch(holder.curSelectId)
	{
		case 1://通道号
		break;
		case 2://开始时间输入
			input_len_set(5);
		break;
		case 3://结束时间输入
			input_len_set(5);
		break;
		case 4://通道号
		break;
		case 5://开始时间输入
			input_len_set(5);
		break;
		case 6://结束时间输入
			input_len_set(5);
		break;
		case 7://通道号
		break;
		case 8://开始时间输入
			input_len_set(5);
		break;
		case 9://结束时间输入
			input_len_set(5);
		break;
	}
}
/*
输入保存
*/
static void content_input_save(void)
{
	switch(holder.curSelectId)
	{		
		case 1://通道号3/照明通道
		break;
		
		case 2://开启时间
			if(input_len_get() == 5 )
			{
				if(input_value_get() != pt_ex1Set->starttime_3 )
				{
					pt_ex1Set->starttime_3 = input_value_get();
					transfer_value_to_time(input_value_get() , pt_content->content[holder.curSelectId-1].val);
					e2p_set_ex1(pt_ex1Set);
					
					//汇报后台

				}
				lcd_show_character(&pt_word.warring[0]);//设置成功 
			}
			else
			{
				lcd_show_character(&pt_word.warring[1]);//设置失败 
			}
		break;
			
		case 3://关闭时间
			if(input_len_get() == 5 )
			{
				if(input_value_get() != pt_ex1Set->stoptime_3 )
				{
					pt_ex1Set->stoptime_3 = input_value_get();
					transfer_value_to_time(input_value_get() , pt_content->content[holder.curSelectId-1].val);
					e2p_set_ex1(pt_ex1Set);
					
					//汇报后台
				}
				lcd_show_character(&pt_word.warring[0]);//设置成功 
			}
			else
			{
				lcd_show_character(&pt_word.warring[1]);//设置失败 
			}
		break;
			
		case 4://通道号5/玻璃加热
		break;
		
		case 5://开启时间
			if(input_len_get() == 5 )
			{
				if(input_value_get() != pt_ex2Set->starttime_5 )
				{
					pt_ex2Set->starttime_5 = input_value_get();
					transfer_value_to_time(input_value_get() , pt_content->content[holder.curSelectId-1].val);
					e2p_set_ex2(pt_ex2Set);
					
					//汇报后台

				}
				lcd_show_character(&pt_word.warring[0]);//设置成功 
			}
			else
			{
				lcd_show_character(&pt_word.warring[1]);//设置失败 
			}
		break;
			
		case 6://关闭时间
			if(input_len_get() == 5 )
			{
				if(input_value_get() != pt_ex2Set->stoptime_5 )
				{
					pt_ex2Set->stoptime_5 = input_value_get();
					transfer_value_to_time(input_value_get() , pt_content->content[holder.curSelectId-1].val);
					e2p_set_ex2(pt_ex2Set);
					
					//汇报后台
				}
				lcd_show_character(&pt_word.warring[0]);//设置成功 
			}
			else
			{
				lcd_show_character(&pt_word.warring[1]);//设置失败 
			}
		break;
			
		case 7://通道号6/备用2通道
		break;
		
		case 8://开启时间
			if(input_len_get() == 5 )
			{
				if(input_value_get() != pt_ex2Set->starttime_6 )
				{
					pt_ex2Set->starttime_6 = input_value_get();
					transfer_value_to_time(input_value_get() , pt_content->content[holder.curSelectId-1].val);
					e2p_set_ex2(pt_ex2Set);
					
					//汇报后台

				}
				lcd_show_character(&pt_word.warring[0]);//设置成功 
			}
			else
			{
				lcd_show_character(&pt_word.warring[1]);//设置失败 
			}
		break;
			
		case 9://关闭时间
			if(input_len_get() == 5 )
			{
				if(input_value_get() != pt_ex2Set->stoptime_6 )
				{
					pt_ex2Set->stoptime_6 = input_value_get();
					transfer_value_to_time(input_value_get() , pt_content->content[holder.curSelectId-1].val);
					e2p_set_ex2(pt_ex2Set);
					
					//汇报后台
				}
				lcd_show_character(&pt_word.warring[0]);//设置成功 
			}
			else
			{
				lcd_show_character(&pt_word.warring[1]);//设置失败 
			}
		break;
			
		
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
	pt_ex1Set = _malloc(sizeof(SETUP_EX1), &err);
	assert_param(pt_ex1Set);
	
	pt_ex2Set = _malloc(sizeof(SETUP_EX2), &err);
	assert_param(pt_ex2Set);
	
	pt_content = _malloc(sizeof(AGGREGATE), &err); 
	assert_param(pt_content);
	
	
	content_value_read();
	
	/*
	第二步：内容初始化 
	*/
	/*
	照明策略
	*/
	pt_content->content[0].item = &pt_word.set[38];//"照明"
	pt_content->content[0].valueX = pt_word.set[38].valx; 
	pt_content->content[0].valueY = pt_word.set[38].valy;
	pt_content->content[0].val[0] = '\0';

	pt_content->content[1].item = &pt_word.set[39];//"开启:"
	pt_content->content[1].valueX = pt_word.set[39].valx; 
	pt_content->content[1].valueY = pt_word.set[39].valy;
	transfer_value_to_time(pt_ex1Set->starttime_3, pt_content->content[1].val);

	pt_content->content[2].item = &pt_word.set[40];//"关闭:"
	pt_content->content[2].valueX = pt_word.set[40].valx; 
	pt_content->content[2].valueY = pt_word.set[40].valy;
	transfer_value_to_time(pt_ex1Set->stoptime_3, pt_content->content[2].val);
	
	/*
	玻璃加热策略
	*/
	pt_content->content[3].item = &pt_word.set[44];//"玻璃加热"
	pt_content->content[3].valueX = pt_word.set[44].valx; 
	pt_content->content[3].valueY = pt_word.set[44].valy;
	pt_content->content[3].val[0] = '\0';

	pt_content->content[4].item = &pt_word.set[45];//"开启:"
	pt_content->content[4].valueX = pt_word.set[45].valx; 
	pt_content->content[4].valueY = pt_word.set[45].valy;
	transfer_value_to_time(pt_ex2Set->starttime_5, pt_content->content[4].val);

	pt_content->content[5].item = &pt_word.set[46];//"关闭:"
	pt_content->content[5].valueX = pt_word.set[46].valx; 
	pt_content->content[5].valueY = pt_word.set[46].valy;
	transfer_value_to_time(pt_ex2Set->stoptime_5, pt_content->content[5].val);

	/*
	备用2策略
	*/
	pt_content->content[6].item = &pt_word.set[47];//备用2:"
	pt_content->content[6].valueX = pt_word.set[47].valx; 
	pt_content->content[6].valueY = pt_word.set[47].valy;
	pt_content->content[6].val[0] = '\0';

	pt_content->content[7].item = &pt_word.set[48];//"开启:"
	pt_content->content[7].valueX = pt_word.set[48].valx; 
	pt_content->content[7].valueY = pt_word.set[48].valy;
	transfer_value_to_time(pt_ex2Set->starttime_6, pt_content->content[7].val);

	pt_content->content[8].item = &pt_word.set[49];//"关闭:"; 
	pt_content->content[8].valueX = pt_word.set[49].valx; 
	pt_content->content[8].valueY = pt_word.set[49].valy;
	transfer_value_to_time(pt_ex2Set->stoptime_6, pt_content->content[8].val);



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
	contant_load(&holder);	//载入容器
	
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
	_free(pt_ex1Set);
	_free(pt_ex2Set);
}



/*
商品设置界面状态机
*/
uint8_t fsm_peripheralSet(FSMMSG* cmsg)
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
				content_creat();//显示内容
				
			}
			else
			if(cmsg->stype == FSM_STATUS_EXIT)
			{
				content_free();
				input_clear();//输入控件清零
				select_clear();//货道输入控件清零
				//lcd_clear_warring();//提示信息清零
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
				content_page_down(&holder);
				input_clear();
				content_input_set();
			}
			else
			if(cmsg->value == KEY_UP_VAL)
			{
				content_page_up(&holder);
				input_clear();
				content_input_set();
			}
			else
			if(cmsg->value == '#')//数据保存
			{
				content_input_save();
			}
			else
			{
				if( !(holder.curSelectId == 1 || holder.curSelectId == 4 ||  holder.curSelectId == 7) )//时间输入
				{
					if(input_time(cmsg->value))
					lcd_show_string(input_char_get(),holder.contentList[holder.curSelectId-1].valueX, holder.contentList[holder.curSelectId-1].valueY, holder.numcolor, holder.background, *(holder.font));
				}
			}
		break;
	}
	return 0;
}


