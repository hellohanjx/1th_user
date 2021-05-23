/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                      APPLICATION CONFIGURATION
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   STM3210B-EVAL Evaluation Board
*
* Filename      : app_cfg.h
* Version       : V1.00
* Programmer(s) : Brian Nagel
*********************************************************************************************************
*/

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__

/*
*********************************************************************************************************
*                                       ADDITIONAL uC/MODULE ENABLES
*********************************************************************************************************
*/

#define  uC_PROBE_OS_PLUGIN              DEF_DISABLED            /* DEF_ENABLED = Present, DEF_DISABLED = Not Present        */
#define  uC_PROBE_COM_MODULE             DEF_DISABLED

/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/
#define  APP_TASK1_PRIO                 4
#define  APP_TASK2_PRIO                 5
#define  APP_TASK3_PRIO                 6
#define  APP_TASK4_PRIO                 7     
#define  APP_TASK5_PRIO                	8
#define  APP_TASK6_PRIO                	9
#define  APP_TASK7_PRIO                 10
#define  APP_TASK8_PRIO                 11

#define  OS_PROBE_TASK_PRIO                     (OS_LOWEST_PRIO-4) 
#define  OS_PROBE_TASK_ID                       (OS_LOWEST_PRIO-4) 

#define  OS_TASK_TMR_PRIO              (OS_LOWEST_PRIO - 2)

/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*********************************************************************************************************
*/


#define  APP_TASK1_STK_SIZE				256
#define  APP_TASK2_STK_SIZE             256
#define  APP_TASK3_STK_SIZE             256
#define  APP_TASK4_STK_SIZE             256
#define  APP_TASK5_STK_SIZE             256
#define  APP_TASK6_STK_SIZE             256
#define  APP_TASK7_STK_SIZE             256
#define  APP_TASK8_STK_SIZE             256

#define  APP_TASK_PROBE_STR_STK_SIZE         64

#define  OS_PROBE_TASK_STK_SIZE              64

/*
*********************************************************************************************************
*                               uC/Probe plug-in for uC/OS-II CONFIGURATION
*********************************************************************************************************
*/

#define  OS_PROBE_TASK                         1                /* Task will be created for uC/Probe OS Plug-In             */
#define  OS_PROBE_TMR_32_BITS                  0                /* uC/Probe OS Plug-In timer is a 16-bit timer              */
#define  OS_PROBE_TIMER_SEL                    2
#define  OS_PROBE_HOOKS_EN                     1




#endif
