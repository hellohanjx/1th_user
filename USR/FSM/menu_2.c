/*
���˵�

*/
#include "header.h"

/*
������ʾ����
*/
static void content_creat(void)
{
	#if (SCREEN != TM12864Z && SCREEN != TM12864_NO_FONT)

	//��ȡ���ݽṹ��
	SETUP_DEVICE mac_data;
	SETUP_COMMON vm_common;
	SETUP_COMMUCATION commucation;
	//�����˽���Ҫ��ʾ�����ݣ�����ʼ��
//	lcd_fill_color(300, 0, (lcd_get_xsize() - 300), lcd_get_ysize(), BLUE);//ˢ��

	//��ʾ����
	//��ȡҪ��ʾ������
	e2p_get_vminfo(&mac_data);
	e2p_get_common(&vm_common);
	e2p_get_commucation(&commucation);
	
	//"������Ϣ"
	lcd_show_string(pt_word.menu[7].character, pt_word.menu[7].x, pt_word.menu[7].y, pt_word.menu[7].color, pt_word.menu[7].background, *(pt_word.menu[7].font));
	lcd_show_character(&pt_word.menu[11]);//"����ID:"
	lcd_show_num(mac_data.facId, pt_word.menu[11].valx, pt_word.menu[11].valy, pt_word.menu[11].color, pt_word.menu[11].background, *(pt_word.menu[11].font), 1, 10);

	lcd_show_character(&pt_word.menu[12]);//"�汾��:"
	lcd_show_string((const char*)version, pt_word.menu[12].valx, pt_word.menu[12].valy, pt_word.menu[12].color, pt_word.menu[12].background, *(pt_word.menu[12].font));
	
	lcd_show_character(&pt_word.menu[13]);//"������:"
	if(pt_driverBoard[0]->state == 1)//"����"
		lcd_show_string(pt_word.menu[14].character, pt_word.menu[13].valx, pt_word.menu[13].valy, pt_word.menu[14].color, pt_word.menu[14].background, *(pt_word.menu[14].font));
	else//"δ����"
		lcd_show_string(pt_word.menu[15].character, pt_word.menu[13].valx, pt_word.menu[13].valy, pt_word.menu[15].color, pt_word.menu[15].background, *(pt_word.menu[15].font));
	
	lcd_show_character(&pt_word.menu[16]);//"������:"
	if(cIC.state == 1)//"����"
		lcd_show_string(pt_word.menu[14].character, pt_word.menu[16].valx, pt_word.menu[16].valy, pt_word.menu[14].color, pt_word.menu[14].background, *(pt_word.menu[14].font));
	else//"δ����"
		lcd_show_string(pt_word.menu[15].character, pt_word.menu[16].valx, pt_word.menu[16].valy, pt_word.menu[15].color, pt_word.menu[15].background, *(pt_word.menu[15].font));
	
	lcd_show_character(&pt_word.menu[17]);//"ֽ����:"
	if(billequip.eqiu_status > 1)//"δ����"
		lcd_show_string(pt_word.menu[15].character, pt_word.menu[17].valx, pt_word.menu[17].valy, pt_word.menu[15].color, pt_word.menu[15].background, *(pt_word.menu[15].font));
	else//"����"
		lcd_show_string(pt_word.menu[14].character, pt_word.menu[17].valx, pt_word.menu[17].valy, pt_word.menu[14].color, pt_word.menu[14].background, *(pt_word.menu[14].font));
	
	lcd_show_character(&pt_word.menu[18]);//"Ӳ����:
	if(coinequip.eqiu_status > 1)//"δ����"
		lcd_show_string(pt_word.menu[15].character, pt_word.menu[18].valx, pt_word.menu[18].valy, pt_word.menu[15].color, pt_word.menu[15].background, *(pt_word.menu[15].font));
	else//"����"
		lcd_show_string(pt_word.menu[14].character, pt_word.menu[18].valx, pt_word.menu[18].valy, pt_word.menu[14].color, pt_word.menu[14].background, *(pt_word.menu[14].font));

	lcd_show_character(&pt_word.menu[19]);//"��  ��:"
	if(g_vm.linkState == 1)//"����"
		lcd_show_string(pt_word.menu[20].character, pt_word.menu[19].valx, pt_word.menu[19].valy, pt_word.menu[20].color, pt_word.menu[20].background, *(pt_word.menu[20].font));
	else//"δ����"
		lcd_show_string(pt_word.menu[21].character, pt_word.menu[19].valx, pt_word.menu[19].valy, pt_word.menu[21].color, pt_word.menu[21].background, *(pt_word.menu[21].font));
	
	lcd_show_character(&pt_word.menu[22]);//"��  ��:"
	if(pt_driverBoard[0]->temp.state[0] == 1)//�¶�ֵ
		lcd_show_num(pt_driverBoard[0]->temp.value[0], pt_word.menu[22].valx, pt_word.menu[22].valy, pt_word.menu[22].color, pt_word.menu[22].background, *(pt_word.menu[22].font), 0, 10);
	else//"����"
		lcd_show_string(pt_word.menu[23].character, pt_word.menu[22].valx, pt_word.menu[22].valy, pt_word.menu[23].color, pt_word.menu[23].background, *(pt_word.menu[23].font));

	#endif
}






#define MENU_SIZE	8	//�˵���Ŀ

typedef struct MENU{
	menuitem manegertiem[MENU_SIZE];
}MENU;

static MENU *senior ;


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
				//����߲˵���
				lcd_clear();//����
				
				LCD_DRAW_FRAME; //���߿�
				content_creat();//��ʾ���������
				creat_menu();
				
				mymenu.background = RED;	//��ǰѡ��ǰ��ɫ
				mymenu.forceground = BLACK;	//��ǰѡ���ɫ
				mymenu.itemlist = senior->manegertiem;	//��ǰ�˵��׵�ַ
				mymenu.name = &pt_word.menu[9];//�˵�����
				mymenu.height = MENU_SIZE;//�˵�����
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
		    if(cmsg->value==KEY_DOWN_VAL)//�·�
			{
		    	MenuNAVDOWN(&mymenu);
			}
			else
		    if(cmsg->value==KEY_UP_VAL)//�Ϸ�
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


