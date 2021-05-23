/*
uart5
485 ʹ��
��ϧ����stm32f103 ����5û��DMA������ֻ�����ж�ʵ��


ע�⣺������������˵�����һ��������Ҫ����ָ�������ָ���Ƿ��м���Դ�ϣ�����Ҫ����
*/

#include "drv.h"
#include "stm32_lib.h"
#include "uart485.h"
#include "timer3.h"

#define OPEN_485_RECV	GPIO_ResetBits(GPIOG,GPIO_Pin_11);
#define OPEN_485_SEND	GPIO_SetBits(GPIOG,GPIO_Pin_11);


static OS_EVENT* sem_485_lock = NULL;	//��ס485���ߣ����Ӧ��ÿ�ε���485����ǰ��Ҫ��������ź�������ȡ���ź������ܷ�ָ��
static OS_EVENT* sem_485_cmd;			//
static volatile UART485_CMD_DATA* cur485Cmd;	//ָ��ָ��

/*
485��������
*/
void uart_485_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//������Ҫ��һ��GPIO��ʱ�ӣ�������ͳһ���ļ��д򿪣�����Ͳ��ô���
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5 ,ENABLE);//����uart5����ʱ��
	
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
	
	USART_ClearFlag(UART5, USART_FLAG_TC);//�巢����ɱ�־
	USART_ClearFlag(UART5, USART_IT_IDLE);//�巢����ɱ�־
	USART_ClearFlag(UART5, USART_IT_RXNE);//�巢����ɱ�־
	
	USART_ITConfig(UART5,  USART_FLAG_TC, DISABLE); 
	USART_ITConfig(UART5,  USART_IT_IDLE, DISABLE); 
	USART_ITConfig(UART5,  USART_IT_RXNE, DISABLE); 
	USART_ITConfig(UART5,  USART_IT_FE, DISABLE); 
	USART_ITConfig(UART5,  USART_IT_NE, DISABLE); 
	USART_ITConfig(UART5,  USART_IT_PE, DISABLE); 
	USART_ITConfig(UART5,  USART_IT_CTS, DISABLE); 	
	
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn; 		   //USART5�����ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		   //��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(UART5, ENABLE);
	
	USART_GetFlagStatus(UART5, USART_FLAG_TC);
	sem_485_lock = OSSemCreate(1);
	sem_485_cmd = OSSemCreate(0);
}
/*
485��������
*/
void bus_Lock_485(void)
{
	uint8_t err;
//	while(sem_485_lock == NULL);
	OSSemPend(sem_485_lock, 0, &err);
}

/*
485��������
*/
void bus_Unlock_485(void)
{
//	while(sem_485_lock == NULL);
	OSSemPost(sem_485_lock);
}

/*
485���߷������
����ǰһ��Ҫ�������ߡ�
*/
uint8_t send_to_485Bus(UART485_CMD_DATA* cmd)
{
	uint8_t err;
	USART_ITConfig(UART5,USART_IT_RXNE, DISABLE);
	err = UART5->SR;
	err = UART5->DR;//��ϴ�Ĵ���
	
	USART_ClearFlag(UART5, USART_FLAG_TC);//�巢����ɱ�־
	USART_ClearFlag(UART5, USART_IT_IDLE);//�巢����ɱ�־
	USART_ClearFlag(UART5, USART_IT_RXNE);//�巢����ɱ�־
	USART_GetFlagStatus(UART5, USART_FLAG_TC);
//	while (!(UART5->SR & USART_FLAG_TXE));
//		GPIO_SetBits(GPIOG,GPIO_Pin_11);
	OPEN_485_SEND;
	OSSemAccept(sem_485_cmd);//����ź���
	/*
	485ȫ��ָ��ָ��
	*/
	cur485Cmd = cmd;
	cur485Cmd->recv_len = 0;
	cur485Cmd->sendCount = 0;
	USART_ITConfig(UART5,USART_IT_TC, ENABLE);   		//���������ж�
	UART5->DR = cur485Cmd->cmd[cur485Cmd->sendCount++];	//��ʼ������
	OSSemPend(sem_485_cmd, cur485Cmd->timeout, &err);	//�ȴ�
	cur485Cmd = 0;//��ֹҰָ��
	return err;
}


//===============================
//TIM3 �ж�
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
		//������Է���һ����Ϣ
		//���ﲻ��������Ϣ��
		
		if(cur485Cmd != NULL)//��ǰ��������Ҫִ��
		{
			if(cur485Cmd->isFinished(cur485Cmd->recv, &(cur485Cmd->recv_len)))
			{
//				OPEN_485_SEND;			//485����ת��Ϊ����
				OSSemPost(sem_485_cmd); //֪ͨ���ݽ������
				USART_ITConfig(UART5, USART_IT_RXNE, DISABLE);//��485�����ж�
			}
		}
		
		
    } 
} 





void UART5_IRQHandler(void)
{
	uint8_t sr;
	sr = sr;
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)//���յ�����
	{
		start_tim3_ms(20);//��ʼ��������ʱ

		sr = UART5->SR;
		USART_ClearITPendingBit(UART5, USART_IT_RXNE);
//		rx_len %= 1000;
//		rx[rx_len++] = UART5->DR;
		
		
		if(cur485Cmd != NULL)//��ǰ��������Ҫִ��
		{
			uint8_t data = UART5->DR;
			cur485Cmd->recv[cur485Cmd->recv_len++] = data;
			rx[rx_len++] = data;
//			if(cur485Cmd->isFinished(cur485Cmd->recv, &(cur485Cmd->recv_len)))
//			{
//				OPEN_485_SEND;			//485����ת��Ϊ����
//				OSSemPost(sem_485_cmd); //֪ͨ���ݽ������
//				USART_ITConfig(UART5, USART_IT_RXNE, DISABLE);//��485�����ж�
//			}
		}
	}
	else
	if(USART_GetITStatus(UART5, USART_IT_TC) != RESET)//������ɣ�ÿ��һ�ֽ�
	{
		USART_ClearITPendingBit(UART5, USART_IT_TC);//����жϴ�����λ
		if(cur485Cmd != NULL)//��ǰ��������Ҫִ��
		{
			if(cur485Cmd->sendCount<cur485Cmd->cmd_len)
				UART5->DR=cur485Cmd->cmd[cur485Cmd->sendCount++];
			else
			{
				USART_ITConfig(UART5, USART_IT_TC,DISABLE);//��485�����ж�
				if(cur485Cmd->needAck)//��ҪӦ���ָ��
				{
					USART_ITConfig(UART5, USART_IT_TC, DISABLE);//�ط����ж�
					USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//��485�����ж�
					
					OPEN_485_RECV;	//485�����Ϊ����
//					for(sr = 0; sr < 100; sr++);
//					cur485Cmd->recv_len = 0;

					
				}
				else//����ҪӦ���ָ�ֱ���ͷ��ź���
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
		if(USART_GetITStatus(UART5, USART_IT_PE | USART_IT_FE | USART_IT_NE) != RESET)//��������
		{
		uint8_t sr = USART_ReceiveData(UART5);
			USART_ClearITPendingBit(UART5, USART_IT_PE | USART_IT_FE | USART_IT_NE);
		}
	}
}

