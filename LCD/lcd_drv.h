/*
lcd模块内部头文件

*/


#ifndef __LCD_DRV_H__
#define __LCD_DRV_H__

#include "stdint.h" 

/*
屏幕信息结构体
*/
typedef struct SCREEN_INFO {
	uint16_t xSize;
	uint16_t ySize ;
	uint16_t background ;
	uint16_t foreground ;
	uint16_t waringColor;
	uint16_t promptColor;
	uint16_t hightLightBackground;
	uint16_t hightLightforeground;
	uint8_t direction;
}SCREEN_INFO;

void bsp_lcd_hard_congfigure(void);
void bsp_lcd_set_directrion(uint8_t type ,uint16_t *x, uint16_t *y);
void bsp_lcd_block_select(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend) ;
void DrawPixel(uint16_t x, uint16_t y, int Color);
uint16_t bsp_lcd_get_pixel( uint16_t x, uint16_t y);

volatile uint16_t *bsp_get_reg(void);
volatile uint16_t *bsp_get_ram(void);
uint16_t bsp_get_xsize(void);
uint16_t bsp_get_ysize(void);

void print_lattice(const uint8_t *lattice,uint32_t xSize,uint32_t ySize,uint16_t x,uint16_t y,uint16_t color,uint16_t background);
void lcd_print_lattic(const uint8_t *lattice,uint32_t xSize,uint32_t ySize,uint16_t x,uint16_t y,uint16_t forcground,uint16_t background);


#endif

