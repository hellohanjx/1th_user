/*
通讯总线驱动
使用UART3
*/
#include "stm32_lib.h"
#include "bsp_commucation.h"

#define RX_LEN	256
#define TX_LEN	256

uint8_t commucation_recv_buf[RX_LEN];
uint8_t commucation_send_buf[TX_LEN];
uint16_t commucation_rx_len;

COMMUCATION_RECV_DEALWITH commucationo_recv_dealwith; 
/*
串口1初始化
*/
static void usart3_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	USART_InitTypeDef USART_InitStructure;	 
	NVIC_InitTypeDef NVIC_InitStructure;	

	//使能引脚时钟，与复用功能时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO ,  ENABLE );
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 ,ENABLE);

	//TX 引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;			//PB10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//复用的推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    //RX 引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;			//PB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//串口配置
	USART_InitStructure.USART_BaudRate            = 115200  ;				//波特率
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;           	//8位数据
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;              	//1个停止位
	USART_InitStructure.USART_Parity              = USART_Parity_No ;              	//无奇偶检验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//禁用硬件流控制
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;  //使能发送接收
	USART_Init(USART3, &USART_InitStructure);
	
	
	//串口中断配置
//	USART_ITConfig(USART3,USART_IT_RXNE, ENABLE);          //使能接收中断
	USART_ITConfig(USART3,USART_IT_ERR,ENABLE);				//使能错误中断
	
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE );		//使能空闲中断(只要这个就行)
//	USART_ITConfig(USART3, USART_IT_TC, ENABLE);                                  
//	USART_ClearITPendingBit(USART3, USART_IT_RXNE);


	
//	USART_ClearFlag(USART3,USART_FLAG_RXNE); 
	USART_ClearFlag(USART3,USART_FLAG_TC); 				//清发送完标志

	USART_Cmd(USART3, ENABLE);     	//使能uart1外设
	
	//=============================
	//中断使能，对应到中断向量表
	//=============================
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; 		                       	//USART3接收中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;                       //先占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		                        //从优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									//中断通道使能
	NVIC_Init(&NVIC_InitStructure);
}

/*
DMA1-3初始化
配置为串口3的接收 模式
*/
static void uart3_dmaRecv_config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;				
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//DMA1 时钟打开
	
	
	DMA_DeInit(DMA1_Channel3); 
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART3->DR);		//dr寄存器的地址· (0x40013804)
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)commucation_recv_buf;				//接收缓冲的地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;						//外设作为数据来源
	DMA_InitStructure.DMA_BufferSize = RX_LEN;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//dma 不自增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;				//内存自增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//dma数据宽度（Byte宽）
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			//内存数据宽度
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;		//普通模式；非循环接收
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;	//DMA高优先级（DMA设备间的优先级）
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;		//关闭内存到内存传输模式
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);
	
	DMA_ClearFlag(DMA1_FLAG_GL1);       // 清除DMA所有标志   
	
	/*
	注意，这里并没有开DMA接收中断
	*/
//	DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);//传输完成中断
//	DMA_ITConfig(DMA1_Channel3, DMA_IT_TE, ENABLE);//传输错误中断
	
	/* Enable USART3 DMA TX request */
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);	//使能串口1的DMA接收
	USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);	//使能串口1的DMA发送

	DMA_Cmd(DMA1_Channel3, ENABLE);					//打开DMA1-5通道
		
	//=============================
	//DMA中断设置
	//=============================
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn; 		                //中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;                       //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		                        //从优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									//中断使能
	NVIC_Init(&NVIC_InitStructure);

}

/*
DMA1-2初始化
配置为串口1的发送 模式
*/
void uart3_dmaSend_config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;				
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//DMA1 时钟打开

	DMA_DeInit(DMA1_Channel2); 
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART3->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)commucation_send_buf;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;				//外设作为数据目的地
	DMA_InitStructure.DMA_BufferSize = TX_LEN;							//DMA缓存大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//DMA外设地址不自增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;			//DMA内存地址自增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//8bit 数据宽度，dma
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//8bit 数据宽度，内存
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;					//发完一次就停
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;				//高优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;					//进制内存到内存
	
	DMA_Init(DMA1_Channel2, &DMA_InitStructure);					//配置寄存器
	
		DMA_ClearFlag(DMA1_FLAG_GL1);       // 清除DMA所有标志   

	
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);					//发送完中断
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TE, ENABLE);					//传输错误中断
	
	/* Enable USART3 DMA TX request */
	USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);
	DMA_Cmd(DMA1_Channel2, DISABLE);				//关闭DMA1-4通道
	
	//=============================
	//DMA中断设置
	//=============================
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn; 		                //中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;                       //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		                        //从优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									//中断使能
	NVIC_Init(&NVIC_InitStructure);

}


/*
DMA1-3	
接收中断
*/
static uint8_t DMA_fl = 0;
void DMA1_Channel3_IRQHandler(void)
{
	DMA_fl = !DMA_fl;
	if(DMA_GetITStatus(DMA1_IT_TC3) == SET)    
	{
		DMA_ClearITPendingBit(DMA1_IT_TC3);
	}
	if(DMA_GetITStatus(DMA1_IT_TE3) == SET)
	{
		DMA_ClearITPendingBit(DMA1_IT_TE3);
	}
	DMA_ClearITPendingBit(DMA1_IT_TC3);
	DMA_ClearITPendingBit(DMA1_IT_TE3);
	DMA_Cmd(DMA1_Channel3, DISABLE);//关DMA防止处理时有数据过来
	DMA1_Channel3->CNDTR = 100;//重装填
	DMA_Cmd(DMA1_Channel3, ENABLE);//数据处理完，重开DMA
}


/*
DMA1-2
发送中断
*/
void DMA1_Channel2_IRQHandler(void)
{
//	DMA_fl = !DMA_fl;
	if(DMA_GetITStatus(DMA1_IT_TC2) == SET)    
	{
		DMA_ClearITPendingBit(DMA1_IT_TC2);
	}
	if(DMA_GetITStatus(DMA1_IT_TE2) == SET)
	{
		DMA_ClearITPendingBit(DMA1_IT_TE2);
	}
	DMA_ClearITPendingBit(DMA1_IT_TC2);
	DMA_ClearITPendingBit(DMA1_IT_TE2);
	DMA_Cmd(DMA1_Channel2, DISABLE);//关
	
	DMA1_Channel3->CNDTR = RX_LEN;//重设置接收缓冲长度
	DMA_Cmd(DMA1_Channel3, ENABLE);	//打开接收DMA
//	show_string("11", 500, 350);

}



/*
数据返回默认处理函数
*/
void commucation_recv_default(uint8_t * dat, uint16_t len)
{
	//这里可以作为那种全双工通讯的通道
}





/*
串口3中断
*/
void USART3_IRQHandler(void)
{
	uint32_t  sr;
	sr = sr;//防止编译器报错
	//sr = USART3->SR;
	if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)//发送空
	{
		USART_ClearITPendingBit(USART3, USART_IT_TXE);
	}
	
	if(USART_GetITStatus(USART3, USART_IT_TC) != RESET)//发送完成
	{
		USART_ClearITPendingBit(USART3, USART_IT_TC);
		USART_ITConfig(USART3, USART_IT_TC, DISABLE);                                  
	}
	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//收到数据
	{	 	
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);//清除接收中断标志
//		rx_len %= 100;
//		rx_buffer[rx_len++]=USART_ReceiveData(USART3);//¶ÁÈ¡½ÓÊÕµ½µÄÊý¾ÝUSART3->DR,×Ô¶¯Çå³ý±êÖ¾Î»
	}
	
	if(USART_GetITStatus(USART3, USART_IT_ORE) != RESET)//溢出错误
	{//RDR 中有数据未读，但是移位寄存器有数据需要发送到RDR中
		sr = USART_ReceiveData(USART3);
		USART_ClearITPendingBit(USART3, USART_IT_ORE);//请溢出错误标志
	}
	
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)//空闲
	{
//			show_string("0 ", 500, 350);

		DMA_fl = 3;
		sr = USART_ReceiveData(USART3);
		DMA_Cmd(DMA1_Channel3, DISABLE);//关闭DMA，防止处理时有数据过来
		USART_ClearITPendingBit(USART3, USART_IT_IDLE );
		//获取DMA接收数据长度
		commucation_rx_len = RX_LEN - DMA_GetCurrDataCounter(DMA1_Channel3);//这个和手上的中文手册不同
		
		commucationo_recv_dealwith(commucation_recv_buf, commucation_rx_len);
		commucationo_recv_dealwith = commucation_recv_default;
		
//		DMA1_Channel3->CNDTR = 100;//重设置接收缓冲长度
//		DMA_Cmd(DMA1_Channel3, ENABLE);	//打开DMA
	}

	if(USART_GetITStatus(USART3 ,USART_IT_PE | USART_IT_FE | USART_IT_NE) != RESET)//其他错误
	{//没开对应的中断使能的话，是不会触发的
		sr = USART_ReceiveData(USART3);
		USART_ClearITPendingBit(USART3, USART_IT_PE | USART_IT_FE | USART_IT_NE);
	}
}




/*
通讯总线初始化
*/
void bsp_commucation_init(void)
{
	usart3_config();
	uart3_dmaSend_config();
	uart3_dmaRecv_config();
	commucationo_recv_dealwith = commucation_recv_default;
}


/*
通讯口发送
参数：size：DMA要发送的字节数
*/
void commucation_send(uint8_t *dat, uint16_t size, COMMUCATION_RECV_DEALWITH callback)  
{
//	show_string("13", 500, 350);
	memcpy(commucation_send_buf, dat, size);//拷贝要发送的内容
	commucationo_recv_dealwith = callback;
	commucation_rx_len = 0;
//		show_string("12", 500, 350);
    DMA_Cmd(DMA1_Channel2, DISABLE);//开始DMA发送
    DMA1_Channel2->CNDTR = size; // 设置要发送的字节数目
    DMA_Cmd(DMA1_Channel2, ENABLE);//开始DMA发送
}

/*
返回接收缓冲指针
*/
uint8_t *commucation_recv_get(void)
{
	return commucation_recv_buf;
}

/*
获取发送缓冲指针
*/
uint8_t *commucation_send_get(void)
{
	return commucation_send_buf;
}

/*
获取接收数据长度
*/
uint32_t commucation_rxlen_get(void)
{
	return commucation_rx_len;
}
