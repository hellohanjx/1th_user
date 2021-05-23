/*
主菜单

*/
#include "header.h"

/*
创建显示内容
*/
static void content_creat(void)
{
	#if (SCREEN != TM12864Z && SCREEN != TM12864_NO_FONT)

	//读取数据结构体
	SETUP_DEVICE mac_data;
	SETUP_COMMON vm_common;
	SETUP_COMMUCATION commucation;
	//声明此界面要显示的内容，并初始化
//	lcd_fill_color(300, 0, (lcd_get_xsize() - 300), lcd_get_ysize(), BLUE);//刷屏

	//显示内容
	//读取要显示的数据
	e2p_get_vminfo(&mac_data);
	e2p_get_common(&vm_common);
	e2p_get_commucation(&commucation);
	
	//"机器信息"
	lcd_show_string(pt_word.menu[7].character, pt_word.menu[7].x, pt_word.menu[7].y, pt_word.menu[7].color, pt_word.menu[7].background, *(pt_word.menu[7].font));
	lcd_show_character(&pt_word.menu[11]);//"机器ID:"
	lcd_show_num(mac_data.facId, pt_word.menu[11].valx, pt_word.menu[11].valy, pt_word.menu[11].color, pt_word.menu[11].background, *(pt_word.menu[11].font), 1, 10);

	lcd_show_character(&pt_word.menu[12]);//"版本号:"
	lcd_show_string((const char*)version, pt_word.menu[12].valx, pt_word.menu[12].valy, pt_word.menu[12].color, pt_word.menu[12].background, *(pt_word.menu[12].font));
	
	lcd_show_character(&pt_word.menu[13]);//"驱动板:"
	if(pt_driverBoard[0]->state == 1)//"正常"
		lcd_show_string(pt_word.menu[14].character, pt_word.menu[13].valx, pt_word.menu[13].valy, pt_word.menu[14].color, pt_word.menu[14].background, *(pt_word.menu[14].font));
	else//"未连接"
		lcd_show_string(pt_word.menu[15].character, pt_word.menu[13].valx, pt_word.menu[13].valy, pt_word.menu[15].color, pt_word.menu[15].background, *(pt_word.menu[15].font));
	
	lcd_show_character(&pt_word.menu[16]);//"读卡器:"
	if(cIC.state == 1)//"正常"
		lcd_show_string(pt_word.menu[14].character, pt_word.menu[16].valx, pt_word.menu[16].valy, pt_word.menu[14].color, pt_word.menu[14].background, *(pt_word.menu[14].font));
	else//"未连接"
		lcd_show_string(pt_word.menu[15].character, pt_word.menu[16].valx, pt_word.menu[16].valy, pt_word.menu[15].color, pt_word.menu[15].background, *(pt_word.menu[15].font));
	
	lcd_show_character(&pt_word.menu[17]);//"纸币器:"
	if(billequip.eqiu_status > 1)//"未连接"
		lcd_show_string(pt_word.menu[15].character, pt_word.menu[17].valx, pt_word.menu[17].valy, pt_word.menu[15].color, pt_word.menu[15].background, *(pt_word.menu[15].font));
	else//"正常"
		lcd_show_string(pt_word.menu[14].character, pt_word.menu[17].valx, pt_word.menu[17].valy, pt_word.menu[14].color, pt_word.menu[14].background, *(pt_word.menu[14].font));
	
	lcd_show_character(&pt_word.menu[18]);//"硬币器:
	if(coinequip.eqiu_status > 1)//"未连接"
		lcd_show_string(pt_word.menu[15].character, pt_word.menu[18].valx, pt_word.menu[18].valy, pt_word.menu[15].color, pt_word.menu[15].background, *(pt_word.menu[15].font));
	else//"正常"
		lcd_show_string(pt_word.menu[14].character, pt_word.menu[18].valx, pt_word.menu[18].valy, pt_word.menu[14].color, pt_word.menu[14].background, *(pt_word.menu[14].font));

	lcd_show_character(&pt_word.menu[19]);//"网  络:"
	if(g_vm.linkState == 1)//"连网"
		lcd_show_string(pt_word.menu[20].character, pt_word.menu[19].valx, pt_word.menu[19].valy, pt_word.menu[20].color, pt_word.menu[20].background, *(pt_word.menu[20].font));
	else//"未连网"
		lcd_show_string(pt_word.menu[21].character, pt_word.menu[19].valx, pt_word.menu[19].valy, pt_word.menu[21].color, pt_word.menu[21].background, *(pt_word.menu[21].font));
	
	lcd_show_character(&pt_word.menu[22]);//"温  度:"
	if(pt_driverBoard[0]->temp.state[0] == 1)//温度值
		lcd_show_num(pt_driverBoard[0]->temp.value[0], pt_word.menu[22].valx, pt_word.menu[22].valy, pt_word.menu[22].color, pt_word.menu[22].background, *(pt_word.menu[22].font), 0, 10);
	else//"故障"
		lcd_show_string(pt_word.menu[23].character, pt_word.menu[22].valx, pt_word.menu[22].valy, pt_word.menu[23].color, pt_word.menu[23].background, *(pt_word.menu[23].font));

	#endif
}






#define MENU_SIZE	8	//菜单数目

typedef struct MENU{
	menuitem manegertiem[MENU_SIZE];
}MENU;

static MENU *senior ;


/*
释放显示内容
如果是_malloc的全局指针，则在这里释放指针内容
如果只是一次打印到屏幕上的内容，则空着就好
*/
static void menu_free(void)
{
	_free(senior);
}



/*
创建菜单
*/
static void creat_menu(void)
{
	uint8_t err;
	senior = _malloc(sizeof(MENU), &err);
	log_output(senior);
	if(senior)
	{
		senior->manegertiem[0].item = &pt_word.menu[0]; 
		senior->manegertiem[0].status_id = fsm_id_goodsSet; 
		
		senior->manegertiem[1].item = &pt_word.menu[1]; 
		senior->manegertiem[1].status_id = fsm_id_channelTest; 
		
		senior->manegertiem[2].item = &pt_word.menu[2]; 
		senior->manegertiem[2].status_id = fsm_id_channelSet; 
		
		senior->manegertiem[3].item = &pt_word.menu[3]; 
		senior->manegertiem[3].status_id = fsm_id_temperatureSet; 
		
		senior->manegertiem[4].item = &pt_word.menu[4]; 
		senior->manegertiem[4].status_id = fsm_id_peripheralSet; 
		
		senior->manegertiem[5].item = &pt_word.menu[5]; 
		senior->manegertiem[5].status_id = fsm_id_payment; 
		
		senior->manegertiem[6].item = &pt_word.menu[25]; 
		senior->manegertiem[6].status_id = fsm_id_statistic; 
		
		senior->manegertiem[7].item = &pt_word.menu[7]; 
		senior->manegertiem[7].status_id = fsm_id_vmSet; 
		
	}
}


uint8_t fsm_fun_seniorMenu(FSMMSG* cmsg)
{
	switch (cmsg->type)
	{
		case MsgSYS:
			if(cmsg->stype == FSM_STATUS_INIT)
			{
				//画左边菜单栏
				lcd_clear();//清屏
				
				LCD_DRAW_FRAME; //画边框
				content_creat();//显示侧边栏内容
				creat_menu();
				
				mymenu.background = RED;	//当前选项前景色
				mymenu.forceground = BLACK;	//当前选项背景色
				mymenu.itemlist = senior->manegertiem;	//当前菜单首地址
				mymenu.name = &pt_word.menu[9];//菜单名称
				mymenu.height = MENU_SIZE;//菜单数量
				mymenu.sizePer = G_HOLDER_ITEM_SIZE; 
				LoadMenu(&mymenu);
			}
			else
			if(cmsg->stype == FSM_STATUS_EXIT)
			{
				menu_free();
			}
		break;
		
		case MsgKEY:
			if(cmsg->value == DOOR_CLOSE_VAL)
			{
				Load_status(fsm_id_init);
			}
			else
		    if(cmsg->value =='*')
			{
		    	Load_status(fsm_id_init);
			}
		    else
		    if(cmsg->value=='#')
			{
		    	intermenuitem(mymenu.selectitem,&mymenu);
			}
		    else
		    if(cmsg->value==KEY_DOWN_VAL)//下翻
			{
		    	MenuNAVDOWN(&mymenu);
			}
			else
		    if(cmsg->value==KEY_UP_VAL)//上翻
			{
		    	MenuNAVUP(&mymenu);
			}
			else
			if(cmsg->value == '0')
			{
				Load_status(fsm_id_vmInfo);
			}
//			else
//			if(cmsg->value == '9')
//			{
//				Load_status(fsm_id_driverSet);
//			}
//		    else
//			{
//		    	if(cmsg->value>'0' && cmsg->value<='9')
//		        	intermenuitem((cmsg->value-'0'),&mymenu);
//		        else
//		        if(cmsg->value>='A' && cmsg->value<='F')
//					intermenuitem((cmsg->value-'A'+10),&mymenu);
//			}
		break;
			
		default:
		break;
	}
	return 0;
}


