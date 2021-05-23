/*
驱动板基础驱动
实现通讯协议的指令
*/
#include "drv.h"
#include "header.h"
/*
协议-命令
*/
enum{ 
	CMD_CHECK = 1,	
	CMD_SALE,	
	CMD_RESET, 
	CMD_CHECKALL,  	
	CMD_RESET_ALL,	
	CMD_CHECK_TEMP,
	CMD_CHECK_EX,
	CMD_ENABLE_EX,
	CMD_DISABLE_EX,
	CMD_RESETALL_EX,	//关闭所有外部通道	
	CMD_CHECK_VER,		//查询版本号
	CMD_SET_PARAM,		//设置参数
	CMD_CHECK_PARAM,	//查询参数
	CMD_RESET_PARAM,	//复位参数
	CMD_RESET_DRIVERBOARD,	//复位驱动板
	CMD_CHECK_DROP,		//查询调货检测
	CMD_CHECK_EXECUTE = 222,//查询命令执行情况
};

/*
执行结果
说明：这个是针对指令执行情况而言的
*/
 enum
 {
	DRV_RESULT_SUCCESS = 1,	//成功
	DRV_RESULT_FAILURE = 2,	//失败
	DRV_RESULT_BUSY = 3,		//驱动板忙
	DRV_RESULT_CHKERR = 4,	//校验和错误
	DRV_RESULT_LENERR = 5,	//发送数据长度错误（驱动板接收到的长度不对）
	DRV_RESULT_NOSUPPORT = 6,//不支持的指令
	DRV_RESULT_HEADERR = 7,	//指令头错误
	DRV_RESULT_STOPLESS = 8,//出货时间太短
	DRV_RESULT_NOSTART = 9,	//超时未启动
	DRV_RESULT_NOSTOP = 10,	//超时未停止
	DRV_RESULT_SHORT = 11,	//短路
	DRV_RESULT_OVER = 12,		//过载
	DRV_RESULT_ADDRERR = 13,//驱动板地址错误（这个其实是没有的，因为地址错了驱动板是无回复的）
	DRV_RESULT_CMDERR = 14,	//指令错误，收到的回复与应答不对应
	DRV_RESULT_TIMEOUT = 15,//指令超时
	DRV_RESULT_CMDINVALID = 16,//发送的指令无效 
	DRV_RESULT_OTHERS = 17,	//其他错误
};
 
/*
电机状态说明
说明：这个是针对电机状态而言的
#define MOTOR_STATE_NORMAL		1	//正常
#define MOTOR_STATE_OUT			2	//不在位
#define MOTOR_STATE_OVERLOAD	3	//过流
#define MOTOR_STATE_NOSTART		4	//超时未启动
#define MOTOR_STATE_NOSTOP		5	//超时未停止
#define MOTOR_STATE_NOINSTALL	6	//未安装电机
#define MOTOR_STATE_SHORT		7	//电机短路
*/



 /*
常量
*/
#define M_STX		'$'	//主机包头
#define S_STX		'@'	//驱动板包头
//#define ETX			'!'	//包尾
#define M_ADDR		1	//主机地址


/*
参数：data：收到的数据；len，数据长度指针
*/
static bool isRecvFinished(volatile uint8_t* data,volatile uint8_t* len)
{
	uint8_t i,chk;
	
	if(data[0] == S_STX)
	{
		if(*len == (data[3] << 8 | data[4]))//包长符合
		{
			//计算校验
			for(i = 0, chk = 0; i < *len - 1; i++)
			{
				chk += data[i];
			}
			
			if(chk == data[(*len) - 1])//校验通过
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

/*
功能：汇报驱动板状态变化
参数：addr：驱动板地址；state：驱动板状态
*/
static void driverboard_state_change(uint8_t addr, uint8_t state)
{
	
	if(addr > 9)//防止指针错误
		return;
	if(state != pt_driverBoard[addr]->state && pt_driverBoard[addr] != 0)
	{
		if(pt_driverBoard[addr]->state == INIT)
		{
			FSMMSG *to_fsm;
			to_fsm = apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
			log_output(to_fsm);
			if(to_fsm != 0)
			{
				pt_driverBoard[addr]->state = state;
				to_fsm->type = MsgCHAN;
				to_fsm->stype = M_DRV_INIT;
				to_fsm->value = state; 
				if(!send_fsm_msg(to_fsm))
				release_fsm_msg(to_fsm);
			}
		}
		else
		{
			pt_driverBoard[addr]->state = state;
			report_state_change(STATE_DRV, (addr + 1), state);//驱动状态改变
		}
	}
}



/*
功能：组包发送驱动板指令
参数：addr：驱动板地址；*cmd：待发数据指针；cmd_len:待发数据长度；*recv接收数据数据指针
分为2个步骤
1：发要执行命令
2：查询命令执行情况
*/
static uint8_t send_cmd(uint8_t addr, uint8_t *cmd, uint16_t cmd_len ,uint8_t *recv)
{
	uint8_t rs ,j,err, chk;
	UART485_CMD_DATA str;
	
	bus_Lock_485();//锁住发送过程
	
	rs = OS_TIMEOUT;
	err = OS_TIMEOUT;
	
	str.cmd = _malloc(63, &err);//申请内存
	
	log_output(str.cmd);
	
	if(err == OS_NO_ERR)
	{
		rs = OS_TIMEOUT;
		str.recv = recv;					//接收缓冲区指向
		str.needAck = TRUE;					//指令需要应答
		str.timeout = OS_TICKS_PER_SEC/3;	//返回等待超时时间
		str.isFinished = isRecvFinished;	//返回处理函数，其实就是个回调函数
		
		str.cmd_len = 0;
		str.cmd[str.cmd_len++] = M_STX;		//包头
		str.cmd[str.cmd_len++] = M_ADDR;	//主机地址
		str.cmd[str.cmd_len++] = addr;		//驱动板地址
		str.cmd[str.cmd_len++] = ' ';		//包长
		str.cmd[str.cmd_len++] = ' ';
		for(j = 0;j < cmd_len ;j++)//填充数据
		{
			str.cmd[str.cmd_len++] = cmd[j];
		}
		str.cmd[3] = (str.cmd_len + 1) >> 8;		//包长
		str.cmd[4] = (str.cmd_len + 1);
		
		//添加校验
		for(j = 0, chk = 0; j < str.cmd_len; j++)
		{
			chk +=  str.cmd[j];
		}
		str.cmd[str.cmd_len++] = chk;
		
		if(send_to_485Bus(&str) == OS_NO_ERR)//收到数据
		{
			if(str.recv[2] == str.cmd[2])//驱动板地址符合
			{
				if(str.recv[5] == str.cmd[5])//是对发送指令的应答
				{
					if(str.recv[6] == 1)//驱动板接收命令
					{
						uint8_t chk, i;
						err = OS_TIMEOUT;
						/*
						命令执行情况查询
						这条指令是固定的
						*/
						str.cmd_len = 0;
						str.cmd[str.cmd_len++] = M_STX;		//包头
						str.cmd[str.cmd_len++] = M_ADDR;	//主机地址
						str.cmd[str.cmd_len++] = addr;		//驱动板地址
						str.cmd[str.cmd_len++] = ' ';		//有效数据长
						str.cmd[str.cmd_len++] = ' ';
						str.cmd[str.cmd_len++] = CMD_CHECK_EXECUTE;
						str.cmd[3] = (str.cmd_len + 1) >> 8;		//包长
						str.cmd[4] = (str.cmd_len + 1);
						//添加校验
						for(j = 0, chk = 0; j < str.cmd_len; j++)
						{
							chk +=  str.cmd[j];
						}
						str.cmd[str.cmd_len++] = chk;

						i = 0;
						do
						{
							err = send_to_485Bus(&str);
							if(str.recv[6] == 3)
								OSTimeDly(OS_TICKS_PER_SEC/2);//如果忙，等待500ms再次询问，忙状态最长时间15s（什么电机都应该转完了）
						}while( ( err == OS_NO_ERR && (str.recv[6] == 3) && (i++ < 30) ) || ( err != OS_NO_ERR && (i++ < 2) ) ); 
							
						
						if(str.recv[6] == 3)//驱动板忙
						{
							rs = DRV_RESULT_BUSY;
						}
						else
						{
							if(str.recv[2] == str.cmd[2])//驱动板地址符合
							{
								if(str.recv[5] == str.cmd[5])//应答的是发送命令
								{
									rs = OS_NO_ERR;
								}
							}
							else//地址错误
							{
								rs = DRV_RESULT_ADDRERR;
							}
						}
					}
					else//驱动板拒收命令
					{
						rs = DRV_RESULT_CMDERR;
					}
				}
				else//应答非当前指令
				{
					rs = DRV_RESULT_CMDERR;
				}
			}
			else//驱动板地址错，第一条指令
			{
				rs = DRV_RESULT_ADDRERR;
			}
		}
		else//超时未应答
		{
			rs = DRV_RESULT_TIMEOUT;
		}
	}
	else//未申请到内存，此条指令作废
	{
		
		rs = DRV_RESULT_CMDINVALID;
	}
	
//	if(err != 0)//测试代码
//	{
//		rs = rs;
//	}

	_free(str.cmd);//释放内存
	bus_Unlock_485();
	return rs;
}




/*
功能：货道状态查询
参数：addr：地址；channel：内部货道编码（从0~79）
*/
uint8_t drv_cmd_channleCheck(uint8_t addr, uint8_t channel)
{
	uint8_t rs;
	uint8_t cmd[30],cmd_len;
	uint8_t recv[20];
	uint8_t state;//电机状态
	
	cmd_len = 0; 
	cmd[cmd_len++] = CMD_CHECK;		//电机查询指令
	if(pt_driverBoard[addr]->channelState[channel] == MOTO_LINK)//如果电机是联动的(开机时读参数，然后更新电机状态)
		cmd[cmd_len++] = 2;	//电机数量
	else	//电机非联动
		cmd[cmd_len++] = 1;			
	cmd[cmd_len++] = channel;	
	if(pt_driverBoard[addr]->channelState[channel] == MOTO_LINK)
		cmd[cmd_len++] = channel + 1;

	rs = send_cmd(addr, cmd, cmd_len, recv);
	if(rs == OS_NO_ERR)//执行完成，但是执行是否成功不一定
	{
		 
		if(pt_driverBoard[addr]->channelState[channel] != recv[9])//电机状态变化
		{
			pt_driverBoard[addr]->channelState[channel] = recv[9];//更新电机状态
			
			if(pt_driverBoard[addr]->channelState[channel] == 1)
			{
				#if ZJ_VS_XY == 1
				e2p_get_channelState(channel, &state);//读电机状态
				if(pt_driverBoard[addr]->channelState[channel] != state)
					pt_driverBoard[addr]->channelState[channel] = state;//更新电机状态
				#endif
			}
			report_state_change(STATE_CHANNEL, (addr+1), recv[9] - 1);//汇报货道状态改变
		}
		rs = recv[6];//执行结果
	}
	return rs;
	
}

/*
功能：货道出货指令
参数：addr：地址；channel：内部货道编码（从0~79）
*/
uint8_t drv_cmd_sale(uint8_t addr, uint8_t channel)
{
	uint8_t rs;
	uint8_t cmd[30],cmd_len;
	uint8_t recv[20];
	uint8_t state;
	
	cmd_len = 0; 
	cmd[cmd_len++] = CMD_SALE;		//电机查询指令
	if(pt_driverBoard[addr]->channelState[channel] == MOTO_LINK)//如果电机是联动的(开机时读参数，然后更新电机状态)
	cmd[cmd_len++] = 2;	//电机数量
	else	//电机非联动
	cmd[cmd_len++] = 1;			
	cmd[cmd_len++] = channel;	
	if(pt_driverBoard[addr]->channelState[channel] == MOTO_LINK)
	cmd[cmd_len++] = channel + 1;

	#if ZJ_VS_XY == 1
	state = 0;
	e2p_set_channelState(channel, &state);//先置位为不在位
	#endif
	
	
	rs = send_cmd(addr, cmd, cmd_len, recv);
	if(rs == OS_NO_ERR)//执行完成，但是执行是否成功不一定
	{
		if(pt_driverBoard[addr]->channelState[channel] != recv[9])//电机状态变化
		{
			pt_driverBoard[addr]->channelState[channel] = recv[9];//更新电机状态
			report_state_change(STATE_CHANNEL, (addr+1), recv[9] - 1);//汇报货道状态改变
		}
		rs = recv[6];//执行结果
	}
	#if ZJ_VS_XY == 1
	if(pt_driverBoard[addr]->channelState[channel] == 1)//驱动板报电机正常
	{
		state = 1;
		e2p_set_channelState(channel, &state);//置位为在位
	}
	#endif
	return rs;
}

/*
功能：货道复位指令
参数：addr：地址；channel：内部货道编码（从0~79）
*/
uint8_t	drv_cmd_channelReset(uint8_t addr, uint8_t channel)
{
	uint8_t rs = FALSE;
	uint8_t cmd[30],cmd_len;
	uint8_t recv[20];
	uint8_t state;
	
	cmd_len = 0; 
	cmd[cmd_len++] = CMD_RESET;		//电机查询指令
	if(pt_driverBoard[addr]->channelState[channel] == MOTO_LINK)//如果电机是联动的(开机时读参数，然后更新电机状态)
		cmd[cmd_len++] = 2;	//电机数量
	else	//电机非联动
		cmd[cmd_len++] = 1;			
	cmd[cmd_len++] = channel;	
	if(pt_driverBoard[addr]->channelState[channel] == MOTO_LINK)
		cmd[cmd_len++] = channel + 1;

	#if ZJ_VS_XY == 1
	state = 0;
	e2p_set_channelState(channel, &state);//先置位为不在位
	#endif
	
	rs = send_cmd(addr, cmd, cmd_len, recv);
	if(rs == OS_NO_ERR)//执行完成（可能执行成功，也可能执行失败）
	{
		if(pt_driverBoard[addr]->channelState[channel] != recv[9])//电机状态变化
		{
			pt_driverBoard[addr]->channelState[channel] = recv[9];//更新电机状态
			report_state_change(STATE_CHANNEL, (addr+1), recv[9] - 1);//汇报货道状态改变
		}
		rs = recv[6];//执行结果
	}
	#if ZJ_VS_XY == 1
	if(pt_driverBoard[addr]->channelState[channel] == 1)//驱动板报电机正常
	{
		state = 1;
		e2p_set_channelState(channel, &state);//置位为在位
	}
	#endif
	return rs;
}

/*
功能：查询整机货道状态
参数：addr：驱动板地址
*/
uint8_t	drv_cmd_channelCheckall(uint8_t addr)
{
	uint8_t rs;
	uint8_t cmd[20],cmd_len;
	uint8_t recv[100];
	uint8_t state;

	cmd_len = 0;
	cmd[cmd_len++] = CMD_CHECKALL;

	rs = send_cmd(addr, cmd, cmd_len, recv);
	if(rs == OS_NO_ERR)//执行完成，但是执行是否成功不一定
	{
		if(recv[6] == DRV_RESULT_SUCCESS)//只有执行成功才可以更新整机货道
		{
			//更新货道状态
			uint8_t i;
			for(i = 0; i < CHANNEL_SIZE; i++)
			{
				pt_driverBoard[addr]->channelState[i] = recv[i+9];
				if(pt_driverBoard[addr]->channelState[i] == 1)
				{
					#if ZJ_VS_XY == 1
					e2p_get_channelState(i, &state);//查询电机状态
					pt_driverBoard[addr]->channelState[i] = state;
					#endif
				}
			}
		}
		rs = recv[6];
	}
	return rs;
}

/*
功能：复位整机货道
参数：addr：驱动板地址
*/
uint8_t	drv_cmd_channelResetall(uint8_t addr)
{
	uint8_t rs;
	uint8_t cmd[20],cmd_len;
	uint8_t recv[20];

	cmd_len = 0;
	cmd[cmd_len++] = CMD_RESETALL_EX;

	rs = send_cmd(addr, cmd, cmd_len, recv);
	if(rs == OS_NO_ERR)//执行完成，但是执行是否成功不一定
	{
		if(recv[6] == DRV_RESULT_SUCCESS)//只有执行成功才可以更新整机货道
		{
			uint8_t i;
			for(i = 0; i < EX_SIZE; i++)
			pt_driverBoard[addr]->exState[i] = 0;//复位成功，所有回路关闭
		}
		rs = recv[6];
	}
	return rs; 
}

/*
功能：打开通道
参数：addr：驱动板地址（0~9）；exchannel：外部回路内部编码（0~5）
*/
uint8_t	drv_cmd_openEX(uint8_t addr, uint8_t exchannel)
{
	
	uint8_t rs;
	uint8_t cmd[20],cmd_len;
	uint8_t recv[20];
	
	cmd_len = 0; 
	cmd[cmd_len++] = CMD_ENABLE_EX;	//电机查询指令
	cmd[cmd_len++] = exchannel;	

	rs = send_cmd(addr, cmd, cmd_len, recv);
	if(rs == OS_NO_ERR)//执行完成，但是执行是否成功不一定
	{
		if(recv[6] == DRV_RESULT_SUCCESS)
		{
			uint8_t exID;
			//更新回路状态
			if(pt_driverBoard[addr]->exState[exchannel] != recv[9])
			{
				pt_driverBoard[addr]->exState[exchannel] = recv[9];//更新输出回路状态
				switch(exchannel)
				{
					case 0: exID = STATE_COLD; break;
					case 1: exID = STATE_WIND; break;
					case 2: exID = STATE_LAMP; break;
					case 3: exID = STATE_DISINFACT; break;
					case 4: exID = STATE_GLASS; 	break;
					case 5: exID = STATE_BACKUP2; 	break;
				}
				report_state_change(exID, (addr + 1), pt_driverBoard[addr]->exState[exchannel]);//汇报后台
			}
		}

		rs = recv[6];
	}
	return rs;
}

/*
功能：关闭通道
参数：addr：驱动板地址（0~9）；exchannel：外部回路内部编码（0~5）
*/
uint8_t drv_cmd_closeEX(uint8_t addr, uint8_t exchannel )
{
	uint8_t rs;
	uint8_t cmd[30],cmd_len;
	uint8_t recv[20];
	
	cmd_len = 0; 
	cmd[cmd_len++] = CMD_DISABLE_EX;	//电机查询指令
	cmd[cmd_len++] = exchannel;	

	rs = send_cmd(addr, cmd, cmd_len, recv);
	if(rs == OS_NO_ERR)//执行完成，但是执行是否成功不一定
	{
//		pt_driverBoard[addr]->state = 1;//更新货柜状态
		if(recv[6] == DRV_RESULT_SUCCESS)
		{
			uint8_t exID;
			//更新货道状态
			if(pt_driverBoard[addr]->exState[exchannel] != recv[9])
			{

				pt_driverBoard[addr]->exState[exchannel] = recv[9];//更新输出回路状态
				switch(exchannel)
				{
					case 0: exID = STATE_COLD; break;
					case 1: exID =STATE_WIND; break;
					case 2: exID =STATE_LAMP; break;
					case 3: exID =STATE_DISINFACT; break;
					case 4: exID =STATE_GLASS; break;
//					case 5: exID = STATE_BACKUP2; break;
				}
				if(exchannel != 5)//5通道协议中没有
				report_state_change(exID, (addr + 1), pt_driverBoard[addr]->exState[exchannel]);//汇报后台
			}
		}
		rs = recv[6];
	}
	return rs;
}

/*
功能：查询通道
参数：addr：驱动板地址（0~9）；exchannel：外部回路内部编码（0~5）

*/
uint8_t	drv_cmd_checkEX(uint8_t addr, uint8_t exchannel )
{
	
	uint8_t rs;
	uint8_t cmd[20],cmd_len;
	uint8_t recv[50];
	
	cmd_len = 0; 
	cmd[cmd_len++] = CMD_CHECK_EX;	//电机查询指令
	cmd[cmd_len++] = exchannel;	

	rs = send_cmd(addr, cmd, cmd_len, recv);
	if(rs == OS_NO_ERR)//执行完成，但是执行是否成功不一定
	{
		if(recv[6] == DRV_RESULT_SUCCESS)
		{
			pt_driverBoard[addr]->exState[exchannel] = recv[9];//更新输出回路状态
			//这里不需要汇报，在打开或关闭中汇报
		}
		rs = recv[6];
	}
	return rs;
}

/*
功能：查询驱动板版本号
参数：addr：驱动板地址
*/
uint8_t	drv_cmd_checkVER(uint8_t addr )
{
	
	uint8_t rs;
	uint8_t cmd[20],cmd_len;
	uint8_t recv[50];
	
	cmd_len = 0; 
	cmd[cmd_len++] = CMD_CHECK_VER;	//版本号查询

	rs = send_cmd(addr, cmd, cmd_len, recv);
	if(rs == OS_NO_ERR)//执行完成，但是执行是否成功不一定
	{
		if(recv[6] == DRV_RESULT_SUCCESS)
		{
			//取得版本号
			uint8_t i;
			for(i = 0; (i < VER_SIZE) && (recv[8+i] != 0); i++)
			pt_driverBoard[addr]->version[i] = recv[8+i];//更新版本号
		}
		rs = recv[6];
//		driverboard_state_change(addr, NORMAL );
	}
	else
	{
//		driverboard_state_change(addr, AB_NORMANL);
	}
	return rs;

}

/*
功能：查询温度
参数：addr：地址；id：温度检测回路内部编码（0~1）
说明：这条指令也用来作为和驱动板的心跳使用
*/
uint8_t	drv_cmd_checkTemp(uint8_t addr, uint8_t id )
{
	
	uint8_t rs;
	uint8_t cmd[30],cmd_len;
	uint8_t recv[50];
	
	cmd_len = 0; 
	cmd[cmd_len++] = CMD_CHECK_TEMP;	//电机查询指令
	cmd[cmd_len++] = id;	

	rs = send_cmd(addr, cmd, cmd_len, recv);
	if(rs == OS_NO_ERR)//执行完成，但是执行是否成功不一定
	{
		if(recv[6] == DRV_RESULT_SUCCESS)
		{
			//更新温度与温度探头状态
			//汇报温度探头状态
			if( pt_driverBoard[addr]->temp.state[id] != recv[9] )
			{
				pt_driverBoard[addr]->temp.state[id] = recv[9];//温度状态
				//放到温度策略中去汇报
//				report_state_change(STATE_TEMP, id + 1, state);//温度探头状态汇报
			}
			pt_driverBoard[addr]->temp.value[id] = recv[10];//温度值
		}
		rs = recv[6];
		driverboard_state_change(addr, NORMAL );
	}
	else
	{
		driverboard_state_change(addr, AB_NORMANL);
	}
	return rs;

}

/*
功能：读取驱动板参数
参数：addr：驱动板地址
*/
uint8_t	drv_cmd_getParam(uint8_t addr , uint16_t *dat)
{
	uint8_t rs;
	uint8_t cmd[30],cmd_len;
	uint8_t recv[60];
	
	cmd_len = 0; 
	cmd[cmd_len++] = CMD_CHECK_PARAM;	//参数查询

	rs = send_cmd(addr, cmd, cmd_len, recv);
	if(rs == OS_NO_ERR)//执行完成，但是执行是否成功不一定
	{
		if(recv[6] == DRV_RESULT_SUCCESS)
		{
			uint8_t i;
			for(i = 0; i < 13; i++)
			dat[i] = (recv[8+i*2] << 8) | recv[8+i*2+1];//驱动板参数
		}
		rs = recv[6];
//		driverboard_state_change(addr, NORMAL );
	}
	else
	{
//		driverboard_state_change(addr, AB_NORMANL);
	}
	return rs;

}

/*
功能：设置驱动板参数
参数：addr：驱动板地址
		dat:传入的待设置参数
*/
uint8_t	drv_cmd_setParam(uint8_t addr , uint16_t *dat)
{
	uint8_t rs;
	uint8_t cmd[60],cmd_len;
	uint8_t recv[50];
	
	cmd_len = 0; 
	cmd[cmd_len++] = CMD_SET_PARAM;	//参数查询
	
	//驱动板类型
	cmd[cmd_len++] = (dat[0] >> 8)&0xff;
	cmd[cmd_len++] = (dat[0])&0xff;
	
	//温度探头类型
	cmd[cmd_len++] = (dat[1] >> 8)&0xff;	
	cmd[cmd_len++] = dat[1] & 0xff;	
	
	//启动电流(mA)
	cmd[cmd_len++] = (dat[2] >> 8) & 0xff 	;	
	cmd[cmd_len++] = dat[2] & 0xff;	
	
	//短路电流(mA)
	cmd[cmd_len++] = (dat[3] >> 8) & 0xff	;
	cmd[cmd_len++] = dat[3] & 0xff	;

	//电机过载电流(mA)
	cmd[cmd_len++] = (dat[4] >> 8) & 0xff	;	
	cmd[cmd_len++] = dat[4] & 0xff	;	

	//过载允许时间(s)
	cmd[cmd_len++] = ((dat[5]) >> 8) & 0xff;	
	cmd[cmd_len++] = (dat[5]) & 0xff;	

	//超时未启动时间(s)
	cmd[cmd_len++] = ((dat[6]) >> 8) & 0xff	;	
	cmd[cmd_len++] = (dat[6]) & 0xff	;	

	//超时未停止时间(s)
	cmd[cmd_len++] = ((dat[7]) >> 8) & 0xff ;	
	cmd[cmd_len++] = (dat[7]) & 0xff	;	
	
	//电机停止延时(ms)
	cmd[cmd_len++] = (dat[8]	>> 8) & 0xff	;	
	cmd[cmd_len++] = dat[8] & 0xff	;	

	//出货信号检测次数
	cmd[cmd_len++] = (dat[9] >> 8) & 0xff ;	
	cmd[cmd_len++] = dat[9] & 0XFF;
	
	//出货信号检测间隔(us)
	cmd[cmd_len++] = (dat[10] >> 8) & 0xff;	
	cmd[cmd_len++] = dat[10] & 0XFF;	
	
	//查询信号检测次数
	cmd[cmd_len++] = (dat[11] >> 8) & 0xff ;	
	cmd[cmd_len++] = dat[11] & 0XFF;
	
	//查询信号检测间隔(us)
	cmd[cmd_len++] = (dat[12] >> 8) & 0xff;	
	cmd[cmd_len++] = dat[12] & 0XFF;	
	rs = send_cmd(addr, cmd, cmd_len, recv);
	if(rs == OS_NO_ERR)//执行完成，但是执行是否成功不一定
	{
		if(recv[6] == DRV_RESULT_SUCCESS)
		{
		}
		rs = recv[6];
//		driverboard_state_change(addr, NORMAL );
	}
	else
	{
//		driverboard_state_change(addr, AB_NORMANL);
	}
	return rs;

}


/*
功能：回复驱动板默认参数
参数：addr：驱动板地址
*/
uint8_t	drv_cmd_resetParam(uint8_t addr)
{
	uint8_t rs;
	uint8_t cmd[30],cmd_len;
	uint8_t recv[20];
	
	cmd_len = 0; 
	cmd[cmd_len++] = CMD_RESET_PARAM;	//参数复位

	rs = send_cmd(addr, cmd, cmd_len, recv);
	if(rs == OS_NO_ERR)//执行完成，但是执行是否成功不一定
	{
		rs = recv[6];
//		driverboard_state_change(addr, NORMAL );
	}
	else
	{
//		driverboard_state_change(addr, AB_NORMANL);
	}
	return rs;

}
/*
（新增，用来使得驱动板重新启动）
复位驱动板
可以将驱动板pc指向0，重新开始执行
*/
uint8_t drv_cmd_reset(uint8_t addr)
{
	uint8_t rs;
	uint8_t cmd[30],cmd_len;
	uint8_t recv[20];
	
	cmd_len = 0; 
	cmd[cmd_len++] = CMD_RESET_DRIVERBOARD;	//参数查询

	rs = send_cmd(addr, cmd, cmd_len, recv);
	if(rs == OS_NO_ERR)//执行完成，但是执行是否成功不一定
	{
		if(recv[6] == DRV_RESULT_SUCCESS)
		{
		}
		rs = recv[6];
//		driverboard_state_change(addr, NORMAL );
	}
	else
	{
//		driverboard_state_change(addr, AB_NORMANL);
	}
	return rs;
}


