/*
֧������
*/

#include "header.h"
#include "menu_format.h"

#define HOLDER_SIZE	3						//�˲˵���������������µĲ˵���Ҫ�ı������С��

/*
����һ���ṹ��,�����ݵļ���
*/
typedef struct PT{
	CONTENT content[HOLDER_SIZE];//���������Ҫע������
}PT;


//��ȡ���ݽṹ��
static SETUP_CASHLESS *cashless ;
static PT *pt_content ;//ָ�����ݵļ���



/*
�������볤��
*/
static void content_input_set(void)
{
	switch(holder.curSelectId)
	{
		case 1://ɨ��֧��ʹ��
			input_len_set(1);
		break;
		case 2://ˢ���ӿ�
			input_len_set(4);
		break;
	}
}
/*
���뱣��
*/
static void content_input_save(void)
{
	switch(holder.curSelectId)
	{
		case 1://ɨ��ʹ��
		{
			if(input_value_get() < 2 && input_len_get() != 0)
			{
				//���ȸ������ݼ����е�����
				holder.contentList[holder.curSelectId-1].value = input_value_get();
				//Ȼ���ж��Ƿ����ݲ�ͬ
				if(cashless->applyEnable != input_value_get())
				{
					cashless->applyEnable = input_value_get();
					g_vm.applyEnable = cashless->applyEnable;//����ȫ�ֱ���
					transfer_value_to_char(cashless->applyEnable, holder.contentList[holder.curSelectId-1].val);
					//��������
					e2p_set_cashless(cashless);
					lcd_show_character(&pt_word.warring[0]);//���óɹ� 
					
					//�㱨��̨
					report_device_set(PARAM_SCAN_SWITCH, g_vm.applyEnable);
				}
				else
				{
					lcd_show_character(&pt_word.warring[1]);//����ʧ��
				}
			}
			else
			{
				lcd_show_character(&pt_word.warring[1]);//����ʧ�� 
			}
			//�㱨��̨
		}
		break;
		case 2://ˢ���ӿ�
			if(input_len_get() == 4 )//Ҫ��4λ����
			{
				//���ȸ������ݼ����е�����
				holder.contentList[holder.curSelectId-1].value = input_value_get();
				//Ȼ���ж��Ƿ����ݲ�ͬ
				if(cashless->port != input_value_get())
				{
					cashless->port = input_value_get();
					
					transfer_value_to_char(cashless->port, holder.contentList[holder.curSelectId-1].val);
					//��������
					e2p_set_cashless(cashless);
					lcd_show_character(&pt_word.warring[0]);//���óɹ� 

					//�㱨��̨
					report_device_set(PARAM_CARD_INTERFACE, cashless->port);
				}
				else
				{
					lcd_show_character(&pt_word.warring[1]);//����ʧ�� 
				}
			}
			else
			{
				lcd_show_character(&pt_word.warring[1]);//����ʧ�� 
			}
			//�㱨��̨
		break;
		case 3://ȡ���ӿ�
			
		break;
	}
}
/*
��ȡҪ��ʾ�����ݲ�����ռ�
*/
static void content_value_read(void)
{
	uint8_t err;
	
	cashless = _malloc(sizeof(SETUP_CASHLESS), &err);
	assert_param(cashless);
	
	pt_content = _malloc(sizeof(PT), &err); 
	assert_param(pt_content);
	
	e2p_get_cashless(cashless);//��ȡ����
}



/*
������ʾ����
*/
static void content_creat(void)
{
	//�����˽���Ҫ��ʾ�����ݣ�����ʼ��
	pt_content->content[0].item = &pt_word.set[28];//"ɨ��֧��(1��0��):"
	pt_content->content[0].value = cashless->applyEnable;
	pt_content->content[0].valueX = pt_word.set[28].valx; pt_content->content[0].valueY = pt_word.set[28].valy;
	transfer_value_to_char(cashless->applyEnable, pt_content->content[0].val);
	
	
	pt_content->content[1].item = &pt_word.set[29];//"ˢ���ӿ�:"
	pt_content->content[1].value = cashless->port;
	pt_content->content[1].valueX = pt_word.set[29].valx; pt_content->content[1].valueY = pt_word.set[29].valy;
	transfer_value_to_char(cashless->port, pt_content->content[1].val);
	
	
	pt_content->content[2].item = &pt_word.set[30];//"ȡ���ӿ�:"; 
	pt_content->content[2].value = 0;
	pt_content->content[2].valueX = pt_word.set[30].valx; pt_content->content[2].valueY = pt_word.set[30].valy;
	transfer_value_to_char(0, pt_content->content[2].val);

	//�˵�����
	holder.size = HOLDER_SIZE;				//������С
	holder.font = G_HOLDER_FONT; 				//��������
	holder.shownumPage = G_HOLDER_ITEM_SIZE;	//һҳ��ʾ���ٸ�����
	holder.zicolor = G_HOLDER_CUR_ZI_COLOR;	//��ǰѡ��������ɫ
	holder.numcolor = G_HOLDER_CUR_NAME_COLOR;//��ǰѡ��������ɫ
	holder.foreground = G_HOLDER_FOREGROUND;	//ǰ��ɫ
	holder.background = G_HOLDER_BACKGROUND;	//����ɫ
	holder.contentList = pt_content->content;
	holder.size = HOLDER_SIZE;//������С
	
	contant_load(&holder);
	content_input_set();	//����Ĭ�ϳ���
}

/*
�ͷ���ʾ����
�����_malloc��ȫ��ָ�룬���������ͷ�ָ������
���ֻ��һ�δ�ӡ����Ļ�ϵ����ݣ�����žͺ�
*/
static void content_free(void)
{
	_free(pt_content);
	_free(cashless);
}



/*
֧�����ý���
*/
uint8_t fsm_fun_payment(FSMMSG* cmsg)
{
	switch (cmsg->type)
	{
		case MsgSYS:
			if(cmsg->stype == FSM_STATUS_INIT)
			{
				input_clear();//����ؼ�����
				//ˢ�ұ���
				LCD_CLEAR_SIDE;//ˢ��
				content_value_read();
				//��ʾ����
				content_creat();
			}
			else
			if(cmsg->stype == FSM_STATUS_EXIT)
			{
				content_free();
				input_clear();//����ؼ�����
//				//lcd_clear_warring(prompt->x, prompt->y, prompt->len);
			}
		break;
		
		case MsgKEY:
			lcd_clear_character(&pt_word.warring[0]);//����±���ʾ
			
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
			if(cmsg->value == KEY_DOWN_VAL)//�·�
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
			if(cmsg->value == '#')//���ݱ���
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


