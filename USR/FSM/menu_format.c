/*
菜单控件
*/

#include "header.h"

textbox mytextboxxxxx;
menu mymenu;
HOLDER holder;

void LoadMenu(menu* cmenu)
{
	uint8_t i=0;
	lcd_show_character(cmenu->name); //显示菜单名
	
	cmenu->index[0] = '0';
	cmenu->index[1] = '.';
	cmenu->index[2] = ' ';
	cmenu->index[0] = i +'1';//下标
	//显示下标
	//显示菜单第一项
	lcd_show_string(cmenu->index, cmenu->itemlist[i].item->x, cmenu->itemlist[i].item->y,  cmenu->forceground, cmenu->background, *(cmenu->itemlist[i].item->font));//自已跟随后面文字字体
	lcd_show_string(cmenu->itemlist[i].item->character, cmenu->itemlist[i].item->x + cmenu->itemlist[i].item->font->size, cmenu->itemlist[i].item->y, cmenu->forceground, cmenu->background, *(cmenu->itemlist[i].item->font));
	
	i++;
	while(i < cmenu->sizePer && i < cmenu->height)
	{
		cmenu->index[0]=i+'1';
		//显示下标
		//显示菜单剩余项
		lcd_show_string(cmenu->index, cmenu->itemlist[i].item->x, cmenu->itemlist[i].item->y, cmenu->itemlist[i].item->color, cmenu->itemlist[i].item->background, *(cmenu->itemlist[i].item->font));
		lcd_show_string(cmenu->itemlist[i].item->character, cmenu->itemlist[i].item->x +cmenu->itemlist[i].item->font->size, cmenu->itemlist[i].item->y, cmenu->itemlist[i].item->color, cmenu->itemlist[i].item->background, *(cmenu->itemlist[i].item->font));
		i++;
	}
	cmenu->selectitem = 1;
}

/*
菜单下翻
*/
void MenuNAVDOWN(menu* cmenu)
{
	uint8_t dfnum;
	if(cmenu->selectitem < cmenu->sizePer && cmenu->selectitem < cmenu->height)//没有滚动到此页最后的菜单项
	{
		dfnum = cmenu->selectitem - 1;
		
		cmenu->index[0] = dfnum + '0' + 1;
		if(cmenu->index[0] > '9')
			cmenu->index[0] = cmenu->index[0]-'9'+'A';
		
		/*
		当前项显示本来颜色
		*/
		//显示下标
		//显示菜单项
		lcd_show_string(cmenu->index, cmenu->itemlist[dfnum].item->x, cmenu->itemlist[dfnum].item->y, cmenu->itemlist[dfnum].item->color, cmenu->itemlist[dfnum].item->background, *(cmenu->itemlist[dfnum].item->font));
		lcd_show_string(cmenu->itemlist[dfnum].item->character, cmenu->itemlist[dfnum].item->x + cmenu->itemlist[dfnum].item->font->size, cmenu->itemlist[dfnum].item->y, cmenu->itemlist[dfnum].item->color, cmenu->itemlist[dfnum].item->background, *(cmenu->itemlist[dfnum].item->font));
		
		
		/*
		下一项显示高亮
		*/
		(cmenu->selectitem)++;
		dfnum = cmenu->selectitem - 1;
		
		cmenu->index[0] = dfnum + '0' + 1;
		if(cmenu->index[0] > '9')
			cmenu->index[0] = cmenu->index[0]-'9'+'A';
		
		//显示下标
		//显示菜单下一项
		lcd_show_string(cmenu->index, cmenu->itemlist[dfnum].item->x, cmenu->itemlist[dfnum].item->y, cmenu->forceground, cmenu->background, *(cmenu->itemlist[dfnum].item->font));
		lcd_show_string(cmenu->itemlist[dfnum].item->character, cmenu->itemlist[dfnum].item->x + cmenu->itemlist[dfnum].item->font->size, cmenu->itemlist[dfnum].item->y, cmenu->forceground, cmenu->background, *(cmenu->itemlist[dfnum].item->font));
	}
	else
	if(cmenu->selectitem == cmenu->height)//最后一项
	{
		cmenu->selectitem = 1;
		LoadMenu(cmenu);
	}
	else//滚动到了最下面的菜单项
	{
		uint8_t i, j, k;
		dfnum = cmenu->selectitem ;//当前菜单项
		
		//每页要显示的菜单项,每次滚动,刷新此页所有菜单项
		for(i = 0, j = 0, k = cmenu->sizePer - 1; i < cmenu->sizePer ; i++)
		{
			cmenu->index[0] = dfnum +'1';
			if(cmenu->index[0]>'9')
			{
				cmenu->index[0] = 'A'+ j;
				j++;
			}
			if(i == 0)//此页最后一项
			{		
				/*
				高亮显示
				*/
				//显示下标
				//显示菜单第一项
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
菜单上翻
*/
void MenuNAVUP(menu* cmenu)
{
	uint8_t dfnum;
	if(cmenu->selectitem <= cmenu->sizePer && cmenu->selectitem <= cmenu->height && cmenu->selectitem != 1)//不是最上面，不需要滚动
	{
		dfnum = cmenu->selectitem;
		
		cmenu->index[0] = dfnum + '0';
		if(cmenu->index[0] > '9')
			cmenu->index[0] = cmenu->index[0]- '9' + 'A' - 1;
		
		dfnum --;
		/*
		当前项显示本来颜色
		*/
		//显示下标
		//显示菜单项
		lcd_show_string(cmenu->index,cmenu->itemlist[dfnum].item->x, cmenu->itemlist[dfnum].item->y, cmenu->itemlist[dfnum].item->color, cmenu->itemlist[dfnum].item->background, *(cmenu->itemlist[dfnum].item->font));
		lcd_show_string(cmenu->itemlist[dfnum].item->character, cmenu->itemlist[dfnum].item->x + cmenu->itemlist[dfnum].item->font->size, cmenu->itemlist[dfnum].item->y, cmenu->itemlist[dfnum].item->color, cmenu->itemlist[dfnum].item->background, *(cmenu->itemlist[dfnum].item->font));
		
		(cmenu->selectitem)--;
		
		dfnum = cmenu->selectitem;
		
		cmenu->index[0] = dfnum + '0';
		if(cmenu->index[0] > '9')
			cmenu->index[0] = cmenu->index[0]- '9' + 'A' - 1;
		
		dfnum --;
		//显示下标
		//显示菜单下一项
		lcd_show_string(cmenu->index, cmenu->itemlist[dfnum].item->x, cmenu->itemlist[dfnum].item->y, cmenu->forceground, cmenu->background, *(cmenu->itemlist[dfnum].item->font)); 
		lcd_show_string(cmenu->itemlist[dfnum].item->character, cmenu->itemlist[dfnum].item->x + cmenu->itemlist[dfnum].item->font->size, cmenu->itemlist[dfnum].item->y, cmenu->forceground, cmenu->background, *(cmenu->itemlist[dfnum].item->font)); 
	}
	else
	if(cmenu->selectitem == 1)//最后一项
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
			高亮显示
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
	else//需要滚动
	{
		uint8_t i, k;
		dfnum = cmenu->selectitem -1;
		
		for(i = 0, k = cmenu->sizePer - 1; i < cmenu->sizePer; i++)//滚动刷新（每一项都需要刷新）
		{
			cmenu->index[0] = dfnum + '0';
			if(cmenu->index[0] > '9')
				cmenu->index[0] = cmenu->index[i]-'9'+'A'-1;
			
			if(i == 0)
			{			
				/*
				高亮显示
				*/
				//显示下标
				//显示菜单第一项
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
			//Display("↓",6,14,0);	
		
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
内容显示的方法来自于菜单方法
是菜单方法的延伸
*/


/*
载入内容
*/
void contant_load(HOLDER* holder)
{
	uint8_t i=0;
	
//	if(EX_SIZE < 800)
//	show_string(holder->title, (g_lcd.xSize - sizeof(holder->title)/2),  0); //显示容器名字（在没有侧边栏的时候需要）
	
	
	lcd_show_string(holder->contentList[i].item->character, holder->contentList[i].item->x, holder->contentList[i].item->y,  holder->zicolor, holder->background, *(holder->contentList[i].item->font) );//打印选择项的内容名字
	lcd_show_string(holder->contentList[i].val, holder->contentList[i].valueX, holder->contentList[i].valueY, holder->numcolor, holder->background, *(holder->contentList[i].item->font)); //打印内容对应值,这个字体与内容名相同
	
	
	i++;
	while(i < holder->shownumPage && i < holder->size)
	{
		lcd_show_string(holder->contentList[i].item->character,  holder->contentList[i].item->x  ,holder->contentList[i].item->y, holder->foreground, holder->background ,*(holder->contentList[i].item->font));//显示内容
		lcd_show_string(holder->contentList[i].val, holder->contentList[i].valueX, holder->contentList[i].valueY, holder->foreground, holder->background, *(holder->contentList[i].item->font)); //打印内容对应值
		i++;
	}
	holder->curSelectId = 1;	//当前选择为第1项（这里为啥等于1？？）
}


/*
内容下翻或后翻
*/
void content_page_down(HOLDER* holder)
{
	uint8_t cur,i=0;
	if(holder->curSelectId % holder->shownumPage !=0 && holder->curSelectId < holder->size)//当前选择在同一页内
	{
		cur = holder->curSelectId - 1;
		//先刷屏再显示
		lcd_show_string("                          ",  holder->contentList[cur].item->x  ,holder->contentList[cur].item->y, holder->foreground, holder->background, *(holder->font));//显示内容
		lcd_show_string(holder->contentList[cur].item->character, holder->contentList[cur].item->x, holder->contentList[cur].item->y, holder->foreground, holder->background, *(holder->font));//刷新上一项（就是未按键时的选择项） 
		lcd_show_string(holder->contentList[cur].val, holder->contentList[cur].valueX, holder->contentList[cur].valueY, holder->foreground, holder->background,  *(holder->font)); //打印内容对应值
		(holder->curSelectId)++;	//序号+
		
		cur = holder->curSelectId - 1;
		//显示当前选择项
		lcd_show_string(holder->contentList[cur].item->character, holder->contentList[cur].item->x,  holder->contentList[cur].item->y, holder->zicolor, holder->background, *(holder->font));//打印选择项的内容名字
		lcd_show_string(holder->contentList[cur].val, holder->contentList[cur].valueX, holder->contentList[cur].valueY, holder->numcolor, holder->background, *(holder->font)); //打印内容对应值 
	}
	else
	if(holder->curSelectId == holder->size)//因为是从1开始角标，所以最后项角标正好等于容器大小
	{
		//如果到了最后一项，则什么都不做

	}
	else//超出了屏幕,重新显示一页
	{
		//if(holder->curSelectId < holder->size)
		{
			//清屏然后显示下一页
			for(cur = 0; cur < holder->shownumPage && cur < holder->size; cur++)
			{
				lcd_show_string("                            ",  holder->contentList[cur].item->x  ,holder->contentList[cur].item->y, holder->foreground, holder->background,  *(holder->font));//清屏
			}
			
			//前面for循环已将+1了
			cur = holder->curSelectId;
			
			lcd_show_string(holder->contentList[cur].item->character, holder->contentList[cur].item->x,  holder->contentList[cur].item->y, holder->zicolor, holder->background, *(holder->font));//打印选择项的内容名字
			lcd_show_string(holder->contentList[cur].val, holder->contentList[cur].valueX, holder->contentList[cur].valueY, holder->numcolor, holder->background, *(holder->font));
			
			cur++;
			
			i = 1;
			while(i < holder->shownumPage && cur < holder->size)
			{
				lcd_show_string(holder->contentList[cur].item->character,  holder->contentList[cur].item->x  ,holder->contentList[cur].item->y, holder->foreground, holder->background,  *(holder->font));//显示内容
				lcd_show_string(holder->contentList[cur].val, holder->contentList[cur].valueX, holder->contentList[cur].valueY, holder->foreground, holder->background,  *(holder->font));//打印内容对应值
				cur++;
				i++;
			}

			(holder->curSelectId)++;
		}
	}
} 


/*
内容上翻或前翻
*/
void content_page_up(HOLDER* holder)
{
	int cur,i;
	
	if(holder->curSelectId > 1 && ((holder->curSelectId - 1) % holder->shownumPage != 0))//非每页第一项
	{

		cur = holder->curSelectId - 1;
		lcd_show_string("                                                     ",  holder->contentList[cur].item->x  ,holder->contentList[cur].item->y, holder->foreground, holder->background,  *(holder->font));//显示内容
		lcd_show_string(holder->contentList[cur].item->character, holder->contentList[cur].item->x, holder->contentList[cur].item->y, holder->foreground, holder->background,  *(holder->font));//刷新上一项（就是未按键时的选择项）
		lcd_show_string(holder->contentList[cur].val, holder->contentList[cur].valueX, holder->contentList[cur].valueY, holder->foreground, holder->background,  *(holder->font)); //打印内容对应值
		(holder->curSelectId)--;	//序号-
		
		cur = holder->curSelectId-1;
		//显示当前选择项
		lcd_show_string(holder->contentList[cur].item->character, holder->contentList[cur].item->x,  holder->contentList[cur].item->y, holder->zicolor, holder->background, *(holder->font));//打印选择项的内容名字
		lcd_show_string(holder->contentList[cur].val, holder->contentList[cur].valueX, holder->contentList[cur].valueY, holder->numcolor, holder->background, *(holder->font)); //打印内容对应值 
	}
	else
	if(holder->curSelectId == 1)//因为是从1开始角标，所以最后项角标正好等于容器大小
	{
		//选择第一项时，上翻是没有用的
	}
	else//超出了屏幕,重新显示一页
	{
		//清屏然后显示上一页
		cur = holder->curSelectId;
		for( (cur = holder->curSelectId - 1), i = 0; (i <  holder->shownumPage) && (cur+i < holder->size); i++)
		{
			uint32_t len,j;
			char tmp[30];
			len =  strlen(holder->contentList[cur+i].item->character);
			for(j = 0; j < (len+1); j++)//这里计算了itemname 和 val中间的空格
			tmp[j] = ' ';
			tmp[j] = '\0';
			lcd_show_string(tmp,  holder->contentList[cur+i].item->x  ,holder->contentList[cur+i].item->y, holder->foreground, holder->background, *(holder->font));//刷屏，给上一屏幕空出空间
			
			len = strlen(holder->contentList[cur+i].val);
			for(j = 0; j < (len+1); j++)//这里计算了itemname 和 val中间的空格
			tmp[j] = ' ';
			tmp[j] = '\0';
			lcd_show_string(tmp,  holder->contentList[cur+i].valueX  ,holder->contentList[cur+i].valueY, holder->foreground, holder->background, *(holder->font));//刷屏，给上一屏幕空出空间
		}
		
		
		
		(holder->curSelectId)--;
		
		cur = holder->curSelectId - 1;
		lcd_show_string(holder->contentList[cur].item->character, holder->contentList[cur].item->x,  holder->contentList[cur].item->y, holder->zicolor, holder->background, *(holder->font));//打印选择项的内容名字
		lcd_show_string(holder->contentList[cur].val, holder->contentList[cur].valueX, holder->contentList[cur].valueY, holder->numcolor, holder->background, *(holder->font));
		cur--;
		
		i = 1;//这里取1
		while(i < holder->shownumPage &&  cur >= 0)
//		while(i < holder->shownumPage - 1 &&  cur >=0)这里会死机
		{
			lcd_show_string(holder->contentList[cur].item->character,  holder->contentList[cur].item->x  ,holder->contentList[cur].item->y, holder->foreground, holder->background, *(holder->font));//显示内容
			lcd_show_string(holder->contentList[cur].val, holder->contentList[cur].valueX, holder->contentList[cur].valueY, holder->foreground, holder->background, *(holder->font)); //打印内容对应值
			cur--;
			i++;
		}
	}
} 

/*
刷新当前项
*/
void content_item_fls(HOLDER* holder)
{
	lcd_show_string("                ", 								holder->contentList[holder->curSelectId - 1].valueX, holder->contentList[holder->curSelectId - 1].valueY, holder->foreground, holder->background,  *(holder->font)); //打印内容对应值
	lcd_show_string(holder->contentList[holder->curSelectId - 1].val,	holder->contentList[holder->curSelectId - 1].valueX, holder->contentList[holder->curSelectId - 1].valueY, holder->numcolor , holder->background,  *(holder->font)); //打印内容对应值
}

/*
数值转换为字符串
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
功能：数值转换为时间字符串
说明：时间是固定格式
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
功能：数值转换为日期字符串
说明：日期是固定格式
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
数值转换金额字符串
*/
void transfer_value_to_money(uint32_t val, char *addr)
{
	char *format = addr;
	uint8_t len,i;
	
	len = sprintf(format,"%u",val);

	if(g_vm.point != 0)
	{
		if(len <= g_vm.point)//不够个位的前面补零
		{
			for(i = 0; i < len; i++)//首先移位
			{
				format[len + g_vm.point - len - i] = format[len - 1 - i ]; 
			}
			for(i = 0; i < (g_vm.point - len + 1); i++)//补零
			{
				format[i] = '0';
			}
		}
		if(len <= g_vm.point)//补零后长度变化
			len = len + i;
		
		//每个数字只需要移动1位
		for(i = 0; i < (g_vm.point); i++)//小数点是几位数则需要移动几位数字
		{
			format[len - i] = format[len - 1 - i ];//小数点后面的部分移后一位
		}
		
		format[len - g_vm.point] = '.';
		len ++;
	}
	format[len]='\0';
}
