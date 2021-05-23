/*
�������������
*/

#include "header.h"

#define HOLDER_SIZE	14						//�˲˵���������������µĲ˵���Ҫ�ı������С��


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
static uint16_t *driver_param;//���������


/*
�������볤��
*/
static void content_input_set(void)
{
	switch(holder.curSelectId)
	{
		case 1://��������
			input_len_set(1);
		break;
		case 2://�¶�����
			input_len_set(1);
		break;
		case 3://��������
			input_len_set(4);
		break;
		case 4://��·����
			input_len_set(4);
		break;
		case 5://���ص���
			input_len_set(4);
		break;
		case 6://����ʱ��
			input_len_set(4);
		break;
		case 7://��ʱδ����ʱ��
			input_len_set(2);
		break;
		case 8://��ʱδֹͣʱ��
			input_len_set(2);
		break;
		case 9://���ֹͣ��ʱ
			input_len_set(4); 
		break;
		case 10://����ʱ�źż�����
			input_len_set(2);
		break;
		case 11://����ʱ�źż��ļ��us
			input_len_set(4);
		break;
		case 12://��ѯʱ�źż�����
			input_len_set(2);
		break;
		case 13://��ѯʱ�źż��ļ��us
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
		case 1://��������
			if(input_len_get() == 1 && input_value_get() < 2 )
			{
				driver_param[0] = input_value_get();
				transfer_value_to_char(input_value_get(), holder.contentList[holder.curSelectId -1].val);
				lcd_show_character(&pt_word.warring[0]);//���óɹ�
			}
			else
			{
				lcd_show_character(&pt_word.warring[1]);//����ʧ��
			}
		break;
		
		case 2://�¶�����
			if(input_len_get() == 1 && input_value_get() < 3 )
			{
				driver_param[1] = input_value_get();
				transfer_value_to_char(input_value_get(), holder.contentList[holder.curSelectId -1].val);
				lcd_show_character(&pt_word.warring[0]);//���óɹ�
			}
			else
			{
				lcd_show_character(&pt_word.warring[1]);//����ʧ��
			}
		break;
			
		case 3://��������
			driver_param[2] = input_value_get();
			transfer_value_to_char(input_value_get(), holder.contentList[holder.curSelectId -1].val);
			lcd_show_character(&pt_word.warring[0]);//���óɹ� 
		break;
		
		case 4://��·����
			driver_param[3] = input_value_get();
			transfer_value_to_char(input_value_get(), holder.contentList[holder.curSelectId -1].val);
			lcd_show_character(&pt_word.warring[0]);//���óɹ� 
		break;
		
		case 5://���ص���
			driver_param[4] = input_value_get();
			transfer_value_to_char(input_value_get(), holder.contentList[holder.curSelectId -1].val);
			lcd_show_character(&pt_word.warring[0]);//���óɹ� 
		break;
			
		case 6://����ʱ��
			driver_param[5] = input_value_get();
			transfer_value_to_char(input_value_get(), holder.contentList[holder.curSelectId -1].val);
			lcd_show_character(&pt_word.warring[0]);//���óɹ� 
		break;
			
		case 7://��ʱδ����ʱ��
			driver_param[6] = input_value_get();
			transfer_value_to_char(input_value_get(), holder.contentList[holder.curSelectId -1].val);
			lcd_show_character(&pt_word.warring[0]);//���óɹ� 
		break;
		
		case 8://��ʱδֹͣʱ��
			driver_param[7] = input_value_get();
			transfer_value_to_char(input_value_get(), holder.contentList[holder.curSelectId -1].val);
			lcd_show_character(&pt_word.warring[0]);//���óɹ� 
		break;
			
		case 9://���ֹͣ��ʱ
			driver_param[8] = input_value_get();
			transfer_value_to_char(input_value_get(), holder.contentList[holder.curSelectId -1].val);
			lcd_show_character(&pt_word.warring[0]);//���óɹ� 
		break;
			
		case 10://����ʱ�źż�����
			driver_param[9] = input_value_get();
			transfer_value_to_char(input_value_get(), holder.contentList[holder.curSelectId -1].val);
			lcd_show_character(&pt_word.warring[0]);//���óɹ� 
		break;
		
		case 11://����ʱ�źż��ļ��us
			driver_param[10] = input_value_get();
			transfer_value_to_char(input_value_get(), holder.contentList[holder.curSelectId -1].val);
			lcd_show_character(&pt_word.warring[0]);//���óɹ� 
		break;		
		
		case 12://��ѯʱ�źż�����
			driver_param[11] = input_value_get();
			transfer_value_to_char(input_value_get(), holder.contentList[holder.curSelectId -1].val);
			lcd_show_character(&pt_word.warring[0]);//���óɹ� 
		break;
		
		case 13://��ѯʱ�źż��ļ��us
			driver_param[12] = input_value_get();
			transfer_value_to_char(input_value_get(), holder.contentList[holder.curSelectId -1].val);
			lcd_show_character(&pt_word.warring[0]);//���óɹ� 
		break;		
		
	}
}

/*
���ݶ�ȡ
*/

static void content_read(void)
{
	uint8_t err;
	/*
	�ڴ����
	*/
	pt_content = (AGGREGATE*)_malloc(sizeof(AGGREGATE), &err);//��ʾ���� 
	assert_param(pt_content);
		
	driver_param = (uint16_t*)_malloc(60, &err);//���������
	assert_param(driver_param);
	
	/*
	��ȡ���������
	���Ͳ�ѯ����
	*/
	drv_cmd_getParam(0, driver_param);
}

/*
�ͷ���ʾ����
�����_malloc��ȫ��ָ�룬���������ͷ�ָ������
���ֻ��һ�δ�ӡ����Ļ�ϵ����ݣ�����žͺ�
*/
static void content_free(void)
{
	_free(driver_param); 
	_free(pt_content);
}
/*
ָ���ʼ������ʽָ��δ���루_malloc()��������_free()
��ֹҰָ����ֵ��µ�_free�ͷ���Ч���ݣ���ɵ�ϵͳ����
*/
static void pointer_init(void)
{
	driver_param = 0;
	pt_content = 0;
	
}

/*
������ʾ����
*/
static void content_creat(void)
{
	/*
	�ڶ��������ݳ�ʼ�� 
	*/
	pt_content->content[0].item = &pt_word.drvset[0];//"��������"
	pt_content->content[0].valueX = pt_word.drvset[0].valx; pt_content->content[0].valueY = pt_word.drvset[0].valy;
	pt_content->content[0].inputContent = CONTENT_DRV_SET1;
	transfer_value_to_char(driver_param[0], pt_content->content[0].val);

	pt_content->content[1].item = &pt_word.drvset[1];//"�¶�����"
	pt_content->content[1].valueX = pt_word.drvset[1].valx; pt_content->content[1].valueY = pt_word.drvset[1].valy;
	pt_content->content[1].inputContent = CONTENT_DRV_SET2;
	transfer_value_to_char(driver_param[1], pt_content->content[1].val);

	pt_content->content[2].item = &pt_word.drvset[2];//"��������"
	pt_content->content[2].valueX = pt_word.drvset[2].valx; pt_content->content[2].valueY = pt_word.drvset[2].valy;
	pt_content->content[2].inputContent = CONTENT_DRV_SET3;
	transfer_value_to_char(driver_param[2], pt_content->content[2].val);

	pt_content->content[3].item = &pt_word.drvset[3];//"��·����"
	pt_content->content[3].valueX = pt_word.drvset[3].valx; pt_content->content[3].valueY = pt_word.drvset[3].valy;
	pt_content->content[3].inputContent = CONTENT_DRV_SET4;
	transfer_value_to_char(driver_param[3], pt_content->content[3].val);

	pt_content->content[4].item = &pt_word.drvset[4];//"���ص���"
	pt_content->content[4].valueX = pt_word.drvset[4].valx; pt_content->content[4].valueY = pt_word.drvset[4].valy;
	pt_content->content[4].inputContent = CONTENT_DRV_SET5;
	transfer_value_to_char(driver_param[4], pt_content->content[4].val);

	pt_content->content[5].item = &pt_word.drvset[5];//"����ʱ��"
	pt_content->content[5].valueX = pt_word.drvset[5].valx; pt_content->content[5].valueY = pt_word.drvset[5].valy;
	pt_content->content[5].inputContent = CONTENT_DRV_SET6;
	transfer_value_to_char(driver_param[5], pt_content->content[5].val);

	pt_content->content[6].item = &pt_word.drvset[6];//"��ʱδ����ʱ��"
	pt_content->content[6].valueX = pt_word.drvset[6].valx; pt_content->content[6].valueY = pt_word.drvset[6].valy;
	pt_content->content[6].inputContent = CONTENT_DRV_SET7;
	transfer_value_to_char(driver_param[6], pt_content->content[6].val);

	pt_content->content[7].item = &pt_word.drvset[7];//"��ʱδֹͣʱ��"
	pt_content->content[7].valueX = pt_word.drvset[7].valx; pt_content->content[7].valueY = pt_word.drvset[7].valy;
	pt_content->content[7].inputContent = CONTENT_DRV_SET8;
	transfer_value_to_char(driver_param[7], pt_content->content[7].val);

	pt_content->content[8].item = &pt_word.drvset[8];//"���ֹͣ��ʱʱ��"
	pt_content->content[8].valueX = pt_word.drvset[8].valx; pt_content->content[8].valueY = pt_word.drvset[8].valy;
	pt_content->content[8].inputContent = CONTENT_DRV_SET9;
	transfer_value_to_char(driver_param[8], pt_content->content[8].val);

	pt_content->content[9].item = &pt_word.drvset[9];//"�����źż�����"
	pt_content->content[9].valueX = pt_word.drvset[9].valx; pt_content->content[9].valueY = pt_word.drvset[9].valy;
	pt_content->content[9].inputContent = CONTENT_DRV_SET10;
	transfer_value_to_char(driver_param[9], pt_content->content[9].val);

	pt_content->content[10].item = &pt_word.drvset[10];//"�����źż����"
	pt_content->content[10].valueX = pt_word.drvset[10].valx; pt_content->content[10].valueY = pt_word.drvset[10].valy;
	pt_content->content[10].inputContent = CONTENT_DRV_SET11;
	transfer_value_to_char(driver_param[10], pt_content->content[10].val);
 
 	pt_content->content[13].item = &pt_word.drvset[13];//"�ָ�Ĭ��"
	pt_content->content[13].valueX = pt_word.drvset[13].valx; pt_content->content[13].valueY = pt_word.drvset[13].valy;
	pt_content->content[13].inputContent = CONTENT_DRVPARAM_RESET;
	pt_content->content[13].val[0] = '\0';
	
	pt_content->content[11].item = &pt_word.drvset[11];//"����źż�����"
	pt_content->content[11].valueX = pt_word.drvset[11].valx; pt_content->content[11].valueY = pt_word.drvset[11].valy;
	pt_content->content[11].inputContent = CONTENT_DRV_SET10;
	transfer_value_to_char(driver_param[11], pt_content->content[11].val);

	pt_content->content[12].item = &pt_word.drvset[12];//"����źż����"
	pt_content->content[12].valueX = pt_word.drvset[12].valx; pt_content->content[12].valueY = pt_word.drvset[12].valy;
	pt_content->content[12].inputContent = CONTENT_DRV_SET11;
	transfer_value_to_char(driver_param[12], pt_content->content[12].val);

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
	holder.contentList = pt_content->content;	//��������ָ��ָ������
	contant_load(&holder);	//��������
	
	content_input_set();	//�������볤��
}



/*
�������������״̬��
*/
uint8_t fsm_driverboardSet(FSMMSG* cmsg)
{
	switch (cmsg->type)
	{
		case MsgSYS:
			if(cmsg->stype == FSM_STATUS_INIT)
			{
				pointer_init();//��ֹҰָ��
				g_fsm.mode = 0;//0�����룻1��ͨ���ã�2�ظ�Ĭ������
				g_fsm.count = 0;//����������
				input_clear();//����ؼ�����
				select_clear();//��������ؼ�����
				LCD_CLEAR_SIDE;//ˢ��

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
					drv_cmd_setParam(0, driver_param);//��������ָ��
				}
				content_free();
				input_clear();	//����ؼ�����
				select_clear();	//��������ؼ�����
			}
		break;
		
		case MsgKEY:
			lcd_clear_character(&pt_word.warring[0]);//����±���ʾ
			if(cmsg->value == DOOR_CLOSE_VAL)
			{
				if(g_fsm.mode == 1)
				{
					drv_cmd_setParam(0, driver_param);//��������ָ��
				}
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
				if(g_fsm.mode== 1)
				{
					content_page_down(&holder);
					input_clear();
					content_input_set();
				}
			}
			else
			if(cmsg->value == KEY_UP_VAL)//�Ϸ�
			{
				if(g_fsm.mode== 1)
				{
					content_page_up(&holder);
					input_clear();
					content_input_set();
				}
			}
			else
			if(cmsg->value == '#')//���ݱ���
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
					if(holder.contentList[holder.curSelectId-1].inputContent == CONTENT_DRVPARAM_RESET)//�ظ����������
					{
						if(cmsg->value == '1' )
						{
							g_fsm.mode = 2;
							lcd_show_character(&pt_word.warring[5]);//���ڻָ���
							if(drv_cmd_resetParam(0))
								lcd_show_character(&pt_word.warring[6]);//"�ָ��ɹ�"
							input_clear();//����ؼ�����
							select_clear();//��������ؼ�����
							drv_cmd_getParam(0, driver_param);//���»�ȡ����
							OSTimeDly(OS_TICKS_PER_SEC);
							LCD_CLEAR_SIDE;//ˢ�ұ���
							content_creat();//��������
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
						LCD_CLEAR_SIDE;//ˢ�ұ���
						g_fsm.mode = 1;//��������״̬
						content_read();
						content_creat();//��ʾ����
					}
				}
			}
		break;
	}
	return 0;
}

