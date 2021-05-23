#include "header.h"
#include "stm32_lib.h"

/********************************************************************************************
*函数名称：void RCC_Configuration(void)
*入口参数：无
*出口参数：无
*功能说明：系统时钟初始化配置
*	       RCC_HSICmd(ENABLE);//使能内部高速晶振 ;
* 	       RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);//选择内部高速时钟作为系统时钟SYSCLOCK=8MHZ	
*	       RCC_HCLKConfig(RCC_SYSCLK_Div1);       //选择HCLK时钟源为系统时钟SYYSCLOCK
*  	       RCC_PCLK1Config(RCC_HCLK_Div4);        //APB1时钟为2M 
*  	       RCC_PCLK2Config(RCC_HCLK_Div4);        //APB2时钟为2M
*  	       RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);//使能APB2外设GPIOB时钟
*******************************************************************************************/
void RCC_Configuration(void) 
{
	/* RCC system reset(for debug purpose) */
  	RCC_DeInit();

  	/* Enable HSE */
  	RCC_HSEConfig(RCC_HSE_ON);	//设置外部告诉晶振

  	/* Wait till HSE is ready */
  	while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);

    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);	//使能预取指缓存

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);	//设置flash 延时时钟
 
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);	//设置AHB 时钟 = 系统时钟
  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1);		//APB2 时钟= HCLK

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);		//APB1 时钟= HCLK/2

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);	//选择外部晶振，9倍频

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);		//使能 PLL

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);	//检查RCC标志

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);			//选择RCC为PLL时钟

    /* Wait till PLL is used as system clock source */
		while(RCC_GetSYSCLKSource() != 0x08);	//等待 PLL时钟 稳定
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC 
						| RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF 
						| RCC_APB2Periph_GPIOG | RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1, 
						ENABLE);
	
}



void NVIC_Configuration(void)
{
	#ifdef  VECT_TAB_RAM  
	  /* Set the Vector Table base location at 0x20000000 */ 
	  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
	#else  /* VECT_TAB_FLASH  */
	  /* Set the Vector Table base location at 0x08000000 */ 
	  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
	#endif	
}
//系统时钟定时器初始化
void  SysTick_Configuration(void)
{
    RCC_ClocksTypeDef  rcc_clocks;
    uint32_t	       cnts;

    RCC_GetClocksFreq(&rcc_clocks);	//读取时钟频率
    cnts = (INT32U)rcc_clocks.HCLK_Frequency/OS_TICKS_PER_SEC;
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	SysTick_Config(cnts);
}

void BUZZER_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//蜂鸣器PA8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);
}

void LED_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//LED 呼吸灯 PC4 下载灯。PC5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	GPIO_SetBits(GPIOC,GPIO_InitStructure.GPIO_Pin);
	
	//LED USB指示灯  PA11
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_InitStructure.GPIO_Pin);
}



void Gpio_Configuration(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA 
						 | RCC_APB2Periph_GPIOB 
						 | RCC_APB2Periph_GPIOC 
						 | RCC_APB2Periph_GPIOD 
						 | RCC_APB2Periph_GPIOE 
						 | RCC_APB2Periph_GPIOF 
						 | RCC_APB2Periph_GPIOG 
						 | RCC_APB2Periph_AFIO , DISABLE);//注意这个复用时钟其实只在 I2C1 用了下
	GPIO_DeInit(GPIOA);
	GPIO_DeInit(GPIOB);
	GPIO_DeInit(GPIOC);
	GPIO_DeInit(GPIOD);
	GPIO_DeInit(GPIOE);
	GPIO_DeInit(GPIOF);
	GPIO_DeInit(GPIOG);
	GPIO_AFIODeInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA 
					 | RCC_APB2Periph_GPIOB 
					 | RCC_APB2Periph_GPIOC 
					 | RCC_APB2Periph_GPIOD 
					 | RCC_APB2Periph_GPIOE 
					 | RCC_APB2Periph_GPIOF 
					 | RCC_APB2Periph_GPIOG 
					 | RCC_APB2Periph_AFIO , ENABLE);//注意这个复用时钟其实只在 I2C1 用了下
	BUZZER_Configuration();
	LED_Configuration();
}





int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{
	}
	return ch;
}


