/*
温度汇报
*/
#include "header.h" 
#include "com_fram.h" 

/*
功能：温度信息上报（定时循环发送）
包类别：0x12-0x32
参数：id：机柜号；state：回路状态；value：温度值
注意：(这里温度探头状态：0正常，其他异常)
*/
void report_temp_value(uint8_t id, uint8_t state, uint32_t value)
{
	uint8_t i,j;
	char tmp[10],len;
	volatile uint8_t*str;
	MAIL * cmail;

	cmail=Apply_for_mail(100); //申请信封
	log_output(cmail);
	if(cmail == 0)
		return ;
	cmail->com_call_back = NULL;
	str = cmail->addr;
	
	i = 0;
	str[i++]=0x12;
	str[i++]='*';
	str[i++]='*';
	str[i++]=0x32;
	str[i++]='*';
	//包体
	//类别 1表示温度，2表示湿度，3表示电量
	str[i++] = '1';
	str[i++] = '*';
	//机柜号,从1号机柜开始 0对应1柜,1对应2柜.......
	len = sprintf(tmp, "%u", (id+1) );
	for(j = 0;j < len;j++)
		str[i++] = tmp[j];
	str[i++] = '*';
	//状态（这个地方平台0为正常）
	/*
	这里平台与终端是相反的，0表示正常，1表示异常
	*/
	if(state == 1)
		state = 0;
	else
	if(state == 0)
		state = 1;
	len = sprintf(tmp,"%u",state);
	for(j = 0;j < len;j++)
		str[i++] = tmp[j];
	str[i++] = '*';
	//值（这个地方平台0为正常）
	if(state == 0)
	{
		len = sprintf(tmp,"%u",value);
		for(j = 0;j < len;j++)
			str[i++] = tmp[j];
	}
	else
		str[i++] = '0';
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

