#ifndef _SYSTEM_CONFIG_H_
#define _SYSTEM_CONFIG_H_

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

typedef enum{OFF = 0, ON = !OFF}SWITCH;					//开关类型
typedef enum{PRESS = 0, UNPRESS = !PRESS}PRESSED;		//按下类型

typedef enum{LOW = 0, HIGH = !LOW}P_STATE;

#define LED_BREATH(X)	(X ? GPIO_SetBits(GPIOC,GPIO_Pin_4) : GPIO_ResetBits(GPIOC,GPIO_Pin_4))	//PC4
#define LED_DOWN(X)		(X ? GPIO_SetBits(GPIOC,GPIO_Pin_5) : GPIO_ResetBits(GPIOC,GPIO_Pin_5))	//PC5
#define LED_USB(X)		(X ? GPIO_SetBits(GPIOA,GPIO_Pin_11) : GPIO_ResetBits(GPIOA,GPIO_Pin_11))//PA11

#define BEEP(X)		(X ? GPIO_SetBits(GPIOA,GPIO_Pin_8) : GPIO_ResetBits(GPIOA,GPIO_Pin_8))		//PA8


void resetFromBootLoader(void);
void RCC_Configuration(void);
void NVIC_Configuration(void);
void Gpio_Configuration(void);
void UART_Init(void);
void SysTick_Configuration(void);
void BKP_Configuration(void);
#endif
