//===============================
//使用定时器3 来计时
//===============================
#include "header.h"
#include "stm32_lib.h"

//===============================
//TIM3 初始化
//===============================





void TIM3_Init(void) 
{ 
	TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure; 
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //开定时器3时钟（最高36 M）
	TIM_TimeBaseStructure.TIM_Period = 0;      				//自动重装载器的值
	TIM_TimeBaseStructure.TIM_Prescaler = 35999;   				//预分频	2kHZ	0.5ms
	
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;			//重复计数 关
	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV4; 	//时钟分割 
	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_FLAG_Update; //计数方式 (向下计数)
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);             //初始化 time3 计数器配置
	//这行放在 TIM_TimeBaseInit 后面，避免开机就进入中断
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);	
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE );                   //允许更新中断
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;             //中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  	//主优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;          //从优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        		//使能通道中断
	NVIC_Init(&NVIC_InitStructure);                            	//初始化中断
	TIM_Cmd(TIM3, DISABLE);                                      //timer3 使能
}


void TIM4_Init(void) 
{ 
	TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure; 
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //开定时器3时钟（最高36 M）
	TIM_TimeBaseStructure.TIM_Period = 0;      				//自动重装载器的值
	TIM_TimeBaseStructure.TIM_Prescaler = 35999;   				//预分频	2kHZ	0.5ms
	
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;			//重复计数 关
	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV4; 	//时钟分割 
	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_FLAG_Update; //计数方式 (向下计数)
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);             //初始化 time3 计数器配置
	//这行放在 TIM_TimeBaseInit 后面，避免开机就进入中断
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);	
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE );                   //允许更新中断
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;             //中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  	//主优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;          //从优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        		//使能通道中断
	NVIC_Init(&NVIC_InitStructure);                            	//初始化中断
	TIM_Cmd(TIM4, DISABLE);                                      //timer3 使能
}

extern uint32_t rx_len ;

////===============================
////TIM3 中断
////===============================
//void TIM3_IRQHandler(void)
//{ 
//  if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
//    { 
//		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
//		TIM_Cmd(TIM3, DISABLE);
//		rx_len = 0;
//		//这里可以发送一个消息
//		//这里不能申请消息。
//    } 
//} 

//===============================
//开始计时1 ms
//===============================
 void start_tim4_ms(uint16_t num)
 {
	 TIM_SetAutoreload(TIM4, num*2);//改变重装值
	 TIM_SetCounter(TIM4,0);		//清除计数器值
	 TIM_Cmd(TIM4, ENABLE);

 }

//===============================
//开始计时1 ms
//===============================
 void start_tim3_ms(uint16_t num)
 {
	 TIM_SetAutoreload(TIM3, num*2);//改变重装值
	 TIM_SetCounter(TIM3,0);		//清除计数器值
	 TIM_Cmd(TIM3, ENABLE);

 }
//===============================
//开始计时1 s
//===============================
 void start_tim3_s(uint8_t num)
 {
	TIM_SetCounter(TIM3,2000*num);
	TIM_SetAutoreload(TIM3, 2000*num);
	TIM_Cmd(TIM3, ENABLE);
 }

