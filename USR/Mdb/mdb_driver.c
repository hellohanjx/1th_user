/*
MDB设备驱动
占用 UART4 串口，使用DMA收发
*/
#include "header.h"
#include "stm32_lib.h"
#include "mdb_driver.h" 

#define MDB_RX_LEN		37	//MDB接收缓冲长度，最长36Byte

static OS_EVENT* sem_mdb_recvd;	//接收完成信号量

static uint16_t	mdb_tx_buff[MDB_RX_LEN];	//发送缓冲区
static uint16_t	mdb_rx_buff[MDB_RX_LEN]; 	//接收缓冲区（根据协议，MDB一次最多返回36字节数据）
static uint8_t 	mdb_rx_len;					//接收数据长度



/*
串口4配置
*/
static void uart4_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//时钟初始化
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO ,  ENABLE );
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_UART4 , ENABLE );
	
	//引脚初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				//TX 引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			//复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				//RX引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING ;	//浮空输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	USART_DeInit(UART4);	//寄存器恢复默认
	USART_InitStructure.USART_BaudRate            = 9600  ;							//9600波特率
	USART_InitStructure.USART_WordLength          = USART_WordLength_9b;           	//9个数据位
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;              	//1个停止位
	USART_InitStructure.USART_Parity              = USART_Parity_No ;              	//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//禁用硬件流控
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;	//使能发送/接收
	USART_Init(UART4, &USART_InitStructure);
	
	//清除标志位
	USART_ClearFlag(UART4, USART_FLAG_TC);//清发送完标志
	USART_ClearFlag(UART4, USART_IT_IDLE);//清空闲标志
	USART_ClearFlag(UART4, USART_IT_RXNE);//清接收标志
	
	//关中断
	USART_ITConfig(UART4,  USART_IT_TXE, DISABLE); 
	USART_ITConfig(UART4,  USART_IT_TC, DISABLE);
	USART_ITConfig(UART4,  USART_IT_RXNE, DISABLE);
	
//	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE );		//使能接收中断
	USART_ITConfig(UART4, USART_IT_ERR,  ENABLE );		//使能错误中断
	USART_ITConfig(UART4, USART_IT_IDLE, ENABLE );		//使能空闲中断(接收数据后超过一个字节时间没收到数据则触发)
	//串口中断配置
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn; 		    //UART4中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;   //先占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		    //次占优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	USART_Cmd(UART4, ENABLE);
}


/*
DMA2-3初始化
配置为串口4的接收
*/
static void dma2_3_config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;				
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);	//DMA2 时钟打开
	
	DMA_DeInit(DMA2_Channel3); 
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&UART4->DR);		//dr寄存器的地址· (0x40013804)
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)mdb_rx_buff;			//接收缓冲的地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;						//外设作为数据来源
	DMA_InitStructure.DMA_BufferSize = MDB_RX_LEN;							//多设几个长度，防止有错误脉冲过来
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//dma 不自增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//内存自增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//dma数据宽度（半字 16bit）
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;			//内存数据宽度
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;		//普通模式；非循环接收
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;	//DMA高优先级（DMA设备间的优先级）
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;		//关闭内存到内存传输模式
	DMA_Init(DMA2_Channel3, &DMA_InitStructure);
	
	DMA_ClearFlag(DMA2_FLAG_GL1);       // 清除DMA所有标志   
		
	/*
	注意，DMA2-3 并没有开启中断
	*/
	USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE);	//使能串口4的DMA接收
	DMA_Cmd(DMA2_Channel3, DISABLE);				//关闭DMA2-3
		
	//=============================
	//DMA中断设置
	//=============================
//	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel3_IRQn; 		                //中断通道
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;                       //抢占优先级
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		                        //从优先级
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									//中断使能
//	NVIC_Init(&NVIC_InitStructure);

}

/*
DMA2-5初始化
配置为串口4的发送
*/
static void dma2_5_config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;				
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);	//DMA2 时钟打开

	DMA_DeInit(DMA2_Channel5); 
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&UART4->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)mdb_tx_buff;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;				//外设作为数据目的地
	DMA_InitStructure.DMA_BufferSize = MDB_RX_LEN;					//DMA缓存大小,先设定一个，发的时候重设置
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//DMA外设地址不自增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;			//DMA内存地址自增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//外设数据宽度，半字
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;		//内存数据宽度，半字
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;					//发完一次就停
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;				//高优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;					//进制内存到内存
	DMA_Init(DMA2_Channel5, &DMA_InitStructure);					//配置寄存器
	
	DMA_ClearFlag(DMA2_FLAG_GL1);       // 清除DMA所有标志   

	DMA_ITConfig(DMA2_Channel5, DMA_IT_TC, ENABLE);					//发送完中断
	DMA_ITConfig(DMA2_Channel5, DMA_IT_TE, ENABLE);					//传输错误中断
	
	USART_DMACmd(UART4, USART_DMAReq_Tx, ENABLE);	//使能串口DMA发送方式
	DMA_Cmd(DMA2_Channel5, DISABLE);				//关闭DMA2-5
	
	//=============================
	//DMA中断设置
	//=============================
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel4_5_IRQn; 		                //中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;                       //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		                        //从优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									//中断使能
	NVIC_Init(&NVIC_InitStructure);

}

/*
DMA2-3
接收中断
*/
void DMA2_Channel3_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_IT_TC3) == SET)    
	{
		DMA_ClearITPendingBit(DMA2_IT_TC3);
	}
	if(DMA_GetITStatus(DMA2_IT_TE3) == SET)
	{
		DMA_ClearITPendingBit(DMA2_IT_TE3);
	}
	DMA_ClearITPendingBit(DMA2_IT_TC3);
	DMA_ClearITPendingBit(DMA2_IT_TE3);
	DMA_Cmd(DMA2_Channel3, DISABLE);	//关DMA防止处理时有数据过来
	DMA2_Channel3->CNDTR = MDB_RX_LEN;	//重装填
	DMA_Cmd(DMA2_Channel3, ENABLE);		//数据处理完，重开DMA
}


/*
DMA2-5
发送中断
*/
void DMA2_Channel4_5_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_IT_TC5) == SET)    
	{
		DMA_ClearITPendingBit(DMA2_IT_TC5);
	}
	if(DMA_GetITStatus(DMA2_IT_TE5) == SET)
	{
		DMA_ClearITPendingBit(DMA2_IT_TE5);
	}
	DMA_ClearITPendingBit(DMA2_IT_TC5);
	DMA_ClearITPendingBit(DMA2_IT_TE5);
	DMA_Cmd(DMA2_Channel5, DISABLE);//关
//	
	//dma2_3开始接收
//	DMA2_Channel3->CNDTR = RX_LEN;//重设置接收缓冲长度
//	DMA_Cmd(DMA2_Channel3, ENABLE);	//打开接收DMA

}








/*
** 函数名称 ：SendACK
** 函数功能 ：发送ACK应答
** 入口参数 ：str	要发送的字符串的指针
** 出口参数 ：无
*/
static void send_ack(void)
{
	mdb_tx_buff[0] = (ACK & 0xff);
    DMA2_Channel5->CNDTR = 1; 		//设置要发送的字节数目
    DMA_Cmd(DMA2_Channel5, ENABLE);	//开始DMA发送
	
}
/*
** 函数名称 ：SendNAK
** 函数功能 ：发送NCK应答
** 入口参数 ：无
** 出口参数 ：无
*/

static void send_nak(void)
{
	mdb_tx_buff[0] = (NAK & 0xff);
    DMA2_Channel5->CNDTR = 1; 		//设置要发送的字节数目
    DMA_Cmd(DMA2_Channel5, ENABLE);	//开始DMA发送
}
/*
** 函数名称 ：SendRET
** 函数功能 ：发送RET应答,协议中这个就是要求设备重新发送上条指令（注意，有些设备并不支持）
** 入口参数 ：无
** 出口参数 ：无
*/

static void send_ret(void)
{
	mdb_tx_buff[0] = (RET & 0xff);
    DMA2_Channel5->CNDTR = 1; 		//设置要发送的字节数目
    DMA_Cmd(DMA2_Channel5, ENABLE);	//开始DMA发送
}


volatile uint32_t test;

/*
mdb接收处理函数
*/
static uint8_t mdb_rx_handle(uint16_t timeout)
{
	uint8_t err = 0xff,i = 0, chk;
	/*
	这样做超时可能会等待时间长些,但是简单易用
	setup 指令耗时最长，最大200ms
	*/
	OSSemPend(sem_mdb_recvd, 220, &err);//等待接收数据,最多等200ms，每个字节最多5ms
	//这里超时也要关闭接收
	DMA_Cmd(DMA2_Channel3, DISABLE);	//关闭DMA接收
	
//	OSSemPend(sem_mdb_recvd, 20, &err);//第一个字节允许20ms
//	recv = mdb_rx_len;
//	while((err==OS_TIMEOUT && recv != 0) && i++<37)
//	{
//		OSSemPend(sem_mdb_recvd, 10, &err);
//		recv = (mdb_rx_len - recv);
//		if(err == OS_TIMEOUT && recv == 0)
//			break;
//	}
	USART_ITConfig(UART4, USART_IT_RXNE, DISABLE );		//关接收中断
	
	
   if(err == OS_NO_ERR)
   {
		if(mdb_rx_len == 1)      //如果只收到一个字节数据检查是否是应答
		{
			mdb_rx_buff[0] &= 0xff;
			
			if(mdb_rx_buff[0] == ACK)
			{
				return MDB_RECEIVE_ACK;
			}
			else
			if(mdb_rx_buff[0] == NAK)
			{
				return MDB_RECEIVE_NAK;
			}
			else	
			if(mdb_rx_buff[0] == DROP || (mdb_rx_buff[0]&0x01) == DROP )
			{
				return MDB_RECEIVE_DROP;
			}
		}
		else//如果是多字节数据
		{
			mdb_rx_len--;  						//去除多出的位(就是指校验和)
			for(i = 0, chk = 0; i < mdb_rx_len; i++)//计算接收数据校验和
			{
				chk += (mdb_rx_buff[i] & 0XFF);
			}
   
			if(chk == (mdb_rx_buff[mdb_rx_len] & 0xff))	//如果校验和正确
			{
				send_ack();	 //发送ACK应答
				return MDB_RECEIVE_DATA;
			}
			else
			{
				lcd_show_num(test++, 0, 6, WHITE, BLUE, *(pt_word.choose[0].font), 0, 10); 
				return ERR_CHECKSUM;
			}
		}
	}
	else//没收到数据
	{
		if(mdb_rx_len == 0)    //如果缓冲区内没有数据
		{
			return NO_RESPONSE;
		}
	}
	return 0;
}


/*
串口4中断
*/
void  UART4_IRQHandler(void)
{
	uint32_t  sr;
	sr = sr;//防止编译器报错
	//sr = USART3->SR;
	if(USART_GetITStatus(UART4, USART_IT_TXE) != RESET)//发送空
	{
		USART_ClearITPendingBit(UART4, USART_IT_TXE);
	}
	
	if(USART_GetITStatus(UART4, USART_IT_TC) != RESET)//发送完成
	{
		USART_ClearITPendingBit(UART4, USART_IT_TC);
		USART_ITConfig(UART4, USART_IT_TC, DISABLE);                                  
	}
	
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)//收到数据
	{	 	
		USART_ClearITPendingBit(UART4, USART_IT_RXNE);//清除接收中断标志
		sr = USART_ReceiveData(UART4);
		mdb_rx_buff[mdb_rx_len++] = sr;
		if(mdb_rx_buff[mdb_rx_len - 1] & 0x100)//收到最后字节
		{
			OSSemPost(sem_mdb_recvd);//释放信号量 
		}
	}
	
	if(USART_GetITStatus(UART4, USART_IT_ORE) != RESET)//溢出错误
	{//RDR 中有数据未读，但是移位寄存器有数据需要发送到RDR中
		sr = USART_ReceiveData(UART4);
		USART_ClearITPendingBit(UART4, USART_IT_ORE);//请溢出错误标志
	}
	
	if(USART_GetITStatus(UART4, USART_IT_IDLE) != RESET)//空闲中断
	{
//		sr = UART4->SR;
		sr = UART4->DR;
//		DMA_Cmd(DMA2_Channel3, DISABLE);//关闭DMA接收，防止处理时有数据过来
		USART_ClearITPendingBit(UART4, USART_IT_IDLE );
		//获取DMA接收数据长度
		mdb_rx_len = MDB_RX_LEN - DMA_GetCurrDataCounter(DMA2_Channel3);//接收数据长度
		
		if( mdb_rx_buff[mdb_rx_len - 1] & 0x100)//收到的是结束字符
		{
			DMA_Cmd(DMA2_Channel3, DISABLE);//关闭DMA接收，防止处理时有数据过来
			OSSemPost(sem_mdb_recvd);//释放信号量 
		}
	}

	if(USART_GetITStatus(UART4 ,USART_IT_PE | USART_IT_FE | USART_IT_NE) != RESET)//其他错误
	{
		//没开对应的中断使能的话，是不会触发的
		sr = USART_ReceiveData(UART4);
		USART_ClearITPendingBit(UART4, USART_IT_PE | USART_IT_FE | USART_IT_NE);
	}
}



/*
参数 ：str	要发送的字符串的指针; n 命令的长度，ret 允许的重复次数,timeout 应答的最大时限, 
		rs:接收数据处理结果, rx_len 返回数据长度指针
返回值：接收数据指针
*/
uint16_t* mdb_send_cmd(uint8_t *cmd, uint8_t n, uint8_t ret, uint16_t timeout, uint8_t *rs, uint16_t *rx_len)
{
	uint8_t i, j = 0;
	uint8_t checksum = 0;
	
//	while(OSSemAccept(sem_mdb_recvd));//清除信号量
//	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE );		//开收中断
	
	mdb_tx_buff[0] = cmd[0] | 0x100;  //地址字节，最高位为1
	checksum += mdb_tx_buff[0];
	for(i = 1; i < n; i++)
	{	
		mdb_tx_buff[i] = cmd[i] & 0xff;
		checksum += cmd[i];
	}
	mdb_tx_buff[i] = checksum;
	
	while(OSSemAccept(sem_mdb_recvd));
	for(i = 0; i < MDB_RX_LEN; i++)
	{
		mdb_rx_buff[i] = 0;
	}
	
	/*
	打开DMA接收
	在此处打开接收而不在DMA发送中断里打开是因为：
	主机应答从机，发送ack时，从机并不应答
	*/
	DMA2_Channel3->CNDTR = MDB_RX_LEN;//重设置接收缓冲长度
	DMA_Cmd(DMA2_Channel3, ENABLE);	//打开接收DMA
	mdb_rx_len = 0;  				//复位接收缓冲区计数
//	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE );		
	
    DMA2_Channel5->CNDTR = (n + 1); //设置要发送的字节数目(多了一个校验和)
    DMA_Cmd(DMA2_Channel5, ENABLE);	//开始DMA发送
	
    *rs = mdb_rx_handle(timeout);//处理接收数据
    
    while(j < ret && ((*rs) == ERR_TIME_OUT || (*rs) == NO_RESPONSE || (*rs) == ERR_CHECKSUM))//如果接收不正确
    {
    	j++;
    	//send_ret(); //发送重发命令，目前发现很多都不支持
    	OSTimeDly(100);
		mdb_rx_len = 0;  		//复位接收缓冲区计数
		
		DMA2_Channel5->CNDTR = (n + 1); 		//设置要发送的字节数目
		DMA_Cmd(DMA2_Channel5, ENABLE);	//开始DMA发送

    	*rs = mdb_rx_handle(timeout);
    }
	*rx_len = mdb_rx_len;
	return mdb_rx_buff;
}




/*
串口4初始化
*/
void uart4_init(void)
{
	uart4_config();
	dma2_3_config();
	dma2_5_config();
	sem_mdb_recvd = OSSemCreate(0);//接收数据信号量
}

