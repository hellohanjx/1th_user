/*
状态机文件

*/
#ifndef __WORD_H__
#define __WORD_H__

#include "stdint.h"
#include "font_header.h"
#include "configure.h" 

//信息界面



#if ((SCREEN == TM12864Z) || (SCREEN == TM12864_NO_FONT))

	#define G_HOLDER_FONT			(&font16)	//菜单内容字体
	#define G_HOLDER_ITEM_SIZE		3		//每页显示的内容行数
	#define G_HOLDER_FOREGROUND		WHITE	//内容前景色
	#define G_HOLDER_BACKGROUND		BLUE	//内容背景色
	#define G_HOLDER_CUR_ZI_COLOR	BLACK	//内容当前选项数字颜色
	#define G_HOLDER_CUR_NAME_COLOR	WHITE	//内容当前选项名称颜色

	//显示二维码
	#define G_ERCODE_X		0	
	#define G_ERCODE_Y		0
	#define G_ERCODE_SCALE	1

#elif ( (SCREEN == SCREENILI9806) || (SCREEN == SCREENRA8875))

	#define G_HOLDER_FONT		(&font48)	//菜单内容字体
	#define G_HOLDER_ITEM_SIZE	7			//每页显示的内容行数
	#define G_HOLDER_FOREGROUND		WHITE	//内容前景色
	#define G_HOLDER_BACKGROUND		BLUE	//内容背景色
	#define G_HOLDER_CUR_ZI_COLOR		transfer24Bit_to_16Bit(0xff9900)	//内容当前选项数字颜色
	#define G_HOLDER_CUR_NAME_COLOR	RED									//内容当前选项名称颜色

	//显示二维码
	#define G_ERCODE_X		500	
	#define G_ERCODE_Y		100
	#define G_ERCODE_SCALE	6

#elif (SCREEN == V_SCREENRA8875)

	#define G_HOLDER_FONT		(&font48)	//菜单内容字体
	#define G_HOLDER_ITEM_SIZE	7			//每页显示的内容行数
	#define G_HOLDER_FOREGROUND		WHITE	//内容前景色
	#define G_HOLDER_BACKGROUND		BLUE	//内容背景色
	#define G_HOLDER_CUR_ZI_COLOR		transfer24Bit_to_16Bit(0xff9900)	//内容当前选项数字颜色
	#define G_HOLDER_CUR_NAME_COLOR	RED									//内容当前选项名称颜色

	//显示二维码
	#define G_ERCODE_X		100	
	#define G_ERCODE_Y		300
	#define G_ERCODE_SCALE	6

#endif









typedef struct WORD{
	const char *character;
	uint16_t x;
	uint16_t y;
	uint16_t color;
	uint16_t background;
	const BX_FONT *font;
	uint16_t valx;
	uint16_t valy;
}WORD;

void word_init(void);




typedef struct CHARACTER{
	const WORD *standby;
	const WORD *start;
	const WORD *choose;
	const WORD *menu;
	const WORD *salling;
	const WORD *set;
	const WORD *warring;
	const WORD *info;
	const WORD *drvset;
}CHARACTER;




extern CHARACTER pt_word;

extern volatile char version[];




#endif
