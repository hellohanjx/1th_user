/*
������·����
*/

#include "header.h"

#define HOLDER_SIZE	9						//�˲˵���������������µĲ˵���Ҫ�ı������С��


/*
����һ���ṹ��,�����ݵļ���
����������Ϊ���ڴ��������
ֻ���ڴ��������ģ����� holder���������ſ���
����Ϊholder����������ָ������λcontent�ģ�����ڴ治��������ָ��ָ��δ֪�ط���
*/
typedef struct AGGREGATE{
	CONTENT content[HOLDER_SIZE];//->>>>>>>>>���������Ҫע���������Ĵ�С
}AGGREGATE;


/*
����Ҫ�õ������ݽṹ������ָ�룬
��Щָ��ʹ�õ�ʱ����Ҫ�����ڴ棬
����ע��ռ���������ͷ�
*/
static AGGREGATE *pt_content ;//ָ�����ݵļ���
static SETUP_EX1 *pt_ex1Set ;
static SETUP_EX2 *pt_ex2Set ;


/*
��ȡҪ��ʾ������
*/
static void content_value_read(void)
{
	//��ȡ����·����
	e2p_get_ex1(pt_ex1Set);	
	e2p_get_ex2(pt_ex2Set);	
}



/*
�������볤��
*/
static void content_input_set(void)
{
	switch(holder.curSelectId)
	{
		case 1://ͨ����
		break;
		case 2://��ʼʱ������
			input_len_set(5);
		break;
		case 3://����ʱ������
			input_len_set(5);
		break;
		case 4://ͨ����
		break;
		case 5://��ʼʱ������
			input_len_set(5);
		break;
		case 6://����ʱ������
			input_len_set(5);
		break;
		case 7://ͨ����
		break;
		case 8://��ʼʱ������
			input_len_set(5);
		break;
		case 9://����ʱ������
			input_len_set(5);
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
		case 1://ͨ����3/����ͨ��
		break;
		
		case 2://����ʱ��
			if(input_len_get() == 5 )
			{
				if(input_value_get() != pt_ex1Set->starttime_3 )
				{
					pt_ex1Set->starttime_3 = input_value_get();
					transfer_value_to_time(input_value_get() , pt_content->content[holder.curSelectId-1].val);
					e2p_set_ex1(pt_ex1Set);
					
					//�㱨��̨

				}
				lcd_show_character(&pt_word.warring[0]);//���óɹ� 
			}
			else
			{
				lcd_show_character(&pt_word.warring[1]);//����ʧ�� 
			}
		break;
			
		case 3://�ر�ʱ��
			if(input_len_get() == 5 )
			{
				if(input_value_get() != pt_ex1Set->stoptime_3 )
				{
					pt_ex1Set->stoptime_3 = input_value_get();
					transfer_value_to_time(input_value_get() , pt_content->content[holder.curSelectId-1].val);
					e2p_set_ex1(pt_ex1Set);
					
					//�㱨��̨
				}
				lcd_show_character(&pt_word.warring[0]);//���óɹ� 
			}
			else
			{
				lcd_show_character(&pt_word.warring[1]);//����ʧ�� 
			}
		break;
			
		case 4://ͨ����5/��������
		break;
		
		case 5://����ʱ��
			if(input_len_get() == 5 )
			{
				if(input_value_get() != pt_ex2Set->starttime_5 )
				{
					pt_ex2Set->starttime_5 = input_value_get();
					transfer_value_to_time(input_value_get() , pt_content->content[holder.curSelectId-1].val);
					e2p_set_ex2(pt_ex2Set);
					
					//�㱨��̨

				}
				lcd_show_character(&pt_word.warring[0]);//���óɹ� 
			}
			else
			{
				lcd_show_character(&pt_word.warring[1]);//����ʧ�� 
			}
		break;
			
		case 6://�ر�ʱ��
			if(input_len_get() == 5 )
			{
				if(input_value_get() != pt_ex2Set->stoptime_5 )
				{
					pt_ex2Set->stoptime_5 = input_value_get();
					transfer_value_to_time(input_value_get() , pt_content->content[holder.curSelectId-1].val);
					e2p_set_ex2(pt_ex2Set);
					
					//�㱨��̨
				}
				lcd_show_character(&pt_word.warring[0]);//���óɹ� 
			}
			else
			{
				lcd_show_character(&pt_word.warring[1]);//����ʧ�� 
			}
		break;
			
		case 7://ͨ����6/����2ͨ��
		break;
		
		case 8://����ʱ��
			if(input_len_get() == 5 )
			{
				if(input_value_get() != pt_ex2Set->starttime_6 )
				{
					pt_ex2Set->starttime_6 = input_value_get();
					transfer_value_to_time(input_value_get() , pt_content->content[holder.curSelectId-1].val);
					e2p_set_ex2(pt_ex2Set);
					
					//�㱨��̨

				}
				lcd_show_character(&pt_word.warring[0]);//���óɹ� 
			}
			else
			{
				lcd_show_character(&pt_word.warring[1]);//����ʧ�� 
			}
		break;
			
		case 9://�ر�ʱ��
			if(input_len_get() == 5 )
			{
				if(input_value_get() != pt_ex2Set->stoptime_6 )
				{
					pt_ex2Set->stoptime_6 = input_value_get();
					transfer_value_to_time(input_value_get() , pt_content->content[holder.curSelectId-1].val);
					e2p_set_ex2(pt_ex2Set);
					
					//�㱨��̨
				}
				lcd_show_character(&pt_word.warring[0]);//���óɹ� 
			}
			else
			{
				lcd_show_character(&pt_word.warring[1]);//����ʧ�� 
			}
		break;
			
		
	}
}


/*
������ʾ����
*/
static void content_creat(void)
{
	uint8_t err;
	/*
	��һ�����ڴ����
	*/
	pt_ex1Set = _malloc(sizeof(SETUP_EX1), &err);
	assert_param(pt_ex1Set);
	
	pt_ex2Set = _malloc(sizeof(SETUP_EX2), &err);
	assert_param(pt_ex2Set);
	
	pt_content = _malloc(sizeof(AGGREGATE), &err); 
	assert_param(pt_content);
	
	
	content_value_read();
	
	/*
	�ڶ��������ݳ�ʼ�� 
	*/
	/*
	��������
	*/
	pt_content->content[0].item = &pt_word.set[38];//"����"
	pt_content->content[0].valueX = pt_word.set[38].valx; 
	pt_content->content[0].valueY = pt_word.set[38].valy;
	pt_content->content[0].val[0] = '\0';

	pt_content->content[1].item = &pt_word.set[39];//"����:"
	pt_content->content[1].valueX = pt_word.set[39].valx; 
	pt_content->content[1].valueY = pt_word.set[39].valy;
	transfer_value_to_time(pt_ex1Set->starttime_3, pt_content->content[1].val);

	pt_content->content[2].item = &pt_word.set[40];//"�ر�:"
	pt_content->content[2].valueX = pt_word.set[40].valx; 
	pt_content->content[2].valueY = pt_word.set[40].valy;
	transfer_value_to_time(pt_ex1Set->stoptime_3, pt_content->content[2].val);
	
	/*
	�������Ȳ���
	*/
	pt_content->content[3].item = &pt_word.set[44];//"��������"
	pt_content->content[3].valueX = pt_word.set[44].valx; 
	pt_content->content[3].valueY = pt_word.set[44].valy;
	pt_content->content[3].val[0] = '\0';

	pt_content->content[4].item = &pt_word.set[45];//"����:"
	pt_content->content[4].valueX = pt_word.set[45].valx; 
	pt_content->content[4].valueY = pt_word.set[45].valy;
	transfer_value_to_time(pt_ex2Set->starttime_5, pt_content->content[4].val);

	pt_content->content[5].item = &pt_word.set[46];//"�ر�:"
	pt_content->content[5].valueX = pt_word.set[46].valx; 
	pt_content->content[5].valueY = pt_word.set[46].valy;
	transfer_value_to_time(pt_ex2Set->stoptime_5, pt_content->content[5].val);

	/*
	����2����
	*/
	pt_content->content[6].item = &pt_word.set[47];//����2:"
	pt_content->content[6].valueX = pt_word.set[47].valx; 
	pt_content->content[6].valueY = pt_word.set[47].valy;
	pt_content->content[6].val[0] = '\0';

	pt_content->content[7].item = &pt_word.set[48];//"����:"
	pt_content->content[7].valueX = pt_word.set[48].valx; 
	pt_content->content[7].valueY = pt_word.set[48].valy;
	transfer_value_to_time(pt_ex2Set->starttime_6, pt_content->content[7].val);

	pt_content->content[8].item = &pt_word.set[49];//"�ر�:"; 
	pt_content->content[8].valueX = pt_word.set[49].valx; 
	pt_content->content[8].valueY = pt_word.set[49].valy;
	transfer_value_to_time(pt_ex2Set->stoptime_6, pt_content->content[8].val);



	/*
	����������ӵ�����
	*/
	holder.size = HOLDER_SIZE;				//������С
	holder.font = G_HOLDER_FONT; 				//��������
	holder.shownumPage = G_HOLDER_ITEM_SIZE;	//һҳ��ʾ���ٸ�����
	holder.zicolor = G_HOLDER_CUR_ZI_COLOR;	//��ǰѡ��������ɫ
	holder.numcolor = G_HOLDER_CUR_NAME_COLOR;//��ǰѡ��������ɫ
	holder.foreground = G_HOLDER_FOREGROUND;	//ǰ��ɫ
	holder.background = G_HOLDER_BACKGROUND;	//����ɫ
	holder.contentList = pt_content->content;
	contant_load(&holder);	//��������
	
	content_input_set();	//�������볤��
}

/*
�ͷ���ʾ����
�����_malloc��ȫ��ָ�룬���������ͷ�ָ������
���ֻ��һ�δ�ӡ����Ļ�ϵ����ݣ�����žͺ�
*/
static void content_free(void)
{
	_free(pt_content);
	_free(pt_ex1Set);
	_free(pt_ex2Set);
}



/*
��Ʒ���ý���״̬��
*/
uint8_t fsm_peripheralSet(FSMMSG* cmsg)
{
	switch (cmsg->type)
	{
		case MsgSYS:
			if(cmsg->stype == FSM_STATUS_INIT)
			{
				input_clear();//����ؼ�����
				select_clear();//��������ؼ�����
				
				//ˢ�ұ���
				LCD_CLEAR_SIDE;//ˢ��
				content_creat();//��ʾ����
				
			}
			else
			if(cmsg->stype == FSM_STATUS_EXIT)
			{
				content_free();
				input_clear();//����ؼ�����
				select_clear();//��������ؼ�����
				//lcd_clear_warring();//��ʾ��Ϣ����
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
				if( !(holder.curSelectId == 1 || holder.curSelectId == 4 ||  holder.curSelectId == 7) )//ʱ������
				{
					if(input_time(cmsg->value))
					lcd_show_string(input_char_get(),holder.contentList[holder.curSelectId-1].valueX, holder.contentList[holder.curSelectId-1].valueY, holder.numcolor, holder.background, *(holder.font));
				}
			}
		break;
	}
	return 0;
}


