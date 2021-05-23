/*
主菜单

*/
#include "header.h"

#define MENU_SIZE	1	//菜单数目

typedef struct MENU{
	menuitem manegertiem[MENU_SIZE];
}MENU;

static MENU *senior;

/*
创建显示内容
*/
static void content_creat(void)
{
}

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
		
//		senior->manegertiem[1].item = &pt_word.menu[1]; 
//		senior->manegertiem[1].status_id = fsm_id_channelTest; 
		
		
	}
}



//static const menuitem manegertiem[]={ 
////									{"机器信息",0},
//									{"商品设置", fsm_id_goodsSet},
//									{"货道测试", fsm_id_channelTest},
//								};


uint8_t fsm_fun_lowerMenu(FSMMSG* cmsg)
{
	switch (cmsg->type)
	{
		case MsgSYS:
			if(cmsg->stype == FSM_STATUS_INIT)
			{
				//画左边菜单栏
				lcd_clear();	//清屏
				LCD_DRAW_FRAME; //画边框 
				//显示内容
				content_creat();
				creat_menu();
				
				mymenu.background = RED;
				mymenu.forceground = BLACK;
				mymenu.itemlist = senior->manegertiem;
				mymenu.name = &pt_word.menu[9];//菜单开始地址
				mymenu.height = MENU_SIZE;//菜单数量
				mymenu.sizePer = 8; 
				LoadMenu(&mymenu);
			}
			else
			if(cmsg->stype == FSM_STATUS_EXIT)
			{
				menu_free();
			}
		break;
		
		case MsgKEY:
		    if(cmsg->value=='*')
		    	Load_status(fsm_id_init);
		    else
		    if(cmsg->value=='#')
		    	intermenuitem(mymenu.selectitem,&mymenu);
		    
		    else
		    if(cmsg->value==KEY_DOWN_VAL)//下翻
		    	MenuNAVDOWN(&mymenu);
			else
		    if(cmsg->value==KEY_UP_VAL)//上翻
			{
		    	MenuNAVUP(&mymenu);
			}
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


