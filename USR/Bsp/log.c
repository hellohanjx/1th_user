/*
日志输出
使用串口2 ，异步方式
因为串口2的DMA被I2C占用了，所以日志输出就用这个端口吧
日志输出优先级设置为最低
*/

#include "log.h"
#include "stm32_lib.h"

bool TX_READY = FALSE;
uint8_t *pt,pt_len=0;

/*
串口2初始化
*/
static void usart2_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	USART_InitTypeDef USART_InitStructure;	 
//	NVIC_InitTypeDef NVIC_InitStructure;	
	

	//使能引脚时钟，与复用功能时钟
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2 , ENABLE);  
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA ,  ENABLE );
	//TX 引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;			//PA2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//复用的推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    //RX 引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;			//PA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//串口配置
	USART_InitStructure.USART_BaudRate            = 115200  ;				//波特率
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;           	//8位数据
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;              	//1个停止位
	USART_InitStructure.USART_Parity              = USART_Parity_No ;              	//无奇偶检验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//禁用硬件流控制
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx  ;  //使能发送接收
	USART_Init(USART2, &USART_InitStructure);
	
	
	//串口中断配置
//	USART_ITConfig(USART2,USART_IT_RXNE, ENABLE);          //使能接收中断
//	USART_ITConfig(USART2,USART_IT_ERR,ENABLE);				//使能错误中断
//	
//	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE );		//使能空闲中断(只要这个就行)
//	USART_ITConfig(USART2, USART_IT_TC, ENABLE);       
//	USART_ClearITPendingBit(USART2,USART_FLAG_TC);
//	USART_ClearITPendingBit(USART2, USART_IT_RXNE);


	
//	USART_ClearFlag(USART2,USART_FLAG_RXNE); 
//	USART_ClearFlag(USART2,USART_FLAG_TC); 				//清发送完标志

	USART_Cmd(USART2, ENABLE);     	//使能uart2外设
	
	//=============================
	//中断使能，对应到中断向量表
	//=============================
//	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; 		                       	//USART2接收中断
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;                       //先占优先级
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		                        //从优先级
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									//中断通道使能
//	NVIC_Init(&NVIC_InitStructure);
}





/*
串口2中断
*/
void USART2_IRQHandler(void)
{
	uint32_t  sr;
	sr = sr;//防止编译器报错
	//sr = USART2->SR;
	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)//发送空
	{
		USART_ClearITPendingBit(USART2, USART_IT_TXE);
	}
	
	if(USART_GetITStatus(USART2, USART_IT_TC) != RESET)//发送完成
	{
		USART_ClearITPendingBit(USART2, USART_IT_TC);
		if(pt[pt_len]!=0)
		//USART_SendData(USART2,pt[pt_len]);
		USART2->DR = pt[pt_len];
//		USART_ITConfig(USART2, USART_IT_TC, DISABLE);                                  
	}
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//收到数据
	{	 	
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);//清除接收中断标志
//		rx_len %= 100;
//		rx_buffer[rx_len++]=USART_ReceiveData(USART2);//¶ÁÈ¡½ÓÊÕµ½µÄÊý¾ÝUSART2->DR,×Ô¶¯Çå³ý±êÖ¾Î»
	}
	
	if(USART_GetITStatus(USART2, USART_IT_ORE) != RESET)//溢出错误
	{//RDR 中有数据未读，但是移位寄存器有数据需要发送到RDR中
		sr = USART_ReceiveData(USART2);
		USART_ClearITPendingBit(USART2, USART_IT_ORE);//请溢出错误标志
	}
	
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)//空闲
	{
		sr = USART_ReceiveData(USART2);
		//获取DMA接收数据长度
		
	}

	if(USART_GetITStatus(USART2 ,USART_IT_PE | USART_IT_FE | USART_IT_NE) != RESET)//其他错误
		{//没开对应的中断使能的话，是不会触发的
		sr = USART_ReceiveData(USART2);
		USART_ClearITPendingBit(USART2, USART_IT_PE | USART_IT_FE | USART_IT_NE);
	}
}

/*
串口发送
*/
void usart2_send(uint8_t data)
{
	//TXE 弹仓空
	//TC 枪膛空
//	while(USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET );//Ã²ËÆµÃÓÃÕâ¸ö
//	while(!(USART_GetFlagStatus(USART2,USART_FLAG_TXE) != RESET ));//
	while((USART2->SR&0X40)==0){};//循环发送,直到发送完毕   
//	USART_SendData(USART2,(uint8_t)data);
   USART2->DR = (data & 0xff);	 

}







/*
日志发送
参数：size
*/
void log_send(uint8_t *dat, uint16_t line)
{
	uint16_t i ,len;
	char tmp[10];
	pt = dat;pt_len=0;
	if(!TX_READY)
		return;
	
	
	USART_Cmd(USART2, ENABLE);
//	USART_ITConfig(USART2, USART_IT_TC, ENABLE);                                  
	
	for(i = 0; i < 300 && dat[i] != 0 ; i++)
	{
		usart2_send(dat[i]); 
	}
	
	usart2_send('\n');
	usart2_send('\r');
	
	len = sprintf(tmp, "%u", line);
	for(i = 0; i < len; i++)
	{
		usart2_send( tmp[i]);
	}
}

/*
日志系统初始化
*/
void log_init(void)
{
	TIM_DeInit(TIM2);
	TIM_DeInit(TIM3);
	
	usart2_config();
	TX_READY = TRUE;
}
