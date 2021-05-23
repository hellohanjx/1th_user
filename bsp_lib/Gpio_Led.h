#ifndef _GPIO_LED_H_
#define _GPIO_LED_H_ 

#include "stm32f10x_conf.h"

/*****led1½Ó¿ÚÉùÃ÷*****/
#define GPIO_LED            GPIOF
#define LED_BREATH			GPIO_Pin_6
#define LED_USB				GPIO_Pin_7
#define LED_DOWN			GPIO_Pin_8

#define BEEP				GPIO_Pin_8

void GpioLed_Init(void);
void LED_Display(void);
void LED_Toggle_On(uint8_t led);
void LED_Toggle_Off(uint8_t led);

#endif
