/*
机器其他参数设置
*/
#include "header.h"
#include "com_fram.h"

/*
机器参数设置汇报
参数：
type：参数类别
	1.目标温度；
	2.购买方式；
	3.接收面额；
	4.温控方式；
	5.扫码设置(0关，1开)；
	6.刷卡接口；
	7.取货接口；
	8.系统语言(0中文，1英文)
	9.机柜数量；
	10.小数位数；
	11.价格位数；
	12.输入位数；
	13.货道位数；
	14.托盘数量；
	
	15.温控开启时间
value：参数值
*/
void report_device_set(uint8_t type, int value)
{
	uint8_t i,j;
	char tmp[10],len;
	volatile uint8_t*str;
	MAIL * cmail;

	cmail = Apply_for_mail (100); //申请信封
	log_output(cmail);
	if(cmail == 0)
		return;
	
	cmail->com_call_back = NULL;
	str = cmail->addr;
	
	i = 0;
	str[i++]=0x11;
	str[i++]='*';
	str[i++]='*';
	str[i++]=0x30;
	str[i++]='*';
	
	//类别
	str[i++] = 0x31;
	str[i++] = '*';
	//操作人
	str[i++] = '0';
	str[i++] = '*';
	//内容
	len = sprintf(tmp, "%d", value);
	for(j =0;j < len;j++)
	str[i++] = tmp[j];
	str[i++]='*';
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


