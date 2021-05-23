/*******************************************************************************
** �ļ���: 		stm32f10x_conf.h
** �汾��  		1.0
** ��������: 	RealView MDK-ARM 4.20
** ����:		���������ļ�
** ����ļ�:	��
** �޸���־��	2017-03-25   
** �����ĵ�		2017-03-22
*******************************************************************************/

/* ��ֹ�ض��� ----------------------------------------------------------------*/
#ifndef __STM32F10x_CONF_H
#define __STM32F10x_CONF_H


//=============================
//�궨��
//=============================
#undef NULL
#define NULL	0

/*
C ���ļ� �ſ���ͷ�ȽϺá�
*/
#include "string.h"			
#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"
//#include "arm_math.h"



/*****
ȫ�ֺ�����
*****/


#include "global.h"
#include "configure.h"
/*
���Ӧ�þ��Ƕ�����
*/
#ifdef  USE_FULL_ASSERT

/**
  * @˵��   assert_param ���ڲ������.
  * @����  expr: ����expr��flase, ������� assert_param
  *   ������������ڵ�Դ�ļ��������ڵ�����
  *   ����expr �� true, ��������ֵ.
  * @����ֵ ��
  */
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* ��������------------------------------------------------------- */
//  void assert_failed(uint8_t* file, uint32_t line)
//{
//	  
//	  
//  }
#else
  #define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

  
  
 /*
  �û���־���
  */
#ifdef  USER_LOG
    #define log_output(expr) ((expr) ? (void)0 : log_fail((uint8_t *)__FILE__, __LINE__))
#else
  #define log_output(expr) ((void)0)
#endif
  
  
  
  
#endif /* __STM32F10x_CONF_H */

  

/***********************************�ļ�����***********************************/
