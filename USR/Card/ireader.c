/*
读卡头驱动

*/
#include "header.h"
#include "ireader.h"
#include "card_reader_fram.h"

OS_EVENT * sem_icreader_recv;//读卡头接收数据信号量



/*
设置设备回调函数
这个只做共性的检测，比如长度和校验
*/
static void icreader_recv_callback(uint8_t *dat, uint16_t len)
{
	uint8_t err, i, chk;//包长
	err = 0;
	if(len == dat[1])//包长符合
	{
		for(i = 0, chk = 0; i < len-1; i++)
			chk ^= dat[i];
		chk = ~chk;
		if(dat[len - 1] == chk )//校验符合
		{
			OSSemPost(sem_icreader_recv);
			err = TRUE;
		}
		else
		{
			err = 2;
		}
	}
	else
	{
		if(err)//防止报错
		err = 3;
	}
}


/*
设置设备工作方式
*/
static uint8_t  ireader_set(void)
{
	uint8_t i, j, chk, cmd[10], err;
	uint8_t *recv;

	i = 0;
	cmd[i++] = 0x3;
	cmd[i++] = 0x8;
	cmd[i++] = 0xc1;
	cmd[i++] = 0x20;
	cmd[i++] = 0x1;
	cmd[i++] = 0;
	cmd[i++] = 0x1;
	for(j = 0, chk = 0;j < i; j++)
	{
		chk ^= cmd[j];
	}
	cmd[i++] = ~chk;
	recv = cmd_icreader_send(cmd, i , icreader_recv_callback);
	OSSemPend(sem_icreader_recv, OS_TICKS_PER_SEC / 2, &err);//等待200ms
	if(err == OS_NO_ERR)
	{
		if(recv[4] == 0 && recv[2] == cmd[2])//设置成功
		{
			return TRUE;
		}
	}
	return FALSE;
}
/*
查设备状态
*/
void ireader_link(void)
{
	uint8_t i, j, chk, cmd[10], err;
	uint8_t *recv;
	
	i = 0;
	cmd[i++] = 0x2;
	cmd[i++] = 0x8;
	cmd[i++] = 0xb1;
	cmd[i++] = 0x20;
	cmd[i++] = 0;
	cmd[i++] = 0;
	cmd[i++] = 0;
	for(j = 0, chk = 0;j < i; j++)
	{
		chk ^= cmd[j];
	}
	cmd[i++] = ~chk;
	recv = cmd_icreader_send(cmd, i , icreader_recv_callback);	
	OSSemPend(sem_icreader_recv, OS_TICKS_PER_SEC /2, &err);//等待500ms
	if(err == OS_NO_ERR)
	{
		if(recv[4] == 0 && recv[2] == cmd[2])//查询成功 
		{
			if(recv[7] != 0x1)//不是被动模式
			{
				if(ireader_set())
				icreader_state_change(NORMAL);
				else
				icreader_state_change(AB_NORMAL); 
			}
			else
			{
				icreader_state_change(NORMAL);
			}
		}
	}
	else
	{
//		if(ireader_set())
//		icreader_state_change(NORMAL);
//		else
		icreader_state_change(AB_NORMAL); 
	}
}

/*
设备读指令，主要用来读卡ID
*/
uint8_t ireader_read(void)
{
	uint8_t i, j, chk, cmd[10], err, rs;
	uint8_t *recv;
	
	rs = FALSE;
	i = 0;
	cmd[i++] = 0x1;
	cmd[i++] = 0x8;
	cmd[i++] = 0xa1;
	cmd[i++] = 0x20;
	cmd[i++] = 0x0;
	cmd[i++] = 1;
	cmd[i++] = 0x0;
	for(j = 0, chk = 0;j < i; j++)
	{
		chk ^= cmd[j];
	}
	cmd[i++] = ~chk;
	recv = cmd_icreader_send(cmd, i , icreader_recv_callback);
	OSSemPend(sem_icreader_recv, OS_TICKS_PER_SEC / 2, &err);//等待200ms
	if(err == OS_NO_ERR)
	{
		if(recv[4] == 0 && recv[2] == cmd[2])//读成功
		{
			sprintf(cIC.PrintSnr, "%010u",  ( (recv[10] << 24) | (recv[9] << 16) | (recv[8] << 8) | (recv[7]) ) );
			rs = TRUE;
		}
		icreader_state_change(NORMAL);		
	}
	else
	{
		icreader_state_change(AB_NORMAL);
	}
	return rs;
}


/*
扣款指令
参数：money：要扣的金额
*/
uint8_t ireader_pay(uint32_t money)
{

	return FALSE;
}
