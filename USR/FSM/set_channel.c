/*
��������
���õ���Ȧ��������ʹ��
*/

#include "header.h"

#define HOLDER_SIZE	3						//�˲˵���������������µĲ˵���Ҫ�ı������С��


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
static SETUP_CHANNEL *pt_channelSet;



/*
��ȡҪ��ʾ������
*/
static void content_value_read(void)
{
	//��ȡ����ʹ��
	uint8_t i;
	for(i = 0; i < holder.size; i++)
	{
		if(holder.contentList[i].inputContent == CONTENT_CHANNEL)
		{
			
		}
		else
		if(holder.contentList[i].inputContent == CONTENT_SPRING)//����
		{
			//ˢ��
			lcd_show_string(input_char_get(), pt_content->content[i].valueX, pt_content->content[i].valueY, holder.foreground, holder.background, *(holder.font));
			//��ȡ��������
			if(g_vm.channelBit == 1)
				e2p_get_spring((g_trade.curDiverBoard.container *100 + g_trade.curDiverBoard.tray*10 ), &pt_channelSet->spring);
			else
				e2p_get_spring((g_trade.curDiverBoard.container *100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col), &pt_channelSet->spring);
			transfer_value_to_char(pt_channelSet->spring, pt_content->content[i].val);
			lcd_show_num(pt_channelSet->spring, pt_content->content[i].valueX, pt_content->content[i].valueY, holder.foreground, holder.background, *(holder.font), 0, 10);
		}
		else
		if(holder.contentList[i].inputContent == CONTENT_CHANNLTYPE)//ʹ��
		{
			uint32_t fac, tray_state ;
			char tmp[10];
			//ˢ��
			lcd_show_string(input_char_get(), pt_content->content[i].valueX, pt_content->content[i].valueY, holder.foreground, holder.background, *(holder.font));
			if(g_vm.channelBit == 1)//��������������
			{
				uint8_t j;
				for(j = 10, fac = 1, tray_state = 0;j != 0; j--)//��ȡ���������л���״̬
				{
					e2p_get_channelEnable( (g_trade.curDiverBoard.container *100 + g_trade.curDiverBoard.tray*10 + (j-1)), &pt_channelSet->type);
					if(j != MAX_COLUMN_SIZE)
						fac *= 10;
					tray_state += fac*pt_channelSet->type;
					sprintf(tmp, "%010u", tray_state);
					memcpy(pt_content->content[i].val, tmp,10);
					lcd_show_string(tmp, pt_content->content[i].valueX, pt_content->content[i].valueY, holder.foreground, holder.background, *(holder.font));	
					//�㱨��ƽ̨
				}
			}
			else//����������
			{
				e2p_get_channelEnable( (g_trade.curDiverBoard.container *100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col), &pt_channelSet->type);
				transfer_value_to_char(pt_channelSet->type, pt_content->content[i].val);
				lcd_show_num(pt_channelSet->type, pt_content->content[i].valueX, pt_content->content[i].valueY, holder.foreground, holder.background, *(holder.font), 0, 10);	
			
			}
		}
	}
}



/*
�������볤��
*/
static void content_input_set(void)
{	
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_CHANNEL)//��������
	{
		input_len_set(g_vm.channelBit);//���볤��
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_PRICE)//�۸�����
	{
		input_len_set(g_vm.priceBit);//���볤��
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_GOODSID)//��Ʒ��������
	{
		input_len_set(g_vm.goodsIdBit);//���볤��
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_STORE)//���/����Ȧ������
	{
		input_len_set(g_vm.storeBit);//���볤��
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_SPRING)//���/����Ȧ������
	{
		input_len_set(g_vm.storeBit);//���볤��
	}	
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_VMID)
	{
		input_len_set(10);//������10λ
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_PASSWORD)
	{
		input_len_set(6);//��������6λ
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_LINKENABLE)
	{
		input_len_set(1);//����ʹ��1λ
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_LANGUAGE)
	{
		input_len_set(1);//����1λ
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_DRVBIT)
	{
		input_len_set(1);//��������1λ
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_POINTBIT)
	{
		input_len_set(1);//С����1λ
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_CHANNELBIT)
	{
		input_len_set(1);//����λ��1λ
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_PRICEBIT)
	{
		input_len_set(1);//�۸�λ��1λ
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_STOREBIT)
	{
		input_len_set(1);//���λ��1λ
	}	
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_CHANNLTYPE)//����/ʹ������
	{
		if(g_vm.channelBit == 1)
		input_len_set(10);
		else
		input_len_set(1);
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
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_CHANNEL)//��������
	{

	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_SPRING)//��������
	{
		if(input_len_get() != 0)
		{
			if(input_value_get() != pt_channelSet->spring)
			{
				transfer_value_to_char(input_value_get(), pt_content->content[holder.curSelectId -1].val);
				pt_channelSet->spring = input_value_get();
				if(g_vm.channelBit == 1)
					e2p_set_spring((g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 ), &pt_channelSet->spring);
				else
					e2p_set_spring((g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col), &pt_channelSet->spring);
				
				//�㱨��̨
				report_channel_set(C_CHANNEL_SPRING, pt_channelSet->spring);
			}
			//ˢ�µ�ǰ��
			cur_item_fls();
			lcd_show_character(&pt_word.warring[0]);//���óɹ�
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//����ʧ��
		}
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_CHANNLTYPE)//����ʹ������
	{
		if( (g_vm.channelBit !=1 && input_value_get() < 2 && input_len_get() != 0))//����һ������������
		{
			if(input_value_get() != pt_channelSet->type)
			{
				transfer_value_to_char(input_value_get(), pt_content->content[holder.curSelectId -1].val);
				pt_channelSet->type = input_value_get();
				e2p_set_channelEnable((g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col), &pt_channelSet->type);
				//�㱨��̨
				report_channel_set(C_CHANNEL_ENABLE, pt_channelSet->type);
			}
			lcd_show_character(&pt_word.warring[0]);//���óɹ�
		}
		else
		if(g_vm.channelBit == 1 && input_len_get() == 10)//����һ�����̵�����
		{
			for(g_trade.curDiverBoard.col = 0; g_trade.curDiverBoard.col < 10; g_trade.curDiverBoard.col++)
			{
				pt_channelSet->type = input_char_get()[g_trade.curDiverBoard.col] - '0';
				pt_content->content[holder.curSelectId -1].val[g_trade.curDiverBoard.col] =  input_char_get()[g_trade.curDiverBoard.col];
				e2p_set_channelEnable((g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col), &pt_channelSet->type);
				//�㱨��̨
				report_channel_set(C_CHANNEL_ENABLE, pt_channelSet->type);
			}
			lcd_show_character(&pt_word.warring[0]);//���óɹ�
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//����ʧ��

		}		
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
	pt_channelSet = _malloc(sizeof(SETUP_CHANNEL), &err);
	assert_param(pt_channelSet);
	
	pt_content = _malloc(sizeof(AGGREGATE), &err); 
	assert_param(pt_content);
	
	
	/*
	�ڶ��������ݳ�ʼ��
	*/
	pt_content->content[0].item = &pt_word.set[50];//"����:";
	pt_content->content[0].valueX = pt_word.set[50].valx ; pt_content->content[0].valueY = pt_word.set[50].valy;
	pt_content->content[0].val[0] = '\0';
	pt_content->content[0].id = 0;
	pt_content->content[0].inputContent = CONTENT_CHANNEL;
	
	
	pt_content->content[1].item = &pt_word.set[51];//"����Ȧ��:"; 
	pt_content->content[1].valueX = pt_word.set[51].valx ; pt_content->content[1].valueY = pt_word.set[51].valy;
	pt_content->content[1].val[0] = '\0';
	pt_content->content[1].id = 1;
	pt_content->content[1].inputContent = CONTENT_SPRING;//��������
	
	
	pt_content->content[2].item = &pt_word.set[52];//"״̬(0����1ʹ��):"
	pt_content->content[2].valueX = pt_word.set[52].valx ; pt_content->content[2].valueY = pt_word.set[52].valy;
	pt_content->content[2].val[0] = '\0';
	pt_content->content[2].id = 2;
	pt_content->content[2].inputContent = CONTENT_CHANNLTYPE;


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
	
	content_input_set();//�������볤��
}

/*
�ͷ���ʾ����
�����_malloc��ȫ��ָ�룬���������ͷ�ָ������
���ֻ��һ�δ�ӡ����Ļ�ϵ����ݣ�����žͺ�
*/
static void content_free(void)
{
	_free(pt_content);
	_free(pt_channelSet);
}



/*
�������ý���״̬��
*/
uint8_t fsm_fun_channelSet(FSMMSG* cmsg)
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
				//content_value_read();
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
				//if( (input_len_get() == 2 && g_vm.channelBit < 3) || (input_len_get() == 2 && g_vm.channelBit == 3) )
				if(select_state_get())
				{
					content_page_down(&holder);
					input_clear();
					content_input_set();
				}
			}
			else
			if(cmsg->value == KEY_UP_VAL)//�Ϸ�
			{
//				if( (input_len_get() == 2 && g_vm.channelBit < 3) || (input_len_get() == 2 && g_vm.channelBit == 3) )
				if(select_state_get())
				{
					/*
					������˳�򲻵ô��ң�1.ѡ��仯��2.�ؼ���λ��3.�������ݳ�������
					*/
					content_page_up(&holder);
					input_clear();
					content_input_set();
				}
			}
			else
			if(cmsg->value == '#')//���ݱ���
			{
				if(holder.curSelectId != 1)
				content_input_save();
			}
			else
			{
				if(holder.contentList[holder.curSelectId-1].inputContent == CONTENT_CHANNEL)//��������
				{
					select_goods(cmsg->value);
					transfer_value_to_char(g_trade.select.value, pt_content->content[0].val);
//					show_string(select_char_get(),holder.contentList[holder.curSelectId-1].valueX, holder.contentList[holder.curSelectId-1].valueY);
					lcd_show_string(select_char_get(),holder.contentList[holder.curSelectId-1].valueX, holder.contentList[holder.curSelectId-1].valueY, holder.numcolor, holder.background, *(holder.font));
					if(select_state_get())//���ѡ�û�
					{
						content_value_read();//��ʾ������������
					}
				}
				else
				if(holder.contentList[holder.curSelectId-1].inputContent == CONTENT_PRICE)//�������
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
				|| holder.contentList[holder.curSelectId-1].inputContent == CONTENT_SPRING
				|| holder.contentList[holder.curSelectId-1].inputContent == CONTENT_STOREBIT
				|| holder.contentList[holder.curSelectId-1].inputContent == CONTENT_VMID
				|| holder.contentList[holder.curSelectId-1].inputContent == CONTENT_PRICEBIT)//ֵ����
				{
					input_num(cmsg->value);
					//��ʾ������ֵ
					lcd_show_string(input_char_get(),holder.contentList[holder.curSelectId-1].valueX, holder.contentList[holder.curSelectId-1].valueY,  holder.numcolor, holder.background, *(holder.font));
				}
				else
				if(holder.contentList[holder.curSelectId-1].inputContent == CONTENT_CHANNLTYPE)//����ʹ������
				{
					if(cmsg->value < '2')
					{
						input_num(cmsg->value);
						//��ʾ����ʹ����ֵ
						lcd_show_string(input_char_get(),holder.contentList[holder.curSelectId-1].valueX, holder.contentList[holder.curSelectId-1].valueY, holder.numcolor, holder.background, *(holder.font));
					}
				}
			}
		break;
	}
	return 0;
}


