/*
��������
��װ��һЩ�������ú���
*/

#include "header.h"
#include "stm32_lib.h"
#include "setup.h"

/**************************************************************************************
��ȡ����
**************************************************************************************/
/*
��ȡ����������Ϣ
*/
void e2p_get_vminfo(SETUP_DEVICE *pt)
{
	if(pt != 0)
	{
		I2C_EE_ReadBuffer((uint8_t*)pt,SETUP_DEVICE_ADDR,sizeof(SETUP_DEVICE));
	}
}

/*
��ȡͨ������
*/
void e2p_get_common(SETUP_COMMON* pt)
{
	if(pt != 0)
	{
		I2C_EE_ReadBuffer((uint8_t*)pt,SETUP_COMMON_ADDR,sizeof(SETUP_COMMON));
	}
	
}
/*
��ȡ���ֽ�����
*/
void e2p_get_cashless(SETUP_CASHLESS* pt)
{
	if(pt != 0)
	{
		I2C_EE_ReadBuffer((uint8_t*)pt,SETUP_CASHLESS_ADDR,sizeof(SETUP_CASHLESS));
	}
}


/*
��ȡ�ֽ�����
*/
void e2p_get_cash(SETUP_CASH* pt)
{
	if(pt != 0)
	{
		I2C_EE_ReadBuffer((uint8_t*)pt,SETUP_CASH_ADDR,sizeof(SETUP_CASH));
	}
}


/*
��ȡ��һ�������·����
*/
void e2p_get_ex1(SETUP_EX1* pt)
{
	if(pt != 0)
	{
		I2C_EE_ReadBuffer((uint8_t*)pt,SETUP_EX1_ADDR,sizeof(SETUP_EX1));
	}
}


/*
��ȡ��2 �������·����
*/
void e2p_get_ex2(SETUP_EX2* pt)
{
	if(pt != 0)
	{
		I2C_EE_ReadBuffer((uint8_t*)pt,SETUP_EX2_ADDR,sizeof(SETUP_EX2));
	}
}


/*
��ȡͨѶ����
*/
void e2p_get_commucation(SETUP_COMMUCATION* pt)
{
	if(pt != 0)
	{
		I2C_EE_ReadBuffer((uint8_t*)pt, SETUP_COMMUCATION_ADDR, sizeof(SETUP_COMMUCATION));
	}
}


/*
��ȡ��������
*/
void e2p_get_channel(uint16_t channel, SETUP_CHANNEL* pt)
{
	if(pt != 0)
	{
		//ע�⣬ÿ������ֻ��80���������������Ҫת��һ��
		I2C_EE_ReadBuffer((uint8_t*)pt, (SETUP_CHANNEL_ADDR + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,sizeof(SETUP_CHANNEL));
	}
}






/******************
@���ܣ���ȡ����״̬
@������channel,�����ţ�dat����������ָ��
@˵����ֻ�����м���Ԫ���ź��ߵ��
******************/
bool e2p_get_channelState(uint16_t channel, uint8_t *dat)
{
	bool rs = FALSE;
	uint8_t chk = 0xff, i = 0;
	
	if(dat != 0)
	{
		//����ȡ5��ȷ��
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
@���ܣ����û���״̬
@������channel,�����ţ�dat����������ָ��
@˵����ֻ�����м���Ԫ���ź��ߵ��
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
�������ȡ
*************************************************/
/*
��ȡ�۸�����
*/
bool e2p_get_price(uint16_t channel, uint32_t *dat)
{
	bool rs = FALSE;
	uint32_t  i = 0;
	uint8_t chk = 0xff, contrast;//ע������Ϊ���ֽں�У�飬��������0xff
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
��ȡ����ʹ������
*/
bool e2p_get_channelEnable(uint16_t channel, uint8_t *dat)
{
	bool rs = FALSE;
	uint8_t chk = 0xff, i = 0;
	
	if(dat != 0)
	{
		//����ȡ5��ȷ��
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
��ȡ�����������
*/
bool e2p_get_store(uint16_t channel, uint16_t* dat)
{
	bool rs = FALSE;
	uint16_t chk = 0xff, i = 0;
	
	if(dat != 0)
	{
		//����ȡ5��ȷ��
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
��ȡ������汸������
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
��ȡ������������
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
��ȡ������������
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
��ȡ����ͳ��
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
���ò���
**************************************************************************************/
/*
���û�����Ϣ
*/
void e2p_set_vminfo(SETUP_DEVICE *pt)
{
	if(pt != 0)
	{
		I2C_EE_WriteBuffer((uint8_t*)pt,SETUP_DEVICE_ADDR,sizeof(SETUP_DEVICE));
	}
}

/*
ͨ������
*/
void e2p_set_common(SETUP_COMMON* pt)
{
	if(pt != 0)
	{
		I2C_EE_WriteBuffer((uint8_t*)pt,SETUP_COMMON_ADDR,sizeof(SETUP_COMMON));
	}
	
}

/*
���÷��ֽ�
*/
void e2p_set_cashless(SETUP_CASHLESS* pt)
{
	if(pt != 0)
	{
		I2C_EE_WriteBuffer((uint8_t*)pt,SETUP_CASHLESS_ADDR,sizeof(SETUP_CASHLESS)); 
	}
}

/*
�����ֽ�
*/
void e2p_set_cash(SETUP_CASH* pt)
{
	if(pt != 0)
	{
		I2C_EE_WriteBuffer((uint8_t*)pt,SETUP_CASH_ADDR,sizeof(SETUP_CASH)); 
	}
}


/*
���������·1
*/
void e2p_set_ex1(SETUP_EX1* pt)
{
	if(pt != 0)
	{
		I2C_EE_WriteBuffer((uint8_t*)pt,SETUP_EX1_ADDR,sizeof(SETUP_EX1)); 
	}
}

/*
���������·2
*/
void e2p_set_ex2(SETUP_EX2* pt)
{
	if(pt != 0)
	{
		I2C_EE_WriteBuffer((uint8_t*)pt,SETUP_EX2_ADDR,sizeof(SETUP_EX2)); 
	}
}

/*
����ͨѶ
*/
void e2p_set_commucation(SETUP_COMMUCATION* pt)
{
	if(pt != 0)
	{
		I2C_EE_WriteBuffer((uint8_t*)pt,SETUP_COMMUCATION_ADDR,sizeof(SETUP_COMMUCATION)); 
	}
}


/*************************************************
����������
*************************************************/
/*
���û���
*/
bool e2p_set_channel(uint16_t channel, SETUP_CHANNEL* pt)
{
	bool rs = FALSE;
	uint8_t i = 0; 
	SETUP_CHANNEL tmp;
	
	if(pt != 0)
	{
		//ע������ÿ������ֻ��80������
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
���û����۸�
*/
bool e2p_set_price(uint16_t channel, uint32_t *dat)
{
	uint32_t price = 0xff;
	uint8_t i = 0, bkp = 0xff, bkpChk = 0xff;
	bool rs = FALSE;
	
	if(dat != 0)
	{
		//д����֤5��
		bkp = (((*dat) >> 24)&0xff) + (((*dat) >> 16)&0xff) + (((*dat) >> 8)&0xff) + ((*dat)&0xff);
		do{
			//д��۸�
			I2C_EE_WriteBuffer((uint8_t*)dat, (SETUP_CHANNEL_ADDR + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,4);
			I2C_EE_ReadBuffer((uint8_t*)&price, (SETUP_CHANNEL_ADDR + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,4);
			//д��۸񱸷�
			I2C_EE_WriteBuffer((uint8_t*)&bkp, (SETUP_CHANNEL_ADDR + CHANNEL_PRICEBKP_OFFSET + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,1);
			I2C_EE_ReadBuffer((uint8_t*)&bkpChk, (SETUP_CHANNEL_ADDR + CHANNEL_PRICEBKP_OFFSET + EEPROM_PAGE_SIZE*(channel%100) + (channel/100)*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE ) ,1);
			
			if(price == (*dat) && (bkp == bkpChk))
				rs = TRUE;
		}while(rs == FALSE && i++ < 5);
	}
	return rs;
}
/*
���û���ʹ��
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
���û������
*/
bool e2p_set_store(uint16_t channel, uint16_t* dat)
{
	uint16_t chk = 0xff, i = 0; 
	bool rs = FALSE;
	
	if(dat != 0)
	{
		//д������֤5��
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
���û�����汸��
*/
bool e2p_set_storeBkp(uint16_t channel, uint16_t* dat)
{
	uint16_t chk = 0xff, i = 0; 
	bool rs = FALSE;
	
	if(dat != 0)
	{
		//д������֤5��
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
���û�������
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
���û�������
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
���ӻ���ͳ�� ��������
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
���û���ͳ�� ��������
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
��λ����
**************************************************************************************/

/*
����������Ϣ��λ
*/
static void e2p_reset_device(void)
{
	SETUP_DEVICE	mac;
	mac.data = 20170501;//2017��5��1��
	mac.facId = 1000000000;
	mac.usrId = 999999;
	I2C_EE_WriteBuffer((uint8_t*)(&mac), SETUP_DEVICE_ADDR, sizeof(SETUP_DEVICE));
}

/*
ͨ������
*/
static void e2p_reset_common(void)
{
	SETUP_COMMON common;
	
	common.drvnum = 1;		//������
	common.traynum = 6;		//������
	common.factor = 1;		//��������
	common.language = 1;	//1����0Ӣ��
	common.numChannel = 1;	//����λ��
	common.numPrice = 4;	//�۸�λ��
	common.numStore = 3;	//���λ��
	common.point = 2;		//С����λ��
	common.numGoodsId = 6;	//��Ʒ����λ��
	common.screenHorizontal = 0;//��������0���᣻1����
	
	I2C_EE_WriteBuffer((uint8_t*)(&common),SETUP_COMMON_ADDR,sizeof(SETUP_COMMON));
	
}


/*
���ֽ����ø�λ
*/
static void e2p_reset_cashless(void)
{
	SETUP_CASHLESS	cashlessSet;
	cashlessSet.applyEnable = 0;
	cashlessSet.port = 1001;
	I2C_EE_WriteBuffer((uint8_t*)(&cashlessSet), SETUP_CASHLESS_ADDR, sizeof(SETUP_CASHLESS));

}
/*
�ֽ����ø�λ
*/
static void e2p_reset_cash(void)
{
	SETUP_CASH	cash_set;
	cash_set.coinLever = NULL;//�����Ʋ㼶
	I2C_EE_WriteBuffer((uint8_t*)(&cash_set), SETUP_CASH_ADDR, sizeof(SETUP_CASH));

}
/*
ǰ������ƻ�·��λ
*/
static void e2p_reset_ex1(void)
{
	SETUP_EX1	ex1Set;
	ex1Set.tempTarget = 5;	//Ŀ���¶�
	ex1Set.workMode = 0;	//0���䣻1����
	ex1Set.starttime_1 = 0;
	ex1Set.starttime_2 = 2359;
	ex1Set.starttime_3 = 0;
	ex1Set.stoptime_1 = 2359;
	ex1Set.stoptime_2 = 2359;
	ex1Set.stoptime_3 = 2359;

	I2C_EE_WriteBuffer((uint8_t*)(&ex1Set), SETUP_EX1_ADDR, sizeof(SETUP_EX1));

}
/*
��������ƻ�·��λ
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
ͨѶ��λ
*/
static void e2p_reset_commucation(void)
{
	SETUP_COMMUCATION	comSet;
	comSet.enable = 1;
	comSet.password = 000000;	//ͨѶ����
	comSet.port = 0;			//ͨѶ�ӿ�
	comSet.type = 1;			//ͨѶЭ��  1.DTUͨѶ��2.dex
	I2C_EE_WriteBuffer((uint8_t*)(&comSet), SETUP_COMMUCATION_ADDR, sizeof(SETUP_COMMUCATION));
}
/*
�������ø�λ
*/
uint32_t size;
uint32_t len;
 void e2p_reset_channel(void)
{
	uint8_t i,j;
	for(j = 0;j <MAX_CONTAINER_SIZE; j++)//���9������
	{
		for(i = 0; i < MAX_CHANNEL_SIZE; i++)//������Ӳ�����֧�����80������
		{
			SETUP_CHANNEL	channelSet;
			channelSet.price = 9000 + (j+1)*10 + i;
			//�۸񱸷ݾ��Ǽ۸��ÿ�ֽ�У���
			channelSet.priceCHK = (channelSet.price & 0xff) + ((channelSet.price >>24) & 0xff) + ((channelSet.price >>16) & 0xff) + ((channelSet.price >>8 ) & 0xff);
			channelSet.spring = 10;
			channelSet.store  = 10;
			channelSet.storeBkp = 10;//��汸��
			channelSet.type   = 1;//0:���ܣ�1:ʹ�ܣ�2:����
			channelSet.code   = 000000; 
			//ÿ������ռ��1ҳ��ÿҳ�˷�2Byte
			size = sizeof(SETUP_CHANNEL);
			len = SETUP_CHANNEL_ADDR + i*EEPROM_PAGE_SIZE + j*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE;
			I2C_EE_WriteBuffer((uint8_t*)(&channelSet), ( SETUP_CHANNEL_ADDR + i*EEPROM_PAGE_SIZE + j*EEPROM_PAGE_SIZE*MAX_CHANNEL_SIZE) , sizeof(SETUP_CHANNEL));
		}
	}
}


/*
����ͳ�Ƹ�λ
���9������
ÿ���������80������
*/
void e2p_reset_channelStatistic(void)
{
	uint8_t i,j;
	for(j = 0; j < MAX_CONTAINER_SIZE; j++)//���9������
	{
		for(i = 0; i < MAX_CHANNEL_SIZE; i++)//������Ӳ�����֧�����80������
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
���ܣ����в����ָ�Ĭ��ֵ
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
�ϵ��ʼ��
**************************************************************************************/
void e2p_poweron_init(void)
{
	//���ж��Ƿ���Ч�������Ǹ���־λ�Ƿ���ȷ������ȷ��ָ�
	SETUP_FLAG valid;
	uint8_t i = 0;
	
	do
	{
		I2C_EE_ReadBuffer( (uint8_t*)(&valid), SETUP_FLAG_ADDR, sizeof(SETUP_FLAG));//����־λ
	}while(valid.validFlag != EEPROM_VALID && i++ < 5);
	
	
	if(valid.validFlag != EEPROM_VALID)//�����Ч
	{
		e2p_reset_all();
		valid.validFlag = EEPROM_VALID;
		I2C_EE_WriteBuffer((uint8_t*)(&valid), SETUP_FLAG_ADDR, sizeof(SETUP_FLAG));//��д��־λ
		lcd_show_character(&pt_word.warring[12]);
		OSTimeDly(OS_TICKS_PER_SEC*2);
	}
}


/*
���ܣ���ȡȫ�ֲ���������ʱ��ȡ��
*/
void e2p_get_global(void)
{
	SETUP_COMMON common;
	SETUP_CASHLESS cashless;
	
	e2p_get_common(&common); 
	e2p_get_cashless(&cashless);
	
    g_vm.language = common.language;		//����
	g_vm.maxContainerNum = common.drvnum;	//������
	g_vm.maxtrayNum = common.traynum;		//������
	g_vm.point = common.point;				//С��λ��
	g_vm.priceBit = common.numPrice;		//�۸�λ��
	g_vm.storeBit = common.numStore;		//���λ��
	g_vm.channelBit = common.numChannel;	//����λ��
	g_vm.goodsIdBit = common.numGoodsId;	//����λ��
	g_vm.applyEnable = cashless.applyEnable;//��ά��֧������
	
//	common.screenHorizontal//����
	
}



