/*
�˵��ؼ�
*/

#include "header.h"

textbox mytextboxxxxx;
menu mymenu;
HOLDER holder;

void LoadMenu(menu* cmenu)
{
	uint8_t i=0;
	lcd_show_character(cmenu->name); //��ʾ�˵���
	
	cmenu->index[0] = '0';
	cmenu->index[1] = '.';
	cmenu->index[2] = ' ';
	cmenu->index[0] = i +'1';//�±�
	//��ʾ�±�
	//��ʾ�˵���һ��
	lcd_show_string(cmenu->index, cmenu->itemlist[i].item->x, cmenu->itemlist[i].item->y,  cmenu->forceground, cmenu->background, *(cmenu->itemlist[i].item->font));//���Ѹ��������������
	lcd_show_string(cmenu->itemlist[i].item->character, cmenu->itemlist[i].item->x + cmenu->itemlist[i].item->font->size, cmenu->itemlist[i].item->y, cmenu->forceground, cmenu->background, *(cmenu->itemlist[i].item->font));
	
	i++;
	while(i < cmenu->sizePer && i < cmenu->height)
	{
		cmenu->index[0]=i+'1';
		//��ʾ�±�
		//��ʾ�˵�ʣ����
		lcd_show_string(cmenu->index, cmenu->itemlist[i].item->x, cmenu->itemlist[i].item->y, cmenu->itemlist[i].item->color, cmenu->itemlist[i].item->background, *(cmenu->itemlist[i].item->font));
		lcd_show_string(cmenu->itemlist[i].item->character, cmenu->itemlist[i].item->x +cmenu->itemlist[i].item->font->size, cmenu->itemlist[i].item->y, cmenu->itemlist[i].item->color, cmenu->itemlist[i].item->background, *(cmenu->itemlist[i].item->font));
		i++;
	}
	cmenu->selectitem = 1;
}

/*
�˵��·�
*/
void MenuNAVDOWN(menu* cmenu)
{
	uint8_t dfnum;
	if(cmenu->selectitem < cmenu->sizePer && cmenu->selectitem < cmenu->height)//û�й�������ҳ���Ĳ˵���
	{
		dfnum = cmenu->selectitem - 1;
		
		cmenu->index[0] = dfnum + '0' + 1;
		if(cmenu->index[0] > '9')
			cmenu->index[0] = cmenu->index[0]-'9'+'A';
		
		/*
		��ǰ����ʾ������ɫ
		*/
		//��ʾ�±�
		//��ʾ�˵���
		lcd_show_string(cmenu->index, cmenu->itemlist[dfnum].item->x, cmenu->itemlist[dfnum].item->y, cmenu->itemlist[dfnum].item->color, cmenu->itemlist[dfnum].item->background, *(cmenu->itemlist[dfnum].item->font));
		lcd_show_string(cmenu->itemlist[dfnum].item->character, cmenu->itemlist[dfnum].item->x + cmenu->itemlist[dfnum].item->font->size, cmenu->itemlist[dfnum].item->y, cmenu->itemlist[dfnum].item->color, cmenu->itemlist[dfnum].item->background, *(cmenu->itemlist[dfnum].item->font));
		
		
		/*
		��һ����ʾ����
		*/
		(cmenu->selectitem)++;
		dfnum = cmenu->selectitem - 1;
		
		cmenu->index[0] = dfnum + '0' + 1;
		if(cmenu->index[0] > '9')
			cmenu->index[0] = cmenu->index[0]-'9'+'A';
		
		//��ʾ�±�
		//��ʾ�˵���һ��
		lcd_show_string(cmenu->index, cmenu->itemlist[dfnum].item->x, cmenu->itemlist[dfnum].item->y, cmenu->forceground, cmenu->background, *(cmenu->itemlist[dfnum].item->font));
		lcd_show_string(cmenu->itemlist[dfnum].item->character, cmenu->itemlist[dfnum].item->x + cmenu->itemlist[dfnum].item->font->size, cmenu->itemlist[dfnum].item->y, cmenu->forceground, cmenu->background, *(cmenu->itemlist[dfnum].item->font));
	}
	else
	if(cmenu->selectitem == cmenu->height)//���һ��
	{
		cmenu->selectitem = 1;
		LoadMenu(cmenu);
	}
	else//��������������Ĳ˵���
	{
		uint8_t i, j, k;
		dfnum = cmenu->selectitem ;//��ǰ�˵���
		
		//ÿҳҪ��ʾ�Ĳ˵���,ÿ�ι���,ˢ�´�ҳ���в˵���
		for(i = 0, j = 0, k = cmenu->sizePer - 1; i < cmenu->sizePer ; i++)
		{
			cmenu->index[0] = dfnum +'1';
			if(cmenu->index[0]>'9')
			{
				cmenu->index[0] = 'A'+ j;
				j++;
			}
			if(i == 0)//��ҳ���һ��
			{		
				/*
				������ʾ
				*/
				//��ʾ�±�
				//��ʾ�˵���һ��
				lcd_show_string(cmenu->index, cmenu->itemlist[k].item->x, cmenu->itemlist[k].item->y, cmenu->forceground, cmenu->background ,*(cmenu->itemlist[k].item->font));
				lcd_show_string(cmenu->itemlist[dfnum].item->character, cmenu->itemlist[k].item->x + cmenu->itemlist[k].item->font->size, cmenu->itemlist[k].item->y, cmenu->forceground, cmenu->background, *(cmenu->itemlist[k].item->font));
			}
			else
			{
				lcd_show_string(cmenu->index, cmenu->itemlist[k].item->x, cmenu->itemlist[k].item->y, cmenu->itemlist[dfnum].item->color, cmenu->itemlist[dfnum].item->background, *(cmenu->itemlist[k].item->font));
				lcd_show_string(cmenu->itemlist[dfnum].item->character, cmenu->itemlist[k].item->x + cmenu->itemlist[k].item->font->size, cmenu->itemlist[k].item->y, cmenu->itemlist[dfnum].item->color, cmenu->itemlist[dfnum].item->background, *(cmenu->itemlist[k].item->font));
			}
			dfnum --;
			k --;
		}
		cmenu->selectitem ++;
	}
} 

/*
�˵��Ϸ�
*/
void MenuNAVUP(menu* cmenu)
{
	uint8_t dfnum;
	if(cmenu->selectitem <= cmenu->sizePer && cmenu->selectitem <= cmenu->height && cmenu->selectitem != 1)//���������棬����Ҫ����
	{
		dfnum = cmenu->selectitem;
		
		cmenu->index[0] = dfnum + '0';
		if(cmenu->index[0] > '9')
			cmenu->index[0] = cmenu->index[0]- '9' + 'A' - 1;
		
		dfnum --;
		/*
		��ǰ����ʾ������ɫ
		*/
		//��ʾ�±�
		//��ʾ�˵���
		lcd_show_string(cmenu->index,cmenu->itemlist[dfnum].item->x, cmenu->itemlist[dfnum].item->y, cmenu->itemlist[dfnum].item->color, cmenu->itemlist[dfnum].item->background, *(cmenu->itemlist[dfnum].item->font));
		lcd_show_string(cmenu->itemlist[dfnum].item->character, cmenu->itemlist[dfnum].item->x + cmenu->itemlist[dfnum].item->font->size, cmenu->itemlist[dfnum].item->y, cmenu->itemlist[dfnum].item->color, cmenu->itemlist[dfnum].item->background, *(cmenu->itemlist[dfnum].item->font));
		
		(cmenu->selectitem)--;
		
		dfnum = cmenu->selectitem;
		
		cmenu->index[0] = dfnum + '0';
		if(cmenu->index[0] > '9')
			cmenu->index[0] = cmenu->index[0]- '9' + 'A' - 1;
		
		dfnum --;
		//��ʾ�±�
		//��ʾ�˵���һ��
		lcd_show_string(cmenu->index, cmenu->itemlist[dfnum].item->x, cmenu->itemlist[dfnum].item->y, cmenu->forceground, cmenu->background, *(cmenu->itemlist[dfnum].item->font)); 
		lcd_show_string(cmenu->itemlist[dfnum].item->character, cmenu->itemlist[dfnum].item->x + cmenu->itemlist[dfnum].item->font->size, cmenu->itemlist[dfnum].item->y, cmenu->forceground, cmenu->background, *(cmenu->itemlist[dfnum].item->font)); 
	}
	else
	if(cmenu->selectitem == 1)//���һ��
	{
		uint8_t i, j;
		cmenu->selectitem = cmenu->height;
		dfnum = cmenu->selectitem;
		
		j = cmenu->selectitem > cmenu->sizePer ? cmenu->sizePer : cmenu->selectitem;
		
		for(i = j ; i != 0; i--)
		{
			cmenu->index[0] = dfnum + '0';
			if(cmenu->index[0] > '9')
			{
				cmenu->index[0] = cmenu->index[i]-'9'+'A'-1;
			}
			/*
			������ʾ
			*/
			if(i == j)
			{
				
				lcd_show_string(cmenu->index, cmenu->itemlist[i - 1].item->x, cmenu->itemlist[i - 1].item->y, cmenu->forceground, cmenu->background, *(cmenu->itemlist[i - 1].item->font));
				lcd_show_string(cmenu->itemlist[dfnum -1].item->character, cmenu->itemlist[i -1].item->x + cmenu->itemlist[dfnum -1].item->font->size, cmenu->itemlist[i -1].item->y, cmenu->forceground, cmenu->background, *(cmenu->itemlist[i - 1].item->font));
			}
			else
			{
				lcd_show_string(cmenu->index, cmenu->itemlist[i - 1].item->x, cmenu->itemlist[i -1].item->y, cmenu->itemlist[dfnum -1].item->color, cmenu->itemlist[dfnum-1].item->background, *(cmenu->itemlist[i - 1].item->font));
				lcd_show_string(cmenu->itemlist[dfnum -1].item->character, cmenu->itemlist[i - 1].item->x + cmenu->itemlist[dfnum -1].item->font->size, cmenu->itemlist[i - 1].item->y, cmenu->itemlist[dfnum -1].item->color, cmenu->itemlist[dfnum -1].item->background, *(cmenu->itemlist[i - 1].item->font));
			}
			dfnum--;
		}
	}
	else//��Ҫ����
	{
		uint8_t i, k;
		dfnum = cmenu->selectitem -1;
		
		for(i = 0, k = cmenu->sizePer - 1; i < cmenu->sizePer; i++)//����ˢ�£�ÿһ���Ҫˢ�£�
		{
			cmenu->index[0] = dfnum + '0';
			if(cmenu->index[0] > '9')
				cmenu->index[0] = cmenu->index[i]-'9'+'A'-1;
			
			if(i == 0)
			{			
				/*
				������ʾ
				*/
				//��ʾ�±�
				//��ʾ�˵���һ��
				lcd_show_string(cmenu->index, cmenu->itemlist[k].item->x, cmenu->itemlist[k].item->y, cmenu->forceground, cmenu->background, *(cmenu->itemlist[k].item->font));
				lcd_show_string(cmenu->itemlist[dfnum -1].item->character, cmenu->itemlist[k].item->x + cmenu->itemlist[dfnum -1].item->font->size, cmenu->itemlist[k].item->y, cmenu->forceground, cmenu->background, *(cmenu->itemlist[k].item->font));
			}
			else
			{
				lcd_show_string(cmenu->index, cmenu->itemlist[k].item->x, cmenu->itemlist[k].item->y, cmenu->itemlist[dfnum -1].item->color, cmenu->itemlist[dfnum-1].item->background, *(cmenu->itemlist[dfnum -1].item->font));
				lcd_show_string(cmenu->itemlist[dfnum -1].item->character, cmenu->itemlist[k].item->x + cmenu->itemlist[dfnum -1].item->font->size, cmenu->itemlist[k].item->y, cmenu->itemlist[dfnum -1].item->color, cmenu->itemlist[dfnum -1].item->background, *(cmenu->itemlist[dfnum -1].item->font));
			}
			dfnum--;
			k --;
		}
		cmenu->selectitem --;
	}
} 


void intermenuitem(uint8_t num,menu* cmenu)
{
	num--;
	if(num<cmenu->height && cmenu->itemlist[num].status_id!= NULL)
		Load_status(cmenu->itemlist[num].status_id);
}



void LoadTextBox(textbox * boxxxxx)
{
	lcd_clear();
	boxxxxx->selectitem=0;
	boxxxxx->itemlist[0]();
}


void TextBoxNAVDOWN(textbox * boxxxxx)
{
	if(boxxxxx->height>1)
	{
	    lcd_clear();
		boxxxxx->selectitem=(boxxxxx->selectitem+1)%(boxxxxx->height);
		
			boxxxxx->itemlist[boxxxxx->selectitem]();
		//if(boxxxxx->selectitem != boxxxxx->height-1)
			//Display("��",6,14,0);	
		
	}
}

void TextBoxDISBYID(textbox *boxxxxx,uint8_t i)
{
	if(i<boxxxxx->height)
	{
	lcd_clear();
	boxxxxx->selectitem=i;
	boxxxxx->itemlist[boxxxxx->selectitem]();
	}
}


/*
������ʾ�ķ��������ڲ˵�����
�ǲ˵�����������
*/


/*
��������
*/
void contant_load(HOLDER* holder)
{
	uint8_t i=0;
	
//	if(EX_SIZE < 800)
//	show_string(holder->title, (g_lcd.xSize - sizeof(holder->title)/2),  0); //��ʾ�������֣���û�в������ʱ����Ҫ��
	
	
	lcd_show_string(holder->contentList[i].item->character, holder->contentList[i].item->x, holder->contentList[i].item->y,  holder->zicolor, holder->background, *(holder->contentList[i].item->font) );//��ӡѡ�������������
	lcd_show_string(holder->contentList[i].val, holder->contentList[i].valueX, holder->contentList[i].valueY, holder->numcolor, holder->background, *(holder->contentList[i].item->font)); //��ӡ���ݶ�Ӧֵ,�����������������ͬ
	
	
	i++;
	while(i < holder->shownumPage && i < holder->size)
	{
		lcd_show_string(holder->contentList[i].item->character,  holder->contentList[i].item->x  ,holder->contentList[i].item->y, holder->foreground, holder->background ,*(holder->contentList[i].item->font));//��ʾ����
		lcd_show_string(holder->contentList[i].val, holder->contentList[i].valueX, holder->contentList[i].valueY, holder->foreground, holder->background, *(holder->contentList[i].item->font)); //��ӡ���ݶ�Ӧֵ
		i++;
	}
	holder->curSelectId = 1;	//��ǰѡ��Ϊ��1�����Ϊɶ����1������
}


/*
�����·����
*/
void content_page_down(HOLDER* holder)
{
	uint8_t cur,i=0;
	if(holder->curSelectId % holder->shownumPage !=0 && holder->curSelectId < holder->size)//��ǰѡ����ͬһҳ��
	{
		cur = holder->curSelectId - 1;
		//��ˢ������ʾ
		lcd_show_string("                          ",  holder->contentList[cur].item->x  ,holder->contentList[cur].item->y, holder->foreground, holder->background, *(holder->font));//��ʾ����
		lcd_show_string(holder->contentList[cur].item->character, holder->contentList[cur].item->x, holder->contentList[cur].item->y, holder->foreground, holder->background, *(holder->font));//ˢ����һ�����δ����ʱ��ѡ��� 
		lcd_show_string(holder->contentList[cur].val, holder->contentList[cur].valueX, holder->contentList[cur].valueY, holder->foreground, holder->background,  *(holder->font)); //��ӡ���ݶ�Ӧֵ
		(holder->curSelectId)++;	//���+
		
		cur = holder->curSelectId - 1;
		//��ʾ��ǰѡ����
		lcd_show_string(holder->contentList[cur].item->character, holder->contentList[cur].item->x,  holder->contentList[cur].item->y, holder->zicolor, holder->background, *(holder->font));//��ӡѡ�������������
		lcd_show_string(holder->contentList[cur].val, holder->contentList[cur].valueX, holder->contentList[cur].valueY, holder->numcolor, holder->background, *(holder->font)); //��ӡ���ݶ�Ӧֵ 
	}
	else
	if(holder->curSelectId == holder->size)//��Ϊ�Ǵ�1��ʼ�Ǳ꣬���������Ǳ����õ���������С
	{
		//����������һ���ʲô������

	}
	else//��������Ļ,������ʾһҳ
	{
		//if(holder->curSelectId < holder->size)
		{
			//����Ȼ����ʾ��һҳ
			for(cur = 0; cur < holder->shownumPage && cur < holder->size; cur++)
			{
				lcd_show_string("                            ",  holder->contentList[cur].item->x  ,holder->contentList[cur].item->y, holder->foreground, holder->background,  *(holder->font));//����
			}
			
			//ǰ��forѭ���ѽ�+1��
			cur = holder->curSelectId;
			
			lcd_show_string(holder->contentList[cur].item->character, holder->contentList[cur].item->x,  holder->contentList[cur].item->y, holder->zicolor, holder->background, *(holder->font));//��ӡѡ�������������
			lcd_show_string(holder->contentList[cur].val, holder->contentList[cur].valueX, holder->contentList[cur].valueY, holder->numcolor, holder->background, *(holder->font));
			
			cur++;
			
			i = 1;
			while(i < holder->shownumPage && cur < holder->size)
			{
				lcd_show_string(holder->contentList[cur].item->character,  holder->contentList[cur].item->x  ,holder->contentList[cur].item->y, holder->foreground, holder->background,  *(holder->font));//��ʾ����
				lcd_show_string(holder->contentList[cur].val, holder->contentList[cur].valueX, holder->contentList[cur].valueY, holder->foreground, holder->background,  *(holder->font));//��ӡ���ݶ�Ӧֵ
				cur++;
				i++;
			}

			(holder->curSelectId)++;
		}
	}
} 


/*
�����Ϸ���ǰ��
*/
void content_page_up(HOLDER* holder)
{
	int cur,i;
	
	if(holder->curSelectId > 1 && ((holder->curSelectId - 1) % holder->shownumPage != 0))//��ÿҳ��һ��
	{

		cur = holder->curSelectId - 1;
		lcd_show_string("                                                     ",  holder->contentList[cur].item->x  ,holder->contentList[cur].item->y, holder->foreground, holder->background,  *(holder->font));//��ʾ����
		lcd_show_string(holder->contentList[cur].item->character, holder->contentList[cur].item->x, holder->contentList[cur].item->y, holder->foreground, holder->background,  *(holder->font));//ˢ����һ�����δ����ʱ��ѡ���
		lcd_show_string(holder->contentList[cur].val, holder->contentList[cur].valueX, holder->contentList[cur].valueY, holder->foreground, holder->background,  *(holder->font)); //��ӡ���ݶ�Ӧֵ
		(holder->curSelectId)--;	//���-
		
		cur = holder->curSelectId-1;
		//��ʾ��ǰѡ����
		lcd_show_string(holder->contentList[cur].item->character, holder->contentList[cur].item->x,  holder->contentList[cur].item->y, holder->zicolor, holder->background, *(holder->font));//��ӡѡ�������������
		lcd_show_string(holder->contentList[cur].val, holder->contentList[cur].valueX, holder->contentList[cur].valueY, holder->numcolor, holder->background, *(holder->font)); //��ӡ���ݶ�Ӧֵ 
	}
	else
	if(holder->curSelectId == 1)//��Ϊ�Ǵ�1��ʼ�Ǳ꣬���������Ǳ����õ���������С
	{
		//ѡ���һ��ʱ���Ϸ���û���õ�
	}
	else//��������Ļ,������ʾһҳ
	{
		//����Ȼ����ʾ��һҳ
		cur = holder->curSelectId;
		for( (cur = holder->curSelectId - 1), i = 0; (i <  holder->shownumPage) && (cur+i < holder->size); i++)
		{
			uint32_t len,j;
			char tmp[30];
			len =  strlen(holder->contentList[cur+i].item->character);
			for(j = 0; j < (len+1); j++)//���������itemname �� val�м�Ŀո�
			tmp[j] = ' ';
			tmp[j] = '\0';
			lcd_show_string(tmp,  holder->contentList[cur+i].item->x  ,holder->contentList[cur+i].item->y, holder->foreground, holder->background, *(holder->font));//ˢ��������һ��Ļ�ճ��ռ�
			
			len = strlen(holder->contentList[cur+i].val);
			for(j = 0; j < (len+1); j++)//���������itemname �� val�м�Ŀո�
			tmp[j] = ' ';
			tmp[j] = '\0';
			lcd_show_string(tmp,  holder->contentList[cur+i].valueX  ,holder->contentList[cur+i].valueY, holder->foreground, holder->background, *(holder->font));//ˢ��������һ��Ļ�ճ��ռ�
		}
		
		
		
		(holder->curSelectId)--;
		
		cur = holder->curSelectId - 1;
		lcd_show_string(holder->contentList[cur].item->character, holder->contentList[cur].item->x,  holder->contentList[cur].item->y, holder->zicolor, holder->background, *(holder->font));//��ӡѡ�������������
		lcd_show_string(holder->contentList[cur].val, holder->contentList[cur].valueX, holder->contentList[cur].valueY, holder->numcolor, holder->background, *(holder->font));
		cur--;
		
		i = 1;//����ȡ1
		while(i < holder->shownumPage &&  cur >= 0)
//		while(i < holder->shownumPage - 1 &&  cur >=0)���������
		{
			lcd_show_string(holder->contentList[cur].item->character,  holder->contentList[cur].item->x  ,holder->contentList[cur].item->y, holder->foreground, holder->background, *(holder->font));//��ʾ����
			lcd_show_string(holder->contentList[cur].val, holder->contentList[cur].valueX, holder->contentList[cur].valueY, holder->foreground, holder->background, *(holder->font)); //��ӡ���ݶ�Ӧֵ
			cur--;
			i++;
		}
	}
} 

/*
ˢ�µ�ǰ��
*/
void content_item_fls(HOLDER* holder)
{
	lcd_show_string("                ", 								holder->contentList[holder->curSelectId - 1].valueX, holder->contentList[holder->curSelectId - 1].valueY, holder->foreground, holder->background,  *(holder->font)); //��ӡ���ݶ�Ӧֵ
	lcd_show_string(holder->contentList[holder->curSelectId - 1].val,	holder->contentList[holder->curSelectId - 1].valueX, holder->contentList[holder->curSelectId - 1].valueY, holder->numcolor , holder->background,  *(holder->font)); //��ӡ���ݶ�Ӧֵ
}

/*
��ֵת��Ϊ�ַ���
*/
void transfer_value_to_char(uint32_t val, char *addr)
{
	uint8_t len;
	char tmp[13];
	len = sprintf(tmp, "%u", val);
	addr[len] = 0;
	memcpy(addr, tmp, len); 
}


/*
���ܣ���ֵת��Ϊʱ���ַ���
˵����ʱ���ǹ̶���ʽ
*/
void transfer_value_to_time(uint32_t val, char *addr)
{
	char tmp[6];
	tmp[5] = 0;
	sprintf(tmp, "%04u", val);
	tmp[4] = tmp[3]; 
	tmp[3] = tmp[2];
	tmp[2] = ':';
	strcpy(addr, tmp);
}

/*
���ܣ���ֵת��Ϊ�����ַ���
˵���������ǹ̶���ʽ
*/
void transfer_value_to_data(uint32_t val, char *addr)
{
	char tmp[11], len, i;
	tmp[10] = 0;
	i = len = sprintf(tmp, "%04u", val/10000);
	tmp[i++] = '/';
	len = sprintf(&tmp[i], "%02u", val%10000/100);
	i += len;
	tmp[i++] = '/';
	len = sprintf(&tmp[i], "%02u", val%10000%100);
	i += len;
	tmp[i++] = '/';
	strcpy(addr, tmp);
}

/*
��ֵת������ַ���
*/
void transfer_value_to_money(uint32_t val, char *addr)
{
	char *format = addr;
	uint8_t len,i;
	
	len = sprintf(format,"%u",val);

	if(g_vm.point != 0)
	{
		if(len <= g_vm.point)//������λ��ǰ�油��
		{
			for(i = 0; i < len; i++)//������λ
			{
				format[len + g_vm.point - len - i] = format[len - 1 - i ]; 
			}
			for(i = 0; i < (g_vm.point - len + 1); i++)//����
			{
				format[i] = '0';
			}
		}
		if(len <= g_vm.point)//����󳤶ȱ仯
			len = len + i;
		
		//ÿ������ֻ��Ҫ�ƶ�1λ
		for(i = 0; i < (g_vm.point); i++)//С�����Ǽ�λ������Ҫ�ƶ���λ����
		{
			format[len - i] = format[len - 1 - i ];//С�������Ĳ����ƺ�һλ
		}
		
		format[len - g_vm.point] = '.';
		len ++;
	}
	format[len]='\0';
}
