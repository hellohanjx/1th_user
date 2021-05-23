/*******************************************************************************
** �ļ���: 		main.c
** �汾��  		1.0
** ��������: 	RealView MDK-ARM 5.10.0
** ����: 		hanjx
** ��������: 	2017-3-22
** ����ļ�:		��
** �޸���־��	2013-11-25
*******************************************************************************/
#include "header.h"
#include "stm32_lib.h"

/*
�����ջ
*/
static OS_STK AppTask1Stk[APP_TASK1_STK_SIZE];
static OS_STK AppTask2Stk[APP_TASK2_STK_SIZE];
static OS_STK AppTask3Stk[APP_TASK3_STK_SIZE];
static OS_STK AppTask4Stk[APP_TASK4_STK_SIZE];
static OS_STK AppTask5Stk[APP_TASK5_STK_SIZE];
static OS_STK AppTask6Stk[APP_TASK6_STK_SIZE];
static OS_STK AppTask7Stk[APP_TASK7_STK_SIZE];
static OS_STK AppTask8Stk[APP_TASK8_STK_SIZE];


static void AppTaskCreate(void);
static void AppTask1(void *p_arg);
static void AppTask2(void *p_arg);
static void AppTask3(void *p_arg);
static void AppTask4(void *p_arg);
static void AppTask5(void *p_arg);
static void AppTask6(void *p_arg);
static void AppTask7(void *p_arg);
static void AppTask8(void *p_arg);

#define FLASH_APP_ADDR		(0x08010000)  	//�û������ַ
/*******************************************************************************
  * @��������	main
  * @����˵��   ������ 
  * @�������   ��
  * @�������   ��
  * @���ز���   ��
  *****************************************************************************/
int  main (void){
	
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, FLASH_APP_ADDR); 	//�����������ַ
	__set_PRIMASK(0); 			//bootloader �ص����жϣ������
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);//�ж����ȼ�����

	RCC_Configuration();		//ʱ�ӳ�ʼ��(SystemInit �Ѿ���ʼ����) 
	OSInit();      				//usos ii��ʼ��
	
	OSTaskCreateExt(AppTask1,(void *)0,
					(OS_STK *)&AppTask1Stk[APP_TASK1_STK_SIZE-1],
					APP_TASK1_PRIO,APP_TASK1_PRIO,(OS_STK *)&AppTask1Stk[0],
					APP_TASK1_STK_SIZE, (void *)0,OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);		//��������1
	OSStart();      //��ʼ�������
}


static  void  AppTaskCreate(void)
{
	OSTaskCreateExt(AppTask2,(void *)0,
					(OS_STK *)&AppTask2Stk[APP_TASK2_STK_SIZE-1],
					APP_TASK2_PRIO,APP_TASK2_PRIO,(OS_STK *)&AppTask2Stk[0],
					APP_TASK2_STK_SIZE,(void *)0, OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);		//��������2
	
	OSTaskCreateExt(AppTask3,(void *)0,
					(OS_STK *)&AppTask3Stk[APP_TASK3_STK_SIZE-1],
					APP_TASK3_PRIO,APP_TASK3_PRIO,(OS_STK *)&AppTask3Stk[0],
					APP_TASK3_STK_SIZE,(void *)0, OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);		//��������3
	
	OSTaskCreateExt(AppTask4,(void *)0,
					(OS_STK *)&AppTask4Stk[APP_TASK4_STK_SIZE-1],
					APP_TASK4_PRIO,APP_TASK4_PRIO,(OS_STK *)&AppTask4Stk[0],
					APP_TASK4_STK_SIZE,(void *)0, OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);		//��������4
	OSTaskCreateExt(AppTask5,(void *)0,
					(OS_STK *)&AppTask5Stk[APP_TASK5_STK_SIZE-1],
					APP_TASK5_PRIO,APP_TASK5_PRIO,(OS_STK *)&AppTask5Stk[0],
					APP_TASK5_STK_SIZE,(void *)0, OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);		//��������5
	OSTaskCreateExt(AppTask6,(void *)0,
					(OS_STK *)&AppTask6Stk[APP_TASK6_STK_SIZE-1],
					APP_TASK6_PRIO,APP_TASK6_PRIO,(OS_STK *)&AppTask6Stk[0],
					APP_TASK6_STK_SIZE,(void *)0, OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);		//��������6
	OSTaskCreateExt(AppTask7,(void *)0,
					(OS_STK *)&AppTask7Stk[APP_TASK7_STK_SIZE-1],
					APP_TASK7_PRIO,APP_TASK7_PRIO,(OS_STK *)&AppTask7Stk[0],
					APP_TASK7_STK_SIZE,(void *)0, OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);		//��������7
	OSTaskCreateExt(AppTask8,(void *)0,
					(OS_STK *)&AppTask8Stk[APP_TASK8_STK_SIZE-1],
					APP_TASK8_PRIO,APP_TASK8_PRIO,(OS_STK *)&AppTask8Stk[0],
					APP_TASK8_STK_SIZE,(void *)0, OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);		//��������8
}


/*
�û�ϵͳ��ʼ��
*/
static void user_system_init(void)
{
	word_init(); //ϵͳ���������ļ�
	//Ӳ����ʼ��
	SysTick_Configuration(); 	//�δ�ʱ�ӳ�ʼ��(ucosϵͳ��ʱ����ʼ��)
	Gpio_Configuration();		//��ʼ�� GPIO
//	Fsmc_Sram_Init();			//��ʼ�� Ƭ��SRAM
	lcd_init();					//��ʼ�� LCD
	I2C_EE_Init();				//��ʼ�� I2c
	Spi_Flash_Configuration();	//��ʼ�� SPI FLASH
	bsp_bkp_init();				//bkp����ʼ��
	Rtc_Init();					//��ʼ�� RTC ʱ�ӣ������п��������ж�
	Mem_Init();					//��ʼ�� �ڴ�ռ�
	uart_485_config();			//��ʼ�� 485����
	log_init();					//��־�ӿڳ�ʼ��
	bsp_ireader_init();			//����������
	bsp_commucation_init();		//ͨѶ���߳�ʼ��
	TIM3_Init();				//��ʱ��3��ʼ��
	
}

volatile uint32_t t1,t2;
/*
��ʼ����MDB��������
*/
volatile uint16_t bkptmp[42];
volatile uint8_t it;
volatile uint32_t cot;

static  void  AppTask1 (void *p_arg)
{
	user_system_init();
	AppTaskCreate();//������������

//	{
//		for(it = 0; it < 32; it++)
//		{
//			bkptmp[it] = BKP_ReadBackupRegister(BKP_DR11 + it*4);
//		}
//		
//	}
//	it = 0;
//	while(1)
//	{
//		uint16_t num;
//		for(it = 0; it < 32; it++)
//		{
//			BKP_WriteBackupRegister(BKP_DR11 + it*4, it);
//			num = BKP_ReadBackupRegister(BKP_DR11 + it*4);
//			if(num != it)
//			{
//				num = num;
//			}
//			cot++;
//		}
//		
//	}
	
	BEEP(ON);
	OSTimeDlyHMSM(0,0,0,100);
	BEEP(OFF);
	OSTimeDlyHMSM(0,0,0,100);
	BEEP(ON);
	OSTimeDlyHMSM(0,0,0,100);
	BEEP(OFF);
	
	
	MDB_BUS();
	
	while(1)
	{
		OSTimeDly(OS_TICKS_PER_SEC);
	}
}
/*
��������
*/
static  void  AppTask2 (void *p_arg)
{
	Key_Run();
	
	while(1)
	{
		OSTimeDlyHMSM(0,0,1,0);	
	}		    
}
/*
״̬������
*/
static  void  AppTask3 (void *p_arg)
{
	FSM_RUN();
	
	while(1)
	{
		OSTimeDly(OS_TICKS_PER_SEC);
	}
}

/*
����������
*/
static  void  AppTask4 (void *p_arg)
{
	drv_task();
	
	while(1)
	{
		OSTimeDly(OS_TICKS_PER_SEC);
	}
}

/*
ˢ��������
*/
static  void  AppTask5 (void *p_arg)
{
	ireader_task();
	
	while(1)
	{
		OSTimeDly(OS_TICKS_PER_SEC);
	}
}

/*
ͨѶ����
*/
static  void  AppTask6 (void *p_arg)
{
	commucation_task(); 
	
	while(1)
	{
		OSTimeDly(OS_TICKS_PER_SEC);
	}
}


/*
ʱ����������
������״̬�����ṩ��ʱ����
*/
static  void  AppTask7 (void *p_arg)
{
	assistant();
	
	while(1)
	{
		OSTimeDly(OS_TICKS_PER_SEC);
	}
}


/*
����/��������
*/
static void AppTask8(void *p_arg)
{
	uint8_t flip = 0;

	while(1)
	{
		LED_BREATH(flip);
		flip =! flip;
		OSTimeDlyHMSM(0,0,1,0);
	}
}
