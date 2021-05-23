/*
货道设置
0x11-0x31
类别[1.商品编码；2.价格；3.存量；4.容量；5.联动；6禁用；7出货检测；8是否控制库存]
*/

#include "header.h"
#include "com_fram.h"



/*
货道设置汇报
0x11-0x31
参数 :type-设置类型;	rang:设置范围(1-单货道；2-托盘；3-整机)；value：设置值
*/
void report_channel_set(uint8_t type, uint32_t value)
{
	uint8_t i,j;
	char tmp[10],len;
	volatile uint8_t*str;
	MAIL * cmail;

	cmail = Apply_for_mail(100); //申请信封
	log_output(cmail);
	if(cmail == 0)
		return;
	
	cmail->com_call_back = 0;
	str = cmail->addr;
	
	i = 0;
	str[i++]=0x11;
	str[i++]='*';
	str[i++]='*';
	str[i++]=0x31;
	str[i++]='*';
	//类别[1.商品编码；2.价格；3.存量；4.容量；5.联动；6禁用；7出货检测；(8是否控制库存,这个去掉，应该是整机相关)
	str[i++] = type;
	str[i++] = '*';
	//操作人
	if(!cIC.user)
	str[i++] = '0';
	else
	{
		for(j = 0;j < 10;j++)
		str[i++] = cIC.PrintSno[j];
	}
	str[i++] = '*';
	//机柜号
	len = sprintf(tmp,"%u",(g_trade.curDiverBoard.container + 1) );
	for(j = 0;j < len;j++)
		str[i++] = tmp[j];
	str[i++] = '*';
	//设置范围
	if(g_vm.channelBit == 1)
		str[i++] = '2';
	else
	{
		if(select_state_get())
			str[i++] = '1';
	}
	str[i++] = '*';
	//货道
//	len = strlen(select_char_get()); 
//	strcpy((char*)(&str[i]), select_char_get());
//	i += len;
	if(g_vm.channelBit < 3)
		len = sprintf(tmp, "%02u", (g_trade.curDiverBoard.tray+1)*10 + g_trade.curDiverBoard.col);
	else
		len = sprintf(tmp, "%03u", (g_trade.curDiverBoard.container + 1) *100 + (g_trade.curDiverBoard.tray + 1)*10 + g_trade.curDiverBoard.col);
	for(j = 0; j < len; j++)
		str[i++] = tmp[j];
	str[i++] = '*';
	//设置值
	switch(type)
	{
		case 0X31:
			//商品编码
			len = sprintf(tmp, "%06u", value);
			for(j = 0;j < len; j++)
			str[i++] = tmp[j];
		break;
		case 0X32:
		case 0X33:
		case 0X34:
			//货道价格
			len = sprintf(tmp, "%u", value);
			for(j=0;j<len;j++)
			str[i++] = tmp[j];
		break;
		case 0X36:
			//货道禁能/使能
			len = sprintf(tmp, "%u", value);
			for(j=0;j<len;j++)
			str[i++] = tmp[j];
		break;
	}
	str[i++] = '*';
	//时间
	str[i++] = calendar.w_year -2000 + 1;
	str[i++] = calendar.w_month + 1;
	str[i++] = calendar.w_date + 1;
	str[i++] = calendar.hour + 1;
	str[i++] = calendar.min + 1;
	str[i++] = calendar.sec + 1;
	str[2] = i+1;
	
	mail_to_flash(cmail); 
}

