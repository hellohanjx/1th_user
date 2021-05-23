/*
读卡头总线驱动
使用UART1
*/
#include "stm32_lib.h"
#include "ucos_ii.h"
#include "bsp_ireader.h"
#include "header.h"

#define RX_LEN	100
#define TX_LEN	100

uint8_t icreader_recv_buf[RX_LEN];
uint8_t icreader_send_buf[TX_LEN];
uint16_t icreader_rx_len;

static OS_EVENT* semUart1Recv;
static OS_EVENT* semUart1Send;


ICREADER_RECV_DEALWITH icreader_recv_dealwith; //


/*
数据返回默认处理函数
*/
void icreader_recv_default(uint8_t * dat, uint16_t len)
{
	//这里可以作为那种全双工通讯的通道
}






/*
串口1初始化
*/
static void usart1_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	USART_InitTypeDef USART_InitStructure;	 
	NVIC_InitTypeDef NVIC_InitStructure;	

	//使能引脚时钟，与复用功能时钟
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA  ,  ENABLE );

	//TX 引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;			//PA9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//复用的推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    //RX 引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;			//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//串口配置
	USART_InitStructure.USART_BaudRate            = 9600  ;				//波特率
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;           	//8位数据
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;              	//1个停止位
	USART_InitStructure.USART_Parity              = USART_Parity_No ;              	//无奇偶检验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//禁用硬件流控制
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;  //使能发送接收
	USART_Init(USART1, &USART_InitStructure);
	
	
	//串口中断配置
//	USART_ITConfig(USART1,USART_IT_RXNE, ENABLE);          	//使能接收中断
	USART_ITConfig(USART1,USART_IT_ERR,ENABLE);				//使能错误中断
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE );			//使能空闲中断(只要这个就行)
//	USART_ITConfig(USART1, USART_IT_TC, ENABLE);                                  
//	USART_ClearITPendingBit(USART1, USART_IT_RXNE);


	
//	USART_ClearFlag(USART1,USART_FLAG_RXNE); 
	USART_ClearFlag(USART1,USART_FLAG_TC); 				//清发送完标志

	USART_Cmd(USART1, ENABLE);     	//使能uart1外设
	
	//=============================
	//中断使能，对应到中断向量表
	//=============================
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; 		                       	//USART1接收中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;                       //先占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		                        //从优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									//中断通道使能
	NVIC_Init(&NVIC_InitStructure);
}

/*
DMA1-5初始化
配置未串口1的接收 模式
*/
static void uart1_dmaRecv_config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;				
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//DMA1 时钟打开
	
	
	DMA_DeInit(DMA1_Channel4); 
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR);		//dr寄存器的地址· (0x40013804)
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)icreader_recv_buf;				//接收缓冲的地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;						//外设作为数据来源
	DMA_InitStructure.DMA_BufferSize = RX_LEN;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//dma 不自增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;				//内存自增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//dma数据宽度（Byte宽）
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			//内存数据宽度
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;		//普通模式；非循环接收
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;	//DMA高优先级（DMA设备间的优先级）
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;		//关闭内存到内存传输模式
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);
	
	DMA_ClearFlag(DMA1_FLAG_GL1);       // 清除DMA所有标志   
	
	/*
	注意，这里并没有开DMA接收中断
	*/
//	DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);//传输完成中断
//	DMA_ITConfig(DMA1_Channel5, DMA_IT_TE, ENABLE);//传输错误中断
	
	/* Enable USART1 DMA TX request */
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);	//使能串口1的DMA接收

	DMA_Cmd(DMA1_Channel5, ENABLE);					//打开DMA1-5通道
		
	//=============================
	//DMA中断设置
	//=============================
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn; 		                //中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;                       //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		                        //从优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									//中断使能
	NVIC_Init(&NVIC_InitStructure);

}

/*
DMA1-4初始化
配置为串口1的发送 模式
*/
void uart1_dmaSend_config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;				
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//DMA1 时钟打开

	DMA_DeInit(DMA1_Channel4); 
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)icreader_send_buf;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;				//外设作为数据目的地
	DMA_InitStructure.DMA_BufferSize = TX_LEN;							//DMA缓存大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//DMA外设地址不自增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;			//DMA内存地址自增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//8bit 数据宽度，dma
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//8bit 数据宽度，内存
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;					//发完一次就停
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;				//高优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;					//进制内存到内存
	
	DMA_Init(DMA1_Channel4, &DMA_InitStructure);					//配置寄存器
	
		DMA_ClearFlag(DMA1_FLAG_GL4);       // 清除DMA所有标志   

	
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);					//发送完中断
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TE, ENABLE);					//传输错误中断
	
	/* Enable USART1 DMA TX request */
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	DMA_Cmd(DMA1_Channel4, DISABLE);				//关闭DMA1-4通道
	
	//=============================
	//DMA中断设置
	//=============================
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn; 		                //中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;                       //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		                        //从优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									//中断使能
	NVIC_Init(&NVIC_InitStructure);

}


/*
DMA1-5	
接收中断
*/
static uint8_t DMA_fl = 0;
void DMA1_Channel5_IRQHandler(void)
{
	DMA_fl = !DMA_fl;
	if(DMA_GetITStatus(DMA1_IT_TC5) == SET)    
	{
		DMA_ClearITPendingBit(DMA1_IT_TC5);
	}
	if(DMA_GetITStatus(DMA1_IT_TE5) == SET)
	{
		DMA_ClearITPendingBit(DMA1_IT_TE5);
	}
	DMA_ClearITPendingBit(DMA1_IT_TC5);
	DMA_ClearITPendingBit(DMA1_IT_TE5);
	DMA_Cmd(DMA1_Channel5, DISABLE);//关DMA防止处理时有数据过来
	DMA1_Channel5->CNDTR = 100;//重装填
	DMA_Cmd(DMA1_Channel5, ENABLE);//数据处理完，重开DMA
}


/*
DMA1-4
发送中断
*/
void DMA1_Channel4_IRQHandler(void)
{
//	DMA_fl = !DMA_fl;
	if(DMA_GetITStatus(DMA1_IT_TC4) == SET)    
	{
		DMA_ClearITPendingBit(DMA1_IT_TC4);
	}
	if(DMA_GetITStatus(DMA1_IT_TE4) == SET)
	{
		DMA_ClearITPendingBit(DMA1_IT_TE4);
	}
	DMA_ClearITPendingBit(DMA1_IT_TC4);
	DMA_ClearITPendingBit(DMA1_IT_TE4);
	DMA_Cmd(DMA1_Channel4, DISABLE);//关dma
	
	DMA_Cmd(DMA1_Channel5, DISABLE);	//打开接收DMA
	DMA1_Channel5->CNDTR = TX_LEN;//重设置接收缓冲长度
	DMA_Cmd(DMA1_Channel5, ENABLE);	//打开接收DMA

}






/*
串口1中断
*/
void USART1_IRQHandler(void)
{
	uint32_t  sr;
	sr = sr;//防止编译器报错
	//sr = USART1->SR;
	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)//发送空
	{
		USART_ClearITPendingBit(USART1, USART_IT_TXE);
	}
	
	if(USART_GetITStatus(USART1, USART_IT_TC) != RESET)//发送完成
	{
		USART_ClearITPendingBit(USART1, USART_IT_TC);
		USART_ITConfig(USART1, USART_IT_TC, DISABLE);                                  
	}
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//收到数据
	{	 	
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);//清除接收中断标志
//		rx_len %= 100;
//		rx_buffer[rx_len++]=USART_ReceiveData(USART1);//¶ÁÈ¡½ÓÊÕµ½µÄÊý¾ÝUSART1->DR,×Ô¶¯Çå³ý±êÖ¾Î»
	}
	
	if(USART_GetITStatus(USART1, USART_IT_ORE) != RESET)//溢出错误
	{//RDR 中有数据未读，但是移位寄存器有数据需要发送到RDR中
		sr = USART_ReceiveData(USART1);
		USART_ClearITPendingBit(USART1, USART_IT_ORE);//请溢出错误标志
	}
	
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)//空闲
	{
		DMA_fl = 3;
		sr = USART_ReceiveData(USART1);
		DMA_Cmd(DMA1_Channel5, DISABLE);//关闭DMA，防止处理时有数据过来
		USART_ClearITPendingBit(USART3, USART_IT_IDLE );
		//获取DMA接收数据长度
		icreader_rx_len = RX_LEN - DMA_GetCurrDataCounter(DMA1_Channel5);//这个和手上的中文手册不同 
		icreader_recv_dealwith(icreader_recv_buf, icreader_rx_len);//回调函数
//		if(icreader_rx_len > 10)
//		{
//			uint8_t i;
//			OSSemPost(semUart1Recv);
//			//处理数据
//			DMA_Cmd(DMA1_Channel5, DISABLE);//
//			DMA1_Channel5->CNDTR = 100; // 设置要发送的字节数目
//			DMA_Cmd(DMA1_Channel5, ENABLE);//打开DMA
//			for(i = 0; i < 10 ;i++)
//			cIC.PrintSnr[i] = icreader_recv_buf[i+1];
//			icreader_rx_len = 0;
//		}
//		icreader_rx_len = 0;

		
//		icreader_recv_dealwith(icreader_recv_buf, icreader_rx_len);
//		icreader_recv_dealwith = icreader_recv_default;
		/*
		刷新dma（从dma缓冲区起始处重新收数据，需要先关dma，再设置缓冲区长度，再开dma）
		DMA 接收中断需要等dma缓冲满或过半才会触发
		*/
//		DMA1_Channel5->CNDTR = 100;//重设置接收缓冲长度
//		DMA_Cmd(DMA1_Channel5, ENABLE);	//打开DMA
	}

	if(USART_GetITStatus(USART1 ,USART_IT_PE | USART_IT_FE | USART_IT_NE) != RESET)//其他错误
		{//没开对应的中断使能的话，是不会触发的
		sr = USART_ReceiveData(USART1);
		USART_ClearITPendingBit(USART1, USART_IT_PE | USART_IT_FE | USART_IT_NE);
	}
}

/*
锁总线与释放总线
*/
uint8_t lock_ireader_bus(void)
{
	uint8_t err;
	OSSemPend(semUart1Send, 0, &err);
	return err;
}
uint8_t unlock_ireader_bus(void)
{
	return OSSemPost(semUart1Send);
}
/*
等待总线数据
*/
uint8_t wait_ireader_bus(void)
{
	uint8_t err;
	OSSemPend(semUart1Recv, 0, &err);
	return err;
}


/*
总线初始化
*/
void bsp_ireader_init(void)
{
	usart1_config();
	uart1_dmaRecv_config();
	uart1_dmaSend_config();
	//创建信号量
	semUart1Send = OSSemCreate(1);
	semUart1Recv = OSSemCreate(0);
//	DMA_Cmd(DMA1_Channel4, ENABLE);
	DMA_Cmd(DMA1_Channel5, ENABLE);
	icreader_recv_dealwith = icreader_recv_default;
}

/*
总线发送
参数：size：DMA要发送的字节数
*/
uint8_t* cmd_icreader_send(uint8_t *dat, uint16_t size, ICREADER_RECV_DEALWITH callback)  
{
	memcpy(icreader_send_buf, dat, size);//拷贝要发送的内容
	icreader_recv_dealwith = callback;
	icreader_rx_len = 0;
	
    DMA1_Channel4->CNDTR = size; // 设置要发送的字节数目
    DMA_Cmd(DMA1_Channel4, ENABLE);//开始DMA发送
	return icreader_recv_buf;
}


