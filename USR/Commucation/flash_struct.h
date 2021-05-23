/*
spi flash 应用系统框架头文件

*/

#ifndef _FLASH_STRUCT_H_
#define _FLASH_STRUCT_H_

#include "stdint.h"

#define FLASH_4M

#define FLASH_USER_FLAG		0xAAAAAAAC	//flash标志，如果不符合标志，则清空数据，擦除flash

#ifdef FLASH_2M
	#define  FLASH_LSB_ADDR  			0x000000 	//队列起始地址
	#define  FLASH_QUENE_SPACE    	 	0xFD000  	//队列空间大小
	#define  FLASH_QUEUE_SECTOR_SIZE   	0xFD  		//队列的存储扇区数
	#define	 FLASH_USERFLAG_ADDR 		0x1FFFFA 	//新存储结构flash标志地址
	#define  FLASH_LAST_SECTOR		 	0x1FF	  	//最后一扇区编号
	#define  FLASH_BLACK_LIST_OFFSET	 0x100	  	//黑名单偏移地址
#else
	#define  FLASH_LSB_ADDR  			0x000000  	//队列起始地址
	#define  FLASH_QUENE_SPACE     		0x27D000  	//队列空间大小
	#define  FLASH_QUEUE_SECTOR_SIZE   	0x27D	   	//队列的存储扇区数
	#define	 FLASH_USERFLAG_ADDR 		0x3FFFFA	//新存储结构flash标志地址
	#define  FLASH_LAST_SECTOR		 	0x3FF		//最后一扇区编号
	#define  FLASH_BLACK_LIST_OFFSET	0x280		//黑名单偏移地址
#endif

#define  FLASH_FULL   0X03;
#define  FLASH_EMPTY  0X02;
#define  FLASH_OK     0x01;
#define  FLASH_ERR    0x00;

uint8_t flash_poweron_init(void);
void flash_recovery(void);
void save_to_flash(volatile uint8_t* str);
uint8_t load_from_flash(volatile uint8_t *str,volatile uint32_t *used_lenth);
void delet_commucation_data(volatile uint32_t Raddr);
uint8_t use_size(void);

#endif

