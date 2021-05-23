/*
参数设置
封装了一些参数设置函数
*/

#include "header.h"
#include "stm32_lib.h"
#include "setup.h"

/**************************************************************************************
读取参数
**************************************************************************************/
/*
读取机器基本信息
*/
void e2p_get_vminfo(SETUP_DEVICE *pt)
{
	if(pt != 0)
	{
		I2C_EE_ReadBuffer((uint8_t*)pt,SETUP_DEVICE_ADDR,sizeof(SETUP_DEVICE));
	}
}

/*
读取通用设置
*/
void e2p_get_common(SETUP_COMMON* pt)
{
	if(pt != 0)
	{
		I2C_EE_ReadBuffer((uint8_t*)pt,SETUP_COMMON_ADDR,sizeof(SETUP_COMMON));
	}
	
}
/*
读取非现金设置
*/
void e2p_get_cashless(SETUP_CASHLESS* pt)
{
	if(pt != 0)
	{
		I2C_EE_ReadBuffer((uint8_t*)pt,SETUP_CASHLESS_ADDR,sizeof(SETUP_CASHLESS));
	}
}


/*
读取现金设置
*/
void e2p_get_cash(SETUP_CASH* pt)
{
	if(pt != 0)
	{
		I2C_EE_ReadBuffer((uint8_t*)pt,SETUP_CASH_ADDR,sizeof(SETUP_CASH));
	}
}


/*
读取第一组输出回路设置
*/
void e2p_get_ex1(SETUP_EX1* pt)
{
	if(pt != 0)
	{
		I2C_EE_ReadBuffer((uint8_t*)pt,SETUP_EX1_ADDR,sizeof(SETUP_EX1));
	}
}


/*
读取第2 组输出回路设置
*/
void e2p_get_ex2(SETUP_EX2* pt)
{
	if(pt != 0)
	{
		I2C_EE_ReadBuffer((uint8_t*)pt,SETUP_EX2_ADDR,sizeof(SETUP_EX2));
	}
}


/*
读取通讯设置
*/
void e2p_get_commucation(SETUP_COMMUCATION* pt)
{
	if(pt != 0)
	{
		I2C_EE_ReadBuffer((uint8_t*)pt, SETUP_COMMUCATION_ADDR, sizeof(SETUP_COMMUCATION));
	}
}


/*
读取货道设置
*/
void e2p_get_channel(uint16_t channel, SETUP_CHANNEL* pt)
{
	if(pt != 0)
	{
		//注意，每个机柜只有80个电机，所以这里要转换一下
		I2C_EE_ReadBuffer((uint8_t*)pt, (SETUP_CHANNEL_ADDR + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,sizeof(SETUP_CHANNEL));
	}
}






/******************
@功能：读取货道状态
@参数：channel,货道号，dat，返回数据指针
@说明：只用于中吉兴元无信号线电机
******************/
bool e2p_get_channelState(uint16_t channel, uint8_t *dat)
{
	bool rs = FALSE;
	uint8_t chk = 0xff, i = 0;
	
	if(dat != 0)
	{
		//最多读取5遍确认
		do{
			I2C_EE_ReadBuffer((uint8_t*)dat, (SETUP_COLUMN_ADDR + CHANNEL_STATE_OFFSET + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,1);
			I2C_EE_ReadBuffer((uint8_t*)&chk, (SETUP_COLUMN_ADDR + CHANNEL_STATE_OFFSET + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,1);
			if(chk == (*dat))
				rs = TRUE;
		}while(rs == FALSE && i++ < 5);
	}
	return rs;
}
/******************
@功能：设置货道状态
@参数：channel,货道号，dat，返回数据指针
@说明：只用于中吉兴元无信号线电机
******************/
bool e2p_set_channelState(uint16_t channel, uint8_t *dat)
{
	
	uint8_t chk = 0xff, i = 0; 
	bool rs = FALSE;
	
	if(dat != 0)
	{
		do{
			I2C_EE_WriteBuffer((uint8_t*)dat, (SETUP_COLUMN_ADDR + CHANNEL_STATE_OFFSET + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,1);
			I2C_EE_ReadBuffer((uint8_t*)&chk, (SETUP_COLUMN_ADDR + CHANNEL_STATE_OFFSET + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,1);
			if(chk == (*dat))
				rs = TRUE;
		}while(rs == FALSE && i++ < 5);
	}
	return rs;
}




/*************************************************
货道类读取
*************************************************/
/*
读取价格设置
*/
bool e2p_get_price(uint16_t channel, uint32_t *dat)
{
	bool rs = FALSE;
	uint32_t  i = 0;
	uint8_t chk = 0xff, contrast;//注意这里为单字节和校验，不允许超过0xff
	if(dat != 0)
	{
		do{
			I2C_EE_ReadBuffer((uint8_t*)dat, (SETUP_CHANNEL_ADDR + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,4);
			I2C_EE_ReadBuffer((uint8_t*)&chk, (SETUP_CHANNEL_ADDR + CHANNEL_PRICEBKP_OFFSET + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,1);
			contrast = (((*dat) >> 24)&0xff) + (((*dat) >> 16)&0xff) + (((*dat) >> 8)&0xff) + ((*dat)&0xff);
			if(chk == contrast)
				rs = TRUE;
		}while(rs == FALSE && i++ < 5);
	}
	return rs;
}
/*
读取货道使能设置
*/
bool e2p_get_channelEnable(uint16_t channel, uint8_t *dat)
{
	bool rs = FALSE;
	uint8_t chk = 0xff, i = 0;
	
	if(dat != 0)
	{
		//最多读取5遍确认
		do{
			I2C_EE_ReadBuffer((uint8_t*)dat, (SETUP_CHANNEL_ADDR + CHANNEL_ENABLE_OFFSET + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,1);
			I2C_EE_ReadBuffer((uint8_t*)&chk, (SETUP_CHANNEL_ADDR + CHANNEL_ENABLE_OFFSET + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,1);
			if(chk == (*dat))
				rs = TRUE;
		}while(rs == FALSE && i++ < 5);
	}
	return rs;
}
/*
读取货道库存设置
*/
bool e2p_get_store(uint16_t channel, uint16_t* dat)
{
	bool rs = FALSE;
	uint16_t chk = 0xff, i = 0;
	
	if(dat != 0)
	{
		//最多读取5遍确认
		do{
			I2C_EE_ReadBuffer((uint8_t*)dat, (SETUP_CHANNEL_ADDR + CHANNEL_STORE_OFFSET + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,2 );
			I2C_EE_ReadBuffer((uint8_t*)&chk, (SETUP_CHANNEL_ADDR + CHANNEL_STORE_OFFSET + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,2 );
			if(chk == (*dat))
				rs = TRUE;
		}while(rs == FALSE && i++ < 5);
	}
	return rs;
}
/*
读取货道库存备份设置
*/
bool e2p_get_storeBkp(uint16_t channel, uint16_t* dat)
{
	bool rs = FALSE;
	uint16_t chk = 0xff, i = 0;
	
	if(dat != 0)
	{
		do
		{
			I2C_EE_ReadBuffer((uint8_t*)dat, (SETUP_CHANNEL_ADDR + CHANNEL_STOREBKP_OFFSET + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,2 );
			I2C_EE_ReadBuffer((uint8_t*)&chk, (SETUP_CHANNEL_ADDR + CHANNEL_STOREBKP_OFFSET + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,2 );
			if(chk == (*dat))
				rs = TRUE;
		}while(rs == FALSE && i++ < 5);
	}
	return rs;
}
/*
读取货道弹簧设置
*/
bool e2p_get_spring(uint16_t channel, uint16_t* dat)
{
	bool rs = FALSE;
	uint16_t chk, i = 0;
	
	if(dat != 0)
	{
		do{
			I2C_EE_ReadBuffer((uint8_t*)dat, (SETUP_CHANNEL_ADDR + CHANNEL_SPRING_OFFSET + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,2 );
			I2C_EE_ReadBuffer((uint8_t*)&chk, (SETUP_CHANNEL_ADDR + CHANNEL_SPRING_OFFSET + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,2 );
			if(chk == (*dat))
				rs = TRUE;
		}while(rs == FALSE && i++ < 5);
	}
	return rs;
}
/*
读取货道编码设置
*/
bool e2p_get_channelCode(uint16_t channel, uint32_t* dat)
{
	bool rs = FALSE;
	uint32_t chk, i = 0;
	
	if(dat != 0)
	{
		do{
			I2C_EE_ReadBuffer((uint8_t*)dat, (SETUP_CHANNEL_ADDR + CHANNEL_CODE_OFFSET + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,4 );
			I2C_EE_ReadBuffer((uint8_t*)&chk, (SETUP_CHANNEL_ADDR + CHANNEL_CODE_OFFSET + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,4 );
			if(chk == (*dat))
				rs = TRUE;
		}while(rs == FALSE && i++ < 5);
	}
	return rs;
}

/*
读取货道统计
*/
bool e2p_get_channelStatistic(uint16_t channel, uint32_t* num, uint32_t *val)
{
	bool rs = FALSE;
	uint32_t chk1, chk2, i = 0;
	
	if(num != 0 && val != 0)
	{
		do{
			I2C_EE_ReadBuffer((uint8_t*)num,   (SETUP_COLUMN_ADDR + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,4 );
			I2C_EE_ReadBuffer((uint8_t*)&chk1, (SETUP_COLUMN_ADDR + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,4 );
			I2C_EE_ReadBuffer((uint8_t*)val,   (SETUP_COLUMN_ADDR + 4 + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,4 );
			I2C_EE_ReadBuffer((uint8_t*)&chk2, (SETUP_COLUMN_ADDR + 4 + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,4 );
			if(chk1 == (*num) && chk2 == (*val))
				rs = TRUE;
		}while(rs == FALSE && i++ < 5);
	}
	return rs;
}




/**************************************************************************************
设置参数
**************************************************************************************/
/*
设置基本信息
*/
void e2p_set_vminfo(SETUP_DEVICE *pt)
{
	if(pt != 0)
	{
		I2C_EE_WriteBuffer((uint8_t*)pt,SETUP_DEVICE_ADDR,sizeof(SETUP_DEVICE));
	}
}

/*
通用设置
*/
void e2p_set_common(SETUP_COMMON* pt)
{
	if(pt != 0)
	{
		I2C_EE_WriteBuffer((uint8_t*)pt,SETUP_COMMON_ADDR,sizeof(SETUP_COMMON));
	}
	
}

/*
设置非现金
*/
void e2p_set_cashless(SETUP_CASHLESS* pt)
{
	if(pt != 0)
	{
		I2C_EE_WriteBuffer((uint8_t*)pt,SETUP_CASHLESS_ADDR,sizeof(SETUP_CASHLESS)); 
	}
}

/*
设置现金
*/
void e2p_set_cash(SETUP_CASH* pt)
{
	if(pt != 0)
	{
		I2C_EE_WriteBuffer((uint8_t*)pt,SETUP_CASH_ADDR,sizeof(SETUP_CASH)); 
	}
}


/*
设置输出回路1
*/
void e2p_set_ex1(SETUP_EX1* pt)
{
	if(pt != 0)
	{
		I2C_EE_WriteBuffer((uint8_t*)pt,SETUP_EX1_ADDR,sizeof(SETUP_EX1)); 
	}
}

/*
设置输出回路2
*/
void e2p_set_ex2(SETUP_EX2* pt)
{
	if(pt != 0)
	{
		I2C_EE_WriteBuffer((uint8_t*)pt,SETUP_EX2_ADDR,sizeof(SETUP_EX2)); 
	}
}

/*
设置通讯
*/
void e2p_set_commucation(SETUP_COMMUCATION* pt)
{
	if(pt != 0)
	{
		I2C_EE_WriteBuffer((uint8_t*)pt,SETUP_COMMUCATION_ADDR,sizeof(SETUP_COMMUCATION)); 
	}
}


/*************************************************
货道类设置
*************************************************/
/*
设置货道
*/
bool e2p_set_channel(uint16_t channel, SETUP_CHANNEL* pt)
{
	bool rs = FALSE;
	uint8_t i = 0; 
	SETUP_CHANNEL tmp;
	
	if(pt != 0)
	{
		//注意这里每个机柜只有80个货道
		do
		{
			I2C_EE_WriteBuffer((uint8_t*)pt, (SETUP_CHANNEL_ADDR + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,sizeof(SETUP_CHANNEL));
			I2C_EE_ReadBuffer((uint8_t*)&tmp, (SETUP_CHANNEL_ADDR + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,sizeof(SETUP_CHANNEL));
			if(tmp.code == pt->code && tmp.price == pt->price && tmp.priceCHK == pt->priceCHK && tmp.spring == pt->spring 
				&& tmp.store == pt->store && tmp.storeBkp == pt->storeBkp && tmp.type == pt->type)
				rs = TRUE;
		}while(rs == FALSE && i++ < 5);
	}
	return rs;
}


/*
设置货道价格
*/
bool e2p_set_price(uint16_t channel, uint32_t *dat)
{
	uint32_t price = 0xff;
	uint8_t i = 0, bkp = 0xff, bkpChk = 0xff;
	bool rs = FALSE;
	
	if(dat != 0)
	{
		//写入验证5次
		bkp = (((*dat) >> 24)&0xff) + (((*dat) >> 16)&0xff) + (((*dat) >> 8)&0xff) + ((*dat)&0xff);
		do{
			//写入价格
			I2C_EE_WriteBuffer((uint8_t*)dat, (SETUP_CHANNEL_ADDR + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,4);
			I2C_EE_ReadBuffer((uint8_t*)&price, (SETUP_CHANNEL_ADDR + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,4);
			//写入价格备份
			I2C_EE_WriteBuffer((uint8_t*)&bkp, (SETUP_CHANNEL_ADDR + CHANNEL_PRICEBKP_OFFSET + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,1);
			I2C_EE_ReadBuffer((uint8_t*)&bkpChk, (SETUP_CHANNEL_ADDR + CHANNEL_PRICEBKP_OFFSET + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,1);
			
			if(price == (*dat) && (bkp == bkpChk))
				rs = TRUE;
		}while(rs == FALSE && i++ < 5);
	}
	return rs;
}
/*
设置货道使能
*/
bool e2p_set_channelEnable(uint16_t channel, uint8_t *dat)
{
	
	uint8_t chk = 0xff, i = 0; 
	bool rs = FALSE;
	
	if(dat != 0)
	{
		do{
			I2C_EE_WriteBuffer((uint8_t*)dat, (SETUP_CHANNEL_ADDR + CHANNEL_ENABLE_OFFSET + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,1);
			I2C_EE_ReadBuffer((uint8_t*)&chk, (SETUP_CHANNEL_ADDR + CHANNEL_ENABLE_OFFSET + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,1);
			if(chk == (*dat))
				rs = TRUE;
		}while(rs == FALSE && i++ < 5);
	}
	return rs;
}
/*
设置货道库存
*/
bool e2p_set_store(uint16_t channel, uint16_t* dat)
{
	uint16_t chk = 0xff, i = 0; 
	bool rs = FALSE;
	
	if(dat != 0)
	{
		//写入结果验证5次
		do{
			I2C_EE_WriteBuffer((uint8_t*)dat, (SETUP_CHANNEL_ADDR + CHANNEL_STORE_OFFSET + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,2 );
			I2C_EE_ReadBuffer((uint8_t*)&chk, (SETUP_CHANNEL_ADDR + CHANNEL_STORE_OFFSET + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,2 );
			if(chk == (*dat))
				rs = TRUE;
		}while(rs == FALSE && i++ < 5);
	}
	return rs;
}
/*
设置货道库存备份
*/
bool e2p_set_storeBkp(uint16_t channel, uint16_t* dat)
{
	uint16_t chk = 0xff, i = 0; 
	bool rs = FALSE;
	
	if(dat != 0)
	{
		//写入结果验证5次
		do{
			I2C_EE_WriteBuffer((uint8_t*)dat, (SETUP_CHANNEL_ADDR + CHANNEL_STOREBKP_OFFSET + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,2 );
			I2C_EE_ReadBuffer((uint8_t*)&chk, (SETUP_CHANNEL_ADDR + CHANNEL_STOREBKP_OFFSET + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,2 );
			if(chk == (*dat))
				rs = TRUE;
		}while(rs == FALSE && i++ < 5);
	}
	return rs;
}
/*
设置货道弹簧
*/
bool e2p_set_spring(uint16_t channel, uint16_t* dat)
{
	uint16_t chk = 0xff, i = 0; 
	bool rs = FALSE;
	
	if(dat != 0)
	{
		do{
			I2C_EE_WriteBuffer((uint8_t*)dat, (SETUP_CHANNEL_ADDR + CHANNEL_SPRING_OFFSET + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,2 );
			I2C_EE_ReadBuffer((uint8_t*)&chk, (SETUP_CHANNEL_ADDR + CHANNEL_SPRING_OFFSET + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,2 );
			if(chk == (*dat))
				rs = TRUE;
		}while(rs == FALSE && i++ < 5);
	}
	return rs;
}
/*
设置货道编码
*/
bool e2p_set_channelCode(uint16_t channel, uint32_t* dat)
{
	uint32_t chk = 0xff, i = 0; 
	bool rs = FALSE;
	
	if(dat != 0)
	{
		do{
			I2C_EE_WriteBuffer((uint8_t*)dat, (SETUP_CHANNEL_ADDR + CHANNEL_CODE_OFFSET + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,4 );
			I2C_EE_ReadBuffer((uint8_t*)&chk, (SETUP_CHANNEL_ADDR + CHANNEL_CODE_OFFSET + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,4 );
			if(chk == (*dat))
				rs = TRUE;
		}while(rs == FALSE && i++ < 5);
	}
	return rs;
}

/*
增加货道统计 数量与金额
*/
bool e2p_add_channelStatistic(uint16_t channel, uint32_t addnum, uint32_t addval)
{
	uint32_t chk1 = 0xff, chk2 = 0xff, i = 0, j = 0, num, val;
	uint8_t dat[8],datChk[8];
	bool rs = TRUE;
	
	do{
		I2C_EE_ReadBuffer((uint8_t*)&dat,    (SETUP_COLUMN_ADDR + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,8 );
		I2C_EE_ReadBuffer((uint8_t*)&datChk, (SETUP_COLUMN_ADDR + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,8 );
		for(j = 0; j < 8; j++)
		{
			if(dat[j] != datChk[j])
				rs = FALSE;
		}
	}while(rs == FALSE && i++ < 5);
	
	if(rs == TRUE)
	{
		i = 0;
		rs = FALSE;
		num = ( (dat[3] << 24) | (dat[2] << 16) | (dat[1] << 8) | dat[0] ) + (addnum);
		val = ( (dat[7] << 24) | (dat[6] << 16) | (dat[5] << 8) | dat[4] ) + (addval);
		if(addnum != 0 && addval != 0)
		{
			do{
				
				I2C_EE_WriteBuffer((uint8_t*)&num, (SETUP_COLUMN_ADDR + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,4 );
				I2C_EE_ReadBuffer((uint8_t*)&chk1, (SETUP_COLUMN_ADDR + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,4 );
				I2C_EE_WriteBuffer((uint8_t*)&val, (SETUP_COLUMN_ADDR + 4 + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,4 );
				I2C_EE_ReadBuffer((uint8_t*)&chk2, (SETUP_COLUMN_ADDR + 4 + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,4 );
				
				if(chk1 == num && chk2 == val)
					rs = TRUE;
			}while(rs == FALSE && i++ < 5);
		}
	}
	return rs;
}

/*
设置货道统计 数量与金额
*/
bool e2p_set_channelStatistic(uint16_t channel, uint32_t num, uint32_t val)
{
	uint32_t chk1 = 0xff, chk2 = 0xff, i = 0;
	bool rs = FALSE;
	
	do{
		
		I2C_EE_WriteBuffer((uint8_t*)&num, (SETUP_COLUMN_ADDR + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,4 );
		I2C_EE_ReadBuffer((uint8_t*)&chk1, (SETUP_COLUMN_ADDR + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,4 );
		I2C_EE_WriteBuffer((uint8_t*)&val, (SETUP_COLUMN_ADDR + 4 + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,4 );
		I2C_EE_ReadBuffer((uint8_t*)&chk2, (SETUP_COLUMN_ADDR + 4 + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,4 );
		if(chk1 == num && chk2 == val)
			rs = TRUE;
	}while(rs == FALSE && i++ < 5);
	return rs;
}


/**************************************************************************************
复位参数
**************************************************************************************/

/*
机器基本信息复位
*/
static void e2p_reset_device(void)
{
	SETUP_DEVICE	mac;
	mac.data = 20170501;//2017年5月1号
	mac.facId = 1000000000;
	mac.usrId = 999999;
	I2C_EE_WriteBuffer((uint8_t*)(&mac), SETUP_DEVICE_ADDR, sizeof(SETUP_DEVICE));
}

/*
通用设置
*/
static void e2p_reset_common(void)
{
	SETUP_COMMON common;
	
	common.drvnum = 1;		//机柜数
	common.traynum = 6;		//托盘数
	common.factor = 1;		//比例因子
	common.language = 1;	//1中文0英文
	common.numChannel = 1;	//货道位数
	common.numPrice = 4;	//价格位数
	common.numStore = 3;	//库存位数
	common.point = 2;		//小数点位数
	common.numGoodsId = 6;	//商品条码位数
	common.screenHorizontal = 0;//横竖屏：0：横；1：竖
	
	I2C_EE_WriteBuffer((uint8_t*)(&common),SETUP_COMMON_ADDR,sizeof(SETUP_COMMON));
	
}


/*
非现金设置复位
*/
static void e2p_reset_cashless(void)
{
	SETUP_CASHLESS	cashlessSet;
	cashlessSet.applyEnable = 0;
	cashlessSet.port = 1001;
	I2C_EE_WriteBuffer((uint8_t*)(&cashlessSet), SETUP_CASHLESS_ADDR, sizeof(SETUP_CASHLESS));

}
/*
现金设置复位
*/
static void e2p_reset_cash(void)
{
	SETUP_CASH	cash_set;
	cash_set.coinLever = NULL;//不限制层级
	I2C_EE_WriteBuffer((uint8_t*)(&cash_set), SETUP_CASH_ADDR, sizeof(SETUP_CASH));

}
/*
前三组控制回路复位
*/
static void e2p_reset_ex1(void)
{
	SETUP_EX1	ex1Set;
	ex1Set.tempTarget = 5;	//目标温度
	ex1Set.workMode = 0;	//0制冷；1制热
	ex1Set.starttime_1 = 0;
	ex1Set.starttime_2 = 2359;
	ex1Set.starttime_3 = 0;
	ex1Set.stoptime_1 = 2359;
	ex1Set.stoptime_2 = 2359;
	ex1Set.stoptime_3 = 2359;

	I2C_EE_WriteBuffer((uint8_t*)(&ex1Set), SETUP_EX1_ADDR, sizeof(SETUP_EX1));

}
/*
后三组控制回路复位
*/
static void e2p_reset_ex2(void)
{
	SETUP_EX2	ex2Set;
	ex2Set.starttime_4 = 2359; 
	ex2Set.starttime_5 = 2359;
	ex2Set.starttime_6 = 2359;
	ex2Set.stoptime_4 = 2359;
	ex2Set.stoptime_5 = 2359;
	ex2Set.stoptime_6 = 2359;
	I2C_EE_WriteBuffer((uint8_t*)(&ex2Set), SETUP_EX2_ADDR, sizeof(SETUP_EX2));
}
/*
通讯复位
*/
static void e2p_reset_commucation(void)
{
	SETUP_COMMUCATION	comSet;
	comSet.enable = 1;
	comSet.password = 000000;	//通讯密码
	comSet.port = 0;			//通讯接口
	comSet.type = 1;			//通讯协议  1.DTU通讯，2.dex
	I2C_EE_WriteBuffer((uint8_t*)(&comSet), SETUP_COMMUCATION_ADDR, sizeof(SETUP_COMMUCATION));
}
/*
货道设置复位
*/
uint32_t size;
uint32_t len;
 void e2p_reset_channel(void)
{
	uint8_t i,j;
	for(j = 0;j <MAX_CONTAINER_SIZE; j++)//最多9个机柜
	{
		for(i = 0; i < MAX_CHANNEL_SIZE; i++)//单机柜硬件最多支持最多80个货道
		{
			SETUP_CHANNEL	channelSet;
			channelSet.price = 9000 + (j+1)*10 + i;
			//价格备份就是价格的每字节校验和
			channelSet.priceCHK = (channelSet.price & 0xff) + ((channelSet.price >>24) & 0xff) + ((channelSet.price >>16) & 0xff) + ((channelSet.price >>8 ) & 0xff);
			channelSet.spring = 10;
			channelSet.store  = 10;
			channelSet.storeBkp = 10;//库存备份
			channelSet.type   = 1;//0:禁能；1:使能；2:联动
			channelSet.code   = 000000; 
			//每个货道占用1页，每页浪费2Byte
			size = sizeof(SETUP_CHANNEL);
			len = SETUP_CHANNEL_ADDR + i*EEPROM_PAGE_SIZE + j*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE;
			I2C_EE_WriteBuffer((uint8_t*)(&channelSet), ( SETUP_CHANNEL_ADDR + i*EEPROM_PAGE_SIZE + j*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE) , sizeof(SETUP_CHANNEL));
		}
	}
}


/*
货道统计复位
最多9个机柜
每个机柜最多80个货道
*/
void e2p_reset_channelStatistic(void)
{
	uint8_t i,j;
	for(j = 0; j < MAX_CONTAINER_SIZE; j++)//最多9个机柜
	{
		for(i = 0; i < MAX_CHANNEL_SIZE; i++)//单机柜硬件最多支持最多80个货道
		{
			SETUP_COLUMN	statistic;
			statistic.num = 0;
			statistic.val = 0;
			statistic.state = 0;
			I2C_EE_WriteBuffer((uint8_t*)(&statistic), ( SETUP_COLUMN_ADDR + i*EEPROM_PAGE_SIZE + j*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE) , sizeof(SETUP_COLUMN));
		}
	}
}

/*
功能：所有参数恢复默认值
*/
void e2p_reset_all(void)
{
	e2p_reset_device();
	e2p_reset_common();
	e2p_reset_cashless();
	e2p_reset_cash();
	e2p_reset_ex1();
	e2p_reset_ex2();
	e2p_reset_commucation();
	e2p_reset_channel();
	e2p_reset_channelStatistic();
	bkp_trayStatistic_reset();
	bkp_wholeStatistic_reset();
}




/**************************************************************************************
上电初始化
**************************************************************************************/
void e2p_poweron_init(void)
{
	//先判断是否有效，就是那个标志位是否正确，不正确则恢复
	SETUP_FLAG valid;
	uint8_t i = 0;
	
	do
	{
		I2C_EE_ReadBuffer( (uint8_t*)(&valid), SETUP_FLAG_ADDR, sizeof(SETUP_FLAG));//检查标志位
	}while(valid.validFlag != EEPROM_VALID && i++ < 5);
	
	
	if(valid.validFlag != EEPROM_VALID)//如果有效
	{
		e2p_reset_all();
		valid.validFlag = EEPROM_VALID;
		I2C_EE_WriteBuffer((uint8_t*)(&valid), SETUP_FLAG_ADDR, sizeof(SETUP_FLAG));//重写标志位
		lcd_show_character(&pt_word.warring[12]);
		OSTimeDly(OS_TICKS_PER_SEC*2);
	}
}


/*
功能：读取全局参数（开机时读取）
*/
void e2p_get_global(void)
{
	SETUP_COMMON common;
	SETUP_CASHLESS cashless;
	
	e2p_get_common(&common); 
	e2p_get_cashless(&cashless);
	
    g_vm.language = common.language;		//语言
	g_vm.maxContainerNum = common.drvnum;	//机柜数
	g_vm.maxtrayNum = common.traynum;		//托盘数
	g_vm.point = common.point;				//小数位数
	g_vm.priceBit = common.numPrice;		//价格位数
	g_vm.storeBit = common.numStore;		//库存位数
	g_vm.channelBit = common.numChannel;	//货道位数
	g_vm.goodsIdBit = common.numGoodsId;	//条码位数
	g_vm.applyEnable = cashless.applyEnable;//二维码支付开关
	
//	common.screenHorizontal//横屏
	
}



