/*
设置驱动板参数
*/

#include "header.h"

#define HOLDER_SIZE	14						//此菜单内容数量（添加新的菜单项要改变这个大小）


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
static uint16_t *driver_param;//驱动板参数


/*
设置输入长度
*/
static void content_input_set(void)
{
	switch(holder.curSelectId)
	{
		case 1://适配类型
			input_len_set(1);
		break;
		case 2://温度类型
			input_len_set(1);
		break;
		case 3://启动电流
			input_len_set(4);
		break;
		case 4://短路电流
			input_len_set(4);
		break;
		case 5://过载电流
			input_len_set(4);
		break;
		case 6://过载时间
			input_len_set(4);
		break;
		case 7://超时未启动时间
			input_len_set(2);
		break;
		case 8://超时未停止时间
			input_len_set(2);
		break;
		case 9://电机停止延时
			input_len_set(4); 
		break;
		case 10://出货时信号检测次数
			input_len_set(2);
		break;
		case 11://出货时信号检查的间隔us
			input_len_set(4);
		break;
		case 12://查询时信号检测次数
			input_len_set(2);
		break;
		case 13://查询时信号检查的间隔us
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
		case 1://适配类型
			if(input_len_get() == 1 && input_value_get() < 2 )
			{
				driver_param[0] = input_value_get();
				transfer_value_to_char(input_value_get(), holder.contentList[holder.curSelectId -1].val);
				lcd_show_character(&pt_word.warring[0]);//设置成功
			}
			else
			{
				lcd_show_character(&pt_word.warring[1]);//设置失败
			}
		break;
		
		case 2://温度类型
			if(input_len_get() == 1 && input_value_get() < 3 )
			{
				driver_param[1] = input_value_get();
				transfer_value_to_char(input_value_get(), holder.contentList[holder.curSelectId -1].val);
				lcd_show_character(&pt_word.warring[0]);//设置成功
			}
			else
			{
				lcd_show_character(&pt_word.warring[1]);//设置失败
			}
		break;
			
		case 3://启动电流
			driver_param[2] = input_value_get();
			transfer_value_to_char(input_value_get(), holder.contentList[holder.curSelectId -1].val);
			lcd_show_character(&pt_word.warring[0]);//设置成功 
		break;
		
		case 4://短路电流
			driver_param[3] = input_value_get();
			transfer_value_to_char(input_value_get(), holder.contentList[holder.curSelectId -1].val);
			lcd_show_character(&pt_word.warring[0]);//设置成功 
		break;
		
		case 5://过载电流
			driver_param[4] = input_value_get();
			transfer_value_to_char(input_value_get(), holder.contentList[holder.curSelectId -1].val);
			lcd_show_character(&pt_word.warring[0]);//设置成功 
		break;
			
		case 6://过载时间
			driver_param[5] = input_value_get();
			transfer_value_to_char(input_value_get(), holder.contentList[holder.curSelectId -1].val);
			lcd_show_character(&pt_word.warring[0]);//设置成功 
		break;
			
		case 7://超时未启动时间
			driver_param[6] = input_value_get();
			transfer_value_to_char(input_value_get(), holder.contentList[holder.curSelectId -1].val);
			lcd_show_character(&pt_word.warring[0]);//设置成功 
		break;
		
		case 8://超时未停止时间
			driver_param[7] = input_value_get();
			transfer_value_to_char(input_value_get(), holder.contentList[holder.curSelectId -1].val);
			lcd_show_character(&pt_word.warring[0]);//设置成功 
		break;
			
		case 9://电机停止延时
			driver_param[8] = input_value_get();
			transfer_value_to_char(input_value_get(), holder.contentList[holder.curSelectId -1].val);
			lcd_show_character(&pt_word.warring[0]);//设置成功 
		break;
			
		case 10://出货时信号检测次数
			driver_param[9] = input_value_get();
			transfer_value_to_char(input_value_get(), holder.contentList[holder.curSelectId -1].val);
			lcd_show_character(&pt_word.warring[0]);//设置成功 
		break;
		
		case 11://出货时信号检测的间隔us
			driver_param[10] = input_value_get();
			transfer_value_to_char(input_value_get(), holder.contentList[holder.curSelectId -1].val);
			lcd_show_character(&pt_word.warring[0]);//设置成功 
		break;		
		
		case 12://查询时信号检测次数
			driver_param[11] = input_value_get();
			transfer_value_to_char(input_value_get(), holder.contentList[holder.curSelectId -1].val);
			lcd_show_character(&pt_word.warring[0]);//设置成功 
		break;
		
		case 13://查询时信号检测的间隔us
			driver_param[12] = input_value_get();
			transfer_value_to_char(input_value_get(), holder.contentList[holder.curSelectId -1].val);
			lcd_show_character(&pt_word.warring[0]);//设置成功 
		break;		
		
	}
}

/*
数据读取
*/

static void content_read(void)
{
	uint8_t err;
	/*
	内存分配
	*/
	pt_content = (AGGREGATE*)_malloc(sizeof(AGGREGATE), &err);//显示内容 
	assert_param(pt_content);
		
	driver_param = (uint16_t*)_malloc(60, &err);//驱动板参数
	assert_param(driver_param);
	
	/*
	读取驱动板参数
	发送查询命令
	*/
	drv_cmd_getParam(0, driver_param);
}

/*
释放显示内容
如果是_malloc的全局指针，则在这里释放指针内容
如果只是一次打印到屏幕上的内容，则空着就好
*/
static void content_free(void)
{
	_free(driver_param); 
	_free(pt_content);
}
/*
指针初始化，方式指针未申请（_malloc()），但是_free()
防止野指针出现导致的_free释放有效数据，造成的系统崩溃
*/
static void pointer_init(void)
{
	driver_param = 0;
	pt_content = 0;
	
}

/*
创建显示内容
*/
static void content_creat(void)
{
	/*
	第二步：内容初始化 
	*/
	pt_content->content[0].item = &pt_word.drvset[0];//"适配类型"
	pt_content->content[0].valueX = pt_word.drvset[0].valx; pt_content->content[0].valueY = pt_word.drvset[0].valy;
	pt_content->content[0].inputContent = CONTENT_DRV_SET1;
	transfer_value_to_char(driver_param[0], pt_content->content[0].val);

	pt_content->content[1].item = &pt_word.drvset[1];//"温度类型"
	pt_content->content[1].valueX = pt_word.drvset[1].valx; pt_content->content[1].valueY = pt_word.drvset[1].valy;
	pt_content->content[1].inputContent = CONTENT_DRV_SET2;
	transfer_value_to_char(driver_param[1], pt_content->content[1].val);

	pt_content->content[2].item = &pt_word.drvset[2];//"启动电流"
	pt_content->content[2].valueX = pt_word.drvset[2].valx; pt_content->content[2].valueY = pt_word.drvset[2].valy;
	pt_content->content[2].inputContent = CONTENT_DRV_SET3;
	transfer_value_to_char(driver_param[2], pt_content->content[2].val);

	pt_content->content[3].item = &pt_word.drvset[3];//"短路电流"
	pt_content->content[3].valueX = pt_word.drvset[3].valx; pt_content->content[3].valueY = pt_word.drvset[3].valy;
	pt_content->content[3].inputContent = CONTENT_DRV_SET4;
	transfer_value_to_char(driver_param[3], pt_content->content[3].val);

	pt_content->content[4].item = &pt_word.drvset[4];//"过载电流"
	pt_content->content[4].valueX = pt_word.drvset[4].valx; pt_content->content[4].valueY = pt_word.drvset[4].valy;
	pt_content->content[4].inputContent = CONTENT_DRV_SET5;
	transfer_value_to_char(driver_param[4], pt_content->content[4].val);

	pt_content->content[5].item = &pt_word.drvset[5];//"过载时间"
	pt_content->content[5].valueX = pt_word.drvset[5].valx; pt_content->content[5].valueY = pt_word.drvset[5].valy;
	pt_content->content[5].inputContent = CONTENT_DRV_SET6;
	transfer_value_to_char(driver_param[5], pt_content->content[5].val);

	pt_content->content[6].item = &pt_word.drvset[6];//"超时未启动时间"
	pt_content->content[6].valueX = pt_word.drvset[6].valx; pt_content->content[6].valueY = pt_word.drvset[6].valy;
	pt_content->content[6].inputContent = CONTENT_DRV_SET7;
	transfer_value_to_char(driver_param[6], pt_content->content[6].val);

	pt_content->content[7].item = &pt_word.drvset[7];//"超时未停止时间"
	pt_content->content[7].valueX = pt_word.drvset[7].valx; pt_content->content[7].valueY = pt_word.drvset[7].valy;
	pt_content->content[7].inputContent = CONTENT_DRV_SET8;
	transfer_value_to_char(driver_param[7], pt_content->content[7].val);

	pt_content->content[8].item = &pt_word.drvset[8];//"电机停止延时时间"
	pt_content->content[8].valueX = pt_word.drvset[8].valx; pt_content->content[8].valueY = pt_word.drvset[8].valy;
	pt_content->content[8].inputContent = CONTENT_DRV_SET9;
	transfer_value_to_char(driver_param[8], pt_content->content[8].val);

	pt_content->content[9].item = &pt_word.drvset[9];//"出货信号检测次数"
	pt_content->content[9].valueX = pt_word.drvset[9].valx; pt_content->content[9].valueY = pt_word.drvset[9].valy;
	pt_content->content[9].inputContent = CONTENT_DRV_SET10;
	transfer_value_to_char(driver_param[9], pt_content->content[9].val);

	pt_content->content[10].item = &pt_word.drvset[10];//"出货信号检测间隔"
	pt_content->content[10].valueX = pt_word.drvset[10].valx; pt_content->content[10].valueY = pt_word.drvset[10].valy;
	pt_content->content[10].inputContent = CONTENT_DRV_SET11;
	transfer_value_to_char(driver_param[10], pt_content->content[10].val);
 
 	pt_content->content[13].item = &pt_word.drvset[13];//"恢复默认"
	pt_content->content[13].valueX = pt_word.drvset[13].valx; pt_content->content[13].valueY = pt_word.drvset[13].valy;
	pt_content->content[13].inputContent = CONTENT_DRVPARAM_RESET;
	pt_content->content[13].val[0] = '\0';
	
	pt_content->content[11].item = &pt_word.drvset[11];//"检测信号检测次数"
	pt_content->content[11].valueX = pt_word.drvset[11].valx; pt_content->content[11].valueY = pt_word.drvset[11].valy;
	pt_content->content[11].inputContent = CONTENT_DRV_SET10;
	transfer_value_to_char(driver_param[11], pt_content->content[11].val);

	pt_content->content[12].item = &pt_word.drvset[12];//"检测信号检测间隔"
	pt_content->content[12].valueX = pt_word.drvset[12].valx; pt_content->content[12].valueY = pt_word.drvset[12].valy;
	pt_content->content[12].inputContent = CONTENT_DRV_SET11;
	transfer_value_to_char(driver_param[12], pt_content->content[12].val);

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
	holder.contentList = pt_content->content;	//容器内容指针指向内容
	contant_load(&holder);	//载入容器
	
	content_input_set();	//设置输入长度
}



/*
驱动板参数设置状态机
*/
uint8_t fsm_driverboardSet(FSMMSG* cmsg)
{
	switch (cmsg->type)
	{
		case MsgSYS:
			if(cmsg->stype == FSM_STATUS_INIT)
			{
				pointer_init();//防止野指针
				g_fsm.mode = 0;//0输密码；1普通设置；2回复默认设置
				g_fsm.count = 0;//输密码掩码
				input_clear();//输入控件清零
				select_clear();//货道输入控件清零
				LCD_CLEAR_SIDE;//刷屏

				if(pt_driverBoard[0]->state != 1)
				{
					lcd_show_character(&pt_word.drvset[16]);
					return 0;
				}
				
				input_len_set(8);
				lcd_show_character(&pt_word.drvset[14]);
				lcd_show_character(&pt_word.drvset[15]);
			}
			else
			if(cmsg->stype == FSM_STATUS_EXIT)
			{
				if(g_fsm.mode == 1)
				{
					drv_cmd_setParam(0, driver_param);//发送设置指令
				}
				content_free();
				input_clear();	//输入控件清零
				select_clear();	//货道输入控件清零
			}
		break;
		
		case MsgKEY:
			lcd_clear_character(&pt_word.warring[0]);//清除下边提示
			if(cmsg->value == DOOR_CLOSE_VAL)
			{
				if(g_fsm.mode == 1)
				{
					drv_cmd_setParam(0, driver_param);//发送设置指令
				}
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
				if(g_fsm.mode== 1)
				{
					content_page_down(&holder);
					input_clear();
					content_input_set();
				}
			}
			else
			if(cmsg->value == KEY_UP_VAL)//上翻
			{
				if(g_fsm.mode== 1)
				{
					content_page_up(&holder);
					input_clear();
					content_input_set();
				}
			}
			else
			if(cmsg->value == '#')//数据保存
			{
				if(g_fsm.mode== 1)
				{
					content_input_save();
				}
			}
			else
			{
				if(g_fsm.mode== 1)
				{
					if(holder.contentList[holder.curSelectId-1].inputContent == CONTENT_DRVPARAM_RESET)//回复驱动板参数
					{
						if(cmsg->value == '1' )
						{
							g_fsm.mode = 2;
							lcd_show_character(&pt_word.warring[5]);//正在恢复中
							if(drv_cmd_resetParam(0))
								lcd_show_character(&pt_word.warring[6]);//"恢复成功"
							input_clear();//输入控件清零
							select_clear();//货道输入控件清零
							drv_cmd_getParam(0, driver_param);//重新获取数据
							OSTimeDly(OS_TICKS_PER_SEC);
							LCD_CLEAR_SIDE;//刷右边栏
							content_creat();//重新载入
							g_fsm.mode = 1;
						}
					}
					else
					{
						if(cmsg->value <= '9' && cmsg->value >= '0')
						{
							input_num(cmsg->value);
							lcd_show_string(input_char_get(),holder.contentList[holder.curSelectId-1].valueX, holder.contentList[holder.curSelectId-1].valueY, holder.numcolor, holder.background, *(holder.font));
						}
					}
				}
				else
				if(g_fsm.mode== 0)
				{
					input_num(cmsg->value);
					input_char_get()[g_fsm.count++] = '*';
					g_fsm.count %= 8;
					lcd_show_string(input_char_get(), pt_word.drvset[15].valx, pt_word.drvset[15].valy, pt_word.drvset[15].color, pt_word.drvset[15].background,  *(pt_word.drvset[15].font) );
					if(input_value_get() == 20170501)
					{
						LCD_CLEAR_SIDE;//刷右边栏
						g_fsm.mode = 1;//进入设置状态
						content_read();
						content_creat();//显示内容
					}
				}
			}
		break;
	}
	return 0;
}

