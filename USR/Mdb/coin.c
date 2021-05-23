/*
硬币器驱动

*/
#include "header.h"
#include "coin.h"
#include "bill.h"
#include "mdb_driver.h"


#define MAX_COIN_TIMEOUT	OS_TICKS_PER_SEC*3	//最大超时时间3秒
#define COIN_JUST_RESET		0x0B

uint8_t coin_buff[20];
uint8_t checkcoinnum=0;

OS_EVENT*     SEM_COIN_CMD;//用于命令的信号量
CoinChanger   coinchanger={0,0,0,0,0,{0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0},0,{0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0},0};
mdbListNode coinequip;
uint32_t coin_reset_time;
uint8_t coin_cmd[16]={0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
uint8_t coin_cmd2[16]={0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
uint32_t coinfactor=0;
uint8_t coinset;//硬币器当前状态
uint8_t coinchangenum;
uint32_t coin_timeout[2]={0,0};

void CoinChanger_Reset(void);
void Coin_Poll(void);
void Coin_TubeStatus(void);
void Coin_Type(void);
void Coin_Setup(void);
void CoinReset_Poll(void);
void Coin_Exp_Id(void);

uint8_t enloughcoin(void)
{
	int i,j,k,num;
	uint8_t a[16];
	uint16_t b[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	uint32_t value;
	//return TRUE;

	if(billequip.eqiu_status<2)
	{
		//这里可加入对暂存开启的不同处理方式
		for(i=15;i>=0;i--)
		{
			if(billvaltdator.par_value[i]!=0 && billvaltdator.par_value[i]!=0xFF && ((billvaltdator.par_enable>>i)&0x01)==1)
			break;
		}
		if(coinchanger.allvalue<(billvaltdator.par_value[i]*billfactor))
			return FALSE;
	}
	if(credit_get()+billvaltdator.escvalue>coinchanger.allvalue)
		return FALSE;
	if(coinchanger.lowest==coinchanger.highest)
		return TRUE;
	k=0;
	a[0]=coinchanger.par_value[coinchanger.lowest];
	b[0]+=coinchanger.store[coinchanger.lowest];
	for(i=coinchanger.lowest+1;i<=coinchanger.highest;i++)
	{
		if(coinchanger.par_value[i]!=a[k])
		{
			k++;
			a[k]=coinchanger.par_value[i];
		}
		b[k]+=coinchanger.store[i];
	}
	if(k==0)//如果只有一种硬币则返回OK。
	return TRUE;
	if(b[0]==0)//如果不只一种硬币，但是最小的硬币为空则返回FALSE.
	return FALSE;
	for(i=0;i<k;i++)
	{
		value=a[i+1];
		j=i;
		while(value!=0 && j>=0)
		{
			num=value/a[j];
			if(num>b[j])
				num=b[j];
			value-=a[j]*num;
			j--;
		}
		if(value!=0)
			return FALSE;
	}
	return TRUE;
}
uint8_t readcoinset()
{
//	I2C_ReadNByte(CAT24WC256,TWO_BYTE_SUBA,CA10,coin_buff,CA11_LENTH);
	return coin_buff[0];
}

void setcoinCE(uint8_t num)
{
	coin_buff[0]=num;
//	I2C_WriteNByte(CAT24WC256,TWO_BYTE_SUBA,CA10,coin_buff,CA11_LENTH);
}

void CoinChangerInit(void)
{
	coinequip.EQIU_FUNC=CoinChanger_Reset;
	coinequip.cmd_type=COIN_RESET;
	coinequip.cmd_status=cmd_status_complete;
	coinequip.eqiu_status=0xFE;
	register_mdb_equ(&coinequip);
	coin_reset_time=1000;
	coinset=0;
	coinchangenum=0;
}

void set_coin_status(uint8_t num)
{
   FSMMSG * coinmsg;
   if(coinequip.eqiu_status!=num)
   {
   	//添加对状态转换频繁的处理
//   	if(day!=DOM)
//   	{
//	   	day=DOM;
//	   	coinchangenum=0;
//   	}
	
   	if(coinchangenum<50)
   	{
		coinchangenum++;
		if(num>1)
			status_equip_msg(1,0xFF,num);
		else
		if(num==1 && coinequip.eqiu_status>1)
			status_equip_msg(1,0xFF,2);
		else
		if(num==0 && coinequip.eqiu_status>1)
			status_equip_msg(1,0xFF,2);
		else
			coinchangenum--;
	}
	coinmsg=apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
	assert_param(coinmsg);
	coinmsg->type=MsgCOIN;
	if(coinequip.eqiu_status==0xFE)
		coinmsg->stype=M_CS_INIT;
	else
		coinmsg->stype=M_CS_STATUS;
	coinmsg->value=num;
	coinequip.eqiu_status=num;
	if(send_fsm_msg(coinmsg)!=TRUE)
		release_fsm_msg(coinmsg);
   }
}



void savedispense(uint16_t num,uint8_t count)//记录硬币的找零
{
//	send_moneydispence_msg(DATA_CC,num,count);
//	CA401 +=num*count; //总找零金额
//	CA403 +=num*count;
//	CA901 +=num*count;
//	CA902 +=num*count;

//	switch(num)
//	{
//		case 5:
//		CA1202(0)+=count;
//		CA1203(0)+=count;
//		CA1204(0)+=count;
//		CA1205(0)+=count;
//		break;
//		case 10:
//		CA1202(1)+=count;
//		CA1203(1)+=count;
//		CA1204(1)+=count;
//		CA1205(1)+=count;
//		break;
//		case 25:
//		CA1202(2)+=count;
//		CA1203(2)+=count;
//		CA1204(2)+=count;
//		CA1205(2)+=count;
//		break;
//		case 50:
//		CA1202(3)+=count;
//		CA1203(3)+=count;
//		CA1204(3)+=count;
//		CA1205(3)+=count;
//		break;
//		case 75:
//		CA1202(4)+=count;
//		CA1203(4)+=count;
//		CA1204(4)+=count;
//		CA1205(4)+=count;
//		break;
//		case 100:
//		CA1202(5)+=count;
//		CA1203(5)+=count;
//		CA1204(5)+=count;
//		CA1205(5)+=count;
//		break;
//		case 200:
//		CA1202(6)+=count;
//		CA1203(6)+=count;
//		CA1204(6)+=count;
//		CA1205(6)+=count;
//		break;
//		case 500:
//		CA1202(7)+=count;
//		CA1203(7)+=count;
//		CA1204(7)+=count;
//		CA1205(7)+=count;
//		break;
//		case 1000:
//		CA1202(8)+=count;
//		CA1203(8)+=count;
//		CA1204(8)+=count;
//		CA1205(8)+=count;
//		break;
//		default:
//		break;
//	}
}

void savecoinaccept(uint16_t num)//记录硬币的接收
{
	
//  #ifdef COINHOPPER
//	if(coinacceptflag==0)
//		return;
//   	coinacceptflag--;
//  #endif


//	send_moneyaccept_msg(DATA_CC,num,1);
//	CA301+=num;
//	CA305+=num;
//	//CA306+=num
//	//CA307++;
//	switch(num)
//	{
//		case 5:
//		CA1102(0)++;
//		CA1105(0)++;
//		break;
//		case 10:
//		CA1102(1)++;
//		CA1105(1)++;
//		break;
//		case 25:
//		CA1102(2)++;
//		CA1105(2)++;
//		break;
//		case 50:
//		CA1102(3)++;
//		CA1105(3)++;
//		break;
//		case 75:
//		CA1102(4)++;
//		CA1105(4)++;
//		break;
//		case 100:
//		CA1102(5)++;
//		CA1105(5)++;
//		break;
//		case 200:
//		CA1102(6)++;
//		CA1105(6)++;
//		break;
//		case 500:
//		CA1102(7)++;
//		CA1105(7)++;
//		break;
//		case 1000:
//		CA1102(8)++;
//		CA1105(8)++;
//		break;
//		default:
//		break;
//	}
}


void CoinChanger_Reset(void)
{
	uint8_t err,i;
	uint16_t *rx, rx_len;
	rx = rx;//防止编译器报错
	
	if((OSTimeGet()-coin_reset_time)>OS_TICKS_PER_SEC*2)//2.5秒复位一次
    {
		coinequip.cmd_type=M_CS_INIT;
		for(i=0;i<24;i++)
			coinchanger.extenderr[i]=0;
		coin_timeout[1]=OSTimeGet();
		coin_timeout[0]=OSTimeGet();
		coinequip.cmd_status=cmd_status_complete;
		coin_cmd[0]=COIN_RESET;
		coin_reset_time=OSTimeGet();
		rx = mdb_send_cmd(coin_cmd, 1, 1, COIN_NO_RESPENSE, &err, &rx_len);//超过10ms认为是silence 
		if(err == MDB_RECEIVE_ACK)
		{
			coinequip.EQIU_FUNC = CoinReset_Poll;
			coin_timeout[1] = OSTimeGet();//记录第一个reset后的时间
			coin_timeout[0] = OSTimeGet();//记录要监控的流程超时起始时间
		}
		else
			set_coin_status(0xFF);
	}
}

void CoinReset_Poll(void)
{
	uint8_t err;
	uint16_t *rx, rx_len;
	
	coin_cmd[0]=COIN_POLL;
	rx = mdb_send_cmd(coin_cmd,1,1,COIN_NO_RESPENSE, &err, &rx_len);
	
	if(err!=MDB_RECEIVE_ACK && err!=MDB_RECEIVE_DATA)
	{
		if(OSTimeGet()-coin_timeout[0]>MAX_COIN_TIMEOUT)//超时
		{
			set_coin_status(0xFF);
			coinequip.EQIU_FUNC=CoinChanger_Reset;
		}
		return;
	}
	else
	{
		if(err == MDB_RECEIVE_DATA)
		{
			uint8_t i,err=FALSE;
			for(i=0;i<rx_len;i++)
			{
				if(rx[i]==0x02)//硬币器忙
				{
					//coin_timeout[0]=OSTimeGet();
				}
				else
				if(rx[i]==COIN_JUST_RESET)
				{
					err = TRUE;
				}
			}
			if(err != TRUE)
			{
				lcd_show_string("2", 0, 0, RED, WHITE, font48);//没有just reset老款硬币器
			}
		}
		else
		if(err==MDB_RECEIVE_ACK)
		{
			lcd_show_string("1", 50, 0, RED, WHITE, font48);//没有just reset老款硬币器直接回的ack
		}
	}
	coinequip.EQIU_FUNC=Coin_Setup;
	coin_timeout[0]=OSTimeGet();
}

void Coin_Setup(void)
{
	uint8_t err,i,flg=FALSE;
	uint16_t *rx, rx_len;
	
	coin_cmd[0]=COIN_SETUP;
	rx = mdb_send_cmd(coin_cmd,1,1,COIN_NO_RESPENSE*5, &err, &rx_len);//他妈的，居然要等30ms什么玩意啊
	if(err!=MDB_RECEIVE_DATA)
	{
		if(OSTimeGet()-coin_timeout[0]>MAX_COIN_TIMEOUT)
		{
			coinequip.EQIU_FUNC=CoinChanger_Reset;
			set_coin_status(0xFF);
		}
		return;
	}
	//对SETUP命令返回的结果的处理
	coinchanger.level=rx[0];
	if(coin_level != 2)coinchanger.level=1;//金码硬币器,可能有问题
	coinchanger.BCD=rx[2];
	coinchanger.BCD+=rx[1]<<8;
	coinchanger.factor=rx[3];
	coinchanger.point=rx[4];
	coinfactor=sysfactor;
	for(i=0;i<coinchanger.point;i++)
		coinfactor=coinfactor/10;
	coinfactor=coinfactor*coinchanger.factor;
	coinchanger.path=(rx[5]<<8)|rx[6];
	for(i=0;i<16;i++)
	{
		if((coinchanger.path>>i)%2==1)
			break;
	}
	coinchanger.lowest=i;
	for(i=15;i>0;i--)
	{
		if((coinchanger.path>>i)%2==1)
		break;
	} 
	coinchanger.highest=i;
	
	for(i=7;i<rx_len;i++)
	{
		if(flg==TRUE)
		{
			if(coinchanger.par_value[i-7-1]>rx[i] \
			&& coinchanger.par_value[i-7-1]!=0 \
			&& coinchanger.par_value[i-7-1]!=0xFF \
			&& rx[i]!=0 \
			&& rx[i]!=0xFF)//如果前面的面值大于后面的面值
			{
				return;
			}
		}
		flg=TRUE;
		coinchanger.par_value[i-7]=rx[i];
	}
	
	if(coinchanger.level>2)//支持层级3
	{
		coinequip.EQIU_FUNC=Coin_Exp_Id;
	}
	else//支持层级3以下
	{
		coinequip.EQIU_FUNC=Coin_TubeStatus;
	}
	coin_timeout[0]=OSTimeGet();
}


void Coin_Exp_Dignostic(void)//扩展指令的状态查询
{
	uint8_t err,i;
	uint16_t *rx, rx_len;
	
	coin_cmd[0]=0x0F;
	coin_cmd[1]=0x05;
	rx = mdb_send_cmd(coin_cmd,2,1,COIN_NO_RESPENSE*5, &err, &rx_len);//发送扩展命令	
	if(err==MDB_RECEIVE_DATA)
	{
		uint8_t ggerr[24];
		for(i=0;i<24;i++)
			ggerr[i]=0;
		for(i=0;i<rx_len;i+=2)
			ggerr[rx[i]]=rx[i+1];
		if(coinchanger.extenderr[21]==0 && ggerr[21]!=0)
			set_coin_status(0xFB);
		if(coinchanger.extenderr[21]!=0 && ggerr[21]==0)//用来判断状态的转换
			set_coin_status(coinset);
		for(i=0;i<24;i++)
			coinchanger.extenderr[i]=ggerr[i];
	}
}

void Coin_Exp_Feature(void)//扩展指令的功能使能
{
	uint8_t err;
	uint16_t *rx, rx_len;
	rx = rx;//防止编译器报错
	
	coin_cmd[0]=0x0F;
	coin_cmd[1]=0x01;
	coin_cmd[2]=0x00;
	coin_cmd[3]=0x00;
	coin_cmd[4]=0x00;
	coin_cmd[5]=0x00;
	if(coinchanger.changetype==1)//如果有扩展找零方式
		coin_cmd[5]|=0x01;
	if(coinchanger.diagnostic==1)//如果有扩展统计方式
		coin_cmd[5]|=0x02;
	if(coinchanger.fill==1)//如果有扩展补币方式
		coin_cmd[5]|=0x00;//暂时不用
	if(coinchanger.ftl==1)//如果有扩展补币方式
		coin_cmd[5]|=0x00;//暂时不用
	rx = mdb_send_cmd(coin_cmd,6,1,COIN_NO_RESPENSE*5, &err, &rx_len);//发送扩展功能使能命令
	if(err!=MDB_RECEIVE_ACK)
	{
		if(OSTimeGet()-coin_timeout[0]>MAX_COIN_TIMEOUT)
		{
			//DisplayLT("4",0,3,0);
			set_coin_status(0xFF);
			coinequip.EQIU_FUNC=CoinChanger_Reset;
		}
		return;
	}
	coinequip.EQIU_FUNC=Coin_TubeStatus;
	coin_timeout[0]=OSTimeGet();
}

void Coin_Exp_Id(void)//扩展指令获取ID
{
	uint8_t err;
	uint16_t *rx, rx_len;
	
	coin_cmd[0]=COIN_EXP;
	coin_cmd[1]=0x00;
	rx = mdb_send_cmd(coin_cmd,2,1,COIN_NO_RESPENSE*10, &err, &rx_len);//发送扩展ID命令
	if(err!=MDB_RECEIVE_DATA)
	{
		if(OSTimeGet()-coin_timeout[0]>MAX_COIN_TIMEOUT)
		{
			//DisplayLT("3",0,2,0);
			set_coin_status(0xFF);
			coinequip.EQIU_FUNC=CoinChanger_Reset;
		}
		return;
	}
	else
	{
		//处理ID命令的返回
		coinchanger.changetype=rx[32] & 0x01;		//找零方式
		coinchanger.diagnostic=(rx[32]>>1) & 0x01;	//扩展状态查询
		coinchanger.fill=(rx[32]>>2) & 0x01;			//补币
		coinchanger.ftl=(rx[32]>>3) & 0x01; 			//ftl传输
		
		if(coinchanger.changetype \
			|coinchanger.diagnostic \
			|coinchanger.fill \
			|coinchanger.ftl!=0)//虽然是level3,但是不支持所有扩展指令
			coinequip.EQIU_FUNC=Coin_Exp_Feature;
		else
		{
			//DisplayLT("7",0,6,0);
			coinequip.EQIU_FUNC=Coin_TubeStatus;
		}
		coin_timeout[0]=OSTimeGet();//计算超时时间
	}
}

void Payout_Status(void)
{
	uint8_t err;
	uint16_t *rx, rx_len;
	FSMMSG *coinmsg;
	
	coin_cmd[0]=COIN_EXP;
	coin_cmd[1]=0x03;
	rx = mdb_send_cmd(coin_cmd,2,1,COIN_NO_RESPENSE*5, &err, &rx_len);
	if(err!=MDB_RECEIVE_DATA && err!=MDB_RECEIVE_ACK)
	{
		if(OSTimeGet()-coin_timeout[0]>MAX_COIN_TIMEOUT)
		{
			set_coin_status(0xFF);
			coinequip.EQIU_FUNC=CoinChanger_Reset;
		}
		return;
	}
	else
	{
		if(err==MDB_RECEIVE_DATA)
		{
			uint8_t i,num;
			uint32_t value;
			
			coinmsg=apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
			assert_param(coinmsg);
			coinmsg->type=MsgCOIN;
			coinmsg->stype=M_CS_PAYRESULT_LEV3;
			for(i=0;i<16;i++)
			{
				num=rx[i];//数量
				if(num!=0)//这里允许返回枚数为0	
				{			
					value=coinchanger.par_value[i]*coinfactor;//面值
					if(value!=0)
					{
						savedispense(value,num);
						if(credit_get()>=value*num)
						credit_set(credit_get() - value*num);//找零后余额
					}
				}		
			}
			if(send_fsm_msg(coinmsg)!=TRUE)
				release_fsm_msg(coinmsg);
			
			coinchanger.allvalue=0;//金额清零
			coinequip.cmd_status=cmd_status_complete;
			coinequip.cmd_type=M_CS_POLL;
			coinequip.EQIU_FUNC=Coin_TubeStatus;
			coin_timeout[0]=OSTimeGet();
		}
		else
		if(err==MDB_RECEIVE_ACK)//硬币器忙
		{
			coin_timeout[0]=OSTimeGet();
			return;
		}
	}
}

void Payout_Value(void)
{
	uint8_t err;
	uint16_t *rx, rx_len;
	FSMMSG *coinmsg;
	
	coin_cmd[0]=COIN_EXP;
	coin_cmd[1]=0x04;
	//while(err!=MDB_RECEIVE_ACK && i<200)//加快退币速度
	{
		rx = mdb_send_cmd(coin_cmd,2,1,COIN_NO_RESPENSE, &err, &rx_len);
		if(err!=MDB_RECEIVE_DATA && err!=MDB_RECEIVE_ACK)
		{
			if(OSTimeGet()-coin_timeout[0]>MAX_COIN_TIMEOUT)
			{
				set_coin_status(0xFF);
				coinequip.EQIU_FUNC=CoinChanger_Reset;
			}
			return;
		}
		else
		{
			if(err==MDB_RECEIVE_DATA)
			{
				if(rx[0]==0)//没有退币
				{
					coin_timeout[0]=OSTimeGet();
					return;
				}
				coinmsg=apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
				assert_param(coinmsg);
				coinmsg->type=MsgCOIN;
				coinmsg->stype=M_CS_DISPENSED;
				coinmsg->value=rx[0]*coinfactor;
				if(send_fsm_msg(coinmsg)!=TRUE)
					release_fsm_msg(coinmsg);
				//credit-=coinmsg->value;//找零后余额
			}
			else
			if(err==MDB_RECEIVE_ACK)//退币完成
			{
				coinequip.EQIU_FUNC=Payout_Status;
				coin_timeout[0]=OSTimeGet();
			}
		}
	}
}

void Coin_Dispense_CMD(void)
{
 	uint8_t err;
 	uint16_t *rx, rx_len;
	rx = rx;//防止编译器报错
	
	if(coinchanger.level>2 && coinchanger.changetype==1)//层级3且支持扩展找零
		rx = mdb_send_cmd(coin_cmd2,3,1,COIN_NO_RESPENSE, &err, &rx_len);
	else
		rx = mdb_send_cmd(coin_cmd2,2,1,COIN_NO_RESPENSE, &err, &rx_len);
	if(err!=MDB_RECEIVE_ACK)
	{
		if(OSTimeGet()-coin_timeout[0]>MAX_COIN_TIMEOUT)
		{
			set_coin_status(0xFF);
			coinequip.EQIU_FUNC=CoinChanger_Reset;
		}
		return;
	}
	else
	if(err==MDB_RECEIVE_ACK)
	{
		if(coinchanger.level>2 && coinchanger.changetype==1)
		{
			coinequip.EQIU_FUNC=Payout_Value;
		}
		else
		{
			coinequip.cmd_status=cmd_status_start;//需要poll监控
			coinequip.EQIU_FUNC=Coin_Poll;
		}
		coin_timeout[0]=OSTimeGet();
	}
}

void Coin_Diagnostic_Status(void)//扩展指令查询硬币器状态
{
	uint8_t err,i;
	uint16_t *rx, rx_len;
	
	coin_cmd[0]=COIN_EXP;
	coin_cmd[1]=0x05;
	rx = mdb_send_cmd(coin_cmd,2,1,COIN_NO_RESPENSE*10, &err, &rx_len);
	if(err!=MDB_RECEIVE_DATA)
	{
		if(OSTimeGet()-coin_timeout[0]>MAX_COIN_TIMEOUT)
		{
			set_coin_status(0xFF);
			coinequip.EQIU_FUNC=CoinChanger_Reset;
		}
		return;
	}
	else
	{
		if(err==MDB_RECEIVE_DATA)
		{
			uint8_t ggerr[24];
			for(i=0;i<24;i++)
				ggerr[i]=0;
			for(i=0;i<rx_len;i+=2)//每两个字节表示一个状态
				ggerr[rx[i]]=rx[i+1];
			if(coinchanger.extenderr[0x15]==0 && ggerr[0x15]!=0)
				coinset=0xFB;
			if(coinchanger.extenderr[0x15]!=0 && ggerr[0x15]==0)
				coinset=1;
			set_coin_status(coinset);
			for(i=0;i<24;i++)
				coinchanger.extenderr[i]=ggerr[i];
		}
		coinequip.EQIU_FUNC=Coin_Setup;
		coin_timeout[0]=OSTimeGet();
	}
}
	
void Coin_TubeStatus(void)//储币管状态查询命令
{
	uint8_t err,i;
	uint16_t *rx, rx_len;
	
	if(coinequip.cmd_type==M_CS_INIT)//reset后第一次TUBESTATUS,等待500ms
	{
		if(OSTimeGet()-coin_timeout[1]<COIN_NO_RESPENSE*50)
			return;
		coinequip.cmd_type=M_CS_POLL;
	}
		
	coin_cmd[0]=COIN_TUBESTATUS;//储币管查询
	rx = mdb_send_cmd(coin_cmd,1,1,COIN_NO_RESPENSE*5, &err, &rx_len);
	if(err!=MDB_RECEIVE_DATA)
	{
		if(OSTimeGet()-coin_timeout[0]>MAX_COIN_TIMEOUT)
		{
			set_coin_status(0xFF);
			coinequip.EQIU_FUNC=CoinChanger_Reset;
		}
		return;
	}
	//对TUBESTATUS命令返回的结果的处理
	coinchanger.allvalue=0;
	coinchanger.full=(rx[0]<<8) | rx[1];
	i=0;
	while(coinchanger.par_value[i]!=0 && i<16)
	{
		coinchanger.store[i]=rx[i+2];
		coinchanger.allvalue+=coinchanger.par_value[i]*coinchanger.store[i]*coinfactor;
		i++;
	}
	
	coinequip.EQIU_FUNC=Coin_Type;
	coin_timeout[0]=OSTimeGet();
}


void Coin_Type(void)//禁/使能硬币器
{
	uint8_t err;
	uint16_t *rx, rx_len;
	rx = rx;//防止编译器报错
	
	if(coinset==1)//只有正常状态才使能，其他都认为是非正常状态
	{
		coin_cmd[0]=COIN_TYPE;
		//添加对各币种的支持
		coin_cmd[1]=0xFF;
		coin_cmd[2]=0xFF;
		coin_cmd[3]=0xFF;
		coin_cmd[4]=0xFF;
	}
	else
	{
		coin_cmd[0]=COIN_TYPE;
		coin_cmd[1]=0;
		coin_cmd[2]=0;
		coin_cmd[3]=0;
		coin_cmd[4]=0;
	}
	rx = mdb_send_cmd(coin_cmd,5,1,COIN_NO_RESPENSE, &err, &rx_len);
	if(err!=MDB_RECEIVE_ACK)
	{
		if(OSTimeGet()-coin_timeout[0]>MAX_COIN_TIMEOUT)
		{
			set_coin_status(0xFF);
			coinequip.EQIU_FUNC=CoinChanger_Reset;
		}
		return;
	}
	//TUBESTATUS命令后续流程
	if(coinequip.cmd_status==cmd_status_need)//有其他命令
	{
		if(coinequip.cmd_type==M_CS_PAYOUT)//退币
		{
			coinequip.EQIU_FUNC=Coin_Dispense_CMD;
			coin_timeout[0]=OSTimeGet();
		}
		return;//这里退出就不发状态转换
	}
	if(coinset==1)
		set_coin_status(1);
	else
	if(coinset==0)
		set_coin_status(0);
	else
		set_coin_status(coinset);
		
	if(coinchanger.level>2 \
	&& coinchanger.diagnostic==1 \
	&& (OSTimeGet()-coin_timeout[1]>OS_TICKS_PER_SEC*5))
	{
		coin_timeout[1]=OSTimeGet();
		coinequip.EQIU_FUNC=Coin_Diagnostic_Status;
	}
	else
	{
		if(OSTimeGet()-coin_timeout[1]>OS_TICKS_PER_SEC*5)
		{
			coin_timeout[1]=OSTimeGet();
			coinequip.EQIU_FUNC=Coin_Setup;
		}
		else
			coinequip.EQIU_FUNC=Coin_Poll;
	}
	coin_timeout[0]=OSTimeGet();
}

void Coin_Poll(void)//POLL循环
{
	uint8_t err,rt,path,type,num,i;
	uint16_t *rx, rx_len;
	FSMMSG * coinmsg;
		
	/*if(coinequip.cmd_status==cmd_status_need)//有别的命令需要执行
	{
		if(coinequip.cmd_type==M_CS_PAYOUT)
		{
			coinequip.EQIU_FUNC=Coin_TubeStatus;
			coin_timeout[0]=OSTimeGet();
		}
	}*/
	
	coin_cmd[0]=COIN_POLL;
	rx = mdb_send_cmd(coin_cmd,1,1,COIN_NO_RESPENSE, &err, &rx_len);
	if(err!=MDB_RECEIVE_ACK && err!=MDB_RECEIVE_DATA)
	{
		if(OSTimeGet()-coin_timeout[0]>MAX_COIN_TIMEOUT)//达到最大超时时间
		{
			set_coin_status(0xFF);
			coinequip.EQIU_FUNC=CoinChanger_Reset;
		}
		return;
	}
	else
	if(err==MDB_RECEIVE_ACK)
	{
		if(coinequip.cmd_status==cmd_status_start)//如果POLL正在监视某个命令的完成情况
		{
			coinequip.cmd_status=cmd_status_complete;
			if(coinequip.cmd_type==M_CS_PAYOUT)//如果是退币任务
			{
				coinchanger.allvalue=0;//金额清零
				coinmsg=apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
				assert_param(coinmsg);
				coinmsg->type=MsgCOIN;
				coinmsg->stype=coinequip.cmd_type;
				num=coin_cmd2[1]>>4;
				type=coin_cmd2[1]&0x0F;
				coinmsg->value=num*coinchanger.par_value[type]*coinfactor;
				if(send_fsm_msg(coinmsg)!=TRUE)
					release_fsm_msg(coinmsg);
					
				credit_set(credit_get() - coinmsg->value);//找零后余额

				savedispense(coinmsg->value,1);
				
				coinequip.cmd_status=cmd_status_complete;	
				coinequip.cmd_type=M_CS_POLL;//改变命令状态，使退币命令可以接收
				coinequip.EQIU_FUNC=Coin_TubeStatus;
			}		
		}
		else
		{
			coinequip.EQIU_FUNC=Coin_TubeStatus;
		}
		coin_timeout[0]=OSTimeGet();
		return;
	}
	//对POLL命令的返回数据包结果的处理
	else
	if(err==MDB_RECEIVE_DATA)
	{   
		for(i=0;i<rx_len;i++)//处理收到的所有字节
		{
			rt=(rx[i] & 0xC0)>>6;
			if(rt==1)//如果是接收硬币
			{
				path=(rx[i] &0x30)>>4;
				type=(rx[i] &0x0F);
				if(path<2)//如果被硬币器接收
				{
					//计算金额:硬币的面值*比例因子*10的（全局金额比例常数-硬币器金额比例常数)次方
					coinmsg=apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
					assert_param(coinmsg);
					coinmsg->type=MsgCOIN;
					coinmsg->stype=M_CS_ACCEPT;
					coinmsg->value=coinchanger.par_value[type]*coinfactor;
		    		if(send_fsm_msg(coinmsg)!=TRUE)
		    			release_fsm_msg(coinmsg);
		    		if(path==0)CA306+=coinmsg->value;//进入溢币盒
					credit_set(credit_get() + coinmsg->value);
		    		//savecoinaccept(coinmsg->value);
		    		
					if(path==1)
					{
						coinchanger.allvalue-=coinchanger.par_value[type]*coinfactor*coinchanger.store[type];
						coinchanger.store[type]=rx[i+1];//跟新储币管数量。
						coinchanger.allvalue+=coinchanger.par_value[type]*coinfactor*coinchanger.store[type];
					}
				}
				coinchanger.store[type]=rx[i+1];
				i++; //该消息占用两字节，所以再加1.
			}
			else
			if(rt==0)//如果是其他硬币器事件
			{
				if(rx[i]==0x01)//按下退币按钮
				{
					coinmsg=apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
					assert_param(coinmsg);
					coinmsg->type=MsgCOIN;
					coinmsg->stype=M_CS_OTHER;
					coinmsg->value=rx[i];
					send_fsm_msg(coinmsg);
				}
				else
				if(rx[i]==0x04||rx[i]==0x05||rx[i]==0x07\
				||rx[i]==0x08||rx[i]==0x09||rx[i]==0x0C)
				{
					if(coinchangenum<20)
				    {	     
				  		status_equip_msg(1,0xFF,1);
				  		coinchangenum++;
				   	}
				}
				else
				if(rx[i]==0x03)//一个硬币被接收但是系统没有给值
				{
					//send_moneydelete_msg(DATA_CC,0);
				}
				else
				if(rx[i]==0x06)//找不到硬币识别头
				{
					set_coin_status(8);
				}
				else
				if(rx[i]==0x0B)//收到一个复位信号
				{
					coinequip.EQIU_FUNC=CoinChanger_Reset;
				}
			}
			else  //人工按键退币事件
			{
				type=(rx[i] &0x0F);//硬币种类
				num=(rx[i] & 0x70)>>4;//硬币数量
				coinmsg=apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
				assert_param(coinmsg);
				coinmsg->type=MsgCOIN;
				coinmsg->stype=M_CS_MANUAL_DIS;
				coinmsg->value=coinchanger.par_value[type]*coinfactor*num;
				coinchanger.allvalue-=coinchanger.par_value[type]*coinfactor*coinchanger.store[type];
				coinchanger.store[type]=rx[i+1];//跟新储币管数量。
				coinchanger.allvalue+=coinchanger.par_value[type]*coinfactor*coinchanger.store[type];
	    		if(send_fsm_msg(coinmsg)!=TRUE)
	    			release_fsm_msg(coinmsg);
				i++; //该消息占用两字节，所以再加1.
			}
		}
		coin_timeout[0]=OSTimeGet();
		return;
	}
}
#ifndef COINHOPPER//***硬币器
uint8_t CheckDispense(uint32_t money)//计算是否可找零
{
	uint8_t type,num;
	uint32_t tmp=0;
	Check_Dispen_Bill(money,&tmp);
	money-=tmp;
	if(money==0)
		return TRUE;
	if(coinequip.eqiu_status>1)
		return FALSE;
	if(coinchanger.allvalue<money)
		return FALSE;
	if((money%coinfactor)!=0)
		return FALSE;
	money=money/coinfactor;
	type=16;
	while(type>0 && money>0)
	{
		if(money<coinchanger.par_value[type-1] || coinchanger.store[type-1]==0)
			type--;
		else
		{
			num=money/coinchanger.par_value[type-1];
			if(num>coinchanger.store[type-1])
				num=coinchanger.store[type-1];
			money-=coinchanger.par_value[type-1]*num;
			type--;
		}
	}
	if(money==0)
	{
		if(coinequip.eqiu_status>1)
			return 2;
		else
			return TRUE;
	}
	else
	return FALSE;
}
#endif
uint8_t DISPENSE(void)//退币
{
	uint8_t num,type,i;//退币数量，退币硬币类型
	
	if(coinequip.cmd_type!=M_CS_PAYOUT)
	{
		if(coinchanger.level>2 && coinchanger.changetype==1)//层级3与支持扩展指令
		{
			uint32_t mm=0;
			mm=credit_get()/coinfactor;
			while(mm>255)
			{	
				mm-=255;
			}
			coin_cmd2[0]=COIN_EXP;
			coin_cmd2[1]=0x02;
			coin_cmd2[2]=mm;
			//coin_cmd2[2]=credit/coinfactor;
		}
		else//层级3以下
		{
			type=15;
			//if(coinequip.eqiu_status!=1)//禁能不能退币？？？？？？
				//return 2;
			//首先需要找到一个有存储的，面值比余额小的但是是符合前面条件中面值最大的硬币。
			while(type>0)
			{
				//从大面额开始寻找可找零硬币
				//添加设置的保底枚数
				if(credit_get()<(coinchanger.par_value[type]*coinfactor) || coinchanger.store[type]<7)
					type--;
				else
					break;
			}
			
			if(type==0)//如果等于0 则证明找遍了硬币器也没找到适合找零的硬币开始寻找那些存储量不足7枚的硬币
			{
				type=15;
				while(type>0)
				{
					if(credit_get()<(coinchanger.par_value[type]*coinfactor) || coinchanger.store[type]==0)
						type--;
					else
						break;
				}
			}
			num=1;
			i=0;
			while(coinequip.cmd_status!=cmd_status_complete && i++<200)
			{
				OSTimeDly(2);
			}
			if(i==200)
				return FALSE;
			coinset=1;//使能硬币器
			coin_cmd2[0]=COIN_DISPENSE;
			coin_cmd2[1]=num<<4 | type;
		}
		coinequip.cmd_type=M_CS_PAYOUT;//告诉轮询,现在执行的是退币命令
		coinequip.cmd_status=cmd_status_need;
		return TRUE;
	}
	return FALSE;
}

uint8_t NewCheckDispense(uint32_t money,uint32_t* real)//计算是否可找零
{
	uint8_t type,num;
	uint32_t amount,compare,tmp=0;
	Check_Dispen_Bill(money,&tmp);
	money-=tmp;
	if(money==0)
		return TRUE;
	if(coinequip.eqiu_status>1)
		return 2;
	if(coinchanger.allvalue==0)
		return FALSE;
	money=money/coinfactor;
	compare=money;
	amount=money*coinfactor;//可能找零金额
	if(amount==0)
		return FALSE;
		
	type=16;
	while(type>0 && money>0)
	{
		if(money<coinchanger.par_value[type-1] || coinchanger.store[type-1]==0)
			type--;
		else
		{
			num=money/coinchanger.par_value[type-1];
			if(num>coinchanger.store[type-1])
				num=coinchanger.store[type-1];
			money-=coinchanger.par_value[type-1]*num;
			type--;
		}
	}
	if(type==0 && money==compare)//确实无可找金额
		return FALSE;
	*real=amount-(money*coinfactor);//实际可找零金额
	return TRUE;
}

uint8_t NewDISPENSE(uint32_t money)//退币
{
	uint8_t num,type,i;//退币数量，退币硬币类型
	
	if(coinequip.cmd_type!=M_CS_PAYOUT)
	{
		if(coinchanger.level>2 && coinchanger.changetype==1)//层级3与支持扩展指令
		{
			uint32_t mm=0;
			mm=money/coinfactor;
			while(mm>255)
			{	
				mm-=255;
			}
			coin_cmd2[0]=COIN_EXP;
			coin_cmd2[1]=0x02;
			coin_cmd2[2]=mm;
			//coin_cmd2[2]=credit/coinfactor;
		}
		else//层级3以下
		{
			type=15;
			//if(coinequip.eqiu_status!=1)//禁能不能退币？？？？？？
				//return 2;
			//首先需要找到一个有存储的，面值比余额小的但是是符合前面条件中面值最大的硬币。
			while(type>0)
			{
				//从大面额开始寻找可找零硬币
				//添加设置的保底枚数
				if(money<(coinchanger.par_value[type]*coinfactor) || coinchanger.store[type]<7)
					type--;
				else
					break;
			}
			
			if(type==0)//如果等于0 则证明找遍了硬币器也没找到适合找零的硬币开始寻找那些存储量不足7枚的硬币
			{
				type=15;
				while(type>0)
				{
					if(money<(coinchanger.par_value[type]*coinfactor) || coinchanger.store[type]==0)
						type--;
					else
						break;
				}
			}
			num=1;
			i=0;
			while(coinequip.cmd_status!=cmd_status_complete && i++<200)
			{
				OSTimeDly(2);
			}
			if(i==200)
				return FALSE;
			coinset=1;//使能硬币器
			coin_cmd2[0]=COIN_DISPENSE;
			coin_cmd2[1]=num<<4 | type;
		}
		coinequip.cmd_type=M_CS_PAYOUT;//告诉轮询,现在执行的是退币命令
		coinequip.cmd_status=cmd_status_need;
		return TRUE;
	}
	return FALSE;
}

uint8_t CE_COIN(uint8_t type)
{
	if(coinset==1 || coinset==0)
	coinset=type;
	//coinequip.cmd_type=COIN_TYPE;
	//coinequip.cmd_status=cmd_status_need;
	return TRUE;
}
