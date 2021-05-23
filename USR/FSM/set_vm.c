/*
机器通用设置
状态机实现

*/

#include "header.h"

#define HOLDER_SIZE				14			//此菜单内容数量（添加新的菜单项要改变这个大小）
/*
定义一个结构体,是内容的集合
说明：定义为一个结构体是很重要的，
保证了内存是连续的
*/
typedef struct AGGREGATE{
	CONTENT content[HOLDER_SIZE];//添加新内容要注意这里
}AGGREGATE;


/*
为了节省内存，使用指针指向动态分配内存
退出状态机则释放内存空间，
否则内存泄漏
*/
static SETUP_DEVICE *mac_data;
static SETUP_COMMON *vm_common ;
static SETUP_COMMUCATION *comucation ;
static AGGREGATE *content;//指向内容的集合


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
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_TRAYNUM)
	{
		input_len_set(1);//库存位数1位
	}
}


/*
输入保存
*/
static void content_input_save(void)
{
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_VMID)//机器号输入
	{
		
		if(input_len_get() == 10 &&  input_char_get()[0]< '4')//要求10位输入并且首位需要小于4
		{
			if(mac_data->facId != input_value_get())
			{
				mac_data->facId = input_value_get();
				input_char_get()[input_len_get()] = 0; //末尾字符归零(这里比较特殊，因为要求是固定长度)
				strcpy(holder.contentList[holder.curSelectId-1].val, input_char_get());
				e2p_set_vminfo(mac_data);//存入数据
				//更新全局变量（没有）
				//汇报(不需要)
			}
			lcd_show_character(&pt_word.warring[0]);//设置成功
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//设置失败
		}
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_PASSWORD)//连网密码输入
	{
		if(input_len_get() == 6)//要求6位输入
		{
			if(comucation->password != input_value_get() )
			{
				comucation->password = input_value_get();
				input_char_get()[input_len_get()] = 0;  //末尾字符归零（这里比较特殊，因为要求是固定长度）
				strcpy(holder.contentList[holder.curSelectId-1].val, input_char_get());
				e2p_set_commucation(comucation);//存入数据 
				//不需要更新全局变量
				//不需要汇报
			}
			lcd_show_character(&pt_word.warring[0]);//设置成功
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//设置失败
		}
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_LINKENABLE)//连网使能输入
	{
		if(input_value_get() < 2 && input_len_get() != 0)//连网使能的最大值
		{
			if(comucation->enable != input_value_get())
			{
				comucation->enable = input_value_get();
				transfer_value_to_char(comucation->enable, holder.contentList[holder.curSelectId-1].val);
				e2p_set_commucation(comucation);//存入数据
				//不需要更新全局变量
				//不需要汇报
			}
			lcd_show_character(&pt_word.warring[0]);//设置成功
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//设置失败
		}
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_LANGUAGE)//语言输入
	{
		if(input_value_get() < 2 && input_len_get() != 0)//语言的最大值
		{
			if(vm_common->language != input_value_get())
			{
				vm_common->language = input_value_get();
				transfer_value_to_char(vm_common->language, holder.contentList[holder.curSelectId-1].val);
				e2p_get_common(vm_common);//存入数据
				
				//汇报后台
				report_device_set(PARAM_LANGUAGE, vm_common->language);
				
			}
			lcd_show_character(&pt_word.warring[0]);//设置成功

		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//设置失败
		}
}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_DRVBIT)//机柜数量输入
	{
		if(input_value_get() < 10 && input_value_get() > 0)//机柜数量最大值
		{
			if(vm_common->drvnum != input_value_get())
			{
				vm_common->drvnum = input_value_get();
				transfer_value_to_char(vm_common->drvnum, holder.contentList[holder.curSelectId-1].val);
				e2p_set_common(vm_common);//存入数据
				g_vm.maxContainerNum = vm_common->drvnum;//更新全局变量
				
				//汇报后台
				report_device_set(PARAM_CONTAINER_BIT, vm_common->drvnum);
				{
					//这里需要特殊处理，分配给驱动板控件，或释放驱动板空间，
					//或重新启动
					
				}
			}
			lcd_show_character(&pt_word.warring[0]);//设置成功
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//设置失败
		}
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_POINTBIT)//小数点位数输入
	{
		if(input_value_get() < 9 && input_len_get() != 0)//小数点位数最大9
		{
			if(vm_common->point != input_value_get())
			{
				vm_common->point = input_value_get();
				transfer_value_to_char(vm_common->point, holder.contentList[holder.curSelectId-1].val);
				e2p_set_common(vm_common);//存入数据
				//不需要汇报
				g_vm.point =vm_common->point; //更新全局变量
				//汇报后台
				report_device_set(PARAM_POINT_BIT, vm_common->point);
			}
			lcd_show_character(&pt_word.warring[0]);//设置成功
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//设置失败
		}
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_CHANNELBIT)//输入货道位数
	{
		if(input_value_get() < 4 && input_value_get() > 0)//目前只允许输入1~3
		{
			if(vm_common->numChannel != input_value_get() )
			{
				vm_common->numChannel = input_value_get();
				transfer_value_to_char(vm_common->numChannel, holder.contentList[holder.curSelectId-1].val);
				e2p_set_common(vm_common);//存入数据
				g_vm.channelBit = vm_common->numChannel;//更新全局变量
				//汇报后台
				report_device_set(PARAM_CHANNEL_BIT, vm_common->numChannel);
			}
			lcd_show_character(&pt_word.warring[0]);//设置成功
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//设置失败
		}
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_PRICEBIT)//输入价格位数
	{
		if(input_value_get() < 10 && input_value_get() > 0)//价格位数最大值
		{
			if(vm_common->numPrice != input_value_get())
			{
				vm_common->numPrice = input_value_get();
				transfer_value_to_char(vm_common->numPrice, holder.contentList[holder.curSelectId-1].val);
				e2p_set_common(vm_common);//存入数据
				g_vm.priceBit = vm_common->numPrice;//更新全局变量
				//汇报后台
				report_device_set(PARAM_PRICE_BIT, vm_common->numPrice);
			}
			lcd_show_character(&pt_word.warring[0]);//设置成功
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//设置失败
		}
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_STOREBIT)//输入弹簧/库存位数
	{
		if(input_value_get() < 10 && input_value_get() > 0)//库存弹簧位数输入最大值
		{
			if(vm_common->numStore != input_value_get())
			{
				vm_common->numStore = input_value_get();
				transfer_value_to_char(vm_common->numStore, holder.contentList[holder.curSelectId-1].val);
				e2p_set_common(vm_common);//存入数据
				g_vm.storeBit = vm_common->numStore;//更新全局变量
				//汇报后台
				report_device_set(PARAM_INPUT_BIT, vm_common->numStore);
			}
			lcd_show_character(&pt_word.warring[0]);//设置成功
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//设置失败
		}
	}	
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_TRAYNUM)//托盘个数输入
	{
		if(input_value_get() < 9 && input_len_get() != 0)//托盘数量最大8
		{
			if(vm_common->traynum != input_value_get())
			{
				vm_common->traynum = input_value_get();
				transfer_value_to_char(vm_common->traynum, holder.contentList[holder.curSelectId-1].val);
				e2p_set_common(vm_common);//存入数据
				g_vm.maxtrayNum = vm_common->traynum;//更新全局变量
				//汇报后台
				report_device_set(PARAM_TRAT_SIZE, vm_common->traynum);
			}
			lcd_show_character(&pt_word.warring[0]);//设置成功
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//设置失败
		}
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_DATA)//日期输入
	{
		if(input_len_get() == 10 )
		{
			if(set_rtc_time(0, input_value_get()/10000, input_value_get()%10000/100, input_value_get()%10000%100, calendar.hour, calendar.min, calendar.sec))
			{
				lcd_show_character(&pt_word.warring[0]);//""设置成功
				transfer_value_to_data(input_value_get(), holder.contentList[holder.curSelectId -1].val);
			}
			else
			{
				lcd_show_character(&pt_word.warring[1]);//"设置失败"
			}
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//"设置失败"
		}
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_TIME)//时间输入
	{
		if(input_len_get() == 5 )
		{
			if(set_rtc_time(0, calendar.w_year, calendar.w_month, calendar.w_date, input_value_get()/100, input_value_get()%100, 0))
			{
				lcd_show_character(&pt_word.warring[0]);//"设置成功"
				transfer_value_to_time(input_value_get(), holder.contentList[holder.curSelectId -1].val);
			}
			else
			{
				lcd_show_character(&pt_word.warring[1]);//"设置失败"
			}
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//"设置失败"
		}
	}
}

/*
读取要显示内容
*/
static void content_value_read(void)
{
	uint8_t err;
	
	//分配空间
//	mac_data = 0;//因为_malloc总会返回值，所以这里不需要初始化指针
	mac_data = _malloc(sizeof(SETUP_DEVICE), &err);
	assert_param(mac_data);
	
//	vm_common = 0;
	vm_common = _malloc(sizeof(SETUP_COMMON), &err);
	assert_param(vm_common);
	
//	comucation = 0;
	comucation = _malloc(sizeof(SETUP_COMMUCATION), &err);
	assert_param(comucation);
	
//	content = 0;
	content = _malloc(sizeof(AGGREGATE), &err); 
	assert_param(content);
	
	
	//读取数据
	e2p_get_common(vm_common);
	e2p_get_vminfo(mac_data);
	e2p_get_commucation(comucation);
	
	
}

/*
创建显示内容
*/
static void content_creat(void)
{	
	char tmp[11];tmp[10] = 0;
	
	content->content[0].item = &pt_word.set[18];	//"机器ID:"
	content->content[0].valueX = pt_word.set[18].valx;content->content[0].valueY = pt_word.set[18].valy;
	sprintf(tmp, "%010u", mac_data->facId);
	strcpy(content->content[0].val, tmp);
	content->content[0].id = 0;
	content->content[0].inputContent = CONTENT_VMID;
	
	content->content[1].item = &pt_word.set[19];	//"连网密码:"
	content->content[1].valueX = pt_word.set[19].valx;content->content[1].valueY = pt_word.set[19].valy;
	sprintf(tmp, "%06u", comucation->password);
	strcpy(content->content[1].val, tmp);
	content->content[1].inputContent = CONTENT_PASSWORD;
	

	content->content[2].item = &pt_word.set[20];	//"连网(1是 0否):"
	content->content[2].valueX = pt_word.set[20].valx;content->content[2].valueY = pt_word.set[20].valy;
	transfer_value_to_char(comucation->enable, content->content[2].val);
	content->content[2].inputContent = CONTENT_LINKENABLE;
	
	content->content[3].item = &pt_word.set[21];	//"语言(1中文 0英文):
	content->content[3].valueX = pt_word.set[21].valx;content->content[3].valueY = pt_word.set[21].valy;
	transfer_value_to_char(vm_common->language, content->content[3].val);
	content->content[3].inputContent = CONTENT_LANGUAGE;

	content->content[4].item = &pt_word.set[22];	//"机柜数量:"
	content->content[4].valueX = pt_word.set[22].valx;content->content[4].valueY = pt_word.set[22].valy;
	transfer_value_to_char(vm_common->drvnum, content->content[4].val);
	content->content[4].inputContent = CONTENT_DRVBIT;
	
	content->content[5].item = &pt_word.set[23];	//"小数点位数:"
	content->content[5].valueX = pt_word.set[23].valx;content->content[5].valueY = pt_word.set[23].valy;
	transfer_value_to_char(vm_common->point, content->content[5].val);
	content->content[5].inputContent = CONTENT_POINTBIT;

	content->content[6].item = &pt_word.set[24];	//"价格位数:"
	content->content[6].valueX = pt_word.set[24].valx;content->content[6].valueY = pt_word.set[24].valy;
	transfer_value_to_char(vm_common->numPrice, content->content[6].val);
	content->content[6].inputContent = CONTENT_PRICEBIT;
	
	content->content[7].item = &pt_word.set[25];	//"库存/弹簧输入位数"; 
	content->content[7].valueX = pt_word.set[25].valx;content->content[7].valueY = pt_word.set[25].valy;
	transfer_value_to_char(vm_common->numStore, content->content[7].val);
	content->content[7].inputContent = CONTENT_STOREBIT;
	
	content->content[8].item = &pt_word.set[26];	//"货道位数:"
	content->content[8].valueX = pt_word.set[26].valx;content->content[8].valueY = pt_word.set[26].valy;
	transfer_value_to_char(vm_common->numChannel, content->content[8].val);
	content->content[8].inputContent = CONTENT_CHANNELBIT;
	
	content->content[9].item = &pt_word.set[57];	//"托盘数量:"
	content->content[9].valueX = pt_word.set[57].valx;content->content[9].valueY = pt_word.set[57].valy;
	transfer_value_to_char(vm_common->traynum, content->content[9].val);
	content->content[9].inputContent = CONTENT_TRAYNUM;
	
	content->content[13].item = &pt_word.set[27];	//"回复出厂(1 确认):
	content->content[13].valueX = pt_word.set[27].valx;content->content[13].valueY = pt_word.set[27].valy;
	content->content[13].inputContent = CONTENT_RESET;
	content->content[13].val[0] = '\0';
	
	content->content[12].item = &pt_word.set[58];	//"清空数据(1 确认):
	content->content[12].valueX = pt_word.set[58].valx;content->content[12].valueY = pt_word.set[58].valy;
	content->content[12].inputContent = CONTENT_DATARESET;
	content->content[12].val[0] = '\0';
	
	content->content[10].item = &pt_word.set[60];	//"日期"
	content->content[10].valueX = pt_word.set[60].valx;content->content[10].valueY = pt_word.set[60].valy;
	content->content[10].inputContent = CONTENT_DATA;
	transfer_value_to_data(calendar.w_year*10000 + calendar.w_month*100 + calendar.w_date, content->content[10].val);
	
	content->content[11].item = &pt_word.set[61];	//"时间"
	content->content[11].valueX = pt_word.set[61].valx;content->content[11].valueY = pt_word.set[61].valy;
	content->content[11].inputContent = CONTENT_TIME;
	transfer_value_to_time(calendar.hour*100 + calendar.min, content->content[11].val);
	
	//菜单配置
	holder.size = HOLDER_SIZE;				//容器大小
	holder.font = G_HOLDER_FONT; 				//容器字体
	holder.shownumPage = G_HOLDER_ITEM_SIZE;	//一页显示多少个内容
	holder.zicolor = G_HOLDER_CUR_ZI_COLOR;	//当前选项数字颜色
	holder.numcolor = G_HOLDER_CUR_NAME_COLOR;//当前选项名称颜色
	holder.foreground = G_HOLDER_FOREGROUND;	//前景色
	holder.background = G_HOLDER_BACKGROUND;	//背景色
	holder.contentList = content->content;	//内容指针
	contant_load(&holder);
	
	content_input_set();//输入默认长度
}

/*
释放显示内容
如果是_malloc的全局指针，则在这里释放指针内容
如果只是一次打印到屏幕上的内容，则空着就好
*/
static void content_free(void)
{
	_free(mac_data);
	_free(vm_common);
	_free(comucation);
	_free(content);
}
/*
指针初始化，方式指针未申请（_malloc()），但是_free()
防止野指针出现导致的_free释放有效数据，造成的系统崩溃
*/
static void pointer_init(void)
{
	mac_data = 0;
	vm_common = 0;
	comucation = 0;
	content = 0;
}

/*
状态机
*/
enum
{
	PASSWORD_MODE,
	SET_MODE,
};

uint8_t fsm_fun_vmSet(FSMMSG* cmsg)
{
	switch (cmsg->type)
	{
		case MsgSYS:
			if(cmsg->stype == FSM_STATUS_INIT)
			{
				pointer_init();//防止野指针
				input_clear();//输入控件清零
				//刷右边栏
				LCD_CLEAR_SIDE;//刷屏
				
				g_fsm.mode = PASSWORD_MODE; //0:输入密码模式；1:设置模式
				g_fsm.count = 0;
				input_len_set(9);//9位密码
				lcd_show_character(&pt_word.drvset[15]);//“请输入密码”
				
//				input_clear();//输入控件清零
//				//刷右边栏
//				LCD_CLEAR_SIDE;//刷屏
//				content_value_read();
//				//显示内容
//				content_creat();
			}
			else
			if(cmsg->stype == FSM_STATUS_EXIT)
			{
				content_free();
				input_clear();//输入控件清零
			}
		break;
		
		case MsgKEY:
			if(cmsg->value == '*')
			{
				Load_status(fsm_id_seniorMenu);
			}
			else
			{
				//参数设置模式
				if(g_fsm.mode == SET_MODE)
				{
					lcd_clear_character(&pt_word.warring[0]);//清除下边提示
					if(cmsg->value == DOOR_CLOSE_VAL)
					{
						Load_status(fsm_id_init);
					}
					else 
					if(cmsg->value == KEY_DOWN_VAL)//下翻
					{
						content_page_down(&holder);
						input_clear();
						content_input_set();
					}
					else
					if(cmsg->value == KEY_UP_VAL)//上翻
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
						if(holder.contentList[holder.curSelectId-1].inputContent == CONTENT_CHANNEL)//货道输入
						{
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
						|| holder.contentList[holder.curSelectId-1].inputContent == CONTENT_STOREBIT
						|| holder.contentList[holder.curSelectId-1].inputContent == CONTENT_VMID
						|| holder.contentList[holder.curSelectId-1].inputContent == CONTENT_PRICEBIT
						|| holder.contentList[holder.curSelectId-1].inputContent == CONTENT_TRAYNUM)//值输入
						{
							input_num(cmsg->value);
							lcd_show_string(input_char_get(),holder.contentList[holder.curSelectId-1].valueX, holder.contentList[holder.curSelectId-1].valueY, holder.numcolor, holder.background, *(holder.font));
						}
						else
						if(holder.contentList[holder.curSelectId-1].inputContent == CONTENT_RESET)//恢复出厂
						{
							if(cmsg->value == '1')//按下确认键
							{
								lcd_show_character(&pt_word.warring[5]);//正在恢复中
								bkp_flash_reset();	//恢复 bkp 区和擦除 flash 区
								e2p_reset_all();	//恢复 E2PROM
								e2p_get_global();	//读取全局变量
								content_value_read();//重新读取数据
								LCD_CLEAR_SIDE;		//清屏
								content_creat();	//重新载入设置数据
								lcd_show_character(&pt_word.warring[6]);//"恢复成功"
								
							}
						}
						else
						if(holder.contentList[holder.curSelectId-1].inputContent == CONTENT_DATARESET)//恢复数据
						{
							if(cmsg->value == '1')//按下确认键
							{
								lcd_show_character(&pt_word.warring[5]);//正在恢复中
								bkp_flash_reset();	//恢复 bkp 区和擦除 flash 区
								lcd_show_character(&pt_word.warring[6]);//"恢复成功"
								
							}
						}
						else
						if(holder.contentList[holder.curSelectId-1].inputContent == CONTENT_DATA)//日期输入
						{
							input_data(cmsg->value);
							lcd_show_string(input_char_get(),holder.contentList[holder.curSelectId-1].valueX, holder.contentList[holder.curSelectId-1].valueY, holder.numcolor, holder.background, *(holder.font));
						}
						else
						if(holder.contentList[holder.curSelectId-1].inputContent == CONTENT_TIME)//时间输入
						{
							input_time(cmsg->value);
							lcd_show_string(input_char_get(),holder.contentList[holder.curSelectId-1].valueX, holder.contentList[holder.curSelectId-1].valueY, holder.numcolor, holder.background, *(holder.font));
						}
					}
				}
				//输入密码模式
				else
				if(g_fsm.mode == PASSWORD_MODE)
				{
					input_num(cmsg->value);
					input_char_get()[g_fsm.count++] = '*';
					g_fsm.count %= 9;
					lcd_show_string(input_char_get(), pt_word.drvset[15].valx, pt_word.drvset[15].valy, pt_word.drvset[15].color, pt_word.drvset[15].background,  *(pt_word.drvset[15].font) );
					if(input_value_get() == 400823823)
					{
						g_fsm.mode = SET_MODE;
						input_clear();//输入控件清零
						//刷右边栏
						LCD_CLEAR_SIDE;//刷屏
						content_value_read();
						//显示内容
						content_creat();
					}
				}
			}
		break;
			
	}
	return 0;
}


