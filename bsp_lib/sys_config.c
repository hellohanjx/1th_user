#include "header.h"
#include "stm32_lib.h"

/********************************************************************************************
*�������ƣ�void RCC_Configuration(void)
*��ڲ�������
*���ڲ�������
*����˵����ϵͳʱ�ӳ�ʼ������
*	       RCC_HSICmd(ENABLE);//ʹ���ڲ����پ��� ;
* 	       RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);//ѡ���ڲ�����ʱ����Ϊϵͳʱ��SYSCLOCK=8MHZ	
*	       RCC_HCLKConfig(RCC_SYSCLK_Div1);       //ѡ��HCLKʱ��ԴΪϵͳʱ��SYYSCLOCK
*  	       RCC_PCLK1Config(RCC_HCLK_Div4);        //APB1ʱ��Ϊ2M 
*  	       RCC_PCLK2Config(RCC_HCLK_Div4);        //APB2ʱ��Ϊ2M
*  	       RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);//ʹ��APB2����GPIOBʱ��
*******************************************************************************************/
void RCC_Configuration(void) 
{
	/* RCC system reset(for debug purpose) */
  	RCC_DeInit();

  	/* Enable HSE */
  	RCC_HSEConfig(RCC_HSE_ON);	//�����ⲿ���߾���

  	/* Wait till HSE is ready */
  	while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);

    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);	//ʹ��Ԥȡָ����

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);	//����flash ��ʱʱ��
 
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);	//����AHB ʱ�� = ϵͳʱ��
  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1);		//APB2 ʱ��= HCLK

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);		//APB1 ʱ��= HCLK/2

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);	//ѡ���ⲿ����9��Ƶ

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);		//ʹ�� PLL

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);	//���RCC��־

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);			//ѡ��RCCΪPLLʱ��

    /* Wait till PLL is used as system clock source */
		while(RCC_GetSYSCLKSource() != 0x08);	//�ȴ� PLLʱ�� �ȶ�
	
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
//ϵͳʱ�Ӷ�ʱ����ʼ��
void  SysTick_Configuration(void)
{
    RCC_ClocksTypeDef  rcc_clocks;
    uint32_t	       cnts;

    RCC_GetClocksFreq(&rcc_clocks);	//��ȡʱ��Ƶ��
    cnts = (INT32U)rcc_clocks.HCLK_Frequency/OS_TICKS_PER_SEC;
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	SysTick_Config(cnts);
}

void BUZZER_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//������PA8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);
}

void LED_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//LED ������ PC4 ���صơ�PC5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	GPIO_SetBits(GPIOC,GPIO_InitStructure.GPIO_Pin);
	
	//LED USBָʾ��  PA11
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
						 | RCC_APB2Periph_AFIO , DISABLE);//ע���������ʱ����ʵֻ�� I2C1 ������
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
					 | RCC_APB2Periph_AFIO , ENABLE);//ע���������ʱ����ʵֻ�� I2C1 ������
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


