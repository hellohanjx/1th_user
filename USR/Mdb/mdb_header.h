/*
mdb头文件
提供对外使用的方法与变量

*/
#ifndef MDB_H
#define MDB_H


#include "stdint.h"
#include "ucos_ii.h"

#define ACK 0x00

#define RET 0xaa

#define NAK 0xff

#define DROP 0x01

#define MDB_POLL_CYCLE   70  //轮询周期

#define NO_RESPONSE 0x80 //*5.0毫秒*/

#define MDB_RECEIVE_ACK 0x78   //接收到ACK

#define MDB_RECEIVE_NAK 0x77   //接收到NAK

#define MDB_RECEIVE_DATA 0x79 //MDB接收正确

#define MDB_RECEIVE_DROP 0x76   //接收到DROP

#define MDB_MAX_DATALEN 36 /*数据块最大长度*/

#define ERR_TIME_OUT 0x81 /*超时*/

#define ERR_NO_MODE_BIT 0x82/*未收到最后字节*/

#define ERR_CHECKSUM 0x83 /*校验和错*/

#define MAX_SENDTIMES OS_TICKS_PER_SEC*2   //最大超时时间2S

#define MAX_CYCLE_COST    180    //180   循环最大耗时180MS
 
#define Min_CYCLE_COST    120    //150    循环最小耗时150MS

#define cmd_status_need  0  //通知轮询 需要执行POLL以外的命令
#define cmd_status_start 1  //通知轮询，监视命令的执行情况
#define cmd_status_end   2  //轮询监视到命令的执行已完成时更改至该状态
#define cmd_status_complete 3

typedef void (*EQIU_COM_FUNC)(void );

typedef struct mdbListNode      //循环链表指针
{
	EQIU_COM_FUNC EQIU_FUNC;      //本次会话的控制函数。
 	uint8_t         eqiu_status;    //0禁能，1正常，2没有连接,3失去连接,4故障
	uint8_t         cmd_status;
	uint8_t         cmd_type;
	struct mdbListNode *prev;
	struct mdbListNode *next;
}mdbListNode;


void MDB_BUS(void);
void register_mdb_equ(mdbListNode* equ);




#endif

