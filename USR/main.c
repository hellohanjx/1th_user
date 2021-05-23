/*******************************************************************************
** 文件名: 		main.c
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 5.10.0
** 作者: 		hanjx
** 生成日期: 	2017-3-22
** 相关文件:		无
** 修改日志：	2013-11-25
*******************************************************************************/
#include "header.h"
#include "stm32_lib.h"

/*
任务堆栈
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

#define FLASH_APP_ADDR		(0x08010000)  	//用户程序地址
/*******************************************************************************
  * @函数名称	main
  * @函数说明   主函数 
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
  *****************************************************************************/
int  main (void){
	
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, FLASH_APP_ADDR); 	//重置向量表地址
	__set_PRIMASK(0); 			//bootloader 关掉了中断，这里打开
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);//中断优先级分组

	RCC_Configuration();		//时钟初始化(SystemInit 已经初始化过) 
	OSInit();      				//usos ii初始化
	
	OSTaskCreateExt(AppTask1,(void *)0,
					(OS_STK *)&AppTask1Stk[APP_TASK1_STK_SIZE-1],
					APP_TASK1_PRIO,APP_TASK1_PRIO,(OS_STK *)&AppTask1Stk[0],
					APP_TASK1_STK_SIZE, (void *)0,OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);		//创建任务1
	OSStart();      //开始任务调度
}


static  void  AppTaskCreate(void)
{
	OSTaskCreateExt(AppTask2,(void *)0,
					(OS_STK *)&AppTask2Stk[APP_TASK2_STK_SIZE-1],
					APP_TASK2_PRIO,APP_TASK2_PRIO,(OS_STK *)&AppTask2Stk[0],
					APP_TASK2_STK_SIZE,(void *)0, OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);		//创建任务2
	
	OSTaskCreateExt(AppTask3,(void *)0,
					(OS_STK *)&AppTask3Stk[APP_TASK3_STK_SIZE-1],
					APP_TASK3_PRIO,APP_TASK3_PRIO,(OS_STK *)&AppTask3Stk[0],
					APP_TASK3_STK_SIZE,(void *)0, OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);		//创建任务3
	
	OSTaskCreateExt(AppTask4,(void *)0,
					(OS_STK *)&AppTask4Stk[APP_TASK4_STK_SIZE-1],
					APP_TASK4_PRIO,APP_TASK4_PRIO,(OS_STK *)&AppTask4Stk[0],
					APP_TASK4_STK_SIZE,(void *)0, OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);		//创建任务4
	OSTaskCreateExt(AppTask5,(void *)0,
					(OS_STK *)&AppTask5Stk[APP_TASK5_STK_SIZE-1],
					APP_TASK5_PRIO,APP_TASK5_PRIO,(OS_STK *)&AppTask5Stk[0],
					APP_TASK5_STK_SIZE,(void *)0, OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);		//创建任务5
	OSTaskCreateExt(AppTask6,(void *)0,
					(OS_STK *)&AppTask6Stk[APP_TASK6_STK_SIZE-1],
					APP_TASK6_PRIO,APP_TASK6_PRIO,(OS_STK *)&AppTask6Stk[0],
					APP_TASK6_STK_SIZE,(void *)0, OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);		//创建任务6
	OSTaskCreateExt(AppTask7,(void *)0,
					(OS_STK *)&AppTask7Stk[APP_TASK7_STK_SIZE-1],
					APP_TASK7_PRIO,APP_TASK7_PRIO,(OS_STK *)&AppTask7Stk[0],
					APP_TASK7_STK_SIZE,(void *)0, OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);		//创建任务7
	OSTaskCreateExt(AppTask8,(void *)0,
					(OS_STK *)&AppTask8Stk[APP_TASK8_STK_SIZE-1],
					APP_TASK8_PRIO,APP_TASK8_PRIO,(OS_STK *)&AppTask8Stk[0],
					APP_TASK8_STK_SIZE,(void *)0, OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);		//创建任务8
}


/*
用户系统初始化
*/
static void user_system_init(void)
{
	word_init(); //系统语言配置文件
	//硬件初始化
	SysTick_Configuration(); 	//滴答时钟初始化(ucos系统定时器初始化)
	Gpio_Configuration();		//初始化 GPIO
//	Fsmc_Sram_Init();			//初始化 片外SRAM
	lcd_init();					//初始化 LCD
	I2C_EE_Init();				//初始化 I2c
	Spi_Flash_Configuration();	//初始化 SPI FLASH
	bsp_bkp_init();				//bkp区初始化
	Rtc_Init();					//初始化 RTC 时钟；并进行开机数据判断
	Mem_Init();					//初始化 内存空间
	uart_485_config();			//初始化 485总线
	log_init();					//日志接口初始化
	bsp_ireader_init();			//读卡器总线
	bsp_commucation_init();		//通讯总线初始化
	TIM3_Init();				//定时器3初始化
	
}

volatile uint32_t t1,t2;
/*
初始化与MDB总线任务
*/
volatile uint16_t bkptmp[42];
volatile uint8_t it;
volatile uint32_t cot;

static  void  AppTask1 (void *p_arg)
{
	user_system_init();
	AppTaskCreate();//创建其他任务

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
键盘任务
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
状态机任务
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
驱动板任务
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
刷卡器任务
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
通讯任务
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
时间助手任务
用来在状态机中提供计时功能
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
空闲/心跳任务
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
