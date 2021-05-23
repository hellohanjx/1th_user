#ifndef __RTC_H
#define __RTC_H

#include "stdint.h"

typedef struct   
{ 
  volatile uint8_t hour; 
  volatile uint8_t min; 
  volatile uint8_t sec;   
  //公历日月年周
  volatile uint16_t   w_year; 
  volatile uint8_t    w_month;
  volatile uint8_t    w_date; 
  volatile uint8_t    week; 
}_calendar_obj; 

extern volatile _calendar_obj calendar;

uint8_t USART_Scanf(uint32_t value);
uint32_t Time_Regulate(void);
void Time_Adjust(void);
void Time_Display(uint32_t TimeVar);
void Rtc_Init(void);
void bkp_flash_reset(void);
bool set_rtc_time(uint8_t type, uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec);


#endif
