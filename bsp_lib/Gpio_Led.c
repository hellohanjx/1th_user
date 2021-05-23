/******************************************************************
**	  ����103�����壨V2.0��
**	  Gpio�����ļ�
**	  ��    ̳��bbs.openmcu.com
**	  ��    ����www.openmcu.com
**	  ��    �䣺support@openmcu.com
**    ��    ����V2.0
**	  ��    �ߣ�jeansonm
**	  �������:	2013.11.25
********************************************************************/
#include "Gpio_Led.h"
/********************************************************************************************
*�������ƣ�void GpioLed_Init(void)
*��ڲ�������
*���ڲ�������
*����˵����led�Ƴ�ʼ������
*******************************************************************************************/
void GpioLed_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_Init(GPIOE, &GPIO_InitStructure);		 
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  GPIO_Init(GPIOG, &GPIO_InitStructure);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
	

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_LED, &GPIO_InitStructure);

  GPIO_SetBits(GPIO_LED, LED1 | LED2 | LED3 | LED4 | LED5);
}


