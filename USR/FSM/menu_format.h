/*
menu菜单实现

*/
#ifndef MENU_H
#define MENU_H

#include "stdint.h"  
#include "word.h"

typedef struct menuitem 
{
	const WORD* item;
	uint8_t status_id;
}menuitem;


typedef struct menu
{
	char index[3];//菜单项前面的角标
	uint8_t height;	//菜单项数目
	uint8_t sizePer;//每一页显示的菜单数目
	const WORD*  name;//菜单名字
	const menuitem* itemlist;//菜单项指针
	uint8_t selectitem;//当前菜单序号
	uint16_t forceground;//当前菜单前景色
	uint16_t background;//当前菜单背景色

}menu;


typedef void (*text_func)(void);

typedef struct textbox
{
	uint8_t height;
	const text_func* itemlist;
	uint8_t selectitem;
}textbox;


extern menu mymenu;
extern textbox mytextbox;

void LoadMenu(menu* cmenu);//装载一个菜单
void MenuNAVDOWN(menu* cmenu);//菜单下移
void intermenuitem(uint8_t num,menu* cmenu); //进入菜单选项
void LoadTextBox(textbox * box);//装载一个文本（不好定义，其实不是GUI意义的文本框）
void TextBoxNAVDOWN(textbox * box);
void TextBoxDISBYID(textbox *box,uint8_t i);
void MenuNAVUP(menu* cmenu);


/*
输入内容类型
*/
typedef enum{
	CONTENT_NULL,		//不要输入的项
	CONTENT_PRICE, 		//价格
	CONTENT_STORE, 		//库存
	CONTENT_CHANNEL,	//货道
	CONTENT_GOODSID, 	//商品编码
	CONTENT_TIME,		//时间
	CONTENT_DATA,		//日期
	CONTENT_VMID,		//机器id
	CONTENT_PASSWORD,	//连网密码
	CONTENT_LINKENABLE,	//连网使能
	CONTENT_LANGUAGE,	//语言
	CONTENT_DRVBIT,		//机柜数量
	CONTENT_POINTBIT,	//小数点位数
	CONTENT_PRICEBIT,	//价格位数
	CONTENT_STOREBIT,	//库存/弹簧位数
	CONTENT_CHANNELBIT,	//货道位数
	CONTENT_CHANNELTEST,//货道测试
	CONTENT_TRAYTEST,	//托盘测试
	CONTENT_WHOLETEST,	//整机测试
	CONTENT_CHANNLTYPE,	//货道使能/禁能/联动
	CONTENT_RESET,		//回复出厂
	CONTENT_SPRING,		//弹簧
	CONTENT_TRAYNUM,	//托盘数量
	CONTENT_DATARESET,	//清空数据
	CONTENT_DRV_SET1,	//驱动板参数1
	CONTENT_DRV_SET2,	//驱动板参数2
	CONTENT_DRV_SET3,	//驱动板参数3
	CONTENT_DRV_SET4,	//驱动板参数4
	CONTENT_DRV_SET5,	//驱动板参数5
	CONTENT_DRV_SET6,	//驱动板参数6
	CONTENT_DRV_SET7,	//驱动板参数7
	CONTENT_DRV_SET8,	//驱动板参数8
	CONTENT_DRV_SET9,	//驱动板参数9
	CONTENT_DRV_SET10,	//驱动板参数10
	CONTENT_DRV_SET11,	//驱动板参数11
	CONTENT_DRV_SET12,	//驱动板参数12
	CONTENT_DRV_SET13,	//驱动板参数13
	CONTENT_DRVPARAM_RESET,	//驱动板参数复位
}CONTENT_TYPE;


/*
内容，字符串
*/
typedef struct CONTENT 
{
	uint32_t value;				//内容代表的值
	uint16_t valueX;					//内容代表的值坐标 
	uint16_t valueY;					//内容代表的值坐标
	char val[12];			//内容值
	uint8_t id;				//在容器中的id号
	CONTENT_TYPE inputContent;//输入内容类型
	const WORD *item; 

}CONTENT;

/*
内容容器
*/
typedef struct HOLDER
{
	uint8_t size;				//容器大小
	const char *title;			//容器标题	
	CONTENT* contentList;		//容器内容指针
	uint8_t curSelectId;		//容器选项
	uint8_t shownumPage;		//一页显示的项目数
	uint16_t zicolor;			//当前选项字符颜色
	uint16_t numcolor;			//当前选项数字颜色
	uint16_t background;	//菜单字体背景色
	uint16_t foreground;	//菜单字体前景色
	const BX_FONT *font; 
}HOLDER;



extern HOLDER holder;

void contant_load(HOLDER* holder);
void content_page_down(HOLDER* holder);
void content_page_up(HOLDER* holder);
void content_item_fls(HOLDER* holder);

void transfer_value_to_char(uint32_t val, char *addr);
void transfer_value_to_time(uint32_t val, char *addr);
void transfer_value_to_money(uint32_t val, char *addr);
void transfer_value_to_data(uint32_t val, char *addr);


#endif
