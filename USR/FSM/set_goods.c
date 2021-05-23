/*
��Ʒ����
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
static SETUP_CHANNEL *pt_channelSet;



/*
��ȡҪ��ʾ������
*/
static void content_value_read(void)
{
	//��ȡ�����۸񣬿�棬��Ʒid
	uint8_t i;
	
	if(g_vm.channelBit == 1) g_trade.curDiverBoard.col = 0;//��������̳�������ÿ��0�����洢���̿��
	
	e2p_get_channel((g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col), pt_channelSet);
	
	for(i = 0; i < holder.size; i++)
	{
		if(holder.contentList[i].inputContent == CONTENT_PRICE)//���۸�
		{
			transfer_value_to_money(pt_channelSet->price, pt_content->content[i].val);//�������ݿ���
			if(i < holder.shownumPage)
			{
				//�Ȱ�ǰ����ʾ��ˢ��(��������൱��ˢ��11���ַ��Ŀո���������Ҫͬһ�еļ�೬��11���ַ�λ)
				lcd_show_string(input_char_get() , pt_content->content[i].valueX, pt_content->content[i].valueY, holder.numcolor, holder.background, *(holder.font));
				//��ʾ��ֵ
				lcd_show_money(pt_channelSet->price , pt_content->content[i].valueX, pt_content->content[i].valueY, holder.foreground, holder.background, *(holder.font));
			}
		}
		else
		if(holder.contentList[i].inputContent == CONTENT_STORE)//�����
		{
			transfer_value_to_char(pt_channelSet->store, pt_content->content[i].val);//�������ݿ���
			if(i < holder.shownumPage)
			{
				//ˢ�´�����ʾ
				lcd_show_string(input_char_get() , pt_content->content[i].valueX, pt_content->content[i].valueY, holder.foreground, holder.background, *(holder.font));
				//��ʾ���(������ʱ��ʾ�����ÿ��)
				lcd_show_num(pt_channelSet->store, pt_content->content[i].valueX, pt_content->content[i].valueY, holder.foreground, holder.background, *(holder.font) ,0 ,10);
			
				if(g_vm.channelBit == 1)
				{
					uint8_t num;
					uint16_t val;
					//��ʾ�������
					lcd_show_num(bkp_trayStore_get(g_trade.curDiverBoard.tray), pt_content->content[i].valueX + 4*8, pt_content->content[i].valueY, holder.foreground, holder.background, *(holder.font) ,0 ,10);
					//��ʾʵ�ʿ��
					bkp_trayStatistic_get(g_trade.curDiverBoard.tray, &num, &val);
					lcd_show_num(pt_channelSet->store - num, pt_content->content[i].valueX + 8*8, pt_content->content[i].valueY, holder.foreground, holder.background, *(holder.font) ,0 ,10);
				}
			
			}
		}
		else
		if(holder.contentList[i].inputContent == CONTENT_GOODSID)//������
		{
			char tmp[11];
			sprintf(tmp, "%06u", pt_channelSet->code);//�̶�6λ����
			strcpy(pt_content->content[i].val, tmp);
			if(i < holder.shownumPage)//�������һ����Ļ�ڣ�����ʾ����
			{
				//ˢ�´�����ʾ
				lcd_show_string(input_char_get() , pt_content->content[i].valueX, pt_content->content[i].valueY, holder.foreground, holder.background, *(holder.font));
				//��ʾ��ֵ
				lcd_show_string(pt_content->content[i].val, pt_content->content[i].valueX, pt_content->content[i].valueY, holder.foreground, holder.background, *(holder.font));
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
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_STORE)//�������
	{
		input_len_set(g_vm.storeBit);//���볤��
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
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_PRICE)//�۸�����
	{
		if(input_len_get() != 0 && input_value_get() != 0)//�۸���Ϊ0
		{
			//�ж��Ƿ����ݲ�ͬ
			if(input_value_get() != pt_channelSet->price)
			{
				pt_channelSet->price = input_value_get();
				transfer_value_to_money(input_value_get() , pt_content->content[holder.curSelectId -1].val);
				if(g_vm.channelBit == 1) g_trade.curDiverBoard.col = 0;
				
				//����۸��У��
				pt_channelSet->priceCHK = ((pt_channelSet->price >> 24)&0xff) + ((pt_channelSet->price >> 16)&0xff) + ((pt_channelSet->price >> 8)&0xff) + (pt_channelSet->price & 0xff);
				//����۸�
				e2p_set_price((g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col),  &pt_channelSet->price);
				//�㱨��̨
				report_channel_set(C_CHANNEL_PRICE, pt_channelSet->price);
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
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_GOODSID)//��Ʒ��������
	{
		if(input_len_get() == g_vm.goodsIdBit)//��Ʒ���볤�ȷ���
		{
			//Ȼ���ж��Ƿ����ݲ�ͬ
			if(input_value_get() != pt_channelSet->code)
			{
				input_char_get()[input_len_get()] = 0;
				strcpy(pt_content->content[holder.curSelectId -1].val, input_char_get());
				pt_channelSet->code = input_value_get();
				if(g_vm.channelBit == 1) g_trade.curDiverBoard.col = 0;
				//������Ʒ���
				e2p_set_channelCode((g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col), &pt_channelSet->code);
				//�㱨��̨
				report_channel_set(C_CHANNEL_GOODSID, pt_channelSet->code);
			}
//			//ˢ�µ�ǰ��
//			cur_item_fls();

			lcd_show_character(&pt_word.warring[0]);//���óɹ�
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//����ʧ��
		}
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_STORE)//�������
	{
		if(input_len_get() != 0 )
		{
			if(g_vm.channelBit > 1)//����1λ�Ļ�����
			{
				if(pt_channelSet->spring >= input_value_get())
				{
					transfer_value_to_char(input_value_get(), pt_content->content[holder.curSelectId -1].val);
					pt_channelSet->store = input_value_get();
					e2p_set_store((g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col), &pt_channelSet->store);
					//�㱨��̨
					report_channel_set(C_CHANNEL_STORE, pt_channelSet->store);
					lcd_show_character(&pt_word.warring[0]);//"���óɹ�"
					//؛����0�_ʼ����
					loop_channel_set(g_trade.curDiverBoard.tray, 0);
					
					//ˢ�µ�ǰ��
					cur_item_fls();
				}
				else
				{
					lcd_show_character(&pt_word.warring[1]);//"����ʧ��"
				}
			}
			else
			if(g_vm.channelBit == 1)//1λ�����ţ�����ѭ������
			{
				uint8_t i, j;
				g_trade.curDiverBoard.col = 0;
				for(i = 0, j = 0; i < MAX_COLUMN_SIZE; i++)
				{
					e2p_get_channelEnable(g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + i, &pt_channelSet->type);
					if(pt_channelSet->type == 1)//��������
						j++;
				}
				if(j*pt_channelSet->spring >= input_value_get() )//����� = ����������*����������
				{
					transfer_value_to_char(input_value_get(), pt_content->content[holder.curSelectId -1].val);
					pt_channelSet->store = input_value_get();
					//������
					e2p_set_store((g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + g_trade.curDiverBoard.col), &pt_channelSet->store);
					
					bkp_trayStore_set(g_trade.curDiverBoard.tray, pt_channelSet->store);//���¿������
					for(i =0; i < MAX_COLUMN_SIZE; i++)//����˲����ͳ��
					{
						e2p_set_channelStatistic(g_trade.curDiverBoard.container*100 + g_trade.curDiverBoard.tray*10 + i, 0, 0);
					}
					bkp_trayStatistic_set(g_trade.curDiverBoard.tray, 0, 0);	//����˲�ͳ��
					report_channel_set(C_CHANNEL_STORE, pt_channelSet->store);	//�㱨��̨
					lcd_show_character(&pt_word.warring[0]);					//���óɹ�
					loop_channel_set(g_trade.curDiverBoard.tray, 0);			//؛����0�_ʼ����
					//ˢ�µ�ǰ��
					cur_item_fls();
					
					{
					uint8_t num;
					uint16_t val;
					//��ʾ�������
					lcd_show_num(bkp_trayStore_get(g_trade.curDiverBoard.tray), pt_content->content[holder.curSelectId -1].valueX + 4*8, pt_content->content[holder.curSelectId -1].valueY, holder.foreground, holder.background, *(holder.font) ,0 ,10);
					//��ʾʵ�ʿ��
					bkp_trayStatistic_get(g_trade.curDiverBoard.tray, &num, &val);
					lcd_show_num(pt_channelSet->store - num, pt_content->content[holder.curSelectId -1].valueX + 8*8, pt_content->content[holder.curSelectId -1].valueY, holder.foreground, holder.background, *(holder.font) ,0 ,10);
					}
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
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//����ʧ��
		}
	}
}


/*
������ʾ����
����Ķ���ʾ����˳��ֻҪ�������������
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
	
	err = 0;
	/*
	�ڶ��������ݳ�ʼ��
	*/ 
	pt_content->content[0].item = &pt_word.set[14];//"��Ʒ:" 
	pt_content->content[0].valueX = pt_word.set[14].valx ; pt_content->content[0].valueY = pt_word.set[14].valy;
	pt_content->content[0].val[err] = '\0'; 
	pt_content->content[0].id = 0;
	pt_content->content[0].inputContent = CONTENT_CHANNEL;//���������ǻ���
	
	
	pt_content->content[1].item = &pt_word.set[15];//"���:"; 
	pt_content->content[1].valueX = pt_word.set[15].valx ; pt_content->content[1].valueY = pt_word.set[15].valy;
	pt_content->content[1].val[err] = '\0';
	pt_content->content[1].id = 1;
	pt_content->content[1].inputContent = CONTENT_STORE; 
	
	
	pt_content->content[2].item = &pt_word.set[16];//"�۸�:"; 
	pt_content->content[2].valueX = pt_word.set[16].valx ; pt_content->content[2].valueY = pt_word.set[16].valy;
	pt_content->content[2].val[err] = '\0';
	pt_content->content[2].id = 2;
	pt_content->content[2].inputContent = CONTENT_PRICE; 

	pt_content->content[3].item = &pt_word.set[17];// "����:";  
	pt_content->content[3].valueX = pt_word.set[17].valx ; pt_content->content[3].valueY = pt_word.set[17].valy;
	pt_content->content[3].val[err] = '\0'; 
	pt_content->content[3].id = 3;
	pt_content->content[3].inputContent = CONTENT_GOODSID;

//	pt_content->content[4].item = &pt_word.set[17];// "���ֵ:"
//	pt_content->content[4].valueX = pt_word.set[17].valx ; pt_content->content[3].valueY = pt_word.set[17].valy;
//	pt_content->content[4].val[err] = '\0'; 
//	pt_content->content[4].id = 4;

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
	holder.contentList = pt_content->content;		//�˵�����
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
	_free(pt_channelSet);
}



/*
��Ʒ���ý���״̬��
*/
uint8_t fsm_fun_goodsSet(FSMMSG* cmsg)
{
	switch (cmsg->type)
	{
		case MsgSYS:
			if(cmsg->stype == FSM_STATUS_INIT)
			{
				input_clear();//����ؼ�����
				select_clear();//��������ؼ�����
				if(g_fsm.shortcut == 1)
					lcd_clear();
				else
					LCD_CLEAR_SIDE;////ˢ�ұ���
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
				if(g_fsm.shortcut == 1)
				{
					Load_status(fsm_id_init);//���س�ʼ����
				}
				else
				{
					Load_status(fsm_id_seniorMenu);//�����Ȩ�޲˵�
				}
			}
			else 
			if(cmsg->value == KEY_DOWN_VAL)//�·�
			{
				if(select_state_get())
				{
					content_page_down(&holder);
					input_clear();
					content_input_set();
				}
			}
			else
			if(cmsg->value == KEY_UP_VAL)
			{
				if(select_state_get())//ֻ��ѡ����Ʒ�������·�
				{
					/*
					������˳�򲻵ô��ң�1.ѡ��仯��2.�ؼ���λ��3.��ǰ�������ݳ�������
					*/
					content_page_up(&holder);
					input_clear();
					content_input_set();
				}
			}
			else
			if(cmsg->value == '#')//���ݱ���
			{
				if(holder.contentList[holder.curSelectId-1].inputContent != CONTENT_CHANNEL)//�����ǲ��ܱ����
				{
					content_input_save();
				}
			}
			else//����Ҳ��������������������
			{
				if(holder.contentList[holder.curSelectId-1].inputContent == CONTENT_CHANNEL)//��������
				{
					select_goods(cmsg->value);
					transfer_value_to_char(g_trade.select.value, pt_content->content[0].val);
					lcd_show_string(select_char_get(),holder.contentList[holder.curSelectId-1].valueX, holder.contentList[holder.curSelectId-1].valueY, holder.numcolor, holder.background, *(holder.font));
					if(select_state_get())//���ѡ�û�
					{
						content_value_read();//��ʾ������������
					}
				}
				else
				if(holder.contentList[holder.curSelectId-1].inputContent == CONTENT_PRICE)//�������
				{
					input_money(cmsg->value);//������
					lcd_show_string(input_char_get(), holder.contentList[holder.curSelectId-1].valueX, holder.contentList[holder.curSelectId-1].valueY, holder.numcolor, holder.background, *(holder.font));
				}
				else
				if(holder.contentList[holder.curSelectId-1].inputContent == CONTENT_GOODSID
				|| holder.contentList[holder.curSelectId-1].inputContent == CONTENT_STORE)//ֵ����
				{
					input_num(cmsg->value);
					lcd_show_string(input_char_get(),holder.contentList[holder.curSelectId-1].valueX, holder.contentList[holder.curSelectId-1].valueY, holder.numcolor, holder.background, *(holder.font));
				}
				
			}
		break;
	}
	return 0;
}


