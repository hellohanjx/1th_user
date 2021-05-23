/*
通讯结构

*/
#include "header.h" 
#include "com_fram.h"
#include "flash_struct.h"

#define PACK_MAX_SIZE 256	//最大数据长度

/*
服务器应答结果
*/
#define ACK_OK     0X30 //确认应答
#define ACK_CHK    0X31 //校验和错误
#define ACK_NA     0X32 //无应答
#define ACK_REJECT 0X33 //拒绝执行
#define ACK_BUSY   0X34 //系统忙
#define ACK_START  0X35 //动作开始
#define ACK_END    0X36 //动作执行完毕



#define COM_QUEEN_SIZE  20	//通讯队列大小

OS_EVENT *commucationQueen;
static void *pt_comucationQueen[COM_QUEEN_SIZE];//队列指针，数据放入队列时就是把这个指针指向队列
OS_EVENT * sem_com_recv;
OS_EVENT * flash_lock;
/*
数据返回回调函数
*/
static void common_recv_dealwith(uint8_t *dat, uint16_t len)
{
	if(dat[2] == len)//接收长度符合
	{
		uint8_t i,chk;
		for(i = 0,chk = 0;i < (len-1);i++)
			chk += dat[i];
		if(chk == dat[len-1])
		{
			for(i = 0,chk = 0;i < len;i++);
			OSSemPost(sem_com_recv); 
		}
	}
}







/*
申请空间用来发数据
*/
MAIL* Apply_for_mail(uint16_t size)
{
	uint8_t err;
	MAIL *mail = 0; 
	
	mail = (MAIL*)_malloc((sizeof(MAIL)), &err);
	log_output(mail);
	if(mail != 0)
	{
//		mail->com_call_back = (COM_RECV_CALLBACK)mail;
//		mail->addr = (uint8_t*)(((uint8_t*)mail + sizeof(COM_RECV_CALLBACK)));
		mail->addr = _malloc(size, &err);
	}
	return mail;
}

/*
释放数据空间
*/
static void  release_commucation_mail(MAIL* mail)
{
	_free(mail->addr);
	_free(mail);
}


/*
信息插入队列尾
*/
void send_mail(MAIL* mymail)
{
	log_output(mymail);
	if (mymail != NULL)
	{
		if(OSQPost(commucationQueen,mymail) != OS_NO_ERR)
			release_commucation_mail(mymail);
	}
}
/*
信息插入队列头
*/
void send_f_mail(MAIL* mymail)
{
	log_output(mymail);
	if (mymail != NULL)
	{
		if(OSQPostFront(commucationQueen,mymail) != OS_NO_ERR)
			release_commucation_mail(mymail);
	}
}


/*
申请的信封存入flash
*/
void mail_to_flash(MAIL  *cmail)
{
	if(cmail != 0)
//	insert_to_flashqueue(cmail->addr);//存入flash
	save_to_flash(cmail->addr);
	release_commucation_mail(cmail);
}







/*
通讯初始化
*/
static void commucation_init(void)
{
	//PC_COM_INIT();//初始化串口
	commucationQueen = OSQCreate(pt_comucationQueen,COM_QUEEN_SIZE); //创建发送队列
	sem_com_recv = OSSemCreate(0);//收到数据信号量
	flash_lock = OSSemCreate(1);//flash存储信号量

}


/*
组织数据发送
*/
static void get_mail_send(uint8_t *dat)
{
	uint8_t i,j,chk,len;
	len = dat[2];

	if(dat[0] != 0x1f
	&& dat[0] != 0x10
	&& dat[0] != 0x11 
	&& dat[0] != 0x12
	&& dat[0] != 0x14	
	&& dat[0] != 0x17
	&& dat[0] != 0xff)
	{
		uint8_t st[260];
		//错误包
		i = 0;
		//包头
		st[i++] = 0x1c;
		st[i++] = dat[1];//使用原始包序号
		st[i++] = '*';	 //数据包长度
		st[i++] = 0x11;	 //业务类型
		
		//更新待发送数据包;这样更新后才能与错误包的回应相对应
		dat[0] = st[0];//
		dat[1] = st[1];//使用原始包序号
		dat[2] = st[2];//数据包长度
		dat[3] = st[3];//业务类型
		
		
		st[i++] = '*';
		//包体
		for(j = 0;j < (len-1); j++)
		st[i++] = dat[j];
		st[i++] = '*';
		st[i++] = ' ';//校验占位
		st[2] = i;
		//校验
		for(j = 0,chk = 0;j < (i-1);j++)
		chk += st[j];
		st[i-1] = chk;
		commucation_send(st, i, common_recv_dealwith);//串口发送 
	}
	else
	{
		for(i = 0,chk = 0;i < (len-1) ; i++)
		{
			if(dat[i] == 0)
			{
				i = i;
			}
		}
		
		for(i = 0,chk = 0;i < (len-1) ; i++)
		{
			chk += dat[i];
		}
		dat[i++] = chk;
		
		
		commucation_send(dat, i, common_recv_dealwith);//串口发送 
	}
}
/*
设备登录
*/
static void send_linking(uint8_t first,volatile uint32_t *correct)//连机
{
	MAIL *cmail;
	uint8_t *str; 
	uint8_t err,i ;
	
	cmail = Apply_for_mail(256);//申请一个信封
	
	log_output(cmail);
	if(cmail == 0)
		return ;
	
	str = cmail->addr; 
	err = OS_TIMEOUT;

	get_link_info(str);//编写一个连接请求信息
	str[1]=g_vm.linkNum+1; //加上编号
	g_vm.linkNum=(g_vm.linkNum+1)%255;//更新编号
	
	while(OSSemAccept(sem_com_recv));
	get_mail_send(str);
	OSSemPend(sem_com_recv,OS_TICKS_PER_SEC*15,&err);
	
	if(err==OS_NO_ERR)
	{				
		i=0;
		do
		{//防止交错的数据包，终端收到的上一次发送数据的回应
			uint32_t timelast;
			if(i==0)
				timelast=OSTimeGet();
			OSSemPend(sem_com_recv, OS_TICKS_PER_SEC, &err);
			if(OSTimeGet()-timelast>=OS_TICKS_PER_SEC)
			{
				timelast=OSTimeGet();
			}
			i++;				
		}while((!(err==OS_TIMEOUT && commucation_recv_get()[0]==0x1D && commucation_recv_get()[1]==str[1]))&& i<100);//消除多余的应答
		
		if(err==OS_TIMEOUT && commucation_recv_get()[0]==0x1D && commucation_recv_get()[1]==str[1])
		{
			if(commucation_recv_get()[0]==0x1D && commucation_recv_get()[1]==str[1])
			{						
				uint8_t ts,ctime[6];
//				OS_CPU_SR  cpu_sr ;
				ctime[0] = (commucation_recv_get()[3]-'0')*10 + commucation_recv_get()[4]-'0';
				ctime[1] = (commucation_recv_get()[5]-'0')*10 + commucation_recv_get()[6]-'0';
				ctime[2] = (commucation_recv_get()[7]-'0')*10 + commucation_recv_get()[8]-'0';
				ctime[3] = (commucation_recv_get()[9]-'0')*10 + commucation_recv_get()[10]-'0';
				ctime[4] = (commucation_recv_get()[11]-'0')*10 + commucation_recv_get()[12]-'0';
				ctime[5] = (commucation_recv_get()[13]-'0')*10 + commucation_recv_get()[14]-'0';
				
//				CPU_CRITICAL_ENTER();//临界代码处理
				ts  = set_rtc_time(1, ctime[0], ctime[1], ctime[2], ctime[3], ctime[4], ctime[5]);
//				CPU_CRITICAL_EXIT();
				
				if(ts)
				{
					if(first)
					{
						get_channel_status(str);
						str[1]=g_vm.linkNum+1; //加上编号
						g_vm.linkNum=(g_vm.linkNum+1)%255;//更新编号
						
						while(OSSemAccept(sem_com_recv));
						get_mail_send(str); //发送到远程管理端
						OSSemPend(sem_com_recv,OS_TICKS_PER_SEC*15,&err);
						
						if(err==OS_NO_ERR && commucation_recv_get()[0]==0x1D && commucation_recv_get()[1]==str[1])
						{
							get_machine_status(str);
							str[1]=g_vm.linkNum+1; //加上编号
							g_vm.linkNum=(g_vm.linkNum+1)%255;//更新编号
							
							while(OSSemAccept(sem_com_recv));
							get_mail_send(str); //发送到远程管理端。
							OSSemPend(sem_com_recv,OS_TICKS_PER_SEC*15,&err);
							
							if(err==OS_NO_ERR && commucation_recv_get()[0]==0x1D && commucation_recv_get()[1]==str[1])
							{
								get_software_version(str);
								str[1]=g_vm.linkNum+1; //加上编号
								g_vm.linkNum=(g_vm.linkNum+1)%255;//更新编号
								
								while(OSSemAccept(sem_com_recv));
								get_mail_send(str); //发送到远程管理端。
								OSSemPend(sem_com_recv,OS_TICKS_PER_SEC*15,&err);
								
								if(err==OS_NO_ERR && commucation_recv_get()[0]==0x1D && commucation_recv_get()[1]==str[1])
								{
									first=0;
									g_vm.linkState=1;//标记网络状态为连接
									(*correct) = OSTimeGet();
								}
							}
						}
					}
					else
					{
						g_vm.linkState = 1;
						(*correct) = OSTimeGet();
					}
				}
			}
		}
	}
	release_commucation_mail(cmail);
}


/*
发送心跳
*/
static uint8_t send_heart(volatile uint32_t *hearTime, uint8_t *xintiao)//心跳包
{
	uint8_t rs = TRUE ,err ;
	if(OSTimeGet() - (*hearTime) >= OS_TICKS_PER_SEC*40)//40s发送一次心跳包
	{
		(*hearTime) = OSTimeGet();
		xintiao[0]=0xff;
		xintiao[1] = g_vm.linkNum+1;
		xintiao[2]=4;
		g_vm.linkNum = (g_vm.linkNum+1)%255;
		
		while(OSSemAccept(sem_com_recv));
		get_mail_send(xintiao);
		OSSemPend(sem_com_recv,OS_TICKS_PER_SEC*10,&err);
		if(!(err == OS_NO_ERR && commucation_recv_get()[0]==0x1D && commucation_recv_get()[1] == xintiao[1] && commucation_recv_get()[2] == 4))//需要判断心跳回应是否正确
		{
			g_vm.linkState = 0;
			rs = FALSE;
		}
	}
	return rs;
}

/*
发送对时
*/
static uint8_t send_correct_time(volatile uint32_t *correct)//发对时
{
	uint8_t err,rs = TRUE; 
		
	if(OSTimeGet()-(*correct) > OS_TICKS_PER_SEC*60*60)//1h 对时一次
	{
		uint8_t *str;
		MAIL *cmail;

		cmail = Apply_for_mail(50);
		
		log_output(cmail);
		
		if(cmail == 0)
			return FALSE; 
		
		str = cmail->addr;
		
		(*correct) = OSTimeGet();
		get_link_info(str);		//编写对时包
		str[1]=g_vm.linkNum+1; 		//加上编号
		g_vm.linkNum=(g_vm.linkNum+1)%255;//更新编号
		
		while(OSSemAccept(sem_com_recv));
		get_mail_send(str);
		OSSemPend(sem_com_recv,OS_TICKS_PER_SEC*30,&err);//30s 超时
		
		if(err==OS_NO_ERR)
		{
			if(commucation_recv_get()[0] == 0x1D && commucation_recv_get()[1] == str[1])
			{
				uint8_t ctime[6];
//				OS_CPU_SR  cpu_sr ;
				ctime[0] = (commucation_recv_get()[3]-'0')*10 + commucation_recv_get()[4]-'0';
				ctime[1] = (commucation_recv_get()[5]-'0')*10 + commucation_recv_get()[6]-'0';
				ctime[2] = (commucation_recv_get()[7]-'0')*10 + commucation_recv_get()[8]-'0';
				ctime[3] = (commucation_recv_get()[9]-'0')*10 + commucation_recv_get()[10]-'0';
				ctime[4] = (commucation_recv_get()[11]-'0')*10 + commucation_recv_get()[12]-'0';
				ctime[5] = (commucation_recv_get()[13]-'0')*10 + commucation_recv_get()[14]-'0';
				//更新时间
//				CPU_CRITICAL_ENTER();
				set_rtc_time(1, ctime[0], ctime[1], ctime[2], ctime[3], ctime[4], ctime[5]);
//				CPU_CRITICAL_EXIT();
			}
		}
		else
		{	
			g_vm.linkState = 0;
			rs = FALSE;
		}
		release_commucation_mail(cmail);//释放掉该信封
	}
	return rs;
}

/*
发送内存中的数据
*/
static uint8_t send_ram_data(void)//发送内存数据
{
	uint8_t rs,err;
	MAIL * cmail;
	
	cmail = (MAIL*)OSQPend(commucationQueen,OS_TICKS_PER_SEC,&err);
	if(err == OS_NO_ERR)
	{
		if(g_vm.linkState == 1)
		{
			rs = TRUE;
			cmail->addr[1] = g_vm.linkNum+1;//发送流水号+1
			g_vm.linkNum = (g_vm.linkNum+1)%255;
			
			while(OSSemAccept(sem_com_recv));	//清除信号量
			get_mail_send(cmail->addr);			//发送数据
			OSSemPend(sem_com_recv,OS_TICKS_PER_SEC*15,&err);
			if(err == OS_NO_ERR)
			{
				if(cmail->com_call_back != 0)//如果注册了回调函数，则调用
				{
					if(!(cmail->com_call_back(commucation_recv_get(), commucation_rxlen_get(), cmail->addr))) //回调函数处理
						g_vm.linkState = 0;//直接踹掉线，重新发起连接
				}
				else//未注册回调函数（用于那些普通回复：表示收到或者什么的）
				{
					if(commucation_recv_get()[0] == 0x1D && commucation_recv_get()[1] == cmail->addr[1] && commucation_recv_get()[3] == ACK_OK)
					{
						//应答是正确的
					}
					else
					{
						g_vm.linkState = 0;//直接开始重新连接
					}
				}
				
			}
			else//无应答掉线
			{
				g_vm.linkState = 0;
			}
		}
		release_commucation_mail(cmail);//对于这种即时数据只发送一次立刻删除
	}
	else//没有数据了
	{
		rs = FALSE;
	}	
	return rs;
}


/*
发送flash中的数据
*/
static void send_flash_data(void)//发送flash中的数据
{
//	uint32_t yy;
//	yy = mailnum_get();
//	yy = mailusedsec_get();
//	yy = flashqueue_rear_get();
//	yy = flashqueue_head_get();
	
	if(mailnum_get() != 0)
	{
		MAIL *cmail;
		uint8_t *str, err;
		uint32_t offset;
		cmail = Apply_for_mail(PACK_MAX_SIZE);
		
		log_output(cmail);
		if(cmail == 0)
			return ;
		
		str = cmail->addr;			//数据读入地址
		if(load_from_flash(str, &offset) != TRUE)
		{
			err = err;
		}
//		read_from_flashqueue(str);	//从FLASH中取出一条信息
		if(mailcount_get() == 0)
			mailcount_set(1);
		str[1] = mailcount_get()+1; 		//加上编号,这个编号只有发送成功才+1
							
		while(OSSemAccept(sem_com_recv));
		get_mail_send(str);
		OSSemPend(sem_com_recv,OS_TICKS_PER_SEC*20,&err);
		
		if(err==OS_NO_ERR)//如果得到应答
		{
			if(commucation_recv_get()[0]==0x1D && commucation_recv_get()[1]==str[1] && commucation_recv_get()[3] == ACK_OK)
			{
				mailcount_set((mailcount_get()+1)%255);
				delet_commucation_data(offset);
//				delete_flashqueue_head(str);//删除掉该信息
			}
			else//不对也要强制掉线
			{
				g_vm.linkState = 0;
			}
		}
		else//如果超时则认为断网
		{
			g_vm.linkState = 0;
		}
		release_commucation_mail(cmail);//释放掉该信封
	}
}
/*
通讯主任务
*/
void commucation_task(void)
{
	volatile uint32_t heart,correct;
	volatile uint8_t firstLink;
	uint8_t xintiao[6];
	
	heart = correct = OSTimeGet();
	
	g_vm.linkState = 0;
	g_vm.linkNum = 0;
	
	xintiao[0]=0xff;
	xintiao[2]=4;
	firstLink = TRUE;
	
	commucation_init();
	
	while(check_w_power_on < 4)//等待启动完成
	OSTimeDly(OS_TICKS_PER_SEC);//等待所有设备初始化完成
	
	while(1)
	{
		//查内存是否有数据要即时发送
		if(!send_ram_data())//没有即时数据需要发送
		{
			if(g_vm.linkState == 1 )//如果连网中
			{
				firstLink = FALSE;
				if(send_heart(&heart,xintiao))//发心跳
				if(send_correct_time(&correct))//发对时
					send_flash_data();//发flash数据
			}
			else
			{
				send_linking(firstLink,&correct);//联机
			}
		}
	}
}


