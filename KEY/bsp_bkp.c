/*
bkp区读写
*/
#include "stm32_lib.h"
#include "bsp_bkp.h"
#include "ucos_ii.h"

/*
控制流程，读的时候不能写，写的时候不能读
互斥型信号量
*/
//static OS_EVENT  *sem_mailnum;
//static OS_EVENT  *sem_flashWrite;
//static OS_EVENT  *sem_flashRead;
//static OS_EVENT  *sem_usedSec;
//static OS_EVENT  *sem_mailcount;
//static OS_EVENT  *sem_tradenum;

/*
bkp区初始化
要使用电池备份区，
平时要读写，所以一直要打开
*/
void bsp_bkp_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);      //使能PWR 和BKP 外设时钟    
	PWR_BackupAccessCmd(ENABLE); 	//允许访问后备区
}


/*
现金
bkp3与bkp4 
*/
uint32_t credit_get(void)
{
	uint32_t tmp;
	tmp = ((BKP_ReadBackupRegister(BKP_DR3) << 16) | (BKP_ReadBackupRegister(BKP_DR4)));
	return tmp;
}

void credit_set(uint32_t money)
{
	BKP_WriteBackupRegister(BKP_DR3, (money>>16));
	BKP_WriteBackupRegister(BKP_DR4, (money & 0xffff)); 
}

/*
数据汇报后台
流水号
信息编号
bkp5与bp6
*/
uint32_t mailcount_get(void)
{
	uint32_t tmp;
	tmp = ((BKP_ReadBackupRegister(BKP_DR5) << 16) | (BKP_ReadBackupRegister(BKP_DR6)));
	return tmp;
}
void mailcount_set(uint32_t num)
{
	BKP_WriteBackupRegister(BKP_DR5, (num>>16));
	BKP_WriteBackupRegister(BKP_DR6, (num & 0xffff)); 
}

/*
falsh 中数据条数
bpk7 与bkp8
*/
uint32_t mailnum_get(void)
{
	uint32_t tmp;
	tmp = ((BKP_ReadBackupRegister(BKP_DR7) << 16) | (BKP_ReadBackupRegister(BKP_DR8)));
	return tmp;
}
void mailnum_set(uint32_t num)
{
	BKP_WriteBackupRegister(BKP_DR7, (num>>16));
	BKP_WriteBackupRegister(BKP_DR8, (num & 0xffff)); 
}

/*
flash数据头
bpk9 与bkp10

*/
uint32_t flashqueue_head_get(void)
{
	uint32_t tmp;
	tmp = ((BKP_ReadBackupRegister(BKP_DR9) << 16) | (BKP_ReadBackupRegister(BKP_DR10)));
	return tmp;
}
void flashqueue_head_set(uint32_t num)
{
	BKP_WriteBackupRegister(BKP_DR9, (num >> 16));
	BKP_WriteBackupRegister(BKP_DR10, (num & 0xffff)); 
}

/*
flash数据尾
bpk11 与bkp12

*/
uint32_t flashqueue_rear_get(void)
{
	uint32_t tmp;
	tmp = ((BKP_ReadBackupRegister(BKP_DR11) << 16) | (BKP_ReadBackupRegister(BKP_DR12)));
	return tmp;

}
void flashqueue_rear_set(uint32_t num)
{
	BKP_WriteBackupRegister(BKP_DR11, (num >> 16));
	BKP_WriteBackupRegister(BKP_DR12, (num & 0xffff)); 
}

/*
flash中已经使用的块数
bpk13 与bkp14
*/
uint32_t mailusedsec_get(void)
{
	uint32_t tmp;
	tmp = ((BKP_ReadBackupRegister(BKP_DR13) << 16) | (BKP_ReadBackupRegister(BKP_DR14)));
	return tmp;
}
void mailusedsec_set(uint32_t num)
{
//	uint8_t err;
//   OSSemPend(sem_usedSec, 0, &err);
	BKP_WriteBackupRegister(BKP_DR13, (num >> 16));
	BKP_WriteBackupRegister(BKP_DR14, (num & 0xffff)); 
//	OSSemPost(sem_usedSec);
}

/*
未找零金额
bpk15 与bkp16
*/
uint32_t nochanger_get(void)
{
	return ((BKP_ReadBackupRegister(BKP_DR15) << 16) | (BKP_ReadBackupRegister(BKP_DR16)));
}
void nochanger_set(uint32_t num)
{
	BKP_WriteBackupRegister(BKP_DR15, (num>>16));
	BKP_WriteBackupRegister(BKP_DR16, (num & 0xffff)); 
}

/*
交易号
*/
void tradenum_set(uint32_t num)
{
	BKP_WriteBackupRegister(BKP_DR17, (num>>16));
	BKP_WriteBackupRegister(BKP_DR18, (num & 0xffff)); 
}
uint32_t tradenum_get(void)
{
	uint32_t tmp;
	tmp = ((BKP_ReadBackupRegister(BKP_DR17) << 16) | (BKP_ReadBackupRegister(BKP_DR18)));
	return tmp;
}
void tradenum_add(void)
{
	uint32_t tmp;

	tmp = ((BKP_ReadBackupRegister(BKP_DR17) << 16) | (BKP_ReadBackupRegister(BKP_DR18))) + 1;
	BKP_WriteBackupRegister(BKP_DR17, (tmp>>16));
	BKP_WriteBackupRegister(BKP_DR18, (tmp & 0xffff)); 
}

/*
获取轮询货道号
*/
uint8_t loop_channel_get(uint8_t id)
{
	uint8_t rs;
	
	id %= 10;

	switch(id)
	{
		case 0://托盘1
			rs =  ( BKP_ReadBackupRegister(BKP_DR19) >> 8 ) ;
		break;
		case 1://托盘2
			rs =  ( BKP_ReadBackupRegister(BKP_DR19) & 0xff ) ;
		break;
		case 2:
			rs = ( BKP_ReadBackupRegister(BKP_DR20) >> 8 ) ;
		break;
		case 3:
			rs = ( BKP_ReadBackupRegister(BKP_DR20) & 0xff ) ;
		break;
		case 4:
			rs = ( BKP_ReadBackupRegister(BKP_DR21) >> 8 ) ;
		break;
		case 5:
			rs = ( BKP_ReadBackupRegister(BKP_DR21) & 0xff ) ;
		break;
		case 6:
			rs = ( BKP_ReadBackupRegister(BKP_DR22) >> 8 ) ;
		break;
		case 7:
			rs = ( BKP_ReadBackupRegister(BKP_DR22) & 0xff ) ;
		break;
	}
	return rs;
}



/*
设置轮询货道号
*/
void loop_channel_set(uint8_t id, uint8_t num)
{
	uint8_t tp_h, tp_l;
	
	switch(id)
	{
		case 0://托盘1
			tp_h = num % 10;
			tp_l = ( BKP_ReadBackupRegister(BKP_DR19) & 0xff);
			BKP_WriteBackupRegister(BKP_DR19 , (tp_h << 8) | tp_l);
		break;
		case 1://托盘2
			tp_h = ( BKP_ReadBackupRegister(BKP_DR19) >> 8);
			tp_l = num % 10;
			BKP_WriteBackupRegister(BKP_DR19 , (tp_h << 8) | tp_l);
		break;
		case 2:
			tp_h = num % 10;
			tp_l = ( BKP_ReadBackupRegister(BKP_DR20) & 0xff);
			BKP_WriteBackupRegister(BKP_DR20 , (tp_h << 8) | tp_l);
		break;
		case 3:
			tp_h = ( BKP_ReadBackupRegister(BKP_DR20) >> 8);
			tp_l = num % 10;
			BKP_WriteBackupRegister(BKP_DR20 , (tp_h << 8) | tp_l);
		break;
		case 4:
			tp_h = num % 10;
			tp_l = ( BKP_ReadBackupRegister(BKP_DR21) & 0xff);
			BKP_WriteBackupRegister(BKP_DR21 , (tp_h << 8) | tp_l);
		break;
		case 5:
			tp_h = ( BKP_ReadBackupRegister(BKP_DR21) >> 8);
			tp_l = num % 10;
			BKP_WriteBackupRegister(BKP_DR21 , (tp_h << 8) | tp_l);
		break;
		case 6:
			tp_h = num % 10;
			tp_l = ( BKP_ReadBackupRegister(BKP_DR22) & 0xff);
			BKP_WriteBackupRegister(BKP_DR22 , (tp_h << 8) | tp_l);
		break;
		case 7:
			tp_h = ( BKP_ReadBackupRegister(BKP_DR22) >> 8);
			tp_l = num % 10;
			BKP_WriteBackupRegister(BKP_DR22 , (tp_h << 8) | tp_l);
		break;
	}	
}

/*
轮询货道号自增
*/
void loop_channel_add(uint8_t id)
{
	uint8_t tp_h, tp_l;
	switch(id)
	{
		case 0://托盘1
			tp_h = (( BKP_ReadBackupRegister(BKP_DR19) >> 8) + 1) % 8;
			tp_l = ( BKP_ReadBackupRegister(BKP_DR19) & 0xff);
			BKP_WriteBackupRegister(BKP_DR19 , (tp_h << 8) | tp_l);
		break;
		case 1://托盘2
			tp_h = ( BKP_ReadBackupRegister(BKP_DR19) >> 8);
			tp_l = (( BKP_ReadBackupRegister(BKP_DR19) & 0xff) + 1) % 8;
			BKP_WriteBackupRegister(BKP_DR19 , (tp_h << 8) | tp_l);
		break;
		case 2:
			tp_h = (( BKP_ReadBackupRegister(BKP_DR20) >> 8) + 1) % 8;
			tp_l = ( BKP_ReadBackupRegister(BKP_DR20) & 0xff);
			BKP_WriteBackupRegister(BKP_DR20 , (tp_h << 8) | tp_l);
		break;
		case 3:
			tp_h = ( BKP_ReadBackupRegister(BKP_DR20) >> 8);
			tp_l = (( BKP_ReadBackupRegister(BKP_DR20) & 0xff) + 1) % 8;
			BKP_WriteBackupRegister(BKP_DR20 , (tp_h << 8) | tp_l);
		break;
		case 4:
			tp_h = (( BKP_ReadBackupRegister(BKP_DR21) >> 8) + 1)% 8;
			tp_l = ( BKP_ReadBackupRegister(BKP_DR21) & 0xff);
			BKP_WriteBackupRegister(BKP_DR21 , (tp_h << 8) | tp_l);
		break;
		case 5:
			tp_h = ( BKP_ReadBackupRegister(BKP_DR21) >> 8);
			tp_l = (( BKP_ReadBackupRegister(BKP_DR21) & 0xff) + 1) % 8;
			BKP_WriteBackupRegister(BKP_DR21 , (tp_h << 8) | tp_l);
		break;
		case 6:
			tp_h = (( BKP_ReadBackupRegister(BKP_DR22) >> 8) + 1) % 8 ;
			tp_l = ( BKP_ReadBackupRegister(BKP_DR22) & 0xff);
			BKP_WriteBackupRegister(BKP_DR22 , (tp_h << 8) | tp_l);
		break;
		case 7:
			tp_h = ( BKP_ReadBackupRegister(BKP_DR22) >> 8);
			tp_l = (( BKP_ReadBackupRegister(BKP_DR22) & 0xff) );
			tp_l = (tp_l + 1) % 8;
			BKP_WriteBackupRegister(BKP_DR22 , (tp_h << 8) | tp_l);
		break;
	}	
}


/*******************
托盘库存
注意：没层托盘的库存不得超过255
*******************/
/*
托盘库存复位
*/
void bkp_trayStore_reset(void)
{
	BKP_WriteBackupRegister(BKP_DR23 , 10);
	BKP_WriteBackupRegister(BKP_DR24 , 10);
	BKP_WriteBackupRegister(BKP_DR25 , 10);
	BKP_WriteBackupRegister(BKP_DR26 , 10);
}
/*
托盘库存设置
*/
void bkp_trayStore_set(uint8_t tray, uint8_t num)
{
	uint16_t val;
	uint8_t bitH, bitL;
	if(tray < 8)
	{
		val = BKP_ReadBackupRegister(BKP_DR23 + (tray/2)*4);
		bitH = val >> 8;
		bitL = val & 0xff;
		if(tray % 2 == 0)
		{
			bitH = num;
		}
		else
		{
			bitL = num;
		}
		val = (bitH << 8) | bitL;
		BKP_WriteBackupRegister(BKP_DR23 + (tray/2)*4 ,val);
	}
}
uint8_t bkp_trayStore_get(uint8_t tray)
{
	if(tray % 2 == 0)
	{
		return BKP_ReadBackupRegister(BKP_DR23 + (tray/2)*4) >> 8;
	}
	else
	{
		return BKP_ReadBackupRegister(BKP_DR23 + (tray/2)*4) & 0xff;
	}
}


/*
托盘统计
*/
void bkp_trayStatistic_reset(void)
{
	//数量
	BKP_WriteBackupRegister(BKP_DR27 ,0);
	BKP_WriteBackupRegister(BKP_DR28 ,0);
	BKP_WriteBackupRegister(BKP_DR29 ,0);
	BKP_WriteBackupRegister(BKP_DR30 ,0);
	//金额
	BKP_WriteBackupRegister(BKP_DR31 ,0);
	BKP_WriteBackupRegister(BKP_DR32 ,0);
	BKP_WriteBackupRegister(BKP_DR33 ,0);
	BKP_WriteBackupRegister(BKP_DR34 ,0);
	BKP_WriteBackupRegister(BKP_DR35 ,0);
	BKP_WriteBackupRegister(BKP_DR36 ,0);
	BKP_WriteBackupRegister(BKP_DR37 ,0);
	BKP_WriteBackupRegister(BKP_DR38 ,0);
}
/*
统计增加
tray:托盘；num:数量；val：金额
*/
void bkp_trayStatistic_add(uint8_t tray, uint8_t num, uint16_t money )
{
	uint16_t val;
	uint8_t bitH, bitL;
	if(tray < 8)
	{
		val = BKP_ReadBackupRegister(BKP_DR27 + (tray/2)*4);
		bitH = val >> 8;
		bitL = val & 0xff;
		if(tray % 2 == 0)
		{
			bitH += num;
		}
		else
		{
			bitL += num;
		}
		val = (bitH << 8) | bitL;
		BKP_WriteBackupRegister(BKP_DR27 + (tray/2)*4 ,val);
		
		val = BKP_ReadBackupRegister(BKP_DR31 + tray*4);
		val += money;
		BKP_WriteBackupRegister(BKP_DR31 + tray*4 ,val);
	}
}
/*
统计设置
tray:托盘；num:数量；val：金额
*/
void bkp_trayStatistic_set(uint8_t tray, uint8_t num, uint16_t money )
{
	uint16_t val;
	uint8_t bitH, bitL;
	if(tray < 8)
	{
		val = BKP_ReadBackupRegister(BKP_DR27 + (tray/2)*4);
		bitH = val >> 8;
		bitL = val & 0xff;
		if(tray % 2 == 0)
		{
			bitH = 0;
		}
		else
		{
			bitL = 0;
		}
		val = (bitH << 8) | bitL;
		BKP_WriteBackupRegister(BKP_DR27 + (tray/2)*4 ,val);
		
		val = money;
		BKP_WriteBackupRegister(BKP_DR31 + tray*4 ,val);
	}
}

void bkp_trayStatistic_get(uint8_t tray, uint8_t *num, uint16_t *money )
{
//	uint32_t tt;
	if(tray % 2 == 0)
	{
//		tt = BKP_ReadBackupRegister(BKP_DR27 + (tray/2)*4);
		*num = BKP_ReadBackupRegister(BKP_DR27 + (tray/2)*4) >> 8;
	}
	else
	{
		*num = BKP_ReadBackupRegister(BKP_DR27 + (tray/2)*4) & 0xff;
	}
	*money = BKP_ReadBackupRegister(BKP_DR31 + tray*4);
}


/*
整机统计
*/
void bkp_wholeStatistic_reset(void)
{
	//数量
	BKP_WriteBackupRegister(BKP_DR39 ,0);
	BKP_WriteBackupRegister(BKP_DR40 ,0);
	//金额
	BKP_WriteBackupRegister(BKP_DR41 ,0);
	BKP_WriteBackupRegister(BKP_DR42 ,0);
}

void bkp_wholeStatistic_add(uint8_t num, uint16_t money )
{
	uint32_t val;
	val = (BKP_ReadBackupRegister(BKP_DR39) << 16) | BKP_ReadBackupRegister(BKP_DR40);
	val += num;
	BKP_WriteBackupRegister(BKP_DR39 ,(val >> 16) );
	BKP_WriteBackupRegister(BKP_DR40 ,(val & 0xffff) );
	
	val = (BKP_ReadBackupRegister(BKP_DR41) << 16) | BKP_ReadBackupRegister(BKP_DR42);
	val += money;
	BKP_WriteBackupRegister(BKP_DR41 ,(val >> 16) );
	BKP_WriteBackupRegister(BKP_DR42 ,(val & 0xffff) );
}
 
void bkp_wholeStatistic_get(uint32_t *num, uint32_t *money )
{
	*num = (BKP_ReadBackupRegister(BKP_DR39) << 16) | BKP_ReadBackupRegister(BKP_DR40);
	*money = (BKP_ReadBackupRegister(BKP_DR41) << 16) | BKP_ReadBackupRegister(BKP_DR42); 
}
