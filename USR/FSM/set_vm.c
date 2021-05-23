/*
����ͨ������
״̬��ʵ��

*/

#include "header.h"

#define HOLDER_SIZE				14			//�˲˵���������������µĲ˵���Ҫ�ı������С��
/*
����һ���ṹ��,�����ݵļ���
˵��������Ϊһ���ṹ���Ǻ���Ҫ�ģ�
��֤���ڴ���������
*/
typedef struct AGGREGATE{
	CONTENT content[HOLDER_SIZE];//���������Ҫע������
}AGGREGATE;


/*
Ϊ�˽�ʡ�ڴ棬ʹ��ָ��ָ��̬�����ڴ�
�˳�״̬�����ͷ��ڴ�ռ䣬
�����ڴ�й©
*/
static SETUP_DEVICE *mac_data;
static SETUP_COMMON *vm_common ;
static SETUP_COMMUCATION *comucation ;
static AGGREGATE *content;//ָ�����ݵļ���


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
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_TRAYNUM)
	{
		input_len_set(1);//���λ��1λ
	}
}


/*
���뱣��
*/
static void content_input_save(void)
{
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_VMID)//����������
	{
		
		if(input_len_get() == 10 &&  input_char_get()[0]< '4')//Ҫ��10λ���벢����λ��ҪС��4
		{
			if(mac_data->facId != input_value_get())
			{
				mac_data->facId = input_value_get();
				input_char_get()[input_len_get()] = 0; //ĩβ�ַ�����(����Ƚ����⣬��ΪҪ���ǹ̶�����)
				strcpy(holder.contentList[holder.curSelectId-1].val, input_char_get());
				e2p_set_vminfo(mac_data);//��������
				//����ȫ�ֱ�����û�У�
				//�㱨(����Ҫ)
			}
			lcd_show_character(&pt_word.warring[0]);//���óɹ�
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//����ʧ��
		}
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_PASSWORD)//������������
	{
		if(input_len_get() == 6)//Ҫ��6λ����
		{
			if(comucation->password != input_value_get() )
			{
				comucation->password = input_value_get();
				input_char_get()[input_len_get()] = 0;  //ĩβ�ַ����㣨����Ƚ����⣬��ΪҪ���ǹ̶����ȣ�
				strcpy(holder.contentList[holder.curSelectId-1].val, input_char_get());
				e2p_set_commucation(comucation);//�������� 
				//����Ҫ����ȫ�ֱ���
				//����Ҫ�㱨
			}
			lcd_show_character(&pt_word.warring[0]);//���óɹ�
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//����ʧ��
		}
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_LINKENABLE)//����ʹ������
	{
		if(input_value_get() < 2 && input_len_get() != 0)//����ʹ�ܵ����ֵ
		{
			if(comucation->enable != input_value_get())
			{
				comucation->enable = input_value_get();
				transfer_value_to_char(comucation->enable, holder.contentList[holder.curSelectId-1].val);
				e2p_set_commucation(comucation);//��������
				//����Ҫ����ȫ�ֱ���
				//����Ҫ�㱨
			}
			lcd_show_character(&pt_word.warring[0]);//���óɹ�
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//����ʧ��
		}
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_LANGUAGE)//��������
	{
		if(input_value_get() < 2 && input_len_get() != 0)//���Ե����ֵ
		{
			if(vm_common->language != input_value_get())
			{
				vm_common->language = input_value_get();
				transfer_value_to_char(vm_common->language, holder.contentList[holder.curSelectId-1].val);
				e2p_get_common(vm_common);//��������
				
				//�㱨��̨
				report_device_set(PARAM_LANGUAGE, vm_common->language);
				
			}
			lcd_show_character(&pt_word.warring[0]);//���óɹ�

		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//����ʧ��
		}
}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_DRVBIT)//������������
	{
		if(input_value_get() < 10 && input_value_get() > 0)//�����������ֵ
		{
			if(vm_common->drvnum != input_value_get())
			{
				vm_common->drvnum = input_value_get();
				transfer_value_to_char(vm_common->drvnum, holder.contentList[holder.curSelectId-1].val);
				e2p_set_common(vm_common);//��������
				g_vm.maxContainerNum = vm_common->drvnum;//����ȫ�ֱ���
				
				//�㱨��̨
				report_device_set(PARAM_CONTAINER_BIT, vm_common->drvnum);
				{
					//������Ҫ���⴦�������������ؼ������ͷ�������ռ䣬
					//����������
					
				}
			}
			lcd_show_character(&pt_word.warring[0]);//���óɹ�
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//����ʧ��
		}
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_POINTBIT)//С����λ������
	{
		if(input_value_get() < 9 && input_len_get() != 0)//С����λ�����9
		{
			if(vm_common->point != input_value_get())
			{
				vm_common->point = input_value_get();
				transfer_value_to_char(vm_common->point, holder.contentList[holder.curSelectId-1].val);
				e2p_set_common(vm_common);//��������
				//����Ҫ�㱨
				g_vm.point =vm_common->point; //����ȫ�ֱ���
				//�㱨��̨
				report_device_set(PARAM_POINT_BIT, vm_common->point);
			}
			lcd_show_character(&pt_word.warring[0]);//���óɹ�
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//����ʧ��
		}
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_CHANNELBIT)//�������λ��
	{
		if(input_value_get() < 4 && input_value_get() > 0)//Ŀǰֻ��������1~3
		{
			if(vm_common->numChannel != input_value_get() )
			{
				vm_common->numChannel = input_value_get();
				transfer_value_to_char(vm_common->numChannel, holder.contentList[holder.curSelectId-1].val);
				e2p_set_common(vm_common);//��������
				g_vm.channelBit = vm_common->numChannel;//����ȫ�ֱ���
				//�㱨��̨
				report_device_set(PARAM_CHANNEL_BIT, vm_common->numChannel);
			}
			lcd_show_character(&pt_word.warring[0]);//���óɹ�
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//����ʧ��
		}
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_PRICEBIT)//����۸�λ��
	{
		if(input_value_get() < 10 && input_value_get() > 0)//�۸�λ�����ֵ
		{
			if(vm_common->numPrice != input_value_get())
			{
				vm_common->numPrice = input_value_get();
				transfer_value_to_char(vm_common->numPrice, holder.contentList[holder.curSelectId-1].val);
				e2p_set_common(vm_common);//��������
				g_vm.priceBit = vm_common->numPrice;//����ȫ�ֱ���
				//�㱨��̨
				report_device_set(PARAM_PRICE_BIT, vm_common->numPrice);
			}
			lcd_show_character(&pt_word.warring[0]);//���óɹ�
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//����ʧ��
		}
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_STOREBIT)//���뵯��/���λ��
	{
		if(input_value_get() < 10 && input_value_get() > 0)//��浯��λ���������ֵ
		{
			if(vm_common->numStore != input_value_get())
			{
				vm_common->numStore = input_value_get();
				transfer_value_to_char(vm_common->numStore, holder.contentList[holder.curSelectId-1].val);
				e2p_set_common(vm_common);//��������
				g_vm.storeBit = vm_common->numStore;//����ȫ�ֱ���
				//�㱨��̨
				report_device_set(PARAM_INPUT_BIT, vm_common->numStore);
			}
			lcd_show_character(&pt_word.warring[0]);//���óɹ�
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//����ʧ��
		}
	}	
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_TRAYNUM)//���̸�������
	{
		if(input_value_get() < 9 && input_len_get() != 0)//�����������8
		{
			if(vm_common->traynum != input_value_get())
			{
				vm_common->traynum = input_value_get();
				transfer_value_to_char(vm_common->traynum, holder.contentList[holder.curSelectId-1].val);
				e2p_set_common(vm_common);//��������
				g_vm.maxtrayNum = vm_common->traynum;//����ȫ�ֱ���
				//�㱨��̨
				report_device_set(PARAM_TRAT_SIZE, vm_common->traynum);
			}
			lcd_show_character(&pt_word.warring[0]);//���óɹ�
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//����ʧ��
		}
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_DATA)//��������
	{
		if(input_len_get() == 10 )
		{
			if(set_rtc_time(0, input_value_get()/10000, input_value_get()%10000/100, input_value_get()%10000%100, calendar.hour, calendar.min, calendar.sec))
			{
				lcd_show_character(&pt_word.warring[0]);//""���óɹ�
				transfer_value_to_data(input_value_get(), holder.contentList[holder.curSelectId -1].val);
			}
			else
			{
				lcd_show_character(&pt_word.warring[1]);//"����ʧ��"
			}
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//"����ʧ��"
		}
	}
	else
	if(holder.contentList[holder.curSelectId -1].inputContent == CONTENT_TIME)//ʱ������
	{
		if(input_len_get() == 5 )
		{
			if(set_rtc_time(0, calendar.w_year, calendar.w_month, calendar.w_date, input_value_get()/100, input_value_get()%100, 0))
			{
				lcd_show_character(&pt_word.warring[0]);//"���óɹ�"
				transfer_value_to_time(input_value_get(), holder.contentList[holder.curSelectId -1].val);
			}
			else
			{
				lcd_show_character(&pt_word.warring[1]);//"����ʧ��"
			}
		}
		else
		{
			lcd_show_character(&pt_word.warring[1]);//"����ʧ��"
		}
	}
}

/*
��ȡҪ��ʾ����
*/
static void content_value_read(void)
{
	uint8_t err;
	
	//����ռ�
//	mac_data = 0;//��Ϊ_malloc�ܻ᷵��ֵ���������ﲻ��Ҫ��ʼ��ָ��
	mac_data = _malloc(sizeof(SETUP_DEVICE), &err);
	assert_param(mac_data);
	
//	vm_common = 0;
	vm_common = _malloc(sizeof(SETUP_COMMON), &err);
	assert_param(vm_common);
	
//	comucation = 0;
	comucation = _malloc(sizeof(SETUP_COMMUCATION), &err);
	assert_param(comucation);
	
//	content = 0;
	content = _malloc(sizeof(AGGREGATE), &err); 
	assert_param(content);
	
	
	//��ȡ����
	e2p_get_common(vm_common);
	e2p_get_vminfo(mac_data);
	e2p_get_commucation(comucation);
	
	
}

/*
������ʾ����
*/
static void content_creat(void)
{	
	char tmp[11];tmp[10] = 0;
	
	content->content[0].item = &pt_word.set[18];	//"����ID:"
	content->content[0].valueX = pt_word.set[18].valx;content->content[0].valueY = pt_word.set[18].valy;
	sprintf(tmp, "%010u", mac_data->facId);
	strcpy(content->content[0].val, tmp);
	content->content[0].id = 0;
	content->content[0].inputContent = CONTENT_VMID;
	
	content->content[1].item = &pt_word.set[19];	//"��������:"
	content->content[1].valueX = pt_word.set[19].valx;content->content[1].valueY = pt_word.set[19].valy;
	sprintf(tmp, "%06u", comucation->password);
	strcpy(content->content[1].val, tmp);
	content->content[1].inputContent = CONTENT_PASSWORD;
	

	content->content[2].item = &pt_word.set[20];	//"����(1�� 0��):"
	content->content[2].valueX = pt_word.set[20].valx;content->content[2].valueY = pt_word.set[20].valy;
	transfer_value_to_char(comucation->enable, content->content[2].val);
	content->content[2].inputContent = CONTENT_LINKENABLE;
	
	content->content[3].item = &pt_word.set[21];	//"����(1���� 0Ӣ��):
	content->content[3].valueX = pt_word.set[21].valx;content->content[3].valueY = pt_word.set[21].valy;
	transfer_value_to_char(vm_common->language, content->content[3].val);
	content->content[3].inputContent = CONTENT_LANGUAGE;

	content->content[4].item = &pt_word.set[22];	//"��������:"
	content->content[4].valueX = pt_word.set[22].valx;content->content[4].valueY = pt_word.set[22].valy;
	transfer_value_to_char(vm_common->drvnum, content->content[4].val);
	content->content[4].inputContent = CONTENT_DRVBIT;
	
	content->content[5].item = &pt_word.set[23];	//"С����λ��:"
	content->content[5].valueX = pt_word.set[23].valx;content->content[5].valueY = pt_word.set[23].valy;
	transfer_value_to_char(vm_common->point, content->content[5].val);
	content->content[5].inputContent = CONTENT_POINTBIT;

	content->content[6].item = &pt_word.set[24];	//"�۸�λ��:"
	content->content[6].valueX = pt_word.set[24].valx;content->content[6].valueY = pt_word.set[24].valy;
	transfer_value_to_char(vm_common->numPrice, content->content[6].val);
	content->content[6].inputContent = CONTENT_PRICEBIT;
	
	content->content[7].item = &pt_word.set[25];	//"���/��������λ��"; 
	content->content[7].valueX = pt_word.set[25].valx;content->content[7].valueY = pt_word.set[25].valy;
	transfer_value_to_char(vm_common->numStore, content->content[7].val);
	content->content[7].inputContent = CONTENT_STOREBIT;
	
	content->content[8].item = &pt_word.set[26];	//"����λ��:"
	content->content[8].valueX = pt_word.set[26].valx;content->content[8].valueY = pt_word.set[26].valy;
	transfer_value_to_char(vm_common->numChannel, content->content[8].val);
	content->content[8].inputContent = CONTENT_CHANNELBIT;
	
	content->content[9].item = &pt_word.set[57];	//"��������:"
	content->content[9].valueX = pt_word.set[57].valx;content->content[9].valueY = pt_word.set[57].valy;
	transfer_value_to_char(vm_common->traynum, content->content[9].val);
	content->content[9].inputContent = CONTENT_TRAYNUM;
	
	content->content[13].item = &pt_word.set[27];	//"�ظ�����(1 ȷ��):
	content->content[13].valueX = pt_word.set[27].valx;content->content[13].valueY = pt_word.set[27].valy;
	content->content[13].inputContent = CONTENT_RESET;
	content->content[13].val[0] = '\0';
	
	content->content[12].item = &pt_word.set[58];	//"�������(1 ȷ��):
	content->content[12].valueX = pt_word.set[58].valx;content->content[12].valueY = pt_word.set[58].valy;
	content->content[12].inputContent = CONTENT_DATARESET;
	content->content[12].val[0] = '\0';
	
	content->content[10].item = &pt_word.set[60];	//"����"
	content->content[10].valueX = pt_word.set[60].valx;content->content[10].valueY = pt_word.set[60].valy;
	content->content[10].inputContent = CONTENT_DATA;
	transfer_value_to_data(calendar.w_year*10000 + calendar.w_month*100 + calendar.w_date, content->content[10].val);
	
	content->content[11].item = &pt_word.set[61];	//"ʱ��"
	content->content[11].valueX = pt_word.set[61].valx;content->content[11].valueY = pt_word.set[61].valy;
	content->content[11].inputContent = CONTENT_TIME;
	transfer_value_to_time(calendar.hour*100 + calendar.min, content->content[11].val);
	
	//�˵�����
	holder.size = HOLDER_SIZE;				//������С
	holder.font = G_HOLDER_FONT; 				//��������
	holder.shownumPage = G_HOLDER_ITEM_SIZE;	//һҳ��ʾ���ٸ�����
	holder.zicolor = G_HOLDER_CUR_ZI_COLOR;	//��ǰѡ��������ɫ
	holder.numcolor = G_HOLDER_CUR_NAME_COLOR;//��ǰѡ��������ɫ
	holder.foreground = G_HOLDER_FOREGROUND;	//ǰ��ɫ
	holder.background = G_HOLDER_BACKGROUND;	//����ɫ
	holder.contentList = content->content;	//����ָ��
	contant_load(&holder);
	
	content_input_set();//����Ĭ�ϳ���
}

/*
�ͷ���ʾ����
�����_malloc��ȫ��ָ�룬���������ͷ�ָ������
���ֻ��һ�δ�ӡ����Ļ�ϵ����ݣ�����žͺ�
*/
static void content_free(void)
{
	_free(mac_data);
	_free(vm_common);
	_free(comucation);
	_free(content);
}
/*
ָ���ʼ������ʽָ��δ���루_malloc()��������_free()
��ֹҰָ����ֵ��µ�_free�ͷ���Ч���ݣ���ɵ�ϵͳ����
*/
static void pointer_init(void)
{
	mac_data = 0;
	vm_common = 0;
	comucation = 0;
	content = 0;
}

/*
״̬��
*/
enum
{
	PASSWORD_MODE,
	SET_MODE,
};

uint8_t fsm_fun_vmSet(FSMMSG* cmsg)
{
	switch (cmsg->type)
	{
		case MsgSYS:
			if(cmsg->stype == FSM_STATUS_INIT)
			{
				pointer_init();//��ֹҰָ��
				input_clear();//����ؼ�����
				//ˢ�ұ���
				LCD_CLEAR_SIDE;//ˢ��
				
				g_fsm.mode = PASSWORD_MODE; //0:��������ģʽ��1:����ģʽ
				g_fsm.count = 0;
				input_len_set(9);//9λ����
				lcd_show_character(&pt_word.drvset[15]);//�����������롱
				
//				input_clear();//����ؼ�����
//				//ˢ�ұ���
//				LCD_CLEAR_SIDE;//ˢ��
//				content_value_read();
//				//��ʾ����
//				content_creat();
			}
			else
			if(cmsg->stype == FSM_STATUS_EXIT)
			{
				content_free();
				input_clear();//����ؼ�����
			}
		break;
		
		case MsgKEY:
			if(cmsg->value == '*')
			{
				Load_status(fsm_id_seniorMenu);
			}
			else
			{
				//��������ģʽ
				if(g_fsm.mode == SET_MODE)
				{
					lcd_clear_character(&pt_word.warring[0]);//����±���ʾ
					if(cmsg->value == DOOR_CLOSE_VAL)
					{
						Load_status(fsm_id_init);
					}
					else 
					if(cmsg->value == KEY_DOWN_VAL)//�·�
					{
						content_page_down(&holder);
						input_clear();
						content_input_set();
					}
					else
					if(cmsg->value == KEY_UP_VAL)//�Ϸ�
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
						if(holder.contentList[holder.curSelectId-1].inputContent == CONTENT_CHANNEL)//��������
						{
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
						|| holder.contentList[holder.curSelectId-1].inputContent == CONTENT_STOREBIT
						|| holder.contentList[holder.curSelectId-1].inputContent == CONTENT_VMID
						|| holder.contentList[holder.curSelectId-1].inputContent == CONTENT_PRICEBIT
						|| holder.contentList[holder.curSelectId-1].inputContent == CONTENT_TRAYNUM)//ֵ����
						{
							input_num(cmsg->value);
							lcd_show_string(input_char_get(),holder.contentList[holder.curSelectId-1].valueX, holder.contentList[holder.curSelectId-1].valueY, holder.numcolor, holder.background, *(holder.font));
						}
						else
						if(holder.contentList[holder.curSelectId-1].inputContent == CONTENT_RESET)//�ָ�����
						{
							if(cmsg->value == '1')//����ȷ�ϼ�
							{
								lcd_show_character(&pt_word.warring[5]);//���ڻָ���
								bkp_flash_reset();	//�ָ� bkp ���Ͳ��� flash ��
								e2p_reset_all();	//�ָ� E2PROM
								e2p_get_global();	//��ȡȫ�ֱ���
								content_value_read();//���¶�ȡ����
								LCD_CLEAR_SIDE;		//����
								content_creat();	//����������������
								lcd_show_character(&pt_word.warring[6]);//"�ָ��ɹ�"
								
							}
						}
						else
						if(holder.contentList[holder.curSelectId-1].inputContent == CONTENT_DATARESET)//�ָ�����
						{
							if(cmsg->value == '1')//����ȷ�ϼ�
							{
								lcd_show_character(&pt_word.warring[5]);//���ڻָ���
								bkp_flash_reset();	//�ָ� bkp ���Ͳ��� flash ��
								lcd_show_character(&pt_word.warring[6]);//"�ָ��ɹ�"
								
							}
						}
						else
						if(holder.contentList[holder.curSelectId-1].inputContent == CONTENT_DATA)//��������
						{
							input_data(cmsg->value);
							lcd_show_string(input_char_get(),holder.contentList[holder.curSelectId-1].valueX, holder.contentList[holder.curSelectId-1].valueY, holder.numcolor, holder.background, *(holder.font));
						}
						else
						if(holder.contentList[holder.curSelectId-1].inputContent == CONTENT_TIME)//ʱ������
						{
							input_time(cmsg->value);
							lcd_show_string(input_char_get(),holder.contentList[holder.curSelectId-1].valueX, holder.contentList[holder.curSelectId-1].valueY, holder.numcolor, holder.background, *(holder.font));
						}
					}
				}
				//��������ģʽ
				else
				if(g_fsm.mode == PASSWORD_MODE)
				{
					input_num(cmsg->value);
					input_char_get()[g_fsm.count++] = '*';
					g_fsm.count %= 9;
					lcd_show_string(input_char_get(), pt_word.drvset[15].valx, pt_word.drvset[15].valy, pt_word.drvset[15].color, pt_word.drvset[15].background,  *(pt_word.drvset[15].font) );
					if(input_value_get() == 400823823)
					{
						g_fsm.mode = SET_MODE;
						input_clear();//����ؼ�����
						//ˢ�ұ���
						LCD_CLEAR_SIDE;//ˢ��
						content_value_read();
						//��ʾ����
						content_creat();
					}
				}
			}
		break;
			
	}
	return 0;
}


