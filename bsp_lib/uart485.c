/*
uart5
485 使用
可惜的是stm32f103 串口5没有DMA，所以只能用中断实现


注意：对于驱动板来说，完成一个动作需要两条指令，这两条指令是否中间可以打断，还需要考虑
*/

#include "drv.h"
#include "stm32_lib.h"
#include "uart485.h"
#include "timer3.h"

#define OPEN_485_RECV	GPIO_ResetBits(GPIOG,GPIO_Pin_11);
#define OPEN_485_SEND	GPIO_SetBits(GPIOG,GPIO_Pin_11);


static OS_EVENT* sem_485_lock = NULL;	//锁住485总线，这个应该每次调用485发送前需要调用这个信号量，获取了信号量才能发指令
static OS_EVENT* sem_485_cmd;			//
static volatile UART485_CMD_DATA* cur485Cmd;	//指令指针

/*
485串口配置
*/
void uart_485_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//这里需要打开一下GPIO口时钟，或者在统一的文件中打开，后面就不用打开了
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5 ,ENABLE);//代开uart5外设时钟
	
	//485 tx
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//485_RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);	
	//485_DIR
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOG, &GPIO_InitStructure);	
	
	
	USART_DeInit (UART5);
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART5, &USART_InitStructure);
	
	USART_ClearFlag(UART5, USART_FLAG_TC);//清发送完成标志
	USART_ClearFlag(UART5, USART_IT_IDLE);//清发送完成标志
	USART_ClearFlag(UART5, USART_IT_RXNE);//清发送完成标志
	
	USART_ITConfig(UART5,  USART_FLAG_TC, DISABLE); 
	USART_ITConfig(UART5,  USART_IT_IDLE, DISABLE); 
	USART_ITConfig(UART5,  USART_IT_RXNE, DISABLE); 
	USART_ITConfig(UART5,  USART_IT_FE, DISABLE); 
	USART_ITConfig(UART5,  USART_IT_NE, DISABLE); 
	USART_ITConfig(UART5,  USART_IT_PE, DISABLE); 
	USART_ITConfig(UART5,  USART_IT_CTS, DISABLE); 	
	
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn; 		   //USART5接收中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		   //次占优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(UART5, ENABLE);
	
	USART_GetFlagStatus(UART5, USART_FLAG_TC);
	sem_485_lock = OSSemCreate(1);
	sem_485_cmd = OSSemCreate(0);
}
/*
485总线上锁
*/
void bus_Lock_485(void)
{
	uint8_t err;
//	while(sem_485_lock == NULL);
	OSSemPend(sem_485_lock, 0, &err);
}

/*
485总线上锁
*/
void bus_Unlock_485(void)
{
//	while(sem_485_lock == NULL);
	OSSemPost(sem_485_lock);
}

/*
485总线发送命令。
调用前一定要锁定总线。
*/
uint8_t send_to_485Bus(UART485_CMD_DATA* cmd)
{
	uint8_t err;
	USART_ITConfig(UART5,USART_IT_RXNE, DISABLE);
	err = UART5->SR;
	err = UART5->DR;//清洗寄存器
	
	USART_ClearFlag(UART5, USART_FLAG_TC);//清发送完成标志
	USART_ClearFlag(UART5, USART_IT_IDLE);//清发送完成标志
	USART_ClearFlag(UART5, USART_IT_RXNE);//清发送完成标志
	USART_GetFlagStatus(UART5, USART_FLAG_TC);
//	while (!(UART5->SR & USART_FLAG_TXE));
//		GPIO_SetBits(GPIOG,GPIO_Pin_11);
	OPEN_485_SEND;
	OSSemAccept(sem_485_cmd);//清除信号量
	/*
	485全局指针指向
	*/
	cur485Cmd = cmd;
	cur485Cmd->recv_len = 0;
	cur485Cmd->sendCount = 0;
	USART_ITConfig(UART5,USART_IT_TC, ENABLE);   		//开启发送中断
	UART5->DR = cur485Cmd->cmd[cur485Cmd->sendCount++];	//开始发数据
	OSSemPend(sem_485_cmd, cur485Cmd->timeout, &err);	//等待
	cur485Cmd = 0;//防止野指针
	return err;
}


//===============================
//TIM3 中断
//===============================
uint8_t rx[200];
uint32_t rx_len = 0;
uint8_t flag = 0;
void TIM3_IRQHandler(void)
{ 
  if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    { 
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		TIM_Cmd(TIM3, DISABLE);
		if(flag == 0)
			rx_len = 100;
		else
			rx_len = 0;
		flag++;
		flag %= 1;
		//这里可以发送一个消息
		//这里不能申请消息。
		
		if(cur485Cmd != NULL)//当前有命令需要执行
		{
			if(cur485Cmd->isFinished(cur485Cmd->recv, &(cur485Cmd->recv_len)))
			{
//				OPEN_485_SEND;			//485方向转换为发送
				OSSemPost(sem_485_cmd); //通知数据接收完成
				USART_ITConfig(UART5, USART_IT_RXNE, DISABLE);//关485接收中断
			}
		}
		
		
    } 
} 





void UART5_IRQHandler(void)
{
	uint8_t sr;
	sr = sr;
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)//接收到数据
	{
		start_tim3_ms(20);//开始计数器计时

		sr = UART5->SR;
		USART_ClearITPendingBit(UART5, USART_IT_RXNE);
//		rx_len %= 1000;
//		rx[rx_len++] = UART5->DR;
		
		
		if(cur485Cmd != NULL)//当前有命令需要执行
		{
			uint8_t data = UART5->DR;
			cur485Cmd->recv[cur485Cmd->recv_len++] = data;
			rx[rx_len++] = data;
//			if(cur485Cmd->isFinished(cur485Cmd->recv, &(cur485Cmd->recv_len)))
//			{
//				OPEN_485_SEND;			//485方向转换为接收
//				OSSemPost(sem_485_cmd); //通知数据接收完成
//				USART_ITConfig(UART5, USART_IT_RXNE, DISABLE);//关485接收中断
//			}
		}
	}
	else
	if(USART_GetITStatus(UART5, USART_IT_TC) != RESET)//发送完成，每次一字节
	{
		USART_ClearITPendingBit(UART5, USART_IT_TC);//清除中断待处理位
		if(cur485Cmd != NULL)//当前有命令需要执行
		{
			if(cur485Cmd->sendCount<cur485Cmd->cmd_len)
				UART5->DR=cur485Cmd->cmd[cur485Cmd->sendCount++];
			else
			{
				USART_ITConfig(UART5, USART_IT_TC,DISABLE);//关485发送中断
				if(cur485Cmd->needAck)//需要应答的指令
				{
					USART_ITConfig(UART5, USART_IT_TC, DISABLE);//关发送中断
					USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//打开485接收中断
					
					OPEN_485_RECV;	//485方向变为接收
//					for(sr = 0; sr < 100; sr++);
//					cur485Cmd->recv_len = 0;

					
				}
				else//不需要应答的指令，直接释放信号量
				{
					OSSemPost(sem_485_cmd);
				}
			}
		}
	}
	else
	{
		if( USART_GetITStatus(UART5, USART_IT_IDLE) != RESET )
		{
			uint8_t sr = USART_ReceiveData(UART5);
			USART_ClearITPendingBit(UART5, USART_IT_IDLE);
		}
		if(USART_GetITStatus(UART5, USART_IT_PE | USART_IT_FE | USART_IT_NE) != RESET)//其他错误
		{
		uint8_t sr = USART_ReceiveData(UART5);
			USART_ClearITPendingBit(UART5, USART_IT_PE | USART_IT_FE | USART_IT_NE);
		}
	}
}

