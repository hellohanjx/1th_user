/*
状态变化汇报

*/
#include "header.h" 
#include "com_fram.h"


/*
状态变化汇报
0x12-0x36
参数： equType：设备类型；equId：设备编码；state：设备状态
*/
void report_state_change(uint32_t equType, uint32_t equId, uint32_t state)
{
	uint8_t i,j;
	char tmp[10],len;
	volatile uint8_t*str;
	MAIL * cmail;

	cmail = Apply_for_mail(150); //申请信封
	log_output(cmail);
	if(cmail == 0)
		return ;
	cmail->com_call_back = 0;
	str = cmail->addr;
	
	i = 0;
	str[i++]=0x12;
	str[i++]='*';
	str[i++]='*';
	str[i++]=0x36;
	str[i++]='*';
	//设备类型
	//1货道，2支付设备，3温控，4门开关，5驱动板，6掉货测试，7制冷开关，8风机开关，9日光灯开关，10消毒灯开关
	//11玻璃加热，12备用2
	len = sprintf(tmp, "%u", equType);
	for(j = 0;j < len;j++)
	str[i++] = tmp[j];	
	str[i++] = '*';
	
	//设备编码
	/*
	1.货道：为货道编码。
	2.支付设备：1硬币，2纸币，3刷卡器，4扫描器。
	3.温控：1主柜，2副柜。
	4.门开关：		1主柜门，2副柜门
	5.驱动板：		1主柜，2副柜
	6.调货检测：	1主柜，2副柜
	7.制冷开关：	1主柜，2副柜
	8.风机开关：	1主柜，2副柜
	9.日光灯开关：	1主柜，2副柜
	10.消毒灯开关：	1主柜，2副柜
	11.玻璃加热：	1主柜，2副柜
	12.备用2：		1主柜，2副柜
	*/
	switch(equType)
	{
		case 1://货道编码
			str[i++] = g_trade.curDiverBoard.tray + '1';//出货货道名称
			str[i++] = g_trade.curDiverBoard.col + '0';
		break;
		
		case 2://支付设备 1硬币，2纸币，3刷卡器，4扫描器
			str[i++] = equId + '0'; 
		break;

		case 5://驱动板编号
		case 3://温控 1主柜，2副柜
		case 4://门开关：1主柜门，2副柜门
		case 6://掉货检测
		case 7://制冷
		case 8://风机
		case 9://照明
		case 10://消毒
		case 11://玻璃加热
		case 12://备用2
			len = sprintf(tmp, "%u", equId);
			for(j = 0;j < len;j++)
			str[i++] = tmp[j];
		break;
	}
	str[i++] = '*';
	
	//状态变化
	/*
	有些状态终端与平台是相反的，这里需要转换一下
	表示开关的，1开，0关
	设备好坏的，0好，1坏
	*/
	if(equType == STATE_DRV || equType == STATE_PAY)
		state = !state;
	//设备状态,1异常，0正常
	len = sprintf(tmp, "%u", state);
	for(j = 0;j < len; j++)
	str[i++] = tmp[j];	
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

/*
*/
