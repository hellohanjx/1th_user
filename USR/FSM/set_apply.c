/*
支付设置
*/

#include "header.h"
#include "menu_format.h"

#define HOLDER_SIZE	3						//此菜单内容数量（添加新的菜单项要改变这个大小）

/*
定义一个结构体,是内容的集合
*/
typedef struct PT{
	CONTENT content[HOLDER_SIZE];//添加新内容要注意这里
}PT;


//读取数据结构体
static SETUP_CASHLESS *cashless ;
static PT *pt_content ;//指向内容的集合



/*
设置输入长度
*/
static void content_input_set(void)
{
	switch(holder.curSelectId)
	{
		case 1://扫码支付使能
			input_len_set(1);
		break;
		case 2://刷卡接口
			input_len_set(4);
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
		case 1://扫码使能
		{
			if(input_value_get() < 2 && input_len_get() != 0)
			{
				//首先更新内容集合中的内容
				holder.contentList[holder.curSelectId-1].value = input_value_get();
				//然后判断是否数据不同
				if(cashless->applyEnable != input_value_get())
				{
					cashless->applyEnable = input_value_get();
					g_vm.applyEnable = cashless->applyEnable;//更新全局变量
					transfer_value_to_char(cashless->applyEnable, holder.contentList[holder.curSelectId-1].val);
					//存入数据
					e2p_set_cashless(cashless);
					lcd_show_character(&pt_word.warring[0]);//设置成功 
					
					//汇报后台
					report_device_set(PARAM_SCAN_SWITCH, g_vm.applyEnable);
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
			//汇报后台
		}
		break;
		case 2://刷卡接口
			if(input_len_get() == 4 )//要求4位输入
			{
				//首先更新内容集合中的内容
				holder.contentList[holder.curSelectId-1].value = input_value_get();
				//然后判断是否数据不同
				if(cashless->port != input_value_get())
				{
					cashless->port = input_value_get();
					
					transfer_value_to_char(cashless->port, holder.contentList[holder.curSelectId-1].val);
					//存入数据
					e2p_set_cashless(cashless);
					lcd_show_character(&pt_word.warring[0]);//设置成功 

					//汇报后台
					report_device_set(PARAM_CARD_INTERFACE, cashless->port);
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
			//汇报后台
		break;
		case 3://取货接口
			
		break;
	}
}
/*
读取要显示的内容并分配空间
*/
static void content_value_read(void)
{
	uint8_t err;
	
	cashless = _malloc(sizeof(SETUP_CASHLESS), &err);
	assert_param(cashless);
	
	pt_content = _malloc(sizeof(PT), &err); 
	assert_param(pt_content);
	
	e2p_get_cashless(cashless);//读取数据
}



/*
创建显示内容
*/
static void content_creat(void)
{
	//声明此界面要显示的内容，并初始化
	pt_content->content[0].item = &pt_word.set[28];//"扫码支付(1开0关):"
	pt_content->content[0].value = cashless->applyEnable;
	pt_content->content[0].valueX = pt_word.set[28].valx; pt_content->content[0].valueY = pt_word.set[28].valy;
	transfer_value_to_char(cashless->applyEnable, pt_content->content[0].val);
	
	
	pt_content->content[1].item = &pt_word.set[29];//"刷卡接口:"
	pt_content->content[1].value = cashless->port;
	pt_content->content[1].valueX = pt_word.set[29].valx; pt_content->content[1].valueY = pt_word.set[29].valy;
	transfer_value_to_char(cashless->port, pt_content->content[1].val);
	
	
	pt_content->content[2].item = &pt_word.set[30];//"取货接口:"; 
	pt_content->content[2].value = 0;
	pt_content->content[2].valueX = pt_word.set[30].valx; pt_content->content[2].valueY = pt_word.set[30].valy;
	transfer_value_to_char(0, pt_content->content[2].val);

	//菜单配置
	holder.size = HOLDER_SIZE;				//容器大小
	holder.font = G_HOLDER_FONT; 				//容器字体
	holder.shownumPage = G_HOLDER_ITEM_SIZE;	//一页显示多少个内容
	holder.zicolor = G_HOLDER_CUR_ZI_COLOR;	//当前选项数字颜色
	holder.numcolor = G_HOLDER_CUR_NAME_COLOR;//当前选项名称颜色
	holder.foreground = G_HOLDER_FOREGROUND;	//前景色
	holder.background = G_HOLDER_BACKGROUND;	//背景色
	holder.contentList = pt_content->content;
	holder.size = HOLDER_SIZE;//容器大小
	
	contant_load(&holder);
	content_input_set();	//输入默认长度
}

/*
释放显示内容
如果是_malloc的全局指针，则在这里释放指针内容
如果只是一次打印到屏幕上的内容，则空着就好
*/
static void content_free(void)
{
	_free(pt_content);
	_free(cashless);
}



/*
支付设置界面
*/
uint8_t fsm_fun_payment(FSMMSG* cmsg)
{
	switch (cmsg->type)
	{
		case MsgSYS:
			if(cmsg->stype == FSM_STATUS_INIT)
			{
				input_clear();//输入控件清零
				//刷右边栏
				LCD_CLEAR_SIDE;//刷屏
				content_value_read();
				//显示内容
				content_creat();
			}
			else
			if(cmsg->stype == FSM_STATUS_EXIT)
			{
				content_free();
				input_clear();//输入控件清零
//				//lcd_clear_warring(prompt->x, prompt->y, prompt->len);
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
				input_num(cmsg->value);
				lcd_show_string(input_char_get(),holder.contentList[holder.curSelectId-1].valueX, holder.contentList[holder.curSelectId-1].valueY, holder.numcolor, holder.background, *(holder.font)); 
			}
		break;
			
	}
	return 0;
}


