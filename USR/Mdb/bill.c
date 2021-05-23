/*
纸币器驱动

*/
#include "header.h"
#include "bill.h"
#include "mdb_driver.h"



#define MAX_BILL_TIMEOUT	OS_TICKS_PER_SEC*6	//最大超时时间6秒
#define BILL_JUST_RESET		0x06	//复位指令

OS_EVENT* SEM_BILL_CMD;  //用于纸币器命令的信号量
OS_SEM_DATA sem_bill_check;
BillValtdator billvaltdator;
mdbListNode billequip;

uint8_t billset=0;											//设备当前状态
uint8_t bill_cmd[16]={0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};	//命令序列1
uint8_t bill_cmd2[16]={0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};	//命令序列3
uint8_t bill_cmd3[10]={0,0,0,0, 0,0,0,0, 0,0};	//命令序列2
uint32_t bill_reset_time;

uint8_t  resetflag=0;				//用来做兼容性处理
uint8_t  billchangenum=0;			//状态翻转计数
uint32_t billfactor;				//比例因子
uint32_t bill_timeout[2]={0.0};	//做计时用

//下面3个变量做安全处理用
uint8_t chkvalue[2];
uint8_t setupCount;	//setup命令计数(ITL纸币器需要5次确认参数正确)
uint8_t typevalue[16];

/*
纸币器所有方法
*/
void BillValtdator_Reset(void);
void BillReset_Poll(void);
void Bill_Setup(void);
void Bill_Poll_CMD(void);
void Bill_Exp_Identifiy(void);
void Bill_Exp_Feature(void);
void Bill_Stacker(void);
void Bill_Exp_EnOption(void);
void Bill_TYPE(void);
void Bill_Escrow_CMD(void);
void Bill_Security(void);
void Recycler_Setup(void);
void Recycler_Enable(void);
void BILL_Dispen_Status(void);
void Payout_Value_Poll(void);
void Bill_Payout_Status(void);
void Bill_Poll_Recy(void);
void Dispense_Bill(void);



uint8_t readbillset(){//读禁使能状态位
	uint8_t tmp;
	//I2C_ReadNByte(CAT24WC256,TWO_BYTE_SUBA,BA10+BA11,&tmp,BA11_LENTH);
	return tmp;
}

void setbillCE(uint8_t num){//写禁使能状态位
	//I2C_WriteNByte(CAT24WC256,TWO_BYTE_SUBA,BA10+BA11,&num,BA11_LENTH);
}


void BillValtdatorInit(void)
{
	uint8_t tmp[2]={1,2},err=FALSE,i=0;
	
	billequip.EQIU_FUNC=BillValtdator_Reset;
	billequip.cmd_status=cmd_status_complete;
	billequip.eqiu_status=0xFE;
	register_mdb_equ(&billequip);
	bill_reset_time = 2500;
	SEM_BILL_CMD=OSSemCreate(1);
	do{//用来判断接收使能是否正确
//		I2C_ReadNByte(CAT24WC256,TWO_BYTE_SUBA,BC00+BC02,tmp,BC02_LENTH);
//		I2C_ReadNByte(CAT24WC256,TWO_BYTE_SUBA,BC00+BC02,&tmp[1],BC02_LENTH);
		tmp[0] = tmp[1] = 0;
		if(tmp[0]==tmp[1])
		{
			billvaltdator.par_enable=tmp[0];
			err=TRUE;
		}
	}while(err!=TRUE && i++<20);
	if(err==FALSE){//接收使能错误
		//DisplayLT("FATAL ERR" ,6,0,0);
		((void (*)())(0x0)) ();
	}
}



void savebillaccept(uint16_t num)
{
	//send_moneyaccept_msg(DATA_BC,num,1);//发送信息到远程管理端
//	CA301+=num;
//	//CA302+=num;
//	CA304+=num;
//	CA305+=num;
//	//CA306+=num;
//	CA308+=num;
//	switch(num)
//	{
//		case 100:
//		CA1402(0)++;
//		CA1403(0)++;
//		CA1404(0)++;
//		CA1405(0)++;
//		break;
//		case 200:
//		CA1402(1)++;
//		CA1403(1)++;
//		CA1404(1)++;
//		CA1405(1)++;
//		break;
//		case 250:
//		CA1402(2)++;
//		CA1403(2)++;
//		CA1404(2)++;
//		CA1405(2)++;;
//		case 500:
//		CA1402(3)++;
//		CA1403(3)++;
//		CA1404(3)++;
//		CA1405(3)++;
//		break;
//		case 750:
//		CA1402(4)++;
//		CA1403(4)++;
//		CA1404(4)++;
//		CA1405(4)++;
//		case 1000:
//		CA1402(5)++;
//		CA1403(5)++;
//		CA1404(5)++;
//		CA1405(5)++;
//		break;
//		case 2000:
//		CA1402(6)++;
//		CA1403(6)++;
//		CA1404(6)++;
//		CA1405(6)++;
//		break;
//		case 5000:
//		CA1402(7)++;
//		CA1403(7)++;
//		CA1404(7)++;
//		CA1405(7)++;
//		break;
//		case 10000:
//		CA1402(8)++;
//		CA1403(8)++;
//		CA1404(8)++;
//		CA1405(8)++;
//		break;
//		default:
//		break;
//	}
}


void set_bill_status(uint8_t num)//保存纸币器状态
{
  if(num!=billequip.eqiu_status){//当前状态改变
   	FSMMSG * billmsg;
   	
//   	if(day!=DOM){//如果日期发生变化
//   	
//	   	day=DOM;
//	   	billchangenum=0;
//   	}
	if(billchangenum<50){//翻转计数限制
	    
		billchangenum++;
//		if(num>1)
//			status_equip_msg(DATA_BA,0xFF,num);
//		else
//		if(num==1 && billequip.eqiu_status>1)
//			status_equip_msg(DATA_BA,0xFF,2);
//		else
//		if(num==0 && billequip.eqiu_status>1)
//			status_equip_msg(DATA_BA,0xFF,2);
//		else
//			billchangenum--;
	}
	billmsg=apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
	assert_param(billmsg);
	billmsg->type=MsgBILL;
	if(billequip.eqiu_status==0xFE)
		billmsg->stype=M_BS_INIT;
	else
		billmsg->stype=M_BS_STATUS;
	billmsg->value=num;
	billequip.eqiu_status=num;
	if(send_fsm_msg(billmsg)!=TRUE)
		release_fsm_msg(billmsg);
   }
}

void BillValtdator_Reset(void) //启动纸币器的初始化
{
	uint8_t err;
	uint16_t *rx, rx_len;//接收数据指针与接收数据长度
	rx = rx;//防止编译器报错
	if((OSTimeGet()-bill_reset_time) > OS_TICKS_PER_SEC*3)//2.5秒复位一次
	{
		//OSSemQuery(SEM_BILL_CMD,&sem_bill_check);
		//if(sem_bill_check.OSCnt==0)
			//OSSemPost(SEM_BILL_CMD);
		resetflag=0;
		billequip.cmd_type=M_BS_POLL;
		//billequip.cmd_type=M_BS_INIT;
		bill_timeout[1]=OSTimeGet();
		bill_timeout[0]=OSTimeGet();
		billequip.cmd_status=cmd_status_complete;
		bill_cmd[0]=BILL_RESET;
		bill_reset_time = OSTimeGet();
		
		rx = mdb_send_cmd(bill_cmd, 1, 1, BILL_NO_RESPENSE, &err, &rx_len);
		
		if(err == MDB_RECEIVE_ACK)
		{
			billequip.EQIU_FUNC=BillReset_Poll;
			bill_timeout[1] = OSTimeGet();
			bill_timeout[0] = OSTimeGet();
		}
		else            
		{
			set_bill_status(0xFF);
		}
	}
}

void BillReset_Poll(void)
{
	uint8_t err;
	uint16_t *rx, rx_len;//接收数据指针与接收数据长度
	
	bill_cmd[0]=BILL_POLL;
	rx = mdb_send_cmd(bill_cmd, 1, 1, BILL_NO_RESPENSE, &err, &rx_len);
	
	if(err!=MDB_RECEIVE_ACK && err!=MDB_RECEIVE_DATA)
	{
		if(OSTimeGet()-bill_timeout[0]>MAX_BILL_TIMEOUT)//超时
		{
			set_bill_status(0xFF);
			billequip.EQIU_FUNC=BillValtdator_Reset;
		}
		return;
	}
	else
	{
		if(err==MDB_RECEIVE_DATA)
		{
			uint8_t i,err=FALSE;
			for(i=0;i < rx_len; i++)
			{
				if(rx[i] == 0x03)//忙
				{
					
				}
				else
				if(rx[i] == BILL_JUST_RESET)
				{
					err = TRUE;
				}
			}
			if(err!=TRUE)
			{
				lcd_show_string("8", 0, 0, RED, WHITE, font48);
			}

		}
		else
		if(err == MDB_RECEIVE_ACK)
		{
			resetflag=1;
			lcd_show_string("9", 0, 0, RED, WHITE, font48);
		}
	}
	setupCount = 0;//setup清零
	billequip.EQIU_FUNC = Bill_Setup;
	bill_timeout[0] = OSTimeGet();
}

/*
参数设置
*/
void Bill_Setup(void)
{
	uint8_t err, i;
	uint16_t *rx, rx_len;//接收数据指针与接收数据长度
	
	bill_cmd[0]=BILL_SETUP;
	rx = mdb_send_cmd(bill_cmd,1,1,BILL_NO_RESPENSE*10, &err, &rx_len);
	if(err!=MDB_RECEIVE_DATA)
	{
		if(OSTimeGet()-bill_timeout[0]>MAX_BILL_TIMEOUT)
		{
			billequip.EQIU_FUNC=BillValtdator_Reset;
			set_bill_status(0xFF);
		}
		return;
	}
	setupCount++;
	//对SETUP命令返回的结果的处理代码
	billvaltdator.level = rx[0];
	billvaltdator.BCD = rx[2] | (rx[1] << 8);
	billvaltdator.factor = rx[4] | (rx[3] << 8);
	billvaltdator.point=rx[5];
	billfactor=sysfactor;
	for(i=0;i<billvaltdator.point;i++)
		billfactor=billfactor/10;
	billfactor=billfactor*billvaltdator.factor;
	billvaltdator.capacity=rx[7]|(rx[6]<<8);
	billvaltdator.security=rx[9]|(rx[8]<<8);
	billvaltdator.escrow=rx[10];
	for(i=11;i<rx_len;i++)
	{
		billvaltdator.par_value[i-11]=rx[i];
		if(setupCount>1)
		{
			if((typevalue[i-11]+billvaltdator.par_value[i-11])!=0xFF)//如果两次读出的不符则重读
			{
				setupCount=0;
				bill_timeout[0]=OSTimeGet();
				return;
			}
		}
		//币值 = 0 不用的币种
		//币值 = 0xFF vend tokens
		if(i>11 && billvaltdator.par_value[i-11]!=0 && billvaltdator.par_value[i-11]!=0xFF)
		{
			if(billvaltdator.par_value[(i-1)-11]>billvaltdator.par_value[i-11])
			{
				setupCount=0;
				bill_timeout[0]=OSTimeGet();
				return;
			}
		}
		typevalue[i-11]=(0xFF-rx[i]);//记录取反
	}
	
	if(setupCount<5)
	{
		bill_timeout[0]=OSTimeGet();
		return;
	}
	
	billequip.EQIU_FUNC=Bill_Security;
	bill_timeout[0]=OSTimeGet();
}

void Bill_Setup2(void)
{
	uint8_t err,i;
	uint16_t *rx, rx_len;//接收数据指针与接收数据长度
	
	FSMMSG *billmsg;
	
	bill_cmd[0]=BILL_SETUP;
	rx = mdb_send_cmd(bill_cmd,1,1,BILL_NO_RESPENSE*10, &err, &rx_len);
	if(err!=MDB_RECEIVE_DATA)
	{
		if(OSTimeGet()-bill_timeout[0]>MAX_BILL_TIMEOUT)
		{
			billequip.EQIU_FUNC=BillValtdator_Reset;
			set_bill_status(0xFF);
		}
		return;
	}
	setupCount++;
	//对SETUP命令返回的结果的处理代码
	billvaltdator.level=rx[0];
	billvaltdator.BCD=rx[2]|(rx[1]<<8);
	billvaltdator.factor=rx[4]|(rx[3]<<8);
	billvaltdator.point=rx[5];
	billvaltdator.recycler=0;//初始化
	billfactor=sysfactor;
	for(i=0;i<billvaltdator.point;i++)
		billfactor=billfactor/10;
	billfactor=billfactor*billvaltdator.factor;
	billvaltdator.capacity=rx[7]|(rx[6]<<8);
	billvaltdator.security=rx[9]|(rx[8]<<8);
	billvaltdator.escrow=rx[10];
	for(i=11;i<rx_len;i++)
	{
		billvaltdator.par_value[i-11]=rx[i];
		if(setupCount>1)//已经读过一次面值
		{
			if((typevalue[i-11]+billvaltdator.par_value[i-11])!=0xFF)//如果两次读出的不符则重读
			{
				setupCount=0;
				bill_timeout[0]=OSTimeGet();
				return;
			}
		}
		//币值 = 0 不用的币种
		//币值 = 0xFF vend tokens
		if(i>11 && billvaltdator.par_value[i-11]!=0 && billvaltdator.par_value[i-11]!=0xFF)//前后面值数据判断
		{
			if(billvaltdator.par_value[(i-1)-11]>billvaltdator.par_value[i-11])
			{
				setupCount=0;
				bill_timeout[0]=OSTimeGet();
				return;
			}
		}
		typevalue[i-11]=(0xFF-rx[i]);//记录取反值
	}
	
	if(setupCount<5)
	{
		bill_timeout[0]=OSTimeGet();
		return;
	}
	
	billmsg=apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
	assert_param(billmsg);
	billmsg->type=MsgBILL;
	billmsg->value=billvaltdator.par_value[chkvalue[1]]*billfactor;
	//////////////////////////////////////////////////////////
	/*
	if(!(billmsg->value==100 
	   ||billmsg->value==500
	   ||billmsg->value==1000
	   ||billmsg->value==2000
	   ||billmsg->value==5000
	   ||billmsg->value==10000))//增加对这个值得判断
	{
		release_fsm_msg(billmsg);
		bill_timeout[0]=OSTimeGet();
		warning_money_err(chkvalue[0],chkvalue[1],billvaltdator.par_value[chkvalue[1]]);
	}*/
	/////////////////////////////////////////////////////////
	//else
	{
		if(chkvalue[0]==0)//钞箱
		{
			if(billequip.cmd_status==cmd_status_start)
			{
				billequip.cmd_status=cmd_status_complete;
				billequip.cmd_type=M_BS_POLL;
			}
			credit_set(credit_get() + billmsg->value);
			billmsg->stype=M_BS_ABOX;
			billvaltdator.escvalue=0;
		}
		else
		if(chkvalue[0]==1)//暂存箱
		{
			billmsg->stype=M_BS_AESC;
			billvaltdator.escvalue=billmsg->value;
		}
		else
		if(chkvalue[0]==2)//纸币退出
		{
			if(billequip.cmd_status==cmd_status_start)
			{
				billequip.cmd_status=cmd_status_complete;
				billequip.cmd_type=M_BS_POLL;
			}
			billmsg->stype=M_BS_AOUT;
			billvaltdator.escvalue=0;
		}
		else
		if(chkvalue[0]==3)//找零箱
		{
			if(billequip.cmd_status==cmd_status_start)
			{
				billequip.cmd_type=M_BS_POLL;
				billequip.cmd_status=cmd_status_complete;
			}
		}
		else
		if(chkvalue[0]==4)//不允许接收的纸币类型(可能是禁能了)
		{
			billmsg->stype=M_BS_DISTYPE;
			billvaltdator.escvalue=0;
		}
		else
		if(chkvalue[0]==5)//人工放入找零箱
		{
			
		
		}
		else
		if(chkvalue[0]==6)//人工找零
		{
			
		
		}
		else
		if(chkvalue[0]==7)//从找零箱到钞箱
		{
			credit_set(credit_get() + billmsg->value);
		}	
		if(send_fsm_msg(billmsg)!=TRUE)
			release_fsm_msg(billmsg);
	}
	billequip.EQIU_FUNC=Bill_Poll_CMD;
	bill_timeout[0]=OSTimeGet();
}

void Bill_Security(void)
{
	uint8_t err;
	uint16_t *rx, rx_len;
	rx = rx;
	
	bill_cmd[0]=BILL_SECURITY;
	bill_cmd[1]=0x00;//安全级别 0:低级;1:高级
	bill_cmd[2]=0x00;
	rx = mdb_send_cmd(bill_cmd,3,1,BILL_NO_RESPENSE, &err, &rx_len);
	if(err!=MDB_RECEIVE_ACK)
	{
		if(OSTimeGet()-bill_timeout[0]>MAX_BILL_TIMEOUT)
		{
			set_bill_status(0xFF);
			billequip.EQIU_FUNC=BillValtdator_Reset;
		}
		return;
	}
	else
	{
		if(billvaltdator.level>1)
			billequip.EQIU_FUNC=Bill_Exp_Identifiy;
		else
			billequip.EQIU_FUNC=Bill_Stacker;
		
		//billequip.EQIU_FUNC=Bill_Exp_Identifiy;
		bill_timeout[0]=OSTimeGet();
	}
}

void Bill_Exp_Identifiy(void)
{
	uint8_t err;
	uint16_t *rx, rx_len;
	rx = rx;//防止编译器报错

	bill_cmd[0]=BILL_EXP;
	bill_cmd[1]=0x00;
	rx = mdb_send_cmd(bill_cmd,2,1,BILL_NO_RESPENSE*10, &err, &rx_len);//发送扩展ID命令
	if(err!=MDB_RECEIVE_DATA)
	{
		if(OSTimeGet()-bill_timeout[0]>MAX_BILL_TIMEOUT)
		{
			set_bill_status(0xFF);
			billequip.EQIU_FUNC=BillValtdator_Reset;
		}
		return;
	}
	else
	{
		//获取
		//1.制造商代码
		//2.串号
		//3.制造分配代码(一机一号?)
		//4.软件版本
		if(billvaltdator.level>1)
			billequip.EQIU_FUNC=Bill_Exp_Feature;
		else
			billequip.EQIU_FUNC=Bill_Stacker;
		bill_timeout[0]=OSTimeGet();
	}
}

void Bill_Exp_Feature(void)
{
	uint8_t err;
	uint16_t *rx, rx_len;
	
	bill_cmd[0]=BILL_EXP;
	bill_cmd[1]=0x02;
	rx = mdb_send_cmd(bill_cmd,2,1,BILL_NO_RESPENSE*10, &err, &rx_len);
	if(err!=MDB_RECEIVE_DATA)
	{
		if(OSTimeGet()-bill_timeout[0]>MAX_BILL_TIMEOUT)
		{
			set_bill_status(0xFF);
			billequip.EQIU_FUNC=BillValtdator_Reset;
		}
		return;
	}
	else
	{
		billvaltdator.ftl=rx[32] & 0x01;			//ftl
		billvaltdator.recycler=(rx[32]>>1) & 0x01;//找零方式
		if((billvaltdator.recycler | billvaltdator.ftl)!=0)//只要有支持的功能
			billequip.EQIU_FUNC=Bill_Exp_EnOption;
		else	
			billequip.EQIU_FUNC=Bill_Stacker;
		bill_timeout[0]=OSTimeGet();
	}
}

void Bill_Exp_EnOption(void)
{
	uint8_t err;
	uint16_t *rx, rx_len;
	rx = rx;//防止编译器报错

	
	bill_cmd[0]=BILL_EXP;
	bill_cmd[1]=0x01;
	bill_cmd[2]=0;//FTL		//0:关闭功能;1:打开功能
	bill_cmd[3]=0;//找零
	bill_cmd[4]=0;//保留
	bill_cmd[5]=billvaltdator.recycler<<1;//目前只支持纸币指令,ftl暂未支持
	rx = mdb_send_cmd(bill_cmd,6,1,BILL_NO_RESPENSE*10, &err, &rx_len);
	if(err != MDB_RECEIVE_ACK)
	{
		if(OSTimeGet()-bill_timeout[0]>MAX_BILL_TIMEOUT)
		{
			set_bill_status(0xFF);
			billequip.EQIU_FUNC=BillValtdator_Reset;
		}
		return;
	}
	else
	{
		if(billvaltdator.recycler==1)//支持找零
			billequip.EQIU_FUNC=Recycler_Setup;
		else
			billequip.EQIU_FUNC=Bill_Stacker;
		bill_timeout[0]=OSTimeGet();
	}
}
	
void Recycler_Setup(void){//查找哪些纸币可以进入找零箱
	uint8_t err,i;
	uint16_t *rx, rx_len;
	
	bill_cmd[0]=BILL_EXP;
	bill_cmd[1]=0x03;
	rx = mdb_send_cmd(bill_cmd,2,1,BILL_NO_RESPENSE, &err, &rx_len);
	if(err!=MDB_RECEIVE_DATA){
		if(OSTimeGet()-bill_timeout[0]>MAX_BILL_TIMEOUT){
			set_bill_status(0xFF);
			billequip.EQIU_FUNC=BillValtdator_Reset;
		}
		return;
	}
	else{
		billvaltdator.recypath=rx[1]|rx[0]<<8;
		for(i=0;i<16;i++){
			if((billvaltdator.recypath>>i)%2==1)
			break;
		}
		billvaltdator.lowest=i;//最小面值
		for(i=15;i>0;i--){
			if((billvaltdator.recypath>>i)%2==1)
			break;
		}
		billvaltdator.highest=i;//最大面值
		billequip.EQIU_FUNC=Recycler_Enable;
		bill_timeout[0]=OSTimeGet();
	}
}
void Recycler_Enable(void){//使能找零面值与进找零钞箱灵敏度
	uint8_t err,i;	
	uint16_t *rx, rx_len;
	rx = rx;//防止编译器报错
	
	bill_cmd[0]=BILL_EXP;
	bill_cmd[1]=0x04;
	//这里可根据收到找零箱的纸币数判定是否需要关闭某种币种的收入
	//billvaltdator.disParNum[i]纸币数量
	bill_cmd[2]=0xFF;//使能全部支持币种进入找零箱
	bill_cmd[3]=0xFF;//使能全部支持币种进入找零箱
	for(i=0;i<16;i++)
		bill_cmd[i+4]=0x03;//最低安全级进找零钞箱
	rx = mdb_send_cmd(bill_cmd,20,1,BILL_NO_RESPENSE, &err, &rx_len);
	if(err!=MDB_RECEIVE_ACK){
		if(OSTimeGet()-bill_timeout[0]>MAX_BILL_TIMEOUT){
			set_bill_status(0xFF);
			billequip.EQIU_FUNC=BillValtdator_Reset;
		}
		return;
	}
	else{
		billequip.EQIU_FUNC=BILL_Dispen_Status;
		bill_timeout[0]=OSTimeGet();
	}
}
void BILL_Dispen_Status(void){//找零箱查询
	uint8_t err,i;
	uint16_t *rx, rx_len;
	
	bill_cmd[0]=BILL_EXP;
	bill_cmd[1]=0x05;
	rx = mdb_send_cmd(bill_cmd,2,1,BILL_NO_RESPENSE, &err, &rx_len); 
	if(err!=MDB_RECEIVE_DATA){
		if(OSTimeGet()-bill_timeout[0]>MAX_BILL_TIMEOUT){
			set_bill_status(0xFF);
			billequip.EQIU_FUNC=BillValtdator_Reset;
		}
		return;
	}
	else{
		uint16_t recyfull;
		billvaltdator.alldisvalue=0;//清空找零箱数值
		recyfull=rx[0]<<8|rx[1];
		for(i=0;i<16;i++){
			billvaltdator.disfull[i]=recyfull&0x01;//找零箱币种已满标志
			recyfull>>=1;
		}
		i=0;
		while(billvaltdator.par_value[i]!=0 && i<32){//找零箱可找零币种数量与总金额
			billvaltdator.disParNum[i/2]=rx[i+2]<<8 | rx[i+3];
			billvaltdator.alldisvalue+=billvaltdator.disParNum[i/2]*billvaltdator.par_value[i/2]*billfactor;
			i+=2;
		}
		if(billequip.cmd_status==cmd_status_need)//准备开始找零
			billequip.EQIU_FUNC=Dispense_Bill;
		else
		if(billequip.cmd_status==cmd_status_start)//退币时poll的时候收到了0x2F
			billequip.EQIU_FUNC=Bill_Poll_Recy;
		else
			billequip.EQIU_FUNC=Bill_Stacker;//查完找零箱查钞箱
		bill_timeout[0]=OSTimeGet();
	}
}
void Dispense_Bill(void){//找零
	uint8_t err;
	uint16_t *rx, rx_len;
	rx = rx;//防止编译器报错
	
	rx = mdb_send_cmd(bill_cmd3,5,1,BILL_NO_RESPENSE, &err, &rx_len);
	if(err!=MDB_RECEIVE_ACK){
		if(OSTimeGet()-bill_timeout[0]>MAX_BILL_TIMEOUT){//超时未有效回复
			set_bill_status(0xFF);
			billequip.EQIU_FUNC=BillValtdator_Reset;
		}
		return;
	}
	else{
		billequip.cmd_status=cmd_status_start;//标记为开始找零
		billequip.EQIU_FUNC=Payout_Value_Poll;
		bill_timeout[0]=OSTimeGet();
	}
}
void Payout_Value_Poll(void){//查询找币过程结果
	uint8_t err;
	uint16_t *rx, rx_len;
	
	bill_cmd[0]=BILL_EXP;
	bill_cmd[1]=0x09;
	rx = mdb_send_cmd(bill_cmd,2,1,BILL_NO_RESPENSE, &err, &rx_len); 
	if(err!=MDB_RECEIVE_DATA && err!=MDB_RECEIVE_ACK){
		if(OSTimeGet()-bill_timeout[0]>MAX_BILL_TIMEOUT){//超时未有效回复
			set_bill_status(0xFF);
			billequip.EQIU_FUNC=BillValtdator_Reset;
		}
		return;
	}
	else
	if(err==MDB_RECEIVE_DATA){
		if(rx[0]==0x00){//未找币(目前来看协议中返回只有0)
			billequip.EQIU_FUNC=Bill_Poll_Recy;
		}
	}	
	else{//找币成功(返回ack)
		billequip.EQIU_FUNC=Bill_Payout_Status;
	}
	bill_timeout[0]=OSTimeGet();
}
void Bill_Payout_Status(void){//查询找币结果
	uint8_t err,i;
	uint16_t payoutnum, type;
	uint16_t *rx, rx_len;
	
	FSMMSG *billmsg;
	bill_cmd[0]=BILL_EXP;
	bill_cmd[1]=0x08;
	rx = mdb_send_cmd(bill_cmd,2,1,BILL_NO_RESPENSE, &err, &rx_len); 
	if(err!=MDB_RECEIVE_DATA && err!=MDB_RECEIVE_ACK){
		if(OSTimeGet()-bill_timeout[0]>MAX_BILL_TIMEOUT){//超时未有效回复
			set_bill_status(0xFF);
			billequip.EQIU_FUNC=BillValtdator_Reset;
		}
		return;
	}
	else
	if(err==MDB_RECEIVE_ACK){//纸币器忙(继续询问)
		//DisplayLT("KKKKKKKKKK",6,0,0);
	}
	else{//查到找零结果
		billmsg=apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
		assert_param(billmsg);
		billmsg->type=MsgBILL;
		billmsg->stype=M_BS_PAYRESULT;
		
		if(rx_len==2)//只返回了0与校验
			billmsg->value=0;
		else{
			for(i=0;i<32;i+=2){
				type=(billvaltdator.recypath>>(i/2))&0x01;//找零种类
				payoutnum=rx[i+1]|(rx[i]<<8);//找零数量
				if(payoutnum!=0 && type!=0){
					billmsg->value=(billvaltdator.par_value[i/2]*billfactor*payoutnum);
					//可扩展终端存储纸币找零
					break;			
				}
			}
		}
		if(send_fsm_msg(billmsg)!=TRUE)
			release_fsm_msg(billmsg);
		if(credit_get() >= billmsg->value)
			credit_set(credit_get() - billmsg->value);
		else
		{
			lcd_show_string("pppppp",6,0, RED, WHITE, font48);
		}

		billvaltdator.alldisvalue=0;//这里保证状态机查询可找时快与MDB总线速度
		billequip.cmd_status=cmd_status_complete;
		billequip.cmd_type=M_BS_POLL;
		billequip.EQIU_FUNC=BILL_Dispen_Status;//查询找零箱金额
	}
	bill_timeout[0]=OSTimeGet();
}

/*
退币查询
*/
void Bill_Poll_Recy(void)
{
	uint8_t err,i,rt;
	uint16_t *rx, rx_len;
	
	FSMMSG *billmsg;
	bill_cmd[0]=BILL_POLL;
	rx = mdb_send_cmd(bill_cmd,1,1,BILL_NO_RESPENSE, &err, &rx_len);
	if(err!=MDB_RECEIVE_DATA && err!=MDB_RECEIVE_ACK){
		if(OSTimeGet()-bill_timeout[0]>MAX_BILL_TIMEOUT){//超时未有效回复
			set_bill_status(0xFF);
			billequip.EQIU_FUNC=BillValtdator_Reset;
		}
		return;
	}
	else
	{
		billmsg=apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
		assert_param(billmsg);
		billmsg->type=MsgBILL;
		billmsg->stype=M_BS_RECYINFO;
		billmsg->value=0;//初始化值
		if(err==MDB_RECEIVE_ACK){//返回ack,说明无异常
			billequip.EQIU_FUNC=Payout_Value_Poll;
			billmsg->value=ACK;
		}
		else
		if(err==MDB_RECEIVE_DATA){
			for(i=0;i<rx_len;i++){
				rt=rx[i]&0x80;
				if(rt==0){//非收币信息
					billmsg->value=rx[i];//收到了找零箱异常信息..............
					switch(billmsg->value){
						case 0x21://压钞请求
							
						break;
						case 0x22://找零忙
							
						break;
						case 0x23://找零器忙,无法回复详细信息
							
						break;
						case 0x24://找零器某个传感器异常
							
						break;
						case 0x25://暂留
						break;
						case 0x26://找零器电机故障
							
						break;
						case 0x27://找零器卡币
						
						break;
						case 0x28://ROM错误
						
						break;
						case 0x29://找零禁止,可能因为损坏纸币或找零口有异物
						
						break;
						case 0x2A://纸币在出口等待取走
						
						break;
						case 0x2B://暂留
						break;
						case 0x2C://暂留
						break;
						case 0x2D://暂留
						break;
						case 0x2E://暂留
						break;
						case 0x2F://按下了什么补币键,需要重新查询找零箱
							billequip.EQIU_FUNC=BILL_Dispen_Status;
						break;
						default:
						break;
					}
					if(billmsg->value!=0x2F)
						billequip.EQIU_FUNC=Payout_Value_Poll;
				}
			}
		}
	}
	if(send_fsm_msg(billmsg)!=TRUE)
		release_fsm_msg(billmsg);
	bill_timeout[0]=OSTimeGet();
}
uint8_t Dispen_Bill(uint32_t money){//判断是否有零钱可找
	uint8_t i,type;
	if(billequip.eqiu_status>1)
		return 2;
	if(money==0)
		return TRUE;
	money/=billfactor;
	type=15;
	for(i=0;i<16;i++){
		if(money<billvaltdator.par_value[type] || billvaltdator.disParNum[type]==0 || ((billvaltdator.recypath>>i) & 0x01) ==0 )
			type--;
		else//前面函数已经判断过可找,此处直接找零
			break;
	}
	if(type==0)//没找到可找金额
		return FALSE;
	bill_cmd3[0]=BILL_EXP;
	bill_cmd3[1]=0x06;
	bill_cmd3[2]=type;
	bill_cmd3[3]=0;
	bill_cmd3[4]=1;//一次只找一张
	billequip.cmd_type=M_BS_PAYOUT;
	billequip.cmd_status=cmd_status_need;
	return TRUE;
}
uint8_t Check_Dispen_Bill(uint32_t money,uint32_t* jackson){//判断是否有零钱可找
	uint8_t type;
	uint32_t mm,num,amount;
	if(billvaltdator.recycler!=1)
		return FALSE;
	if(money==0)
		return TRUE;
	if(billequip.eqiu_status>1)
		return 2;
	if(billvaltdator.alldisvalue==0)
		return 3;//这种可能是没来得急重新查找零箱
	money=money/billfactor;
	mm=money;
	amount=money*billfactor;//可能找零金额
	if(amount==0)
		return FALSE;
	type=16;
	while(type>0 && money>0)
	{
		if(money<billvaltdator.par_value[type-1] \
		|| billvaltdator.disParNum[type-1]==0 \
		|| ((billvaltdator.recypath>>(16-type))&0x01) == 0)
			type--;
		else
		{
			num=money/billvaltdator.par_value[type-1];
			if(num>billvaltdator.disParNum[type-1])
				num=billvaltdator.disParNum[type-1];
			money-=billvaltdator.par_value[type-1]*num;
			type--;
		}
	}
	if(type==0 && mm==money)//找到最后且金额没有变化
		return FALSE;
	*jackson=amount-(money*billfactor);//实际可找零金额
	return TRUE;
}
uint8_t get_dis_money(uint32_t *money,uint32_t *num,uint32_t *type){//隔离驱动层与业务层
						//参数1.可找总金额
						//参数2.可找总张数
						//参数3.可找类型											
	uint8_t i;
	if(billvaltdator.recycler!=1)
		return FALSE;
	if(billequip.eqiu_status>1)
		return 2;
	*money=billvaltdator.alldisvalue;//找零箱金额
	for(i=0;i<16;i++){
		if(((billvaltdator.recypath>>i)&0x0001) != 0){//找币种
			*type=billvaltdator.par_value[i]*billfactor;
			break;//这里目前是只找一种纸币,如果多种则此处不退出
		}
	}
	for(i=0;i<16;i++){
		if(billvaltdator.par_value[i]!=0 && billvaltdator.disParNum[i]!=0){//找数量
			*num=billvaltdator.disParNum[i];
			break;//这里目前是只找一种纸币,如果多种则此处不退出
		}
	}
	return TRUE;
}
uint8_t exit_bill_onlyone(void){//找零函数,只用于有一个找零箱的情况
	uint8_t i,type;
	if(billequip.eqiu_status>1)
		return 2;
	if(billvaltdator.alldisvalue==0)
		return 3;//这种可能是没来得急重新查找零箱
	type=15;
	for(i=0;i<16;i++){
		if(billvaltdator.disParNum[type]==0 \
		|| ((billvaltdator.recypath>>i)&0x01) == 0 )
			type--;
		else
			break;
	}
	if(type==0)//没找到可找金额
		return FALSE;
	bill_cmd3[0]=BILL_EXP;
	bill_cmd3[1]=0x06;
	bill_cmd3[2]=type;
	bill_cmd3[3]=0;
	bill_cmd3[4]=1;//一次只找一张
	billequip.cmd_type=M_BS_PAYOUT;
	billequip.cmd_status=cmd_status_need;
	return TRUE;
}



void Bill_Stacker(void)
{
	uint8_t err;
	uint16_t *rx, rx_len;
	
	bill_cmd[0]=BILL_STACKER;
	rx = mdb_send_cmd(bill_cmd,1,1,BILL_NO_RESPENSE*2, &err, &rx_len);
	if(err!=MDB_RECEIVE_DATA)
	{
		if(OSTimeGet()-bill_timeout[0]>MAX_BILL_TIMEOUT)
		{
			set_bill_status(0xFF);
			billequip.EQIU_FUNC=BillValtdator_Reset;
		}
		return;
	}
	//此对 STACKER 命令返回的结果的处理代码
	billvaltdator.full=(rx[0]&0x80>>7);
	billvaltdator.stock=((rx[0]<<8|rx[1])&0x7FFFF);
	
	billequip.EQIU_FUNC=Bill_TYPE;
	bill_timeout[0]=OSTimeGet();
}

void Bill_TYPE(void)
{
	uint8_t err;	
	uint16_t *rx, rx_len;
	rx = rx;//防止编译器报错
	
	bill_cmd[0]=BILL_TYPE;
	if(billset==1)
	{
		//使能接收纸币种类
		bill_cmd[1]=0xFF;
		bill_cmd[2]=0xFF;
//		bill_cmd[1]=billvaltdator.par_enable>>8;
//		bill_cmd[2]=billvaltdator.par_enable;
	}
	else
	{
		bill_cmd[1]=0x00;
		bill_cmd[2]=0x00;
	}
	if(billvaltdator.escrow==0xFF)
	{
		//使能暂存纸币种类
		bill_cmd[3]=0xFF;
		bill_cmd[4]=0xFF;
//		bill_cmd[3]=billvaltdator.par_enable>>8;
//		bill_cmd[4]=billvaltdator.par_enable;
	}
	else
	{
		bill_cmd[3]=0x00;
		bill_cmd[4]=0x00;
	}
	rx = mdb_send_cmd(bill_cmd,5,1,BILL_NO_RESPENSE, &err, &rx_len);
	if(err!=MDB_RECEIVE_ACK)
	{
		if(OSTimeGet()-bill_timeout[0]>MAX_BILL_TIMEOUT)
		{
			set_bill_status(0xFF);
			billequip.EQIU_FUNC=BillValtdator_Reset;
		}
		return;
	}
	if(billset==1)
		set_bill_status(1);
	else
	if(billset==0)
		set_bill_status(0);
	else
		set_bill_status(billset);
	if(OSTimeGet()-bill_timeout[1]>OS_TICKS_PER_SEC*5)
	{
		bill_timeout[1]=OSTimeGet();
		//billequip.EQIU_FUNC=Bill_Setup;
		billequip.EQIU_FUNC=Bill_Security;
	}
	else
		billequip.EQIU_FUNC=Bill_Poll_CMD;
	bill_timeout[0]=OSTimeGet();
}

void Bill_Poll_CMD(void)
{
	uint8_t err,i,rt,path,type;
	uint16_t *rx, rx_len;
	
	FSMMSG * billmsg;
	
	if(billequip.cmd_status==cmd_status_need)
	{
		if(billequip.cmd_type==M_BS_ESCROW)//压钞指令
		{
			billequip.EQIU_FUNC=Bill_Escrow_CMD;
			bill_timeout[0]=OSTimeGet();
			return;
		}
		else
		if(billequip.cmd_type==M_BS_PAYOUT){//找零指令
			billequip.EQIU_FUNC=BILL_Dispen_Status;//*这里必须先查询找零箱,否则会出现找零结果到状态机早与找完查询找零箱*
			//billequip.EQIU_FUNC=Dispense_Bill;
			bill_timeout[0]=OSTimeGet();
			return;
		}
	}

	bill_cmd[0]=BILL_POLL;
	rx = mdb_send_cmd(bill_cmd,1,1,BILL_NO_RESPENSE, &err, &rx_len);
	if(err!=MDB_RECEIVE_ACK && err!=MDB_RECEIVE_DATA)
	{
		if(OSTimeGet()-bill_timeout[0]>MAX_BILL_TIMEOUT)
		{
			set_bill_status(0xFF);
			billequip.EQIU_FUNC=BillValtdator_Reset;
		}
		return;
	}
	else
	if(err==MDB_RECEIVE_ACK)
	{	
		/*if(billequip.cmd_status==cmd_status_need)//需要执行poll以外命令
		{
			if(billequip.cmd_type==M_BS_ESCROW)//压钞指令
			{
				billequip.EQIU_FUNC=Bill_Escrow_CMD;
				bill_timeout[0]=OSTimeGet();
			}
		}
		else*/
		{
			if(billvaltdator.recycler==1)
				billequip.EQIU_FUNC=BILL_Dispen_Status;
			else
				billequip.EQIU_FUNC=Bill_Stacker;
			bill_timeout[0]=OSTimeGet();
		}
	}
	else
	if(err==MDB_RECEIVE_DATA)
	{
		for(i=0;i<rx_len;i++)
		{
			rt=rx[i]&0x80;
			if(rt==0)//纸币器状态
			{
				/*************** 纸币器错误代码 ******************/
				//1:电机不在位;
				//2:传感器故障;
				//3:纸币器忙;
				//4:ROM错误
				//5:卡币;
				//6:复位;
				//7:纸币退出(暂存箱由未知原因退币);
				//8:钞箱不在位;
				//9:禁能;
				//10:escrow命令错误;
				//11:退币(无法识别);
				//12:貌似盗币
				//
				/**********************************************/
				if(rx[i]==0x01 \
				 ||rx[i]==0x02 \
				 ||rx[i]==0x04 \
				 ||rx[i]==0x05 \
				 ||rx[i]==0x08){//几种无法工作的情况
					set_bill_status(rx[i]+2);
				}
				else
				if(rx[i]==0x03){//忙
					//bill_timeout[0]=OSTimeGet();
				}
				else
				if(rx[i]==0x06){//复位
					//DisplayLT("PPPPPPPPPPPPPPP",0,0,0);
					if(resetflag==0){//
						billequip.eqiu_status=5;
						billequip.EQIU_FUNC=BillValtdator_Reset;
					}
					else
						resetflag=0;
				}
				else
				if(rx[i]==0x07 \
				|| rx[i]==0x0B){//纸币退出(发退钞命令不进入这里)
					billmsg=apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
					assert_param(billmsg);
					billmsg->type=MsgBILL;
					billmsg->stype=M_BS_OTHER;
					billmsg->value=rx[i];
					if(send_fsm_msg(billmsg)!=TRUE)
						release_fsm_msg(billmsg);
					if(rx[i]==0x0B){//不能识别的纸币
						status_equip_msg(1,0xFF,0x42);
					}
					else{
						status_equip_msg(1,0xFF,0x41);
					}
				}
				else
				if(rx[i]==0x09){//禁能
					//set_bill_status(0);
					billequip.EQIU_FUNC=Bill_Stacker;
					bill_timeout[0]=OSTimeGet();
				}
				else
				if(rx[i]==0x0A){//无法接受的压钞命令(可能暂存箱中无纸币)
					if(billequip.cmd_status==cmd_status_start){//如果有正在执行的命令
						billequip.cmd_type=M_BS_POLL;
						billequip.cmd_status=cmd_status_complete;
					}
				}
			}
			else//纸币接收情况
			{
				path=(rx[i]&0x70)>>4;
				type=rx[i]&0x0F;
				if(path<8)
				{
					billmsg=apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
					assert_param(billmsg);
					billmsg->type=MsgBILL;
					billmsg->value=billvaltdator.par_value[type]*billfactor;
					/*************** 对币值的强制判定 ***************/
					/*
					if(!(billmsg->value==100 
					   ||billmsg->value==500
					   ||billmsg->value==1000
					   ||billmsg->value==2000
					   ||billmsg->value==5000
					   ||billmsg->value==10000)
					   || (billvaltdator.par_value[type]+typevalue[type])!=0xFF)//增加对这个值得判断

					{
						release_fsm_msg(billmsg);
						chkvalue[0]=path;
						chkvalue[1]=type;
						setupCount=0;
						billequip.EQIU_FUNC=Bill_Setup2;
						bill_timeout[0]=OSTimeGet();
						return;
					}*/
					/*********************************************/
					if(path==0){//钞箱
						if(billequip.cmd_status==cmd_status_start){//如果有命令正在执行
							billequip.cmd_status=cmd_status_complete;
							billequip.cmd_type=M_BS_POLL;
						}
						credit_set(credit_get() + billmsg->value);
						billmsg->stype=M_BS_ABOX;
						billvaltdator.escvalue=0;
					}
					else
					if(path==1){//暂存箱
						billmsg->stype=M_BS_AESC;
						billvaltdator.escvalue=billmsg->value;
					}
					else
					if(path==2){//纸币退出
						if(billequip.cmd_status==cmd_status_start){//有命令正在执行
							billequip.cmd_status=cmd_status_complete;
							billequip.cmd_type=M_BS_POLL;
						}
						billmsg->stype=M_BS_AOUT;
						billvaltdator.escvalue=0;
					}
					else
					if(path==3){//找零箱
						if(billequip.cmd_status==cmd_status_start)
						{
							billmsg->stype=M_BS_RECY;
							credit_set(credit_get() + billmsg->value);
							billvaltdator.escvalue=0;
							billequip.cmd_type=M_BS_POLL;
							billequip.cmd_status=cmd_status_complete;
						}
					}
					else
					if(path==4){//不允许接收的纸币类型(可能是禁能了)
						billmsg->stype=M_BS_DISTYPE;
						//billmsg->value=0x40;
						billvaltdator.escvalue=0;
						status_equip_msg(1,0xFF,0x40);
					}
					else
					if(path==5){//人工放入找零箱
					
					}
					else
					if(path==6){//人工找零
					
					}
					else
					if(path==7){//从找零箱到钞箱
						billmsg->stype=M_BS_ABOX;
					credit_set(credit_get() + billmsg->value);
						billvaltdator.escvalue=0;
					}	
					if(send_fsm_msg(billmsg)!=TRUE)
						release_fsm_msg(billmsg);
				}
				else
					billvaltdator.escvalue=0;
			}
		}
		bill_timeout[0]=OSTimeGet();
	}
	/*
	if(billequip.cmd_status==cmd_status_start)
	{
		if(OSTimeGet()-bill_timeout[1]>OS_TICKS_PER_SEC*30)//发送压钞或者退币命令超过30秒无协议规定的回复
		{
			//发送吞币???
			//send_moneydelete_msg(DATA_CC,0);
			billequip.cmd_type=M_BS_POLL;
			//DisplayLT("COIN DIS ERR 1",6,0,0);
		}
	}
	*/
}

uint8_t ESCROW(uint8_t direction){//暂存箱操作
	bill_cmd2[0]=BILL_ESCROW;
	bill_cmd2[1]=direction;
	billequip.cmd_type=M_BS_ESCROW;
	billequip.cmd_status=cmd_status_need;
	return TRUE;
}

void Bill_Escrow_CMD(void){//压钞或退钞
	uint8_t err;
	uint16_t *rx, rx_len;
	rx = rx;//防止编译器报错
	
	rx = mdb_send_cmd(bill_cmd2,2,1,BILL_NO_RESPENSE, &err, &rx_len);
	
	if(err!=MDB_RECEIVE_ACK){
		if(OSTimeGet()-bill_timeout[0]>MAX_BILL_TIMEOUT){//超时未有效回复
			set_bill_status(0xFF);
			billequip.EQIU_FUNC=BillValtdator_Reset;
		}
		return;
	}
	else{
		billequip.cmd_status=cmd_status_start;
		billequip.EQIU_FUNC=Bill_Poll_CMD;
		bill_timeout[0]=OSTimeGet();
		bill_timeout[1]=OSTimeGet();
	}
}

uint8_t CE_BILL(uint8_t type){//禁使能

	if(billset==1 || billset==0)
		billset=type;
	//billequip.cmd_type=COIN_TYPE;
	//billequip.cmd_status=cmd_status_need;
	return TRUE;
}

