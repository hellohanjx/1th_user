/*
�¶�����
*/

#include "header.h"

#define HOLDER_SIZE	4						//�˲˵���������������µĲ˵���Ҫ�ı������С��


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
static AGGREGATE *pt_content;//ָ�����ݵļ���
static SETUP_EX1 *pt_ex1Set;



/*
��ȡҪ��ʾ������
*/
static void content_value_read(void)
{
	//��ȡ����·����
	e2p_get_ex1(pt_ex1Set);	
}



/*
�������볤��
*/
static void content_input_set(void)
{
	switch(holder.curSelectId)
	{
		case 1://Ŀ���¶����볤��
			input_len_set(2);
		break;
		case 2://��ʼʱ������
			input_len_set(5);
		break;
		case 3://����ʱ������
			input_len_set(5);
		break;
		case 4://������ʽ���������ȣ�
			input_len_set(1);
		break;
	}
}

/*
ˢ�µ�ǰѡ��
���һ��ֻ���ڲ���������ʱ
*/
static void cur_item_fls(void)
{
	content_item_fls(&holder);
	input_clear();
	content_input_set();
}


/*
���뱣��
*/
static void content_input_save(void)
{
	switch(holder.curSelectId)
	{
		case 1://Ŀ���¶�
			if(input_len_get() != 0)
			{
				if(pt_ex1Set->tempTarget != input_value_get())
				{
					pt_ex1Set->tempTarget = input_value_get();
					transfer_value_to_char(input_value_get() , pt_content->content[holder.curSelectId-1].val);
					e2p_set_ex1(pt_ex1Set);
					
					//�㱨��̨
					report_device_set(PARAM_TEMPTARGET, pt_ex1Set->tempTarget);
					
				}
				cur_item_fls();//ˢ�µ�ǰ��
				lcd_show_character(&pt_word.warring[0]);//���óɹ� 
			}
			else
			{
				lcd_show_character(&pt_word.warring[1]);//����ʧ�� 
			}
		break;
		
		case 2://����ʱ��
			if(input_len_get() == 5 )
			{
				if(input_value_get() != pt_ex1Set->starttime_1 )
				{
					pt_ex1Set->starttime_1 = input_value_get();
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
				if(input_value_get() != pt_ex1Set->stoptime_1 )
				{
					pt_ex1Set->stoptime_1 = input_value_get();
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
		
		case 4://������ʽ
			if(input_value_get() < 2 && input_len_get() != 0)
			{
				//Ȼ���ж��Ƿ����ݲ�ͬ
				if(input_value_get() != pt_ex1Set->workMode)
				{
					strcpy(pt_content->content[3].val, input_char_get());
					pt_ex1Set->workMode = input_value_get();
					transfer_value_to_char(input_value_get() , pt_content->content[holder.curSelectId-1].val);
					e2p_set_ex1(pt_ex1Set);
					
					//�㱨��̨
					report_device_set(PARAM_TEMPETURE_MODE, pt_ex1Set->workMode);
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
	
	pt_content = _malloc(sizeof(AGGREGATE), &err); 
	assert_param(pt_content);
	
	content_value_read();//��ȡ����ֵ
	
	/*
	�ڶ��������ݳ�ʼ��
	*/
	pt_content->content[0].item = &pt_word.set[31];	//"Ŀ���¶�:"; 
	pt_content->content[0].valueX = pt_word.set[31].valx; pt_content->content[0].valueY = pt_word.set[31].valy;
	transfer_value_to_char(pt_ex1Set->tempTarget, pt_content->content[0].val);
	
	pt_content->content[1].item = &pt_word.set[32];	//"����ʱ��:"; 
	pt_content->content[1].valueX = pt_word.set[32].valx; pt_content->content[1].valueY = pt_word.set[32].valy;
	transfer_value_to_time(pt_ex1Set->starttime_1, pt_content->content[1].val);
	
	pt_content->content[2].item = &pt_word.set[33];//"�ر�ʱ��:"; 
	pt_content->content[2].valueX = pt_word.set[33].valx; pt_content->content[2].valueY = pt_word.set[33].valy;
	transfer_value_to_time(pt_ex1Set->stoptime_1, pt_content->content[2].val);

	pt_content->content[3].item = &pt_word.set[34];//"������ʽ(0����1����):"
	pt_content->content[3].valueX = pt_word.set[34].valx; pt_content->content[3].valueY = pt_word.set[34].valy;
	transfer_value_to_char(pt_ex1Set->workMode, pt_content->content[3].val);
	
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
	contant_load(&holder);					//��������

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
}



/*
��Ʒ���ý���״̬��
*/
uint8_t fsm_fun_temperatureSet(FSMMSG* cmsg)
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
				if(holder.curSelectId == 2 || holder.curSelectId == 3 )//ʱ������
				{
					if(input_time(cmsg->value))
					lcd_show_string(input_char_get(),holder.contentList[holder.curSelectId-1].valueX, holder.contentList[holder.curSelectId-1].valueY, holder.numcolor, holder.background, *(holder.font));
				}
				else//��������
				{
					input_num(cmsg->value);
					lcd_show_string(input_char_get(),holder.contentList[holder.curSelectId-1].valueX, holder.contentList[holder.curSelectId-1].valueY, holder.numcolor, holder.background, *(holder.font));
				}
			}
		break;
	}
	return 0;
}


