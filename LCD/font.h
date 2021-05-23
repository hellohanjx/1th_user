/*
字库头文件
*/

#ifndef __FONT_H__
#define __FONT_H__

#include "stdint.h"

#define FONT_48		48
#define FONT_64		64
#define FONT_96		96

enum LCD_TYPE{LCD_HZ , LCD_EN};//汉字/英文

//常用ASCII表
//偏移量32
//ASCII字符集
//偏移量32
extern const unsigned char asc2_48[];
extern const char LT_48[];
extern const unsigned char asc2_64[];
extern const char LT_64[];
extern const unsigned char asc2_96[];
extern const char LT_96[];



#endif
