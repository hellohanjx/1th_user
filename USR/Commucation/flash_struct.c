/*
spi flash 
数据处理结构
*/

#include "header.h"
#include "flash_struct.h" 
#include "SST25VF016BDrv.h"
#include "stm32_lib.h"


/*************************************************************************/
//三层结构
//1.封包(添加包头包尾)
//2.分包(分成若干小包，每个包带校验)
//3.写入数据

//包结构:
//包头(2BYTE)+包长度(2BYTE,包总长度)+包体+校验和(1BYTE,包体校验和)+包尾(2BYTE)
//0xAAAA:包头(包体中遇到 AA 后面补零)
//0xBBBB:包尾(包体中遇到 BB 后面补零)
//块结构:块状态+块内容(拆分包)+块校验
/*************************************************************************/
#define BLOCK_SIZE			32	//块大小
#define BLOCK_DATA_SIZE 	(BLOCK_SIZE-2)	//块有效字节数
#define BLOCK_DATA_OFFSET	1	//块内容偏移地址
#define BLOCK_CHECK_SIZE	1	//块校验长度
#define BLOCK_STATE_SIZE	1	//块状态长度
#define PACKAGE_LSB_OFFSET	4	//包体起始偏低地址

#define INIT_BLOCK		0xFF	//初始块
#define WRITTEN_BLOCK	0xFE	//已写块
#define RUNNING_BLOCK	0xFC	//正读块(提供数据恢复使用)
#define READ_BLOCK		0xF8	//已读块
#define END_BLOCK		0xF0	//结束块(数据包存储不完整时.一般flash满时出现)
#define ERR_BLOCK		0x00	//损坏块

#define RESTART() ((void (*)())(0x0)) ()	//重启cpu
	
static volatile uint8_t flash_dat[520];//存储包数组，如果采用全局变量的话需要锁住存储过程;如果用局部变量则任务堆栈需要加大
extern OS_EVENT *flash_lock;
static void save_blc_data(volatile uint8_t *str,volatile uint16_t len);
static void check_blc_data(volatile uint8_t *str,volatile uint16_t len);
static void load_blc_data(volatile uint8_t *str,volatile uint32_t Raddr);
static void flag_blc_data(volatile uint8_t tmp,volatile uint8_t type);
static void flag_all_rblc(volatile uint8_t tmp,volatile uint32_t taddr);
static void flag_blc_end(volatile uint8_t tmp);
static uint8_t request_blc_status(void);
static uint8_t find_use_blc(volatile uint32_t *Raddr);

uint32_t r1,r2,r3,r4,r5,r6;//测试用



/*
分包
数据分包存储
*/
static void resolve_to_package(volatile uint8_t *pacdat)
{
	volatile uint8_t i,rs,chk,lenh,pac[BLOCK_SIZE];
	volatile uint16_t num;//包数据累加计数
	volatile uint16_t len;//包长度
	volatile uint16_t last_num;
	num = 0;
	len = pacdat[2]<<8 | pacdat[3];
	
	do{
		if(len > BLOCK_DATA_SIZE)//一个块其实最多能存30BYTE.块状态1BYTE,块校验1BYTE
			lenh = BLOCK_DATA_SIZE;
		else
			lenh=len;
		rs = request_blc_status();//申请块
		if(rs == 3)
		{//FLASH满
			if(len != (pacdat[2]<<8 | pacdat[3]) && len!=0)//没写完数据就空了
			{
				flag_blc_end(END_BLOCK);//标记为结束
			}
						len = len;

//			DisplayNUM(flashqueue_rear,4,0,4,0);
//			DisplayLT("FALSH IS FULL 1 ",6,0,0);
			return;
		}
		else
		if(rs==FALSE)//未找到可用块
		{
						len = len;

//			DisplayLT("FLASH IS ERR 1  ",6,0,0);
		}
		else
		if(rs == TRUE)//FLASH有空间
		{
			last_num = num;//记录累计包数据
			rs = FALSE;
			pac[0]=WRITTEN_BLOCK;//修改为已用块
			for(i = 0, chk = pac[0];i < lenh; i++)//截取30字节
			{
				pac[i+1] = pacdat[num++];
				chk += pac[i+1];//校验和是从块状态开始计算
			}
			pac[++i]=chk;
			save_blc_data(pac,lenh + 2);//写入(包括块状态与块校验)
			//清除数据，防止脏数据
			pac[0] = 0;
			pac[1] = 0;
			pac[2] = 0;
			pac[3] = 0;
	  		check_blc_data(pac,lenh + 2);//读出(包括块状态与块校验)
			if(pac[0]==WRITTEN_BLOCK)//写块标记成功
			{
				for(i=0,chk=0;i<(lenh+1);i++)//计算块校验
				{
					chk+=pac[i];
				}
		  		if(chk!=pac[lenh+1])//块校验错
				{
		  			num=last_num;//恢复包累计数据
			  		flag_blc_data(ERR_BLOCK,1);//标记为坏块................
		  		}
		  		else//写块成功
				{
		  			rs=TRUE;
			  		len-=lenh;//剩余包长度
		  		}
		  	}
		  	else
			{
		  		num=last_num;//此次数据写入失败,恢复包序号
		  		flag_blc_data(ERR_BLOCK,1);//标记为坏块................
		  	}
	  	}
	}while(len>0 || rs!=TRUE);
	
	if(rs==TRUE)
	{
		mailnum_set(mailnum_get() + 1);
	}
}


/*
组包
从falsh中读取数据
将一条数据的每个数据块拼成完整包
*/
static uint8_t integrate_package(volatile uint8_t *pacdat,volatile uint32_t *used_lenth)
{
	volatile uint8_t i,chk,lenh,rs,find = FALSE;
	volatile uint8_t pac[BLOCK_SIZE];
	volatile uint16_t len,num=0;
	volatile uint32_t Raddr,tmp,count=0;
	Raddr = flashqueue_head_get();
	
	do
	{
		tmp = Raddr;
		rs = find_use_blc(&Raddr);//找到有效数据
		if(rs == TRUE)
		{
			load_blc_data(pac,tmp);//读取整块
			if((pac[1]==0xAA && pac[2]==0xAA)||find==TRUE)//是包头
			{
				if(find!=TRUE)
				{
					len=pac[3]<<8 | pac[4];//整包长度
				}
				if((pac[1] == 0xAA && pac[2] == 0xAA) && find == TRUE)//又找到一个包头
				{
					find = FALSE;
					num = 0;//重新拼包
					len = pac[3]<<8 | pac[4];;//重定义整包长度
				}
				if(len>BLOCK_DATA_SIZE)
				{
					lenh=BLOCK_DATA_SIZE;
				}
				else
				{
					lenh=len;
				}
				
				for(i=0,chk=0;i<(lenh+1);i++)
				{
					chk+=pac[i];
				}
				
				if(chk==pac[lenh+1] || pac[0]==RUNNING_BLOCK)//如果首块校验正确或者首块是正在读块(正在读块改写了块头，此时校验是错误的)
				{
					if(pac[2] == 0xAA && pac[1] == 0xAA)
					{
						find=TRUE;//已找到包头且包头验证通过
					}
					for(i=0;i<lenh;i++)
					{
						pacdat[num++]=pac[i+1];
					}
					len-=lenh;//剩余未读包长度
				}
			}
		}
		else
		if(rs==0x02)
		{
//			DisplayLT("FALSH IS EMPTY  ",6,0,0);
			len = len;
			return FLASH_EMPTY;
		}
		else
		if(rs==0x04)//发现不完整的数据(存满的时候未能存储完整的1条数据)
		{
//			DisplayLT("  FALSH IS END  ",6,0,0);
			len = len;
		}
	}while((len > 0 || rs != TRUE) && (count++) < FLASH_QUENE_SPACE/BLOCK_SIZE );
	
	if(rs != TRUE)
	{
		return FALSE;
	}
	(*used_lenth) = Raddr;		//取数据完成后的地址.返回给参数
	flag_blc_data(RUNNING_BLOCK, 0);	//标记为正在读
	return TRUE;
}

/*
存储数据，对外接口
参数：str：数据指针
*/
void save_to_flash(volatile uint8_t* str){
	uint8_t err;
	volatile uint8_t chk;//最极端的情况都是AABB
	volatile uint16_t i,j,len;
	
	OSSemPend(flash_lock, 0, &err);//锁住存储过程
	
	len=str[2];
	i=0;
	flash_dat[i++]=0xAA;
	flash_dat[i++]=0xAA;
	flash_dat[i++]='*';//长度高位
	flash_dat[i++]='*';//长度低位
	
	for(j=0,chk=0;j<len;j++)
	{
		flash_dat[i++]=str[j];
		chk+=str[j];
		if(str[j]==0xAA || str[j]==0xBB){
			flash_dat[i++]=0;//AA后补零
		}
	}
	
	if(chk==0xAA || chk==0xBB)
		flash_dat[i++]=0;
	flash_dat[i++]=chk;
	flash_dat[i++]=0xBB;
	flash_dat[i++]=0xBB;
	flash_dat[2]=i>>8;
	flash_dat[3]=i;
	resolve_to_package(flash_dat);
	OSSemPost(flash_lock);//释放信号量
}

/*
功能：读取存储数据
*/
uint8_t load_from_flash(volatile uint8_t *str,volatile uint32_t *used_lenth)
{
	uint8_t err;
	volatile uint8_t chk,rs;
	volatile uint16_t i,j,len;//最极端的情况都是AABB
	
	OSSemPend(flash_lock,0,&err);//锁住存储过程
	rs = integrate_package(flash_dat,used_lenth);
	if(rs!=TRUE)//从flash读数据包
	{
		if(rs != 0x02){
			err = err;
//			lcd_show_string("FALSH IS ERR 2  ",6,0,0);
		}
		OSSemPost(flash_lock);//释放信号量
		return FALSE;
	}
	len = flash_dat[2]<<8|flash_dat[3];//包长度
	for(i=0,j=0,chk=0;i<(len-7);i++)//从第5个字节开始计算(跳过包头与包长)
	{
		chk += flash_dat[i+PACKAGE_LSB_OFFSET];//校验计算必须放在前面
		if(flash_dat[i+PACKAGE_LSB_OFFSET]==0xAA || flash_dat[i+PACKAGE_LSB_OFFSET]==0xBB)//跳过AA或BB后面的0
		{
			str[j++] = flash_dat[i+PACKAGE_LSB_OFFSET];
			i++;
		}
		else
			str[j++] = flash_dat[i+PACKAGE_LSB_OFFSET];
		
	}
	if(chk==flash_dat[len-3]){// && j==str[2])//数据长度与校验和
		OSSemPost(flash_lock);//释放信号量
		return TRUE;
	}
//	DisplayLT("FALSH IS ERR 3  ",6,0,0);
	OSSemPost(flash_lock);//释放信号量
	return FALSE;
}

void delet_commucation_data(volatile uint32_t Raddr)//删除数据 
{
	volatile uint32_t rr1,rr2;
	flag_all_rblc(READ_BLOCK, Raddr);//标记为已读
	rr1 = flashqueue_head_get() / __F032B_SEC_SIZE;
	rr2 = Raddr / __F032B_SEC_SIZE;
	
	if(rr1 != rr2)//跨扇区
	{
		if(rr2 > rr1)
		{
			sst26vf032b_erase(rr1, rr2-1);
			mailusedsec_set( mailusedsec_get() - (rr2 - rr1) );
		}
		else
		{
			sst26vf032b_erase(rr1,FLASH_QUENE_SPACE/__F032B_SEC_SIZE-1);
			if(rr2 > 0)
			{
				sst26vf032b_erase(0, rr2 - 1);
			}
			mailusedsec_set( mailusedsec_get() - (FLASH_QUENE_SPACE/__F032B_SEC_SIZE - rr1) );
			mailusedsec_set( mailusedsec_get() - rr2 );
		}
	}

	flashqueue_head_set(Raddr % FLASH_QUENE_SPACE);	//更新队列头部
	
	if(mailnum_get() > 0)
	{
		mailnum_set( mailnum_get() - 1);		//更新队列中信息数目
	}
}

/*
申请块存储块
*/
static uint8_t request_blc_status(void)
{
	volatile uint8_t status;
	if(mailusedsec_get() == 0)//如果存储空间没有任何字节则将已使用块数+1;
	{
		mailusedsec_set(1);
	}
	if( (flashqueue_rear_get() / __F032B_SEC_SIZE) != ((flashqueue_rear_get() + BLOCK_SIZE) / __F032B_SEC_SIZE))//检查待存入信息是否超出了某一存储块的边界
	{
	  	if(mailusedsec_get() < FLASH_QUEUE_SECTOR_SIZE)//如果还有未使用的存储块
		{
	  		mailusedsec_set(mailusedsec_get() + 1); //已使用块数加1
	  	}
	  	else
		{
//			tt = mailusedsec_get();
	  		//flag_blc_data(END_BLOCK,1);//标记为结束................(用来恢复数据)
			return FLASH_FULL;
	  	}
	}
	
	flashqueue_rear_set(flashqueue_rear_get() % FLASH_QUENE_SPACE);
	sst26vf032b_read(flashqueue_rear_get(), 1, &status);
	if(status == INIT_BLOCK)//初始块
	{
		return TRUE;
	}
	else//非初始块
	{
		flashqueue_rear_set(flashqueue_rear_get() + BLOCK_SIZE);
		flashqueue_rear_set(flashqueue_rear_get() % FLASH_QUENE_SPACE);
		return FALSE;
	}
}

/*
写数据块
*/
static void save_blc_data(volatile uint8_t *str,volatile uint16_t len)
{
	flashqueue_rear_set( flashqueue_rear_get() % FLASH_QUENE_SPACE );
	sst26vf032b_write(flashqueue_rear_get(), str, len);
	flashqueue_rear_set(flashqueue_rear_get() + BLOCK_SIZE);
	flashqueue_rear_set(flashqueue_rear_get() % FLASH_QUENE_SPACE);
}

/*
对比数据块
*/
static void check_blc_data(volatile uint8_t *str,volatile uint16_t len){//对比块
	volatile uint32_t tmp;
	if(flashqueue_rear_get() == 0)
		tmp = FLASH_QUENE_SPACE - BLOCK_SIZE;
	else
		tmp = flashqueue_rear_get() - BLOCK_SIZE;
  	sst26vf032b_read(tmp, len, str);
}

/*
标记块状态
*/
static void flag_blc_data(volatile uint8_t tmp,volatile uint8_t type)
{
	if(type==1)//写
	{
		uint8_t tt,count = 0;
		
		do
		{
			uint32_t taddr;
			tt = 0xFF;
			if(flashqueue_rear_get() == 0)
			{
				taddr = FLASH_QUENE_SPACE - BLOCK_SIZE;
			}
			else
			{
				taddr = flashqueue_rear_get() - BLOCK_SIZE;
			}
			r5 = taddr;
			sst26vf032b_write(taddr, &tmp, 1);
			sst26vf032b_read(taddr, 1, &tt);
		}while(tt != tmp && (count++) <0xFF);
		
		if(count==0xFF)
		{
//			DisplayLT("   FATAL ERR    ",6,0,0);
		}
	}
	else
	if(type==0)//读
	{
		sst26vf032b_write(flashqueue_head_get(), &tmp, 1);
	}
}

/*
标记结束块
*/
static void flag_blc_end(volatile uint8_t tmp)
{
	sst26vf032b_write(flashqueue_rear_get(), &tmp, 1);
}

/*
//标记整条数据所有块
*/
static void flag_all_rblc(volatile uint8_t tmp,volatile uint32_t taddr)
{
	volatile uint32_t i;
	for(i = flashqueue_head_get(); i < taddr; i += BLOCK_SIZE)
	{
		i %= FLASH_QUENE_SPACE;
		sst26vf032b_write(i,&tmp,1);
	}
}

/*
找有效数据块
*/
static uint8_t find_use_blc(volatile uint32_t *Raddr)
{
	volatile uint8_t status;
	if(mailnum_get() == 0)
	{
		return FLASH_EMPTY;
	}
	(*Raddr) %= FLASH_QUENE_SPACE;
	sst26vf032b_read(*Raddr,1,&status);
	(*Raddr) += BLOCK_SIZE;
	(*Raddr) %= FLASH_QUENE_SPACE;

	if(status == WRITTEN_BLOCK || status == RUNNING_BLOCK)//已写块与正在读块都认为是正常的
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


/*
读取整块
*/
static void load_blc_data(volatile uint8_t *str,volatile uint32_t Raddr)
{
	Raddr %= FLASH_QUENE_SPACE;
	sst26vf032b_read(Raddr, BLOCK_SIZE, str);
}




//==============================================
//以下函数用来恢复数据
//==============================================
/*
寻找状态为正在读的块
*/
static uint8_t find_rblc(volatile uint32_t *rding)
{
	volatile uint8_t status;
	volatile uint32_t i,bigaddr=0,smalladdr=0;
	for(bigaddr=0;bigaddr<FLASH_QUENE_SPACE;bigaddr+=__F032B_SEC_SIZE)//按照扇区寻找
	{
		status = 0;//初始化状态
		sst26vf032b_read(bigaddr, 1, &status);//读扇区第1块状态
		
		if(status!=INIT_BLOCK)//已使用
		{
			smalladdr = bigaddr;
			for(i = 0; i < __F032B_SEC_SIZE; i += BLOCK_SIZE)//在扇区里寻找
			{
				status = 0;//初始化状态
				sst26vf032b_read(smalladdr+i, 1, &status);//读取本扇区块状态
				if(status == RUNNING_BLOCK)//如果发现正读块
				{
					(*rding) = smalladdr+i;//正在读的数据地址
					(*rding) %= FLASH_QUENE_SPACE;
//					DisplayNUM(*rding,0,8,6,0);//显示块地址
					return TRUE;
				}
				else
				if(status==READ_BLOCK)//如果是已读块
				{
					(*rding) =  smalladdr+i;//更新读块地址
					(*rding) %= FLASH_QUENE_SPACE;
					(*rding) += BLOCK_SIZE;
					(*rding) %= FLASH_QUENE_SPACE;
				}
			}
		}
	}
//	DisplayNUM(*rding,0,8,6,0);//显示块地址
	return FALSE;
}

/*
寻找可写块起始地址
*/
static uint8_t find_wblc(volatile uint32_t rding,volatile uint32_t *wstart)
{
	volatile uint8_t status,flag=0;
	volatile uint32_t i,j,bigaddr=0,smalladdr=0;
	bigaddr=rding/__F032B_SEC_SIZE;//扇区号
	bigaddr*=__F032B_SEC_SIZE;//地址
	for(i=0;i<FLASH_QUENE_SPACE;bigaddr+=__F032B_SEC_SIZE,i+=__F032B_SEC_SIZE){//按照扇区寻找
		status=0;//初始化状态
		bigaddr%=FLASH_QUENE_SPACE;
		sst26vf032b_read(bigaddr,1,&status);//读扇区第1块状态
		if(status!=INIT_BLOCK){//已使用
			smalladdr=bigaddr;
			if(rding!=0 && flag==0){
				j=rding-bigaddr;//扇区里的相对地址
				flag=1;
			} 
			else
				j=0;
			for(;j < __F032B_SEC_SIZE; j += BLOCK_SIZE){//在扇区里寻找
				
				//显示扫描过的块
				lcd_show_num(smalladdr+j, pt_word.warring[28].valx, pt_word.warring[28].valy, pt_word.warring[28].color, pt_word.warring[28].background, *(pt_word.warring[28].font), 0, 10);
				
				status = 0;//初始化状态
				sst26vf032b_read(smalladdr + j, 1, &status);//读取本扇区块状态
				if(status==INIT_BLOCK){//如果发现正读块
					(*wstart)=smalladdr+j;//正在读的数据地址
					(*wstart)%=FLASH_QUENE_SPACE;
					return TRUE;
				}
			}
		}
		else
		if(status == INIT_BLOCK)
		{//未使用块
			(*wstart)=bigaddr;
			(*wstart)%=FLASH_QUENE_SPACE;
			return TRUE;
		}
	}
	return FALSE;
}

/*
计算存储器中积压的有效数据量 ，条数
*/
static void check_mailnum(uint32_t startaddr,uint32_t endaddr,uint32_t lenh)
{
	volatile uint32_t i,begin;
	volatile uint8_t status[5],pac[BLOCK_SIZE + 1];
	for(i=0,begin=startaddr;i<=lenh;i+=BLOCK_SIZE,begin+=BLOCK_SIZE)
	{
		begin%=FLASH_QUENE_SPACE;
		status[0]=0;status[1]=0;status[2]=0;status[3]=0;status[4]=0;
		if(begin == r5)
			status[0] = 0;
		sst26vf032b_read(begin,5,status);
		if(status[0] == WRITTEN_BLOCK || status[0] == RUNNING_BLOCK)//如果是正在读或者已写未读块
		{
			if(status[1] == 0xAA && status[2] == 0xAA)//如果是包头
			{
				mailnum_set(mailnum_get() + 1);
//				DisplayNUM(mailnum_get(),4,0,4,0);
			}
		}
	}
	
	pac[BLOCK_SIZE] = 0;
	if(endaddr!=0)
	{
		begin = endaddr - BLOCK_SIZE;
		begin %= FLASH_QUENE_SPACE;
	}
	else
	{
		begin = FLASH_QUENE_SPACE - BLOCK_SIZE;
		begin %= FLASH_QUENE_SPACE;
	}
	
	sst26vf032b_read(begin, BLOCK_SIZE, pac);//检查最后一个块是否是包的结束块
	if(pac[0] == WRITTEN_BLOCK || pac[0] == RUNNING_BLOCK){
		for(i = 0; i < BLOCK_SIZE; i++)
		{
			if(pac[i]==0xBB && pac[i+1]==0xBB)
			{
//				DisplayLT("HELLO",6,0,0);
				return;
			}
		}
	}
	if(mailnum_get() > 0)
	{
		mailnum_set(mailnum_get() - 1);
	}
//	DisplayNUM(mailnum_get(),6,0,4,0);
}


/*
功能：电池信息丢失后恢复flash数据
*/
void flash_recovery(void)
{
	volatile uint32_t head=0,rear=0;
	r1 = flashqueue_head_get();//备份
	r2 = flashqueue_rear_get();
	r3 = mailnum_get();
	r4 = mailusedsec_get();
	
	lcd_show_character(&pt_word.warring[28]);//"recovery" 开始恢复数据
	
	mailnum_set(0);
	mailusedsec_set(0); 
	find_rblc(&head);//寻找正在读的地址
	find_wblc(head,&rear);//寻找可写块起始地址
	flashqueue_head_set(head); 
	flashqueue_rear_set(rear); 
	if(rear>=head)//写地址大于读地址
	{
		mailusedsec_set( rear / __F032B_SEC_SIZE-head / __F032B_SEC_SIZE + 1);
		check_mailnum(head, rear, rear-head);
	}
	else//写地址小于读地址
	{
		mailusedsec_set(FLASH_QUENE_SPACE/__F032B_SEC_SIZE-head/__F032B_SEC_SIZE+rear/__F032B_SEC_SIZE+1);
		check_mailnum(head,rear,FLASH_QUENE_SPACE-(head-rear));
	}
	flashqueue_head_set(head);
	flashqueue_rear_set(rear);
	
	//显示恢复数据条数
	lcd_show_character(&pt_word.warring[29]);//"recovery" 开始恢复数据
	lcd_show_num(mailnum_get(), pt_word.warring[29].valx, pt_word.warring[29].valy, pt_word.warring[29].color, pt_word.warring[29].background, *(pt_word.warring[29].font), 0, 10);
}

/*
功能：读取flash参数并判断flash类型，进行擦除
*/
uint8_t flash_poweron_init(void)
{
	uint8_t rs = TRUE;
	uint8_t tmp1[10],tmp2[10],flag[4]={0,0,0,0};
	uint8_t flash_techn,flash_interface,flash_capacity;
	uint16_t i;
	uint32_t id,flash_last_addr;
	
	sst26vf032b_Jedec_ID_Read(&id);	//读取flash硬件信息
	flash_techn=id>>16;		//制造工艺
	flash_interface=id>>8;	//接口类型
	flash_capacity=id;		//容量
	
	if(flash_techn != 0xBF)
	{
		lcd_show_character(&pt_word.warring[26]);
	}
	
	if(flash_interface != 0x26)
	{
		lcd_show_character(&pt_word.warring[27]);
	}
	
	if(flash_capacity == 0x41)
	{
//		DisplayLT("  FLASH IS 2M  ",6,0,0);
		flash_last_addr = 0x1FFFFF;
		if(flash_last_addr<FLASH_USERFLAG_ADDR)
			RESTART();
	}
	else
	if(flash_capacity == 0x42)
	{
//		DisplayLT("  FLASH IS 4M  ",6,0,0);
		flash_last_addr = 0x3FFFFF;
		if(flash_last_addr < FLASH_USERFLAG_ADDR)
			RESTART();
	}
	
	for(i = 0;i < 10; i++)//flash读写测试
	{
		tmp1[i] = i;
	}
	sst26vf032b_erase(FLASH_LAST_SECTOR-1, FLASH_LAST_SECTOR-1);//用flash倒数第2扇区做读写测试
	sst26vf032b_write(flash_last_addr-0x1000,tmp1,10);
	sst26vf032b_read(flash_last_addr-0x1000,10,tmp2);
	sst26vf032b_erase(FLASH_LAST_SECTOR-1, FLASH_LAST_SECTOR-1);
	for(i=0;i<10;i++)
	{
		if(tmp1[i]!=tmp2[i])
		{
			BEEP(ON);
			OSTimeDly(OS_TICKS_PER_SEC);
			BEEP(OFF);
			OSTimeDly(OS_TICKS_PER_SEC);
		}
	}
	
	//检查flash标志位
	sst26vf032b_read(FLASH_USERFLAG_ADDR, 4, flag);
	id = (flag[0]<<24) | (flag[1]<<16) | (flag[2]<<8) | flag[3];
	if(id != FLASH_USER_FLAG)//标志位不符合
	{
		flag[0] = (FLASH_USER_FLAG >> 24) & 0XFF;
		flag[1] = (FLASH_USER_FLAG >> 16) & 0XFF;
		flag[2] = (FLASH_USER_FLAG >> 8) & 0XFF;
		flag[3] = FLASH_USER_FLAG & 0XFF;
		sst26vf032b_erase(0,FLASH_LAST_SECTOR);//重新格式化数据
		sst26vf032b_write(FLASH_USERFLAG_ADDR,flag, 4);
		mailnum_set(0);
		mailusedsec_set(0); 
		flashqueue_rear_set(0);
		flashqueue_head_set(0);
		lcd_show_character(&pt_word.warring[25]);//"flash reset"
		rs = FALSE;
		OSTimeDly(OS_TICKS_PER_SEC*2);
	}
	return rs;
}

/*
判断是否可以继续存储信息
*/
uint8_t use_size(void)
{//是否可以继续存储信息
	if(mailusedsec_get() < (FLASH_QUEUE_SECTOR_SIZE - 1) )
		return TRUE;
	else
		return FALSE;
}

