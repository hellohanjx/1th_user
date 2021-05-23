/*
读卡设备对外接口头文件

*/

#ifndef _CARD_READER_HEADER_H_
#define _CARD_READER_HEADER_H_


#include "stdint.h"



/*
读卡器到业务层消息
*/
enum{ ICREADER_INIT , ICREADER_STATE_CHANEGE};

/*
业务层到读卡器消息，指令类型
*/
enum{ ICREADER_FIND	= 1, ICREADER_PAY = 2};

/*
描述卡信息
*/
typedef struct IC
{
	uint8_t user;	//用户身份识别(1管理员)
	uint8_t useRang;//使用范围
	uint8_t state;//读卡器状态  
	
	char    PrintSnr[13];        	//物理卡号字符型
	char   PrintSno[13];        //卡表面印刻号  
	uint8_t   PsamNno[8];          //终端PSAM卡号  
	uint8_t   Snr[8];                 //物理卡号
	uint8_t   LogicSnr[8];            //逻辑卡号
	uint8_t   CardType;               //卡类型
	uint8_t   CardsType;              //卡子类型
	uint8_t   CardStatus;             //卡状态
	uint32_t balance;			//卡余额
	uint8_t	daymaxpay;			//当天最大消费次数

}IC;

extern IC cIC;
void ireader_task(void);
uint8_t send_icreader_msg(uint8_t cmdtype, uint8_t value);

#endif
