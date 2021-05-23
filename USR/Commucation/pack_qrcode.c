/*
二维码支付
*/

#include "header.h"
#include "com_fram.h"

/*
二维码结果回调
*/
static uint8_t callback_qrcode_payCheck(uint8_t *recv, uint16_t len, uint8_t*send)
{
	uint8_t rs = FALSE;
	FSMMSG* cmsg;
	cmsg = apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
	log_output(cmsg);
	cmsg->type = MsgQrcode;		//二维码支付
	cmsg->stype = QRCODE_CHECK;	//表示支付查询
	cmsg->value = 0;			//未知状态
	
	if(recv[0] == 0x1D && recv[1] == send[1] && recv[3] == 0x30)//包头与序号正确
	{
		cmsg->value = recv[5];//结果
		g_trade.qrcode.type = recv[7];
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
二维码通讯包
参数：type：0x31：二维码结果查询；0x32：二维码出货汇报
rs:结果（主要是出货时用）
*/
void send_qrcode_dat(uint8_t type, uint8_t rs)
{
	char tmp[10];
	uint8_t i, j , *str;
	MAIL *mail;
	SETUP_DEVICE  vmInfomation;
	
	mail = Apply_for_mail(100); //申请信封
	log_output(mail);
	
	if(mail == 0)
		return;
	
	if(type == QRCODE_CHECK)//二维码支付结果查询
		mail->com_call_back = callback_qrcode_payCheck;//支付函数
	else
	if(type == QRCODE_RESULT)//二维码结果汇报
		mail->com_call_back = NULL;//查询回调	
	else
		mail->com_call_back = NULL;
	
	str = mail->addr;
	
	i = 0;
	str[i++] = 0x17;	//包大类
	str[i++] = ' ';		//序号(发送时加上)
	str[i++] = ' ';		//总长度(校验一起计算)
	str[i++] = 0x30;	//包小类
	str[i++] = type;	//指令类型
	str[i++] = '-';
	//联机ID
	e2p_get_vminfo(&vmInfomation);
	sprintf(tmp, "%10u", vmInfomation.facId);
	for(j = 0;j < 10; j++)
	{
		str[i++] = tmp[j];
	}
	str[i++] = '-';
	
	//时间戳拷贝
//	strcpy( (char*)(&str[i]), g_trade.qrcode.timeStamp) ;
	strcpy( (char*)(&str[i]), (char*)g_trade.qrcode.stamp) ;
//	i += strlen(g_trade.qrcode.timeStamp);
	i += strlen((char*)g_trade.qrcode.stamp);
	str[i++] = '-';
	
	//结果
	str[i++] = rs + '0';
	str[i++] = '-';
	
	str[2] = i+1;//包长度
	
	if(type == QRCODE_CHECK)//支付结果查询
	{
		send_mail(mail);//即时信息
	}
	else//出货汇报
	{
		_free(g_trade.qrcode.timeStamp);//释放时间戳
		mail_to_flash(mail);
	}
}





