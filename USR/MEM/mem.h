#ifndef MEM_H_
#define MEM_H_

#include "stdint.h"


/*
¶¯Ì¬ÄÚ´æ·ÖÅä
*/
#define MEM_32_SIZE 	(32) 
#define MEM_64_SIZE 	(64)
#define MEM_128_SIZE 	(128) 
#define MEM_512_SIZE 	(512)
#define MEM_2048_SIZE 	(2048)

#define MEM_32_NUM     30
#define MEM_64_NUM     30
#define MEM_128_NUM    50  
#define MEM_512_NUM    5   
#define MEM_2048_NUM   2

#define MEM_MAX_SIZE (MEM_32_SIZE*MEM_32_NUM + MEM_64_SIZE*MEM_64_NUM + MEM_128_SIZE*MEM_128_NUM + MEM_512_SIZE*MEM_512_NUM + MEM_2048_NUM*MEM_2048_SIZE)

typedef struct MEM_BLOCK
{
	uint8_t mem_32_buf[MEM_32_NUM][MEM_32_SIZE];
	uint8_t mem_64_buf[MEM_64_NUM][MEM_64_SIZE];
	uint8_t mem_128_buf[MEM_128_NUM][MEM_128_SIZE];
	uint8_t mem_512_buf[MEM_512_NUM][MEM_512_SIZE];
	uint8_t mem_2048_buf[MEM_2048_NUM][MEM_2048_SIZE];
	
}MEM_BLOCK;

void Mem_Init(void);
void *_malloc(uint16_t size,uint8_t*err);
void _free(void* ptr);

#endif
