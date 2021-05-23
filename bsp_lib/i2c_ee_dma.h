#ifndef __I2C_EE_DMA_H
#define __I2C_EE_DMA_H

#include "stdint.h"

#define I2C_SPEED              200000	//i2c速度
#define I2C1_ADDRESS7    		0x01	//I2C1 自身地址
#define EEPROM_ADDRESS 			0xA0	//I2C1 从设备地址
#define AT24Cxx     			512  	//AT24C512 型号


typedef enum i2c_result
{
  NO_ERR  = 0,  
  TIMEOUT = 1,
  BUS_BUSY = 2,
  SEND_START_ERR = 3,
  ADDR_MATCH_ERR = 4,
  ADDR_HEADER_MATCH_ERR = 5,
  DATA_TIMEOUT = 6,
  WAIT_COMM = 7,
  STOP_TIMEOUT = 8
}I2C_Result;

typedef enum i2c_state
{
  COMM_DONE  = 0,  		//通讯结束
  COMM_PRE = 1,			//准备通讯
  COMM_IN_PROCESS = 2,	//通讯中
  CHECK_IN_PROCESS = 3,
  COMM_EXIT = 4 		//失败退出通讯
}I2C_STATE;

void I2C_EE_Init(void);
void I2C_EE_WriteBuffer(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite);
void I2C_EE_ReadBuffer(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead);

uint8_t Test_I2c1(void);

#endif
