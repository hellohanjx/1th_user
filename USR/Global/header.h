/*
头文件集,
公用头文件都放在这
*/
#ifndef _HEADER_H_
#define _HEADER_H_

/*
用户配置文件
*/
#include "configure.h"

/*
系统配置文件
*/
#include "stm32f10x_conf.h"	
#include "main.h"
/*
ucosii
*/
#include "ucos_ii.h"
#include "os_cpu.h"
#include "cpu.h"
#include "app_cfg.h"



#include "mem.h"		//动态内存框架
#include "msg.h"		//消息系统
#include "word.h"  		//文字系统
#include "setup.h" 		//设置系统
#include "assist.h" 	//消息计时
#include "fsm_fram.h"	//状态机框架

/*
全局变量,与全局方法
*/
#include "global.h"		

/*
接入设备驱动，并非底层驱动
*/
#include "coin.h"
#include "bill.h"
#include "com_header.h"
#include "drv_header.h"  
#include "card_reader_header.h"


/*
总线驱动
bsp开头的文件一般是将底层驱动封装了
*/
#include "log.h" 
#include "uart485.h"
#include "bsp_commucation.h"
#include "bsp_ireader.h"
#include "bsp_key.h"		//键盘,这个就是IO口控制
#include "drop_check.h"
#include "bsp_bkp.h"		//备份区的操作


/*
底层驱动
*/
#include "SST25VF016BDrv.h"	//spi flash
#include "timer3.h"			//定时器3
#include "rtc.h"			//rtc
#include "i2c_ee_dma.h"		//I2C1 dma 
#include "fsmc_sram.h"		//sram
#include "sys_config.h"		//系统时钟
#include "mdb_header.h"		//mdb总线

/*
字体
*/
#include "font.h"			//字体
#include "lcd_header.h"		//屏幕接口

/*
显示模板
*/
#include "menu_format.h"

/*
二维码
*/
#include "QR_Encode.h"

/*
flash操作接口
*/
#include "flash_struct.h"


#endif

