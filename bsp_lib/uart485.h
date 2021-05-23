#ifndef UART_485_H_
#define UART_485_H_
#include "stdint.h"
typedef bool(*IS_RECV_FINISHED)(volatile uint8_t*,volatile uint8_t*);
typedef struct 
{
	bool    needAck;
	uint8_t cmd_len;
	uint8_t* cmd;
	uint8_t* recv;
	uint8_t  recv_len;
	uint8_t sendCount;
	uint16_t timeout;
	IS_RECV_FINISHED isFinished;
}UART485_CMD_DATA;


void uart_485_config(void);
void bus_Lock_485(void);
void bus_Unlock_485(void);
uint8_t send_to_485Bus(UART485_CMD_DATA* cmd);
#endif

