/*
MDB 系统框架

*/
#include "mdb_header.h"
#include "header.h"
#include "mdb_driver.h" 
#include "math.h"

/*
链表节点
*/
mdbListNode  firstNode;
mdbListNode * listHead; //MDB轮询链表的表头、
mdbListNode * curNode;  //指向当前被轮询设备的结构体指针
int32_t  cycle_s_time,time_consuming;  //一次轮询的开始时间，一次轮询的耗时。

/*
空闲函数
*/
void cycle_time_idle(void)//控制一个循环在100到120毫秒之间
{
	uint8_t i = 0;
	time_consuming = OSTimeGet() - cycle_s_time;
	time_consuming = abs(time_consuming);
	while(i < (Min_CYCLE_COST / (MAX_CYCLE_COST - Min_CYCLE_COST)) && time_consuming < Min_CYCLE_COST)
	{
		OSTimeDly(MAX_CYCLE_COST - Min_CYCLE_COST);
		time_consuming = OSTimeGet() - cycle_s_time;
		time_consuming = abs(time_consuming);
		i++;
	}
//	while((OSTimeGet() > cycle_s_time) && (OSTimeGet() - cycle_s_time < 180) )
//	{
//		OSTimeDly(30);
//	}
//	while((OSTimeGet() < cycle_s_time) && (OSTimeGet() - (0xff - cycle_s_time) < 180))
//	{
//		OSTimeDly(30);
//	}
	cycle_s_time = OSTimeGet();
}

/**********************************************************************************************************
** 函数名称 ：MDB_ICPInit()
** 函数功能 ：MDB总线初始化函数
** 入口参数 ：无
** 出口参数 ：无
**********************************************************************************************************/
static void MDB_ICPInit(void)
{
	uart4_init();//串口初始化
	
	//建立MDB总线轮询时使用的循环链表。
	firstNode.EQIU_FUNC=cycle_time_idle;
	listHead=&firstNode; //表头指针指向first_node
	firstNode.next=listHead;
	firstNode.prev=listHead;
}
/**********************************************************************************************************
** 函数名称 ：RegistMDBEqiu
** 函数功能 ：MDB设备注册到MDB总线
** 入口参数 ：equ 指向被注册的MDB设备结构体的指针。
** 出口参数 ：无
**********************************************************************************************************/
void register_mdb_equ(mdbListNode* equ) 
{
	//表尾插入。
	equ->prev=listHead->prev; //被注册的节点的上一个节点为原表尾节点。
	equ->prev->next=equ;      //原表尾节点的下一个节点为被注册节点
	equ->next=listHead;       //被注册节点的下一个节点为表头
	listHead->prev=equ;       //表头的上一个节点为被注册节点
}
/*********************************************************************************************************
** 函数名称 ：MDB_ICP_DATABUS_RUN
** 函数功能 ：MDB总线运行函数 该函数的作用就是将总线的控制权轮流分配给各个MDB设备驱动程序。
			  分配方式是非抢占的，这点需要特别注意，一个设备的总线控制函数如果一直不返回，
			  则其他设备始终得不到总线的控制权。
** 入口参数 ：dat	要发送的数据
** 出口参数 ：无
*********************************************************************************************************/

void MDB_BUS(void)
{
	MDB_ICPInit();//总线初始化
	CoinChangerInit();
	BillValtdatorInit();
	
	curNode = listHead; //当前指针指向表头;
//	OSTimeDly(OS_TICKS_PER_SEC*4);//等待纸硬币器设备自身启动复位。
	cycle_s_time = OSTimeGet();//每次循环计时
	OSTimeDly(Min_CYCLE_COST);
	while(1)
	{
		curNode->EQIU_FUNC();
		curNode = curNode->next;
	}
}


