/**************
外部回路控制策略
回路1 压缩机/制冷
回路2 风机（控制在回路1的策略中体现）
回路3 照明
回路4 制热（策略集成到回路1的策略），替代了梁里面的消毒（梁的制热与压缩机是同一路）
回路5 玻璃加热
回路6 备用
**************/

#include "header.h"
#include "drv.h"
#include "drv_policy.h"


/*
注意：
这里目前只支持双机柜的
*/
static uint32_t coldStartProtectTime[9];		//压缩机起停保护时间（压缩机停止后不能马上启动）
static uint32_t coldRunProtectTime[9];	//压缩机过载保护时间（压缩机长时间工作会导致工作温度上升）

/*
温度控制策略
*/
void policy_tempeture(void)
{
	//读目标温度与温度时间
	uint8_t i,onoff;
	uint16_t stime1, etime1, mtime;
	SETUP_EX1 temp;
	
	e2p_get_ex1(&temp);
	
	stime1 = (temp.starttime_1/100*60) + temp.starttime_1 % 100;
	etime1 = (temp.stoptime_1/100*60) + temp.stoptime_1 % 100;
	
	mtime = calendar.hour*60 + calendar.min;
	onoff = 0;
	
	if(temp.starttime_1 == 0 && temp.stoptime_1 == 0)
		onoff = 1;
	else
	if(temp.starttime_1 == temp.stoptime_1 == 2359)
		onoff = 0;
	else
	{
		if(stime1 < etime1)
		{
			if(mtime >= stime1 && mtime<=etime1)
				onoff=1;
		}
		else
		if(stime1 > etime1)
		{
			if(mtime >= stime1 || mtime<=etime1)
				onoff=1;
		}
	}

	for(i = 0;i < g_vm.maxContainerNum; i++)//循环所有机柜
	{	
		drv_cmd_checkTemp(i, 0);	//更新驱动板的温度 1
		if(pt_driverBoard[i] != 0 && pt_driverBoard[i]->state == 1)//如果驱动板正常
		{
			drv_cmd_checkTemp(i, 1);//更新驱动板的温度 2
			drv_cmd_checkEX(i, EX_DISINFECT);
			drv_cmd_checkEX(i, EX_COLD);
			if(pt_driverBoard[i]->temp.state[0] == 1)//温控探头正常
			{
				if(onoff == 1)
				{
					if(temp.workMode == 0)//制冷
					{
						if(pt_driverBoard[i]->exState[EX_DISINFECT] == 1)
						{
							drv_cmd_closeEX(i, EX_DISINFECT);//关闭制热	
						}
						
						if(pt_driverBoard[i]->exState[EX_COLD] == 0)//压缩机没有开启
						{
							coldStartProtectTime[i]++;
							if(pt_driverBoard[i]->temp.value[0] >=  temp.tempTarget + 4)//比目标温度高4度
							{
								if(g_vm.door == 0)//如果是关门状态
								{
									if(coldStartProtectTime[i] > 42)//起停保护时间7min
									{
										coldRunProtectTime[i] = 0;
										drv_cmd_openEX(i, EX_COLD);//开启压缩机 
									}
								}
								drv_cmd_openEX(i, EX_WIND);//开启风机回路（风机不跟随门碰，防止结霜）
							}
						}
						else//压缩机已经开启
						{
							coldRunProtectTime[i]++;
							if(g_vm.door == 1)//门开
							{
								drv_cmd_closeEX(i, EX_COLD);//关闭压缩机
								coldStartProtectTime[i] = 0;
								//不关闭风机
							}
							else
							{
								if(coldRunProtectTime[i] >= 318)//达到保护时间
								{
									drv_cmd_closeEX(i, EX_COLD);//关闭压缩机
									coldStartProtectTime[i] = 0;
								}
								if(pt_driverBoard[i]->temp.value[0] <= temp.tempTarget)//达到目标温度
								{
									drv_cmd_closeEX(i, EX_COLD);//关闭压缩机
									drv_cmd_closeEX(i, EX_WIND);//关闭风机
									coldStartProtectTime[i] = 0;
								}
							}
						}
					}
					else//制热
					{
						coldStartProtectTime[i] = 0;//每次都清这个数据，保证了制热制冷切换时的策略严谨性
						coldRunProtectTime[i] = 0;
						
						if(pt_driverBoard[i]->exState[EX_COLD] == 1)
						{
							drv_cmd_closeEX(i, EX_COLD);//关闭压缩机
						}
						
						if(pt_driverBoard[i]->exState[EX_DISINFECT] == 0)//控制回路未开启
						{
							if((pt_driverBoard[i]->temp.value[0] <= temp.tempTarget - 4) && (pt_driverBoard[i]->temp.value[0] <= 30))
							{
								if(g_vm.door == 0 )//如果是关门状态
								{
									coldRunProtectTime[i] = 0;
									drv_cmd_openEX(i,EX_DISINFECT);//开启制热
								}
								drv_cmd_openEX(i, EX_WIND);//开启风机回路
							}
						}
						else//控制回路已开启
						{
							if(g_vm.door == 0)//门关
							{
								if(pt_driverBoard[i]->temp.value[0] > 30 || pt_driverBoard[i]->temp.value[0] >= temp.tempTarget)//当前温度超过30度或超过目标温度
								{
									drv_cmd_closeEX(i, EX_DISINFECT);//关闭压缩机 		
									drv_cmd_closeEX(i, EX_WIND);//关闭风机
								}
							}
							else//门开
							{
								drv_cmd_closeEX(i, EX_DISINFECT);//关闭制热	
								//不关闭风机
//								drv_cmd_closeEX(i, EX_WIND);//关闭风机
							}
						}
					}
				}
				else//设置关闭
				{
					drv_cmd_closeEX(i, EX_COLD);//关闭压缩机 		
					drv_cmd_closeEX(i, EX_WIND);//关闭风机
					drv_cmd_closeEX(i, EX_DISINFECT);//关闭制热
				}
			}
			else//温度探头故障
			{
				drv_cmd_closeEX(i, EX_COLD);//关闭压缩机 		
				drv_cmd_closeEX(i, EX_WIND);//关闭风机
				drv_cmd_closeEX(i, EX_DISINFECT);//关闭制热
			}
		}
	}
}

/*
照明控制策略
*/
void policy_light(void)
{
	uint8_t i,onoff;
	uint16_t stime1, etime1, mtime;
	SETUP_EX1 temp;
	
	e2p_get_ex1(&temp);//读取控制第一部分参数：包括 制冷，风机，照明 共3路
	
	stime1 = (temp.starttime_3/100*60) + temp.starttime_3 % 100;
	etime1 = (temp.stoptime_3/100*60) + temp.stoptime_3 % 100;
	
	mtime = calendar.hour*60 + calendar.min;
	onoff = 0;
	
	if(temp.starttime_3 == 0 && temp.stoptime_3 == 0)
		onoff = 1;
	else
	if(temp.starttime_3 == temp.stoptime_3 == 2359)
		onoff = 0;
	else
	{
		if(stime1 < etime1)
		{
			if(mtime >= stime1 && mtime <= etime1)
				onoff=1;
		}
		else
		if(stime1 > etime1)
		{
			if(mtime >= stime1 || mtime <= etime1)
				onoff=1;
		}
	}

	for(i = 0;i < g_vm.maxContainerNum; i++)
	{	
		if(pt_driverBoard[i] != 0 && pt_driverBoard[i]->state == 1) //驱动板正常
		{
			if(onoff==1)
			{
				drv_cmd_openEX(i, EX_LIGHT);//开启照明
			}
			else
			{
				drv_cmd_closeEX(i, EX_LIGHT);//关闭照明
			}
		}
	}
}

/*
玻璃加热控制策略
*/
void policy_glass(void)
{
	uint8_t i,onoff;
	uint16_t stime1, etime1, mtime;
	SETUP_EX2 tmp;
	
	e2p_get_ex2(&tmp);//读取控制第2部分参数：包括 消毒，备用1，备用2
	
	stime1 = (tmp.starttime_5/100*60) + tmp.starttime_5 % 100;
	etime1 = (tmp.stoptime_5/100*60) + tmp.stoptime_5 % 100;
	
	mtime = calendar.hour*60 + calendar.min;
	onoff = 0;
	
	if(tmp.starttime_5 == 0 && tmp.stoptime_5 == 0)
		onoff = 1;
	else
	if(tmp.starttime_5 == tmp.stoptime_5 == 2359)
		onoff = 0;
	else
	{
		if(stime1 < etime1)
		{
			if(mtime >= stime1 && mtime <= etime1)
				onoff=1;
		}
		else
		if(stime1 > etime1)
		{
			if(mtime >= stime1 || mtime <= etime1)
				onoff=1;
		}
	}

	for(i = 0;i < g_vm.maxContainerNum; i++)
	{	
		if(pt_driverBoard[i] != 0 && pt_driverBoard[i]->state == 1) //驱动板正常
		{
			if(onoff==1)
			{
				drv_cmd_openEX(i, EX_BACKUP1);//备用1
			}
			else
			{
				drv_cmd_closeEX(i, EX_BACKUP1);//备用1
			}
		}
	}
}


/*
备用2
*/
void policy_backup2(void)
{
	uint8_t i,onoff;
	uint16_t stime1, etime1, mtime;
	SETUP_EX2 tmp;
	
	e2p_get_ex2(&tmp);//读取控制第2部分参数：包括 消毒，备用1，备用2
	
	stime1 = (tmp.starttime_6/100*60) + tmp.starttime_6 % 100;
	etime1 = (tmp.stoptime_6/100*60) + tmp.stoptime_6 % 100;
	
	mtime = calendar.hour*60 + calendar.min;
	onoff = 0;
	
	if(tmp.starttime_6 == 0 && tmp.stoptime_6 == 0)
		onoff = 1;
	else
	if(tmp.starttime_6 == tmp.stoptime_6 == 2359)
		onoff = 0;
	else
	{
		if(stime1 < etime1)
		{
			if(mtime >= stime1 && mtime <= etime1)
				onoff=1;
		}
		else
		if(stime1 > etime1)
		{
			if(mtime >= stime1 || mtime <= etime1)
				onoff=1;
		}
	}

	for(i = 0;i < g_vm.maxContainerNum; i++)
	{	
		if(pt_driverBoard[i] != 0 && pt_driverBoard[i]->state == 1) //驱动板正常
		{
			if(onoff==1)
			{
				drv_cmd_openEX(i, EX_BACKUP2);//备用2
			}
			else
			{
				drv_cmd_closeEX(i, EX_BACKUP2);//备用2
			}
		}
	}
}
