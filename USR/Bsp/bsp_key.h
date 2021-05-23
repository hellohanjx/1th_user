/*
键盘驱动头文件

*/

#ifndef _KEY_H_
#define _KEY_H_

#include "configure.h"

#if SCREEN == V_SCREENRA8875 || YICHU_DOOR == 1	//易触机器门碰是相反的
	#define DOOR		(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1))	//PA1
#else
	#define DOOR		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)		//PA1
#endif

#define WAKE_UP		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) 	//pA0

#define USER_1		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2) 	//PE2
#define USER_2		GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7) 	//PB7
#define USER_3		GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6) 	//PB6
#define USER_4		GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5) 	//PB5

#define KEY1_R		GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_3) 	//PC3
#define KEY2_R		GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2) 	//PC2
#define KEY3_R		GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1) 	//PC1
#define KEY4_R		GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0) 	//PC0

#define KEY5_W(x)		(x ? GPIO_SetBits(GPIOF,GPIO_Pin_10) : GPIO_ResetBits(GPIOF,GPIO_Pin_10))	//PF10
#define KEY6_W(x)		(x ? GPIO_SetBits(GPIOF,GPIO_Pin_9)  : GPIO_ResetBits(GPIOF,GPIO_Pin_9))	//PF9
#define KEY7_W(x)		(x ? GPIO_SetBits(GPIOF,GPIO_Pin_8)  : GPIO_ResetBits(GPIOF,GPIO_Pin_8))	//PF8
#define KEY8_W(x)		(x ? GPIO_SetBits(GPIOF,GPIO_Pin_7)  : GPIO_ResetBits(GPIOF,GPIO_Pin_7))	//PF7

//按键值
#define USER1_VAL	100
#define USER2_VAL	101
#define USER3_VAL	102
#define USER4_VAL	103
#define WAKEUP_VAL		104
#define DOOR_OPEN_VAL	105
#define DOOR_CLOSE_VAL	106
#define KEY_DELETE_VAL 	107
#define KEY_UP_VAL   	108
#define KEY_DOWN_VAL 	109


void Key_Run(void);



#endif

