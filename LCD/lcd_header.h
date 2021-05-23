#ifndef __ILI9325_H__
#define __ILI9325_H__

#include "stdint.h"
#include "font_header.h"
#include "word.h"
/*
16bit 565 颜色
*/
#define   BLACK                0x0000                // ??:    0,   0,   0 //
#define   BLUE                 0x001F                // ??:    0,   0, 255 //
#define   GREEN                0x07E0                // ??:    0, 255,   0 //
#define   CYAN                 0x07FF                // ??:    0, 255, 255 //
#define   RED                  0xF800                // ??:  255,   0,   0 //
#define   MAGENTA              0xF81F                // ??:  255,   0, 255 //
#define   YELLOW               0xFFE0                // ??:  255, 255, 0   //
#define   WHITE                0xFFFF                // ??:  255, 255, 255 //
#define   NAVY                 0x000F                // ???:  0,   0, 128 //
#define   DGREEN               0x03E0                // ???:  0, 128,   0 //
#define   DCYAN                0x03EF                // ???:  0, 128, 128 //
#define   MAROON               0x7800                // ???:128,   0,   0 //
#define   PURPLE               0x780F                // ??:  128,   0, 128 //
#define   OLIVE                0x7BE0                // ???:128, 128,   0 //
#define   LGRAY                0xC618                // ???:192, 192, 192 //
#define   DGRAY                0x7BEF                // ???:128, 128, 128 //

#define		RGB_GRAY		0xcccccc	//????
#define		RGB_DGRAY		0x999999	//???







void lcd_fill_color(uint16_t xStart,uint16_t yStart,uint16_t xLong,uint16_t yLong,uint16_t Color);
void bsp_lcd_fill_pic16(uint16_t x, uint16_t y,uint16_t pic_H, uint16_t pic_V, const uint16_t *pic);













void lcd_set_background(uint16_t bk);
void lcd_set_forceground(uint16_t col);
uint16_t lcd_get_xsize(void);
uint16_t lcd_get_ysize(void);
uint16_t transfer24Bit_to_16Bit(uint32_t color);
void lcd_fill_color(uint16_t xStart,uint16_t yStart,uint16_t xLong,uint16_t yLong,uint16_t Color);
void show_ercode(uint16_t x, uint16_t y, uint16_t xylong, uint8_t scale , uint8_t dat[][41]);
void clear_ercode(uint16_t x, uint16_t y, uint16_t xylong, uint8_t scale);




void lcd_init(void);
void lcd_clear(void);
void lcd_show_character(const WORD *character);
void lcd_show_money(uint32_t money,uint16_t x,uint16_t y, uint16_t foreground, uint16_t background, const BX_FONT font);
void lcd_show_string(const char* str,uint16_t x ,uint16_t y, uint16_t foreground, uint16_t background, const BX_FONT font);
void lcd_show_time(const WORD *time);
void lcd_show_num(uint32_t num,uint16_t x,uint16_t y ,uint16_t foreground, uint16_t background, const BX_FONT font ,uint8_t type, uint8_t size);
void lcd_clear_character(const WORD *character);



#endif


