/*
交易结果汇报
*/

#include "header.h"
#include "com_fram.h"

/*
交易结果汇报包
0x14-0x36
参数： num：数量；payType：支付类型; saleResult:出货结果
*/
extern volatile uint32_t t_t;

void report_trade_result(uint8_t num, uint8_t payType, uint8_t saleResult) 
{
	uint8_t i,j;
	char tmp[30],len;
	volatile uint8_t*str; 
	MAIL * cmail;
	
	SETUP_CHANNEL channel;
	e2p_get_channel((g_trade.curDiverBoard.tray*10 + 0), &channel);
	
	cmail = Apply_for_mail(100); //申请信封
	log_output(cmail);
	if(cmail == 0)
		return ;
		
	cmail->com_call_back = NULL;	//无回调函数
	str = cmail->addr;
	
	i = 0;
	str[i++]=0x14;
	str[i++]=' ';//序号(发送时加上)
	str[i++]=' ';//总长度(校验一起计算)
	str[i++]=0x36;
	str[i++] = '*';

	//包体
	//交易流水号
	len = sprintf(tmp,"%u",tradenum_get() );
	for(j=0;j<len;j++)
		str[i++] = tmp[j];
	str[i++] = '*';
	//货道号
//	len = strlen(select_char_get()); 
//	strcpy((char*)(&str[i]), select_char_get());
//	i += len;
	if(g_vm.channelBit < 3)
		len = sprintf(tmp, "%02u", (g_trade.curDiverBoard.tray + 1)*10 + g_trade.curDiverBoard.col);
	else
		len = sprintf(tmp, "%03u", (g_trade.curDiverBoard.container + 1)*100 + (g_trade.curDiverBoard.tray + 1)*10 + g_trade.curDiverBoard.col);
	for(j = 0; j < len; j++)
		str[i++] = tmp[j];
	str[i++] = '*';
	//价格
	len = sprintf(tmp, "%u", g_trade.channel.price);
	for(j=0;j<len;j++)
		str[i++] = tmp[j];
	str[i++] = '*';
	//余额
	if(payType == SALE_TYPE_CARD)
	{
		//卡余额
		len = sprintf(tmp, "%u", cIC.balance);
		for(j=0;j<len;j++)
		str[i++] = tmp[j];
	}
	else
	{
		//其他余额
		str[i++] = '0';
	}
	str[i++] = '*';
	//商品编码
	len = sprintf(tmp, "%06u", channel.code);
	for(j=0;j<len;j++)
	str[i++] = tmp[j];
	str[i++] = '*';
	//数量
	len = sprintf(tmp,"%u",num);
	for(j =0;j < len;j++)
	str[i++] = tmp[j];
	str[i++] = '*';
	//生产日期
	str[i++] = '0';
	str[i++] = ';';//分隔符
	//出货结果
	len = sprintf(tmp, "%u", saleResult);
	for(j = 0; j < len; j++)
		str[i++] = tmp[j];
	str[i++] = '+';
	//出货时间
	str[i++] = t_t/1000 + '0';
	str[i++] = t_t%1000/100 + '0';

	str[i++] = '*';
	//温度
	len = sprintf(tmp,"%d",pt_driverBoard[0]->temp.value[0]);
	for(j =0;j < len;j++)
		str[i++] = tmp[j];
	str[i++] = '*';
	//支付方式
	len = sprintf(tmp,"%u",payType);
	for(j =0;j < len;j++)
		str[i++] = tmp[j];
	str[i++] = '*';
	
	//出货说明
	if(payType == SALE_TYPE_CARD)//刷卡发卡号
	{
		for(j = 0;j < 10; j++)
		str[i++] = cIC.PrintSno[j];
		
	}
	else
	if(payType == SALE_TYPE_APPLY || payType == SALE_TYPE_WEI)//支付宝微信发时间戳
	{
		for(j = 0;j < 14; j++)
		str[i++] = g_trade.qrcode.stamp[j];
	}
	else//现金及其他发'0'占位
	{
		str[i++] = '0';
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

