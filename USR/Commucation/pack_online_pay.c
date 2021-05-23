/*
在线支付
*/

#include "header.h"
#include "com_fram.h"

/*
在线支付申请结果处理
*/
static uint8_t callback_online_cardPay(uint8_t *recv, uint16_t len, uint8_t*send)
{
	uint8_t i, rs = FALSE;
	FSMMSG* cmsg;
	cmsg = apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
	assert_param(cmsg);
	cmsg->type = MsgOnline;	//表示在线卡
	cmsg->stype = ONLINE_PAY;		//表示支付申请
	cmsg->value = 0x50;		//表示超时未回复
	
	if(recv[0] == 0x1D && recv[1] == send[1])//包头与序号正确
	{
		cmsg->value = recv[3];//更改回应类型
		//需要加入长度判断
		if(recv[3] == 0X30)//申请支付成功
		{
			uint32_t fac ;
			uint8_t tmp[10];
									
			for(i = 0;i < 10;i++){//逻辑卡号
				cIC.PrintSno[i] = recv[5+i];
			}
										
			for(i = 0,fac = 1;i < (recv[2]-20);i++)
			{//计算余额位数
				tmp[i] = recv[16+i];
				if(i != 0)
				fac *=10;
			}
			for(i = 0,cIC.balance = 0;i < (recv[2]-4);i++)
			{//计算余额
				cIC.balance += (tmp[i]-'0')*fac;
				fac /= 10;
			}
			cIC.daymaxpay = recv[recv[2]-3];//当天消费次数 
		}
		else//扣款申请/查询失败
		{
			for(i = 0;i < 10;i++)//逻辑卡号
			cIC.PrintSno[i] = recv[5+i];
		}
		rs =  TRUE;
	}
	if(send_fsm_msg(cmsg) != TRUE)
	{
		release_fsm_msg(cmsg);
		rs = 3;
	}
	return rs;
}




/*
在线查询申请结果处理
*/
static uint8_t callback_online_cardCheck(uint8_t *recv, uint16_t len, uint8_t*send)
{
	uint8_t i, rs = FALSE;
	FSMMSG* cmsg;
	cmsg = apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
	assert_param(cmsg);
	cmsg->type = MsgOnline;	//表示在线卡
	cmsg->stype = ONLINE_CHECK;		//表示支付申请
	cmsg->value = 0x50;		//表示超时未回复
	
	if(recv[0] == 0x1D && recv[1] == send[1])//包头与序号正确
	{
		cmsg->value = recv[3];//更改回应类型
		//需要加入长度判断
		if(recv[3] == 0X30)//申请支付成功
		{
			uint32_t fac ;
			uint8_t tmp[10];
									
			for(i = 0;i < 10;i++){//逻辑卡号
				cIC.PrintSno[i] = recv[5+i];
			}
										
			for(i = 0,fac = 1;i < (recv[2]-20);i++)
			{//计算余额位数
				tmp[i] = recv[16+i];
				if(i != 0)
				fac *=10;
			}
			for(i = 0,cIC.balance = 0;i < (recv[2]-4);i++)
			{//计算余额
				cIC.balance += (tmp[i]-'0')*fac;
				fac /= 10;
			}
			cIC.daymaxpay = recv[recv[2]-3];//当天消费次数 
		}
		else//扣款申请/查询失败
		{
			for(i = 0;i < 10;i++)//逻辑卡号
			cIC.PrintSno[i] = recv[5+i];
		}
		rs =  TRUE;
	}
	if(send_fsm_msg(cmsg) != TRUE)
	{
		release_fsm_msg(cmsg);
		rs = 3;
	}
	return rs;
}


/*
在线申请
参数：type：0x31：在线支付申请；0x32：在线查询申请
*/
void send_online_cardRequest(uint8_t type)
{
	uint8_t i,j;
	char tmp[10],len;
	uint8_t *str;
	MAIL *mail;
	SETUP_CASHLESS cashless;
	SETUP_DEVICE vminfo;
	
	mail = Apply_for_mail(100); //申请信封
	log_output(mail);
	if(mail == 0)
		return;
	if(type == ONLINE_PAY)
		mail->com_call_back = callback_online_cardPay;//支付函数
	else
	if(type == ONLINE_CHECK)
		mail->com_call_back = callback_online_cardCheck;//查询回调	
	else
		mail->com_call_back = NULL;
	
	str = mail->addr;
	
	i = 0;
	mail->addr[i++]=0x17;
	mail->addr[i++]=' ';//序号(发送时加上)
	mail->addr[i++]=' ';//总长度(校验一起计算)
	mail->addr[i++]=0x41;
	//str[i++] = '*';//此特殊包体不需要

	//下面包体
	//接口编码
	e2p_get_cashless(&cashless);
	sprintf(tmp, "%04u", cashless.port);
	for(j = 0;j < 4;j++)
	mail->addr[i++] = tmp[j];
	str[i++] = ';';
	
	//指令类型
	str[i++] = type ;
	str[i++] = ';';
	
	//机器id
	e2p_get_vminfo(&vminfo);
	sprintf(tmp, "%010u", vminfo.facId);
	for(j = 0; j < 10; j++)
	{
		str[i++]=tmp[j];
	}
	str[i++] = '*';
	
	//交易流水号
	len = sprintf(tmp, "%u" , tradenum_get());
	for(j=0;j<len;j++)
	str[i++] = tmp[j];
	str[i++] = '*';
	
	//逻辑卡号(默认写0)
//	for(j = 0;j<10;j++)
	str[i++]= '0';
	str[i++]= '*';
	
	//物理卡号
	for(j = 0;j < 10; j++)
	{
		if(cIC.PrintSnr[j] > '9' || cIC.PrintSnr[j] < '0')
			str[i++] = '0';
		else
			str[i++]= cIC.PrintSnr[j];
	}
	str[i++] = '*';
	
	//货道号
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
	
	//价格
	len = sprintf(tmp,"%u",g_trade.channel.price);
	for(j=0;j<len;j++)
	str[i++] = tmp[j];
	str[i++] = '*';
	
	//时间
	len = sprintf(tmp,"%04u",calendar.w_year);
	for(j = 0;j < len;j++)
	str[i++] = tmp[j];
	len = sprintf(tmp,"%02u",calendar.w_month);
	for(j = 0;j < len;j++)
	str[i++] = tmp[j];
	len = sprintf(tmp,"%02u",calendar.w_date);
	for(j = 0;j < len;j++)
	str[i++] = tmp[j];
	len = sprintf(tmp,"%02u",calendar.hour);
	for(j = 0;j < len;j++)
	str[i++] = tmp[j];
	len = sprintf(tmp,"%02u",calendar.min);
	for(j = 0;j < len;j++)
	str[i++] = tmp[j];
	len = sprintf(tmp,"%02u",calendar.sec);
	for(j = 0;j < len;j++)
	str[i++] = tmp[j];
	str[i++] = '*';//校验预留位
	str[2] = i+1;
	
	send_mail(mail);
//	show_string("4", 500, 350);
}






/*
在线刷卡请求结果
参数：type：0x34：出货失败，0x33：出货成功
*/
void send_online_cardRequestResult(uint8_t type)
{
	uint8_t i,j;
	char tmp[10],len;
	uint8_t *str;
	MAIL *mail;
	SETUP_CASHLESS cashless;
	SETUP_DEVICE vminfo;
	
	mail = Apply_for_mail(100); //申请信封
	log_output(mail);
	if(mail == 0)
		return;
	mail->com_call_back = NULL;//回调函数
	str = mail->addr;
	
	i = 0;
	mail->addr[i++]=0x17;
	mail->addr[i++]=' ';//序号(发送时加上)
	mail->addr[i++]=' ';//总长度(校验一起计算)
	mail->addr[i++]=0x41;
	//str[i++] = '*';//此特殊包体不需要

	//下面包体
	//接口编码
	e2p_get_cashless(&cashless);
	sprintf(tmp, "%04u", cashless.port);
	for(j = 0;j < 4;j++)
	mail->addr[i++] = tmp[j];
	str[i++] = ';';
	
	//指令类型
	str[i++] = type;
	str[i++] = ';';
	
	//机器id
	e2p_get_vminfo(&vminfo);
	sprintf(tmp, "%010u", vminfo.facId);
	for(j = 0; j < 10; j++)
	{
		str[i++]=tmp[j];
	}
	str[i++] = '*';
	
	//交易流水号
	len = sprintf(tmp, "%u" , tradenum_get());
	for(j=0;j<len;j++)
	str[i++] = tmp[j];
	str[i++] = '*';
	
	//逻辑卡号(默认写0)
//	for(j = 0;j<10;j++)
	str[i++]= '0';
	str[i++]= '*';
	
	//物理卡号
	for(j = 0;j < 10; j++)
	{
		if(cIC.PrintSnr[j] > '9' || cIC.PrintSnr[j] < '0')
			str[i++] = '0';
		else
			str[i++]= cIC.PrintSnr[j];
	}
	str[i++] = '*';
	
	//货道号
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
	
	//价格
	len = sprintf(tmp,"%u",g_trade.channel.price);
	for(j=0;j<len;j++)
	str[i++] = tmp[j];
	str[i++] = '*';
	
	//时间
	len = sprintf(tmp,"%04u",calendar.w_year);
	for(j = 0;j < len;j++)
	str[i++] = tmp[j];
	len = sprintf(tmp,"%02u",calendar.w_month);
	for(j = 0;j < len;j++)
	str[i++] = tmp[j];
	len = sprintf(tmp,"%02u",calendar.w_date);
	for(j = 0;j < len;j++)
	str[i++] = tmp[j];
	len = sprintf(tmp,"%02u",calendar.hour);
	for(j = 0;j < len;j++)
	str[i++] = tmp[j];
	len = sprintf(tmp,"%02u",calendar.min);
	for(j = 0;j < len;j++)
	str[i++] = tmp[j];
	len = sprintf(tmp,"%02u",calendar.sec);
	for(j = 0;j < len;j++)
	str[i++] = tmp[j];
	str[i++] = '*';//校验预留位
	str[2] = i+1;
	
	mail_to_flash(mail);
}



