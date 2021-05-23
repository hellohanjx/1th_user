/*******************************************************************************
** 文件名: 		stm32f10x_conf.h
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 4.20
** 功能:		工程配置文件
** 相关文件:	无
** 修改日志：	2017-03-25   
** 创建文档		2017-03-22
*******************************************************************************/

/* 防止重定义 ----------------------------------------------------------------*/
#ifndef __STM32F10x_CONF_H
#define __STM32F10x_CONF_H


//=============================
//宏定义
//=============================
#undef NULL
#define NULL	0

/*
C 库文件 放开开头比较好。
*/
#include "string.h"			
#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"
//#include "arm_math.h"



/*****
全局宏设置
*****/


#include "global.h"
#include "configure.h"
/*
这个应该就是断言了
*/
#ifdef  USE_FULL_ASSERT

/**
  * @说明   assert_param 用于参数检查.
  * @参数  expr: 假如expr是flase, 将会调用 assert_param
  *   报告错误发生所在的源文件名和所在的行数
  *   假如expr 是 true, 将步返回值.
  * @返回值 无
  */
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* 函数定义------------------------------------------------------- */
//  void assert_failed(uint8_t* file, uint32_t line)
//{
//	  
//	  
//  }
#else
  #define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

  
  
 /*
  用户日志输出
  */
#ifdef  USER_LOG
    #define log_output(expr) ((expr) ? (void)0 : log_fail((uint8_t *)__FILE__, __LINE__))
#else
  #define log_output(expr) ((void)0)
#endif
  
  
  
  
#endif /* __STM32F10x_CONF_H */

  

/***********************************文件结束***********************************/
