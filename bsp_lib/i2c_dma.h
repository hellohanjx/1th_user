#ifndef __I2C_DMA_H
#define __I2C_DMA_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>
/* Exported types ------------------------------------------------------------*/


typedef enum 
{
  FinishFlag                  = 0,  
  BeginFlag                         = (1<<1),
  ProcessFlag                 = (1<<2),
  ExitFlag                                 = (1<<3),
        OffestDoneFlag        = (1<<4),
        DirectionFlag                = (1<<5),
}I2C_STATE;

void  I2C_MasterCommunication(uint8_t direction,uint16_t slave_addr,uint8_t offset,uint8_t* buffer,uint32_t length);

#endif
