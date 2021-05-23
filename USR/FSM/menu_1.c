/*
���˵�

*/
#include "header.h"

#define MENU_SIZE	1	//�˵���Ŀ

typedef struct MENU{
	menuitem manegertiem[MENU_SIZE];
}MENU;

static MENU *senior;

/*
������ʾ����
*/
static void content_creat(void)
{
}

/*
�ͷ���ʾ����
�����_malloc��ȫ��ָ�룬���������ͷ�ָ������
���ֻ��һ�δ�ӡ����Ļ�ϵ����ݣ�����žͺ�
*/
static void menu_free(void)
{
	
	_free(senior);
}
	


/*
�����˵�
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
////									{"������Ϣ",0},
//									{"��Ʒ����", fsm_id_goodsSet},
//									{"��������", fsm_id_channelTest},
//								};


uint8_t fsm_fun_lowerMenu(FSMMSG* cmsg)
{
	switch (cmsg->type)
	{
		case MsgSYS:
			if(cmsg->stype == FSM_STATUS_INIT)
			{
				//����߲˵���
				lcd_clear();	//����
				LCD_DRAW_FRAME; //���߿� 
				//��ʾ����
				content_creat();
				creat_menu();
				
				mymenu.background = RED;
				mymenu.forceground = BLACK;
				mymenu.itemlist = senior->manegertiem;
				mymenu.name = &pt_word.menu[9];//�˵���ʼ��ַ
				mymenu.height = MENU_SIZE;//�˵�����
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
		    if(cmsg->value==KEY_DOWN_VAL)//�·�
		    	MenuNAVDOWN(&mymenu);
			else
		    if(cmsg->value==KEY_UP_VAL)//�Ϸ�
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


