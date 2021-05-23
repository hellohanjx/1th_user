/*
用户配置文件
提供给程序员修改配置用
*/

#ifndef _CONFIGURE_H_
#define _CONFIGURE_H_

//#define  USER_LOG	//用户日志输出
//#define  USE_FULL_ASSERT	//开启断言


#define SCREENILI9806  	0		//5寸屏
#define SCREENRA8875   	1		//旺宝7寸屏
#define V_SCREENRA8875  2		//旺宝7寸屏,竖屏
#define TM12864Z		3		//带字库的12864屏（3.3V 20p插针）
#define TM12864_NO_FONT	4		//不带字库的12864屏（3.3V 20p插针）

/*
注意，易触门碰与金码门碰是相反的
*/
#define YICHU_DOOR 		1					//0.普通机型门碰（金码）。1.易触门碰
#define SCREEN     		TM12864_NO_FONT		//屏幕类型

#define ZJ_VS_XY	1	//1，检测中吉兴元无信号电机

/*
机柜参数
*/
#define MAX_CHANNEL_SIZE		80	//单一机柜最大货道数
#define MAX_CONTAINER_SIZE		9	//最多支持的机柜数
#define MAX_LAYER_SIZE			8	//最大支持的层数，托盘数
#define MAX_COLUMN_SIZE			10	//最大支持的单层货道数，列数


#if ((SCREEN == TM12864Z) || (SCREEN == TM12864_NO_FONT))	//蓝屏
	#define LCD_CLEAR_SIDE	lcd_clear(); 
	#define LCD_DRAW_FRAME 	 ((void)0)//啥也不执行
	#define JY_MILK		//均亚牛奶
	
#elif (SCREEN == SCREENILI9806 || SCREEN == SCREENRA8875)	//彩屏横屏
	#define LCD_CLEAR_SIDE	lcd_fill_color(300, 0, ((lcd_get_xsize()) - 300), (lcd_get_ysize()), BLUE)//lcd_fill_color(6*48 - 24, 0, 5, 480, WHITE); 
	#define LCD_DRAW_FRAME 	lcd_fill_color(6*48 - 24, 0, 5, 480, WHITE)

#elif SCREEN == V_SCREENRA8875	//彩屏竖屏
	#define LCD_CLEAR_SIDE	lcd_fill_color(0, 400, 480, 400, BLUE)//lcd_fill_color(6*48 - 24, 0, 5, 480, WHITE); 
	#define LCD_DRAW_FRAME 	lcd_fill_color(0, 398, 480, 3, WHITE)

#endif




#define FLASH_APP_ADDR		(0x08010000)  	//用户程序起始地址

#endif
