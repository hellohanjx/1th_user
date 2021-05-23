//===================================
//内存处理
//===================================
#include "header.h"

#define MEM_TYPE_NUM   5  //定义有几种不同存储控件大小的内存


//OS_EVENT* sem_memBlock_lock;//内存分配锁定信号量
OS_MEM* pt_memBlocks[MEM_TYPE_NUM];

//动态内存实际占用的内存空间。
static MEM_BLOCK memBlock;
/**************************************************************************************
** Function name:       void  memInit(void)
** Descriptions:        动态内存初始化
** input parameters:    无
** output parameters:   无
** Returned value:      无
***************************************************************************************/
void  Mem_Init(void)
{
	uint8_t err;
	pt_memBlocks[0]=OSMemCreate(memBlock.mem_32_buf,MEM_32_NUM,MEM_32_SIZE,&err);
	pt_memBlocks[1]=OSMemCreate(memBlock.mem_64_buf,MEM_64_NUM,MEM_64_SIZE,&err);
	pt_memBlocks[2]=OSMemCreate(memBlock.mem_128_buf,MEM_128_NUM,MEM_128_SIZE,&err);
	pt_memBlocks[3]=OSMemCreate(memBlock.mem_512_buf,MEM_512_NUM,MEM_512_SIZE,&err);
	pt_memBlocks[4]=OSMemCreate(memBlock.mem_2048_buf,MEM_2048_NUM,MEM_2048_SIZE,&err);
//	sem_memBlock_lock = OSSemCreate(1);
}

/**************************************************************************************
** Function name:       void*  _malloc(uint16_t size)
** Descriptions:        申请一个大小不小于size 的内存
** input parameters:    size  需要申请的内存大小
** output parameters:   err  申请内存时返回的错误代码
** Returned value:      指向申请到的内存首地址的指针。申请失败返回NULL
***************************************************************************************/
void*  _malloc(uint16_t size,uint8_t*err)
{
	uint8_t i; 
	void* mem;
	*err = OS_ERR_MEM_INVALID_SIZE;
	if(size == 0)
		return NULL;
	if(size > MEM_2048_SIZE)
		return NULL;
	
	for(i = 0; i < MEM_TYPE_NUM; i++)
	{
		if(size < (pt_memBlocks[i]->OSMemBlkSize))
		{
			mem = OSMemGet(pt_memBlocks[i],err);
			if((*err) == OS_ERR_NONE)
			{
				return mem;
			}
		}
	}
	return NULL;

}

/**************************************************************************************
** Function name:       void  _free(void* ptr)
** Descriptions:        释放内存
** input parameters:    *ptr  指向待释放内存首地址的指针
** output parameters:   无
** Returned value:      无
***************************************************************************************/
void  _free(void* ptr)
{
	uint8_t i;
	void* head;
	void* rear;
	if(ptr==NULL)
		return;
	
	for(i=0;i<MEM_TYPE_NUM;i++)
	{
		head = pt_memBlocks[i]->OSMemAddr;
		rear=(void*)(((uint32_t)(pt_memBlocks[i]->OSMemAddr))+(pt_memBlocks[i]->OSMemNBlks)*(pt_memBlocks[i]->OSMemBlkSize));
		if(ptr >= head && ptr < rear)
		{
			OSMemPut(pt_memBlocks[i],ptr);
			return;
		}
	}
}

