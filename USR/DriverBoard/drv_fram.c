/*
驱动板框架

*/
#include "drv.h"
#include "drv_policy.h"
#include "header.h"


#define DRIVER_QUEEN_SIZE	2	//驱动板消息主队列大小

volatile DRIVERBOARD *pt_driverBoard[10] = {0,0,0,0,0,0,0,0,0,0};//驱动板结构指针
static void *pt_drvMsg[DRIVER_QUEEN_SIZE];	//驱动板消息队列
static OS_EVENT *driverQueen;

/*
定义业务层到驱动板的消息结构
*/
typedef struct DRV_MSG
{
	uint8_t id;			//驱动板id
	uint8_t channel;	//驱动板通道
	uint8_t cmd;		//命令类型
	uint16_t extend;	//扩展长度
	uint8_t *dat;		//扩展数据指针
}DRV_MSG; 

/*
功能：申请驱动板消息
参数：cmd:要求驱动板执行的指令；id：驱动板地址；channel：要操作的通道；extend：扩展数据长度，*dat：数据指针，与extend配合使用

说明：驱动板对业务层命令接口。实际上就是业务层发送消息到驱动板，等待驱动板任务获取消息，然后执行。一种任务间异步通讯方式
		为了以后新增驱动板或者其他什么做备用；
*/
uint8_t apply_driverboard_msg(uint8_t cmd, uint8_t addr, uint8_t channel, uint16_t extend, uint8_t *dat)
{
	DRV_MSG *drvmsg = 0;
	uint8_t err,i;
	
	drvmsg = (DRV_MSG*)_malloc(sizeof(DRV_MSG), &err);
	drvmsg->dat = 0;//防止野指针
	log_output(drvmsg);
	
	if(drvmsg)//申请到内存
	{
		drvmsg->cmd = cmd;
		drvmsg->id = addr;
		drvmsg->channel = channel;
		drvmsg->dat = 0;//防止野指针
		if(extend)//申请额外数据空间
		{
			drvmsg->dat = _malloc(extend, &err);
			log_output(drvmsg->dat);
			if(drvmsg->dat)
			{
				drvmsg->extend = extend;
				for(i = 0; i < drvmsg->extend; i++)
				drvmsg->dat[i] = dat[i];
			}
		}
		//发送到队列
		if(OSQPost(driverQueen, drvmsg))//没发送成功
		{
			_free(drvmsg->dat);
			_free(drvmsg);
			return 2;
		}
	}
	else
		return FALSE;
	return TRUE;
}

/*
释放驱动板消息
*/
static void release_drv_msg(DRV_MSG *drvMsg){
	//释放驱动板消息内存
	_free(drvMsg->dat);
	_free(drvMsg);
}

/*
驱动板初始化
采用动态内存的方式，没有设置的驱动板，则不占用内存
*/
static void drv_init(void)
{
	//读设置，观察是多少个驱动板
	
	uint8_t i,j,err;
	if(g_vm.maxContainerNum == 0 || g_vm.maxContainerNum >10)
		g_vm.maxContainerNum = 1; 
	//驱动板申请内存
	for(i = 0;i < g_vm.maxContainerNum; i++)
	{
		pt_driverBoard[i] = 0;
		pt_driverBoard[i] = _malloc(sizeof(DRIVERBOARD),&err); 
		for(j = 0; j < 80; j++)
		{
			pt_driverBoard[i]->channelState[j] = 6;//初始化为未安装电机
		}
		for(j = 0; j < 6; j++)
		{
			pt_driverBoard[i]->exState[i] = 0;
		}
		for(j = 0; j < 2; j++)
		{
			pt_driverBoard[i]->temp.state[j] = 0;
			pt_driverBoard[i]->temp.value[j] = 0;
		}
		pt_driverBoard[i]->temp.stoptimer = 0;
		pt_driverBoard[i]->state = INIT;//初始化状态

		
		//读这两个参数（并且立刻发送给驱动板保存）
		pt_driverBoard[i]->overload.value = 0;
		pt_driverBoard[i]->overload.count = 0;
	}
}


/*
外部回路控制策略
*/
static void drv_export_policy(void)
{
	policy_tempeture();	//制冷策略
	policy_light();		//光照策略
	policy_glass();		//玻璃加热
	policy_backup2();	//备用通道2
	//测试程序
//		drv_cmd_openEX(0, EX_COLD);
//		drv_cmd_openEX(0, EX_WIND);
//		drv_cmd_openEX(0, EX_LIGHT);
//		drv_cmd_openEX(0, EX_DISINFECT);
//		drv_cmd_openEX(0, EX_BACKUP1);
//		drv_cmd_openEX(0, EX_BACKUP2);


}




/*
功能：复位所有机柜
*/
static void drv_reset(void)
{
	uint8_t i;
	//复位所有驱动板
	for(i = 0;i < g_vm.maxContainerNum; i++) 
	drv_cmd_channelResetall(i);
	
}
/*
功能：查询所有机柜货道
*/
static void drv_check(void)
{
	uint8_t i;
	
	//查询所有驱动板
	for(i = 0; (i < g_vm.maxContainerNum) && (pt_driverBoard[i] != 0); i++)
	{
		drv_cmd_checkVER(i);	//查驱动板版本
		drv_cmd_channelCheckall(i);
		drv_cmd_checkTemp(i, 0);//查询温度1
		drv_cmd_checkTemp(i, 1);//查询温度2
		drv_cmd_checkEX(i, EX_DISINFECT);
		drv_cmd_checkEX(i, EX_COLD);
		drv_export_policy();//执行一次控制策略，加速设备开启速度
	}
}

/*
功能：驱动主任务
*/
void drv_task(void)
{
	uint8_t err, i;
	uint32_t policyTimeout, tempReportTimeout;
	DRV_MSG *cur_dmsg;
	FSMMSG *to_fsm = 0;
	
	drv_init();//初始化驱动板
	drv_reset();//复位驱动板
	drv_check();//查所有驱动板信息
	
	driverQueen = OSQCreate(pt_drvMsg,DRIVER_QUEEN_SIZE);//驱动板主队列创建(此队列允许接收3条数据)
	policyTimeout = OSTimeGet();//记录当前时间
	tempReportTimeout = OSTimeGet();
	
	while(1)
	{
		cur_dmsg = (DRV_MSG *) OSQPend(driverQueen, 10*OS_TICKS_PER_SEC ,&err);
		if(err == OS_NO_ERR)//有命令需要执行
		{
			if(pt_driverBoard[cur_dmsg->id] != 0 )//当前驱动板指针不是空指针
			{
				if(pt_driverBoard[cur_dmsg->id]->state == 1 )//判断驱动板是否正常
				{
					switch(cur_dmsg->cmd)//判断命令类型，调用驱动函数
					{
						case M_CHANNEL_SALE://出货指令
							to_fsm = apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
						
							log_output(to_fsm);
						
							to_fsm->type = MsgCHAN;
							to_fsm->stype = M_CHANNEL_SALE;//要和上面类型对应
							to_fsm->value = drv_cmd_sale(cur_dmsg->id, cur_dmsg->channel);
							if(send_fsm_msg(to_fsm) != TRUE)
								release_fsm_msg(to_fsm);
						break;
							
						case M_CHANNEL_TEST://复位指令（电机测试）
							to_fsm = apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
						
							log_output(to_fsm);
						
							to_fsm->type = MsgCHAN;
							to_fsm->stype = M_CHANNEL_TEST;//要和上面类型对应
							to_fsm->value = drv_cmd_channelReset(cur_dmsg->id, cur_dmsg->channel);
							if(send_fsm_msg(to_fsm) != TRUE)
								release_fsm_msg(to_fsm);
						break;
							
						case M_DRV_CHECK://整机查货道指令
							to_fsm = apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
						
							log_output(to_fsm);
						
							to_fsm->type = MsgCHAN;
							to_fsm->stype = M_DRV_CHECK;//要和上面类型对应
							to_fsm->value = drv_cmd_channelCheckall(cur_dmsg->id);
							if(send_fsm_msg(to_fsm) != TRUE)
								release_fsm_msg(to_fsm);
						break;
						
					}
				}
				else//驱动板故障
				{
					to_fsm = apply_fsm_msg(FSMMSG_SIZE_DEFAULT);
				
					log_output(to_fsm);
				
					to_fsm->type = MsgCHAN;
					to_fsm->stype = M_DRV_ERR;//驱动板故障
					to_fsm->value = pt_driverBoard[cur_dmsg->id]->state;
					if(send_fsm_msg(to_fsm) != TRUE)
						release_fsm_msg(to_fsm);
					
					
					//发送复位指令（驱动板增加一条指令，指针指向0，直接复位整个程序）
					drv_cmd_checkTemp(0,0);
					
				}
			}
			else//驱动板是空指针
			{
				//报致命错误
				err = err;
			}
		}
		else//没有命令要执行
		{
			drv_cmd_checkVER(0);//查驱动板1版本号
		}
		release_drv_msg(cur_dmsg);//释放驱动板消息内存
		
		
		/*
		控制策略
		*/
		if((OSTimeGet() - policyTimeout > OS_TICKS_PER_SEC*10) || (OSTimeGet() < policyTimeout))//如果超过10s，运行一次回路控制策略
		{
			drv_export_policy();
			policyTimeout = OSTimeGet();//更新时间
			
			if(OSTimeGet() - tempReportTimeout >= OS_TICKS_PER_SEC*60*10)//10分钟发一条温度信息
			{
				tempReportTimeout = OSTimeGet();
				for(i = 0; i < g_vm.maxContainerNum; i++)
				{
					if(pt_driverBoard[i] != 0)
					{
						report_temp_value(i, pt_driverBoard[i]->temp.state[0], pt_driverBoard[i]->temp.value[0]);
					}
				}
			}
		}
	}
}

