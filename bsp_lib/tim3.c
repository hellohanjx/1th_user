//===============================
//ʹ�ö�ʱ��3 ����ʱ
//===============================
#include "header.h"
#include "stm32_lib.h"

//===============================
//TIM3 ��ʼ��
//===============================





void TIM3_Init(void) 
{ 
	TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure; 
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //����ʱ��3ʱ�ӣ����36 M��
	TIM_TimeBaseStructure.TIM_Period = 0;      				//�Զ���װ������ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = 35999;   				//Ԥ��Ƶ	2kHZ	0.5ms
	
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;			//�ظ����� ��
	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV4; 	//ʱ�ӷָ� 
	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_FLAG_Update; //������ʽ (���¼���)
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);             //��ʼ�� time3 ����������
	//���з��� TIM_TimeBaseInit ���棬���⿪���ͽ����ж�
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);	
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE );                   //��������ж�
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;             //�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  	//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;          //�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        		//ʹ��ͨ���ж�
	NVIC_Init(&NVIC_InitStructure);                            	//��ʼ���ж�
	TIM_Cmd(TIM3, DISABLE);                                      //timer3 ʹ��
}


void TIM4_Init(void) 
{ 
	TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure; 
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //����ʱ��3ʱ�ӣ����36 M��
	TIM_TimeBaseStructure.TIM_Period = 0;      				//�Զ���װ������ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = 35999;   				//Ԥ��Ƶ	2kHZ	0.5ms
	
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;			//�ظ����� ��
	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV4; 	//ʱ�ӷָ� 
	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_FLAG_Update; //������ʽ (���¼���)
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);             //��ʼ�� time3 ����������
	//���з��� TIM_TimeBaseInit ���棬���⿪���ͽ����ж�
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);	
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE );                   //��������ж�
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;             //�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  	//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;          //�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        		//ʹ��ͨ���ж�
	NVIC_Init(&NVIC_InitStructure);                            	//��ʼ���ж�
	TIM_Cmd(TIM4, DISABLE);                                      //timer3 ʹ��
}

extern uint32_t rx_len ;

////===============================
////TIM3 �ж�
////===============================
//void TIM3_IRQHandler(void)
//{ 
//  if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
//    { 
//		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
//		TIM_Cmd(TIM3, DISABLE);
//		rx_len = 0;
//		//������Է���һ����Ϣ
//		//���ﲻ��������Ϣ��
//    } 
//} 

//===============================
//��ʼ��ʱ1 ms
//===============================
 void start_tim4_ms(uint16_t num)
 {
	 TIM_SetAutoreload(TIM4, num*2);//�ı���װֵ
	 TIM_SetCounter(TIM4,0);		//���������ֵ
	 TIM_Cmd(TIM4, ENABLE);

 }

//===============================
//��ʼ��ʱ1 ms
//===============================
 void start_tim3_ms(uint16_t num)
 {
	 TIM_SetAutoreload(TIM3, num*2);//�ı���װֵ
	 TIM_SetCounter(TIM3,0);		//���������ֵ
	 TIM_Cmd(TIM3, ENABLE);

 }
//===============================
//��ʼ��ʱ1 s
//===============================
 void start_tim3_s(uint8_t num)
 {
	TIM_SetCounter(TIM3,2000*num);
	TIM_SetAutoreload(TIM3, 2000*num);
	TIM_Cmd(TIM3, ENABLE);
 }

