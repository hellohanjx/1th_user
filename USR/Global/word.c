/*
常用短语放到这里来
这个可以作为一个配置文件
*/
#include "word.h"
#include "lcd_header.h"
#include "global.h"

volatile char version[] = {"MXXX-1803181"};//版本号

/*============================================================================================
单色屏幕
============================================================================================*/

#if ((SCREEN == TM12864Z) || (SCREEN == TM12864_NO_FONT))

/*
警告提示信息
*/
const WORD chs_warring[] = {
{"设置成功", 	4*8, 6, WHITE, BLUE, &font16},	//0
{"设置失败", 	4*8, 6, WHITE, BLUE, &font16},	//1
{"设置超限", 	4*8, 6, WHITE, BLUE, &font16},	//2
{"设置位数不足", 4*8, 6, WHITE, BLUE, &font16},	//3
{"正在测试中", 	4*8, 6, WHITE, BLUE, &font16},	//4
{"正在恢复...",4*8, 6, WHITE, BLUE, &font16},	//5
{"恢复成功   ",4*8, 6, WHITE, BLUE, &font16},	//6
{"显示时间", 	1*8, 0, WHITE, BLUE, &font16},	//7
{"HARD FAULT", 0, 0, BLACK, BLUE, &font16},//8

//图标
{"號", 		1*8, 6, WHITE, BLUE, &font16},	//9
{"門", 		3*8, 6, WHITE, BLUE, &font16},	//10
{"機", 		5*8, 6, WHITE, BLUE, &font16},	//11

//e2prom复位
{"EEPROM RESET    ", 0, 2, WHITE, BLUE, &font16},	//12
//rtc恢复
{"RTC INIT        ", 0, 0, WHITE, BLUE, &font16},	//13
{"RTC RESET	      ", 0, 0, WHITE, BLUE, &font16},	//14
{"CRYSTAL ERR     ", 0, 6, WHITE, BLUE, &font16},	//15

//图标
{"凍", 7*8,  6, WHITE, BLUE, &font16},//制冷图标			//16
{"燈", 9*8,  6, WHITE, BLUE, &font16},//照明图标			//17
{"殺", 11*8, 6, WHITE, BLUE, &font16},//杀毒图标			//18
{"攝", 0, 0, WHITE, BLUE, &font16, 4*8, 0},//未装温度探头图标 	//19 后面坐标表示快捷键查询中正常温度值的位置
{"熱", 7*8, 6, WHITE, BLUE, &font16},//制热图标 					//20
{"撥", 13*8, 6, WHITE, BLUE, &font16},//玻璃加热图标 			//21
{"數", 8*8, 0, WHITE, BLUE, &font16, 10*8, 0},//待发送数据图标 	//22
{"溫", 2*8, 0, WHITE, BLUE, &font16, 6*8, 0},//摄氏度图标 		//23 后面2个坐标表示快捷键查询温度图标的另一个位置
{"讀", 0, 6, WHITE, BLUE, &font16, 6*8, 6},//刷卡器图标			//24

//flash复位
{"FLASH RESET     ", 0, 4, WHITE, BLUE, &font16},	//25
{"FLASH IS NOT SST", 0, 4, WHITE, BLUE, &font16},	//26
{"FLASH IS NOT SPI", 0, 4, WHITE, BLUE, &font16},	//27
{"RECOVERY        ", 0, 4, WHITE, BLUE, &font16, 9*8, 4},	//28
{"FLASH REC       ", 0, 4, WHITE, BLUE, &font16, 9*8, 4},	//29

};

/*
待机
*/
const WORD chs_standby[] = {
//待机界面
{"  欢迎刷卡购物  ", 0, 3, WHITE, BLUE, &font16},		//0
{"网络故障无法消费", 0, 3, WHITE, BLUE, &font16},		//1
{"刷卡故障无法消费", 0, 3, WHITE, BLUE, &font16},		//2
{"机器故障无法消费", 0, 3, WHITE, BLUE, &font16},		//3
//#键查询在线卡信息界面
{"卡号:", 		0, 0, WHITE, BLUE, &font16, 6*8, 0},		//4
{"余额:", 		0, 2, WHITE, BLUE, &font16, 6*8, 2},		//5
{"限购:", 		0, 4, WHITE, BLUE, &font16, 6*8, 4},		//6
{"请刷卡查询", 	0, 6, WHITE, BLUE, &font16, 14*8,6},		//7
{"网络未连接", 	0, 6, WHITE, BLUE, &font16, 14*8,6},		//8
{"不限", 		6*8, 4, WHITE, BLUE, &font16, 6*8, 2},		//9
{"次", 			10*8, 4, WHITE, BLUE, &font16, 10*8, 4},		//10
{"卡不存在   ", 	0, 6, WHITE, BLUE, &font16, 14*8,6},		//11
{"无效卡     ", 	0, 6, WHITE, BLUE, &font16, 14*8,6},		//12
{"非本校卡   ", 	0, 6, WHITE, BLUE, &font16, 14*8,6},		//13
{"测试卡     ", 	0, 6, WHITE, BLUE, &font16, 14*8,6},		//14
{"非本机卡   ", 	0, 6, WHITE, BLUE, &font16, 14*8,6},		//15
{"处理异常   ", 	0, 6, WHITE, BLUE, &font16, 14*8,6},		//16

//欢迎光临辅助提示
{"网络故障无法消费", 0, 600, WHITE, BLUE, &font16},		//17	//显示位置设置为超限，这里不显示
{"刷卡故障无法消费", 0, 600, WHITE, BLUE, &font16},		//18	//这里不显示
{"机器故障无法消费", 0, 600, WHITE, BLUE, &font16},		//19	//这里不显示
{"请选择商品      ", 0, 600, WHITE, BLUE, &font16},		//20	//这里不显示


};

/*
开机界面
*/
const WORD chs_start[] = {
	{"正在检测设备... ", 0, 0, WHITE, BLUE, &font16},		//0
	{"纸币器正常      ",	0, 6, WHITE, BLUE, &font16},		//1
	{"硬币器正常      ", 0, 6, WHITE, BLUE, &font16},		//2
	{"纸币器未连接    ", 0, 6, WHITE, BLUE, &font16},		//3
	{"硬币器未连接    ", 0, 6, WHITE, BLUE, &font16},		//4
	{"驱动板正常      ", 0, 2, WHITE, BLUE, &font16},		//5
	{"驱动板未连接    ", 0, 2, WHITE, BLUE, &font16},		//6
	{"读卡器正常      ", 0, 4, WHITE, BLUE, &font16},		//7
	{"读卡器未连接    ", 0, 4, WHITE, BLUE, &font16},		//8
};

/*
购买
*/	
const WORD chs_choose[] = {
//选货界面
{"商品:"	, 	0, 0, 	WHITE, BLUE, &font16, 5*8, 0},			//0
{"价格:" , 		0, 3, 	WHITE, BLUE, &font16, 5*8, 3},			//1
{"库存:" , 		500, 500, 	WHITE, BLUE, &font16, 500, 500},	//2 改了位置不显示这个 原始(0, 4    6*8, 4)
{"无此货道", 	0, 6, WHITE, BLUE, &font16}, 		//3
{"货道已禁用      ", 0, 6, WHITE, BLUE, &font16}, 	//4
{"商品已售空      ", 0, 6, WHITE, BLUE, &font16}, 	//5
{"货道停止服务    ", 0, 6, WHITE, BLUE, &font16}, 	//6
{"网络未连接      ", 0, 6, WHITE, BLUE, &font16},			//7
{"请刷卡购物      ", 0, 6, WHITE, BLUE, &font16, 14*8, 6},	//8
{"请刷卡或扫码    ", 0, 6, WHITE, BLUE, &font16},			//9
{"已读卡后台正验证", 0, 6, WHITE, BLUE, &font16},			//10

//在线卡出货
{"卡号:", 		0, 0, WHITE, BLUE, &font16, 5*8, 0},	//11
{"机柜暂停服务", 0, 6, WHITE, BLUE, &font16}, 			//12
{"余额:", 		0, 3, WHITE, BLUE, &font16, 5*8, 3},	//13 改为第三行
{"卡不存在不能消费", 0, 6, WHITE, BLUE, &font16},	//14
{" 无效卡不能消费 ", 0, 6, WHITE, BLUE, &font16},	//15
{"非本校卡不能消费", 0, 6, WHITE, BLUE, &font16},	//16
{"次数超限不能消费", 0, 6, WHITE, BLUE, &font16},	//17
{"非本机卡不能消费", 0, 6, WHITE, BLUE, &font16},	//18
{"余额不足不能消费", 0, 6, WHITE, BLUE, &font16},	//19
{" 第  层商品停售 ", 0, 6, WHITE, BLUE, &font16, 3*8, 6},	//20
{"   扣款失败    ", 0, 6, WHITE, BLUE, &font16},		//21
{"处理异常不能消费", 0, 6, WHITE, BLUE, &font16},	//22
{"   通知管理员   ", 0, 6, WHITE, BLUE, &font16},	//23
{"暂停服务        ", 0, 6, WHITE, BLUE, &font16}, 	//24
{"请扫码支付      ", 0, 6, WHITE, BLUE, &font16}, 	//25
{"价格错误请重选货",  0, 6, WHITE, BLUE, &font16},	//26	
{"库存错误请重选货",  0, 6, WHITE, BLUE, &font16},	//27	

//现金
{"投入:", 0, 0, WHITE, BLUE, &font16, 5*8, 0}, 	//28
{"商品:", 0, 2, WHITE, BLUE, &font16, 5*8, 2}, 	//29
{"价格:", 0, 4, WHITE, BLUE, &font16, 5*8, 4}, 	//30
{"请继续投币      ", 0, 6, WHITE, BLUE, &font16}, 	//31
{"请选择商品      ", 0, 6, WHITE, BLUE, &font16}, 	//32
{"暂停服务        ", 0, 6, WHITE, BLUE, &font16}, 	//33

};

/*
菜单界面
*/
const WORD chs_menu[] = {
{"商品设置" , 0, 2, WHITE, BLUE, &font16},//0
{"货道测试" , 0, 4, WHITE, BLUE, &font16},//1
{"货道设置" , 0, 6, WHITE, BLUE, &font16},//2
{"温控设置" , 0, 2, WHITE, BLUE, &font16},//3
{"控制设置" , 0, 4, WHITE, BLUE, &font16},//4
{"支付设置" , 0, 6, WHITE, BLUE, &font16},//5
{"机器设置" , 0, 2, WHITE, BLUE, &font16},//6
{"机器信息" , 0, 4, WHITE, BLUE, &font16},//7
{"通讯设置" , 300, 200, WHITE, BLUE, &font16},		//8
{"管理设置" , 0, 0, WHITE, BLUE, &font16, 12*8, 0}, //9
{"售货机信息" , 400, 0, WHITE, BLUE, &font16}, 		//10
{"ID:", 300, 50 , WHITE, BLUE, &font16},	//11
{"版本号:", 300, 100, WHITE, BLUE, &font16},//12
{"驱动板:", 300, 150, WHITE, BLUE, &font16},//13
{"正常", 	500, 150, WHITE, BLUE, &font16},//14
{"未连接", 	500, 150, WHITE, BLUE, &font16},//15
{"读卡器:", 300, 200, WHITE, BLUE, &font16},//16
{"纸币器:", 300, 250, WHITE, BLUE, &font16},//17
{"硬币器:", 300, 300, WHITE, BLUE, &font16},//18
{"网  络:", 300, 350, WHITE, BLUE, &font16},//19
{"连网", 	500, 350, WHITE, BLUE, &font16},//20
{"未连网", 	500, 350, WHITE, BLUE, &font16},//21
{"温  度:", 300, 400, WHITE, BLUE, &font16},//22
{"故障", 	500, 400, WHITE, BLUE, &font16},//23
{"通讯密码:",500, 400, WHITE, BLUE, &font16},//24
{"销售统计",0, 4, WHITE, BLUE, &font16},//25
};

/*
出货界面
*/
const WORD chs_salling[] = {

{"余额", 30, 230, WHITE, BLUE, &font48},						//0
{"正在出货请稍候  ", 0, 6, WHITE, BLUE, &font16, 14*8, 6},		//1
{"出货成功        ", 0, 6, WHITE, BLUE, &font16},				//2
{"出货失败        ", 0, 6, WHITE, BLUE, &font16, 14*8, 6},		//3
{"请取走商品      ", 0, 6, WHITE, BLUE, &font16},				//4
};

/*
设置界面提示语
*/
const WORD chs_set[] = {

{"设置成功", 500, 400, WHITE, BLUE, &font16},	//0
{"设置失败", 500, 400, WHITE, BLUE, &font16},	//1
{"设置超限", 500, 400, WHITE, BLUE, &font16}, 	//2
{"设置位数不足", 500, 400, WHITE, BLUE, &font16},//3
{"正在测试", 4*8, 6, WHITE, BLUE, &font16},	//4
{"正常电机数", 500, 400, WHITE, BLUE, &font16},	//5
{"不在位电机", 500, 400, WHITE, BLUE, &font16},	//6
{"正在恢复出厂..." , 500, 400, WHITE, BLUE, &font16},	//7
{"恢复成功", 500, 400, WHITE, BLUE, &font16},			//8
{"条码",	500, 400, WHITE, BLUE, &font48},	//9

//货道测试
{"机柜", 		0 , 	0, WHITE, BLUE, &font16, 4*8, 0},		//10
{"托盘", 		5*8 , 	0, WHITE, BLUE, &font16, 9*8, 0},		//11
{"0123456789", 	0 , 	2, WHITE, BLACK, &font16, 4, 6},		//12	表示货道,valx为货道使能纵坐标，valy为货道状态纵坐标
{"输入", 		11*8 , 	0, WHITE, BLUE, &font16, 12*8, 2},		//13	

//商品设置
{"商品" , 0, 0, WHITE, BLUE, &font16, 5*8, 0},	//14
{"库存" , 0, 2, WHITE, BLUE, &font16, 5*8, 2},	//15
{"价格" , 0, 4, WHITE, BLUE, &font16, 5*8, 4},	//16
{"条码" , 0, 0, WHITE, BLUE, &font16, 5*8, 0},	//17

//机器设置
{"ID:",			0, 0, WHITE, BLUE, &font16, 6*8, 0},			//18
{"连网密码:" , 0, 2, WHITE, BLUE, &font16, 10*8, 2},			//19
{"连网(0关1开):" , 0, 4, WHITE, BLUE, &font16, 14*8, 4},			//20
{"语言(0英1中):" , 0, 0, WHITE, BLUE, &font16, 14*8, 0},			//21
{"机柜数(1-9):" , 0, 2, WHITE, BLUE, &font16, 14*8, 2},			//22
{"小数位(0-8):" , 0, 4, WHITE, BLUE, &font16, 14*8, 4},			//23
{"价格位(1-9):" , 0, 0, WHITE, BLUE, &font48, 14*8, 0},			//24
{"输入位(1-9):" , 0, 2, WHITE, BLUE, &font16, 14*8, 2},			//25
{"货道位(1-3):" , 0, 4, WHITE, BLUE, &font16, 14*8, 4},			//26
{"恢复出厂(1确认)" , 0, 2, WHITE, BLUE, &font16, 12*8, 2},		//27

//支付设置
{"扫码(0关1开):" , 0, 0, WHITE, BLUE, &font16, 14*8, 0},		//28
{"刷卡接口:" , 0, 2, WHITE, BLUE, &font16, 10*8, 2},			//29
{"取货接口:" , 0, 4, WHITE, BLUE, &font16, 10*8, 4},			//30

//温度设置
{"目标温度:" , 	0, 0, WHITE, BLUE, &font16, 10*8, 0},		//31
{"开启:" , 		0, 2, WHITE, BLUE, &font16, 6*8,  2},		//32
{"关闭:" , 		0, 4, WHITE, BLUE, &font16, 6*8,  4},		//33 
{"方式(0冷1热)", 0, 0, WHITE, BLUE, &font16, 14*8, 0},		//34

//控制回路设置
{"风机:" ,	 	0, 0, 	WHITE, BLUE, &font16, 6*8, 0},				//35
{"开启:" , 		0, 2, 	WHITE, BLUE, &font16, 6*8, 2},				//36
{"关闭:" , 		0, 4, 	WHITE, BLUE, &font16, 6*8, 4},				//37 
{"照明:" , 		0, 0, 	WHITE, BLUE, &font16, 6*8, 0},				//38
{"开启:" , 		0, 2, 	WHITE, BLUE, &font16, 6*8, 2},				//39
{"关闭:" , 		0, 4, 	WHITE, BLUE, &font16, 6*8, 4},				//40 
{"消毒:" , 		0, 0, 	WHITE, BLUE, &font16, 6*8, 0},				//41
{"开启:" , 		0, 2, 	WHITE, BLUE, &font16, 6*8, 2},				//42
{"关闭:" , 		0, 4, 	WHITE, BLUE, &font16, 6*8, 4},				//43 
{"玻璃加热:",	0, 0, 	WHITE, BLUE, &font16, 6*8, 0},				//44
{"开启:" , 		0, 2, 	WHITE, BLUE, &font16, 6*8, 2},				//45
{"关闭:" , 		0, 4, 	WHITE, BLUE, &font16, 6*8, 4},				//46 
{"备用2" , 		0, 0, 	WHITE, BLUE, &font16, 6*8, 0},				//47
{"开启:" , 		0, 2, 	WHITE, BLUE, &font16, 6*8, 2},				//48
{"关闭:" , 		0, 4, 	WHITE, BLUE, &font16, 6*8, 4},				//49 

//货道设置
{"货道:" , 	0, 0, 	WHITE, BLUE, &font16, 6*8, 0},				//50
{"圈数:" , 	0, 2, 	WHITE, BLUE, &font16, 6*8, 2},				//51
{"使能:" , 	0, 4, 	WHITE, BLUE, &font16, 6*8, 4},				//52 

//货道测试
{"货道测试:" , 	0, 0, 	WHITE, BLUE, &font16, 10*8, 0},				//53
{"托盘测试:" , 	0, 2, 	WHITE, BLUE, &font16, 10*8, 2},				//54
{"整机测试:" , 	0, 4, 	WHITE, BLUE, &font16, 2*7, 6},				//55 (这个后面两个位置参数表示测试时的当前货道号提示位置)
{"   无此机柜", 2*7 , 6, WHITE, BLUE, &font16},						//56

//机器设置
{"托盘数(0-8):" , 0, 0, WHITE, BLUE, &font16, 14*8, 0},		//57
{"清空数据(1确认)" , 0, 0, WHITE, BLUE, &font16, 12*8, 0},	//58

//货道测试结果
{"执行", 11*8, 4, WHITE, BLUE, &font16, 12*8, 6},	//59

//日期时间设置
{"日期:", 0, 2, WHITE, BLUE, &font16, 6*8, 2},	//60
{"时间:", 0, 4, WHITE, BLUE, &font16, 6*8, 4},	//61

//销售统计
{"货道", 0, 0, 	   WHITE, BLUE, &font16, 7*8, 0},//62
{"销售量", 0, 2, WHITE, BLUE, &font16, 7*8, 2},//63
{"销售额", 0, 4, WHITE, BLUE, &font16, 7*8, 4},//64
{"清除整机统计数据", 0, 3, WHITE, BLUE, &font16},	//65
{"1.确认", 0, 6, WHITE, BLUE, &font16},//66
{"    清除成功    ", 0, 3, WHITE, BLUE, &font16},//67
{"    正在清除    ", 0, 3, WHITE, BLUE, &font16},//68


};	


/*
信息界面
*/
const WORD chs_info[] = {
	
	{"主板-->" , 	0, 0, 	WHITE, BLUE, &font16, 0, 2},	//0	后面两个坐标表示快捷键查看信息的版本号位置
	{"版本:" , 	0, 2, 	WHITE, BLUE, &font16, 5*8, 2},		//1
	{"FLASH:", 	0 ,4, 	WHITE, BLUE, &font16, 6*8, 4},		//2
	{"ID:" , 	0, 6, 	WHITE, BLUE, &font16, 4*8, 6},		//3
	
	{"驱动板-->" , 0, 0, 	WHITE, BLUE, &font16, 0, 4},		//4	后面两个坐标表示快捷键查看信息的版本号位置
	{"版本:" , 	0, 2, 	WHITE, BLUE, &font16, 6*8, 2},		//5
	{"温度1:", 	0 ,4, 	WHITE, BLUE, &font16, 6*8, 4},		//6
	{"温度2:", 	0 ,6, 	WHITE, BLUE, &font16, 6*8, 6},		//7
	{"压缩机:", 	0 ,0, 	WHITE, BLUE, &font16, 6*8, 0},		//8
	{"风机:", 	0 ,2, 	WHITE, BLUE, &font16, 6*8, 2},		//9
	{"照明:", 	0 ,4, 	WHITE, BLUE, &font16, 6*8, 4},		//10
	{"消毒:", 	0 ,6, 	WHITE, BLUE, &font16, 6*8, 6},		//11
	{"备用1:", 	0 ,0, 	WHITE, BLUE, &font16, 6*8, 0},		//12
	{"备用2:", 	0 ,2, 	WHITE, BLUE, &font16, 6*8, 2},		//13

	{"读卡器-->" , 0, 4, 	WHITE, BLUE, &font16},		//14
	{"状态:" , 	0, 6, 	WHITE, BLUE, &font16, 8*8, 6},		//15


	{"网络-->" , 0, 0, 	WHITE, BLUE, &font16,},		//16
	{"状态:" , 	0, 2, 	WHITE, BLUE, &font16, 8*8, 2},		//17
	{"待发:", 	0 ,4, 	WHITE, BLUE, &font16, 8*8, 4},		//18
	{"存储:", 	0 ,6, 	WHITE, BLUE, &font16, 8*8, 6},		//19
	{"已用:" , 	0, 0, 	WHITE, BLUE, &font16, 8*8, 0},		//20
	{"读序号:",	0, 2, 	WHITE, BLUE, &font16, 8*8, 2},		//21
	{"写序号:", 	0 ,4, 	WHITE, BLUE, &font16, 8*8, 4},		//22
	{"交易号:", 	0 ,6, 	WHITE, BLUE, &font16, 8*8, 6},		//23
	{"流水号:", 	0 ,0, 	WHITE, BLUE, &font16, 8*8, 0},		//24

	{"硬币器-->" , 0, 2, 	WHITE, BLUE, &font16, 6*8, 2},		//25
	{"状态:" , 	0, 4, 	WHITE, BLUE, &font16, 6*8, 4},		//26
	{"层级:", 	0 ,6, 	WHITE, BLUE, &font16, 6*8, 6},		//27
	{"小数:" ,  0, 0, 	WHITE, BLUE, &font16, 6*8, 0},		//28
	{"国家代码:",0 ,2, 	WHITE, BLUE, &font16, 6*8, 2},		//29
	{"通道:" , 0, 4, 	WHITE, BLUE, &font16, 6*8, 4},		//30
	{"面值:" , 	0, 6, 	WHITE, BLUE, &font16, 6*8, 6},		//31
	{"金额:", 	0 ,0, 	WHITE, BLUE, &font16, 6*8, 0},		//32

	{"纸币器-->:", 0 ,2, 	WHITE, BLUE, &font16, 6*8, 2},		//33
	{"状态:" , 	0, 4, 	WHITE, BLUE, &font16, 6*8, 4},		//34
	{"层级:", 	0 ,6, 	WHITE, BLUE, &font16, 6*8, 6},		//35
	{"小数:" , 	0, 0, 	WHITE, BLUE, &font16, 6*8, 0},		//36
	{"国家代码:",0 ,2, 	WHITE, BLUE, &font16, 6*8, 2},		//37
	{"通道:" , 	0, 4, 	WHITE, BLUE, &font16, 6*8, 4},		//38
	{"面值:" , 	0, 6, 	WHITE, BLUE, &font16, 6*8, 6},		//39
	{"金额:", 	0 ,0, 	WHITE, BLUE, &font16, 6*8, 0},		//40
	{"暂存箱通道:" , 0, 2, 	WHITE, BLUE, &font16, 6*8, 2},	//41
	{"找零箱面值:" , 0, 4, 	WHITE, BLUE, &font16, 6*8, 4},	//42
	{"找零箱金额:",	0 ,6, 	WHITE, BLUE, &font16, 6*8, 6},	//43

	{"CPU->",	0 ,0, 	WHITE, BLUE, &font16, 6*8, 0},		//44

};

/*
驱动板设置界面
*/
const WORD chs_drvset[] = {
	{"适配类型(0N1J)" , 	0, 0, 	WHITE, BLUE, &font16, 8*15, 0},		//0
	{"温度类型(0-2)" , 	0, 2, 	WHITE, BLUE, &font16, 8*15, 2},		//1
	{"启动电流(MA)" , 	0, 4, 	WHITE, BLUE, &font16, 8*12, 4},		//2
	{"短路电流(MA)" , 	0, 0, 	WHITE, BLUE, &font16, 8*12, 0},		//3
	{"过载电流(MA)" , 	0, 2, 	WHITE, BLUE, &font16, 8*12, 2},		//4
	{"过载时间(MS)" , 	0, 4, 	WHITE, BLUE, &font16, 8*12, 4},		//5
	{"超时未启动(S)" , 	0, 0, 	WHITE, BLUE, &font16, 8*14, 0},	//6
	{"超时未停止(S)" , 	0, 2, 	WHITE, BLUE, &font16, 8*14, 2},	//7
	{"停止延时(MS)" ,	0, 4, 	WHITE, BLUE, &font16, 8*12, 4},	//8
	{"出货信号次数" , 	0, 0, 	WHITE, BLUE, &font16, 8*14, 0},	//9
	{"信号间隔(US)" ,	0, 2, 	WHITE, BLUE, &font16, 8*12, 2},	//10
	{"查询信号次数:" , 	0, 4, 	WHITE, BLUE, &font48, 8*14, 4},	//11
	{"信号间隔(US):" ,	0, 0, 	WHITE, BLUE, &font48, 8*12, 0},	//12
	
	{"恢复默认(1确认)" ,	0, 2, 	WHITE, BLUE, &font16},			//13
	{"驱动板设置" , 	3*8, 0, 	WHITE, BLUE, &font16},			//14
	{"请输入密码" , 	3*8, 2, 	WHITE, BLUE, &font16, 3*8, 4},	//15
	{"驱动板未连接",	2*8, 2, WHITE, BLUE, &font16},				//16
};

#elif ((SCREEN == SCREENILI9806) || (SCREEN == SCREENRA8875))


/*============================================================================================
==============================================================================================
彩色屏幕，横屏
==============================================================================================
============================================================================================*/


/*
警告提示信息
*/

const WORD chs_warring[] = {
{"设置成功", 	400, 400, WHITE, BLUE, &font48},	//0
{"设置失败", 	400, 400, WHITE, BLUE, &font48},	//1
{"设置超限", 	400, 400, WHITE, BLUE, &font48},	//2
{"设置位数不足", 400, 400, WHITE, BLUE, &font48},	//3
{"正在测试中", 	400, 400, WHITE, BLUE, &font48},	//4
{"正在恢复中...",400, 400, WHITE, BLUE, &font48},	//5
{"恢复成功     ", 400, 400, WHITE, BLUE, &font48},	//6
{"显示时间", 			0, 0, WHITE, BLUE, &font48},//7
{"HardFault_Handler", 	0, 0, WHITE, BLUE, &font48},//8
{"號", 		130, 100, WHITE, BLUE, &font48},	//9
{"門", 		210, 100, WHITE, BLUE, &font48},	//10
{"機", 		290, 100, WHITE, BLUE, &font48},	//11
{"EEPROM恢复成功", 	30, 200, WHITE, BLUE, &font48},	//12
{"RTC初始化", 		30, 0, WHITE, BLUE, &font48},	//13
{"RTC配置成功", 		30, 50, WHITE, BLUE, &font48},		//14
{"CRYSTAL ERR", 	30, 100, WHITE, BLUE, &font48},	//15
{"凍", 370,  100, WHITE, BLUE, &font48, },//制冷图标	//16
{"燈", 450,  100, WHITE, BLUE, &font48, },//照明图标	//17
{"殺", 530,  100, WHITE, BLUE, &font48, },//杀毒图标	//18
{"攝", 1000, 0, WHITE, BLUE, &font48, },//未装温度探头图标 //19
{"熱", 7*8, 0, WHITE, BLUE, &font48, },//制热图标 //20
{"撥", 1000, 6, WHITE, BLUE, &font48, },//玻璃加热图标 	//21
{"數", 13*8, 6, WHITE, BLUE, &font48, },//待发送数据图标 	//22
{"溫", 2*8, 0, WHITE, BLUE, &font48,  6*8, 0},//摄氏度图标 	//23 后面2个坐标表示快捷键查询温度图标的另一个位置
{"讀", 0, 4, WHITE, BLUE, &font48, 6*8, 4},//刷卡器图标	//24

};


/*
待机
*/
const WORD chs_standby[] = {
{"欢迎光临", 200, 180, WHITE, BLUE, &font96},		//0
{"欢迎光临", 200, 180, WHITE, BLUE, &font96},		//1
{"欢迎光临", 200, 180, WHITE, BLUE, &font96},		//2
{"欢迎光临", 200, 180, WHITE, BLUE, &font96},		//3
//#键查询在线卡信息界面
{"卡号:", 		30, 30, WHITE, BLUE, &font48, 200, 30},		//4
{"余额:", 		30, 80, WHITE, BLUE, &font48, 200, 80},		//5
{"限购:", 		30, 130, WHITE, BLUE, &font48,200, 130},		//6
{"请刷卡查询", 	30, 400, WHITE, BLUE, &font48, 600, 400},	//7
{"网络未连接", 	30, 400, WHITE, BLUE, &font48, 600, 400},	//8
{"不限", 		200, 130, WHITE, BLUE, &font48},			//9
{"次", 			250, 130, WHITE, BLUE, &font48},			//10
{"卡不存在 ", 	30, 400, WHITE, BLUE, &font48, 14*8,6},		//11
{"无效卡   ", 	30, 400, WHITE, BLUE, &font48, 14*8,6},		//12
{"非本校卡 ", 	30, 400, WHITE, BLUE, &font48, 14*8,6},		//13
{"测试卡   ", 	30, 400, WHITE, BLUE, &font48, 14*8,6},		//14
{"非本机卡 ", 	30, 400, WHITE, BLUE, &font48, 14*8,6},		//15
{"处理异常 ", 	30, 400, WHITE, BLUE, &font48, 14*8,6},		//16

//欢迎光临辅助提示
{"网络故障无法消费", 200, 400, WHITE, BLUE, &font48},		//17
{"刷卡故障无法消费", 200, 400, WHITE, BLUE, &font48},		//18
{"机器故障无法消费", 200, 400, WHITE, BLUE, &font48},		//19
{"请选择商品      ", 200, 400, WHITE, BLUE, &font48},		//20

};

/*
开机界面
*/
const WORD chs_start[] = {
	{"正在检测设备...", 0, 0, WHITE, BLUE, &font48},		//0
	{"纸币器正常",		0, 150, WHITE, BLUE, &font48},		//1
	{"硬币器正常", 		0, 100, WHITE, BLUE, &font48},		//2
	{"纸币器未连接", 	0, 150, WHITE, BLUE, &font48},		//3
	{"硬币器未连接", 	0, 100, WHITE, BLUE, &font48},		//4
	{"驱动板正常", 		0, 250, WHITE, BLUE, &font48},		//5
	{"驱动板未连接", 	0, 250, WHITE, BLUE, &font48},		//6
	{"读卡器正常", 		0, 200, WHITE, BLUE, &font48},		//7
	{"读卡器未连接", 	0, 200, WHITE, BLUE, &font48},		//8
};

/*
选货界面
*/	
const WORD chs_choose[] = {
	{"商品:" , 30, 30, WHITE, BLUE, &font48, 180, 30},			//0
	{"价格:" , 30, 80, WHITE, BLUE, &font48, 180, 80},				//1
	{"库存:" , 30, 130, WHITE, BLUE, &font48, 180, 130},			//2
{"无此货道", 30, 400, WHITE, BLUE, &font48}, 		//3
{"货道已禁用", 30, 400, WHITE, BLUE, &font48}, 		//4
{"商品已售空", 30, 400, WHITE, BLUE, &font48}, 		//5
{"货道停止服务", 30, 400, WHITE, BLUE, &font48}, 	//6
{"网络未连接", 30, 400, WHITE, BLUE, &font48},		//7
{"请刷卡购物", 30, 400, WHITE, BLUE, &font48, 600, 400}, 		//8
{"请刷卡或扫描二维码", 30, 400, WHITE, BLUE, &font48},	//9
{"已读卡后台正验证  ", 30, 400, WHITE, BLUE, &font48},	//10
//在线卡出货
{"卡号:", 30, 180, WHITE, BLUE, &font48, 180, 180},			//11
{"机柜暂停服务", 30, 400, WHITE, BLUE, &font48}, 	//12
{"余额:", 		30, 230, WHITE, BLUE, &font48, 180, 230},	//13
{"卡不存在不能消费", 30, 400, WHITE, BLUE, &font48},	//14
{" 无效卡不能消费 ", 30, 400, WHITE, BLUE, &font48},	//15
{"非本校卡不能消费", 30, 400, WHITE, BLUE, &font48},	//16
{"次数超限不能消费", 30, 400, WHITE, BLUE, &font48},	//17
{"非本机卡不能消费", 30, 400, WHITE, BLUE, &font48},	//18
{"余额不足不能消费", 30, 400, WHITE, BLUE, &font48},	//19
{"第 层商品停售   ", 30, 400, WHITE, BLUE, &font48, 78, 400},	//20
{"扣款失败        ", 30, 400, WHITE, BLUE, &font48},	//21
{"处理异常不能消费", 30, 400, WHITE, BLUE, &font48},	//22
{"通知管理员      ", 30, 400, WHITE, BLUE, &font48},	//23
{"暂停服务        ", 30, 400, WHITE, BLUE, &font48}, //24
{"请扫码支付      ", 30, 400, WHITE, BLUE, &font48}, //25
};

/*
菜单界面
*/
const WORD chs_menu[] = {
{"商品设置" , 0, 50, WHITE, BLUE, &font48},//0
{"货道测试" , 0, 100, WHITE, BLUE, &font48},//1
{"货道设置" , 0, 150, WHITE, BLUE, &font48},//2
{"温控设置" , 0, 200, WHITE, BLUE, &font48},//3
{"控制设置" , 0, 250, WHITE, BLUE, &font48},//4
{"支付设置" , 0, 300, WHITE, BLUE, &font48},//5
{"机器设置" , 0, 350, WHITE, BLUE, &font48},//6
{"机器信息" , 400, 0, WHITE, BLUE, &font48},//7
{"通讯设置" , 300, 200, WHITE, BLUE, &font48},//8
{"管理设置" , 0, 0, WHITE, BLUE, &font48,1000,0}, //9
{"售货机信息" , 400, 0, WHITE, BLUE, &font48}, //10
{"机器ID:", 300, 50 , WHITE, BLUE, &font48, 480, 50},//11
{"版本号:", 300, 100, WHITE, BLUE, &font48, 480, 100},//12
{"驱动板:", 300, 150, WHITE, BLUE, &font48, 480, 150},//13
{"正常", 	500, 150, WHITE, BLUE, &font48},//14
{"未连接", 	500, 150, WHITE, BLUE, &font48},//15
{"读卡器:", 300, 200, WHITE, BLUE, &font48, 480, 200},//16
{"纸币器:", 300, 250, WHITE, BLUE, &font48, 480, 250},//17
{"硬币器:", 300, 300, WHITE, BLUE, &font48, 480, 300},//18
{"网  络:", 300, 350, WHITE, BLUE, &font48, 480, 350},//19
{"连网", 	500, 350, WHITE, BLUE, &font48},//20
{"未连网", 	500, 350, WHITE, BLUE, &font48},//21
{"温  度:", 300, 400, WHITE, BLUE, &font48, 480, 400},//22
{"故障", 	500, 400, WHITE, BLUE, &font48},//23
{"通讯密码:",500, 400, WHITE, BLUE, &font48},//24
};
/*
出货界面
*/
const WORD chs_salling[] = {

{"余额", 30, 230, WHITE, BLUE, &font48},				//0
{"正在出货请稍候     ", 30, 400, WHITE, BLUE, &font48},		//1
{"出货成功           ", 30, 400, WHITE, BLUE, &font48},				//2
{"出货失败           ", 30, 400, WHITE, BLUE, &font48, 600, 400},	//3
{"请取走商品         ", 30, 400, WHITE, BLUE, &font48},				//4
};

/*
设置界面提示语
*/
const WORD chs_set[] = {

{"设置成功", 500, 400, WHITE, BLUE, &font48},	//0
{"设置失败", 500, 400, WHITE, BLUE, &font48},	//1
{"设置超限", 500, 400, WHITE, BLUE, &font48}, 	//2
{"设置位数不足", 500, 400, WHITE, BLUE, &font48},//3
{"正在测试中", 500, 400, WHITE, BLUE, &font48},	//4
{"正常电机数", 500, 400, WHITE, BLUE, &font48},	//5
{"不在位电机", 500, 400, WHITE, BLUE, &font48},	//6
{"正在恢复出厂..." , 500, 400, WHITE, BLUE, &font48},	//7
{"恢复成功", 500, 400, WHITE, BLUE, &font48},			//8
{"条码",	500, 400, WHITE, BLUE, &font48},	//9
{"货道正常", 450 , 400, WHITE, BLUE, &font48},	//10
{"货道异常", 450 , 400, WHITE, BLUE, &font48},	//11
{"货道禁能", 450 , 400, WHITE, BLUE, &font48},	//12
{"   停止测试 ", 350 , 400, WHITE, BLUE, &font48},	//13
//商品设置
{"商品:" , 300, 50, WHITE, BLUE, &font48, 450, 50},	//14
{"库存:" , 300, 100, WHITE, BLUE, &font48, 450, 100},	//15
{"价格:" , 300, 150, WHITE, BLUE, &font48, 450, 150},	//16
{"条码:" , 300, 200, WHITE, BLUE, &font48, 450, 200},	//17
//机器设置
{"机器ID:" , 300, 50, WHITE, BLUE, &font48, 520, 50},					//18
{"连网密码:" , 300, 100, WHITE, BLUE, &font48, 520, 100},				//19
{"开启连网:" , 300, 150, WHITE, BLUE, &font48, 520, 150},				//20
{"语言(0中1英):" , 300, 200, WHITE, BLUE, &font48, 600, 200},			//21
{"机柜数量:" , 300, 250, WHITE, BLUE, &font48, 600, 250},				//22
{"小数位数:" , 300, 300, WHITE, BLUE, &font48, 600, 300},				//23
{"价格位数:" , 300, 350, WHITE, BLUE, &font48, 600, 350},				//24
{"输入位数:" , 300, 100, WHITE, BLUE, &font48, 600, 100},				//25
{"货道位数:" , 300, 150, WHITE, BLUE, &font48, 600, 150},				//26
{"恢复出厂(1 恢复):" , 300, 200, WHITE, BLUE, &font48, 600, 200},		//27
//支付设置
{"扫码支付(1开0关):" , 300, 50, WHITE, BLUE, &font48, 750, 50},			//28
{"刷卡接口:" , 300, 100, WHITE, BLUE, &font48, 600, 100},				//29
{"取货接口:" , 300, 150, WHITE, BLUE, &font48, 600, 150},				//30
//温度设置
{"目标温度:" , 300, 50, WHITE, BLUE, &font48, 550, 50},					//31
{"开启时间:" , 300, 100, WHITE, BLUE, &font48, 550, 100},				//32
{"关闭时间:" , 300, 150, WHITE, BLUE, &font48, 550, 150},				//33
{"温控方式(0冷1热):" , 300, 200, WHITE, BLUE, &font48, 800, 200},		//34
//控制回路设置
{"风机:" , 	300, 50, 	WHITE, BLUE, &font48, 550, 50},				//35
{"开启时间:" , 	300, 100, 	WHITE, BLUE, &font48, 550, 100},			//36
{"关闭时间:" , 	300, 150, 	WHITE, BLUE, &font48, 550, 150},			//37 
{"照明:" , 	300, 200, 	WHITE, BLUE, &font48, 550, 200},			//38
{"开启时间:" , 	300, 250, 	WHITE, BLUE, &font48, 550, 250},			//39
{"关闭时间:" , 	300, 300, 	WHITE, BLUE, &font48, 550, 300},			//40 
{"消毒:" , 	300, 350, 	WHITE, BLUE, &font48, 550, 350},			//41
{"开启时间:" , 	300, 50, 	WHITE, BLUE, &font48, 550, 50},				//42
{"关闭时间:" , 	300, 100, 	WHITE, BLUE, &font48, 550, 100},			//43 
{"玻璃加热:" , 	300, 150, 	WHITE, BLUE, &font48, 550, 150},			//44
{"开启时间:" , 	300, 200, 	WHITE, BLUE, &font48, 550, 200},			//45
{"关闭时间:" , 	300, 250, 	WHITE, BLUE, &font48, 550, 250},			//46 
{"备用2:" , 	300, 300, 	WHITE, BLUE, &font48, 550, 300},			//47
{"开启时间:" , 	300, 350, 	WHITE, BLUE, &font48, 550, 350},			//48
{"关闭时间:" , 	300, 50, 	WHITE, BLUE, &font48, 550, 50},				//49 
//货道设置
{"货道:" , 				300, 50, 	WHITE, BLUE, &font48, 550, 50},		//50
{"弹簧圈数:" , 			300, 100, 	WHITE, BLUE, &font48, 550, 100},	//51
{"状态(0禁能1使能):" , 	300, 150, 	WHITE, BLUE, &font48, 300, 200},	//52 
//货道测试
{"货道测试:" , 	300, 50, 	WHITE, BLUE, &font48, 550, 50},			//53
{"托盘测试:" , 	300, 100, 	WHITE, BLUE, &font48, 550, 100},		//54
{"整机测试:" , 	300, 150, 	WHITE, BLUE, &font48, 350, 400},		//55  //这个后面的参数是显示货道状态时，货道状态前面的货道号的位置
{"   无此机柜", 350 , 400, WHITE, BLUE, &font48},					//56
//机器设置
{"托盘数量:" , 0, 0, WHITE, BLUE, &font48, 10*8, 0},			//57
{"清空数据(1确认)" , 0, 4, WHITE, BLUE, &font48, 12*8, 2},	//58

//货道测试结果
{"执行", 11*8, 4, WHITE, BLUE, &font48, 12*8, 6},	//59

//日期时间设置
{"日期:", 0, 2, WHITE, BLUE, &font48, 6*8, 2},	//60
{"时间:", 0, 4, WHITE, BLUE, &font48, 6*8, 4},	//61

};	

/*
信息界面
*/
const WORD chs_info[] = {
	
	{"主板-" , 	0, 0, 	WHITE, BLUE, &font48, },			//0
	{"版本:" , 	0, 2, 	WHITE, BLUE, &font48, 6*8, 2},		//1
	{"ID:" , 	0, 4, 	WHITE, BLUE, &font48, 6*8, 4},		//2
	{"FLASH:", 	0 ,6, 	WHITE, BLUE, &font48, 6*8, 6},		//3

	{"驱动板-" , 0, 0, 	WHITE, BLUE, &font48, 6*8, 2},		//4
	{"版本:" , 	0, 2, 	WHITE, BLUE, &font48, 6*8, 4},		//5
	{"温度1:", 	0 ,4, 	WHITE, BLUE, &font48, 6*8, 6},		//6
	{"温度2:", 	0 ,6, 	WHITE, BLUE, &font48, 6*8, 6},		//7
	{"压缩机:", 	0 ,0, 	WHITE, BLUE, &font48, 6*8, 6},		//8
	{"风机:", 	0 ,2, 	WHITE, BLUE, &font48, 6*8, 6},		//9
	{"照明:", 	0 ,4, 	WHITE, BLUE, &font48, 6*8, 6},		//10
	{"消毒:", 	0 ,6, 	WHITE, BLUE, &font48, 6*8, 6},		//11
	{"备用1:", 	0 ,0, 	WHITE, BLUE, &font48, 6*8, 6},		//12
	{"备用2:", 	0 ,2, 	WHITE, BLUE, &font48, 6*8, 6},		//13

	{"读卡器-" , 0, 4, 	WHITE, BLUE, &font48, 6*8, 2},		//14
	{"状态:" , 	0, 6, 	WHITE, BLUE, &font48, 6*8, 4},		//15


	{"网络-" , 0, 0, 	WHITE, BLUE, &font48, 6*8, 2},		//16
	{"状态:" , 	0, 2, 	WHITE, BLUE, &font48, 6*8, 4},		//17
	{"待发:", 	0 ,4, 	WHITE, BLUE, &font48, 6*8, 6},		//18
	{"存储:", 	0 ,6, 	WHITE, BLUE, &font48, 6*8, 6},		//19
	{"已用" , 	0, 0, 	WHITE, BLUE, &font48, 6*8, 2},		//20
	{"读序号:",	0, 2, 	WHITE, BLUE, &font48, 6*8, 4},		//21
	{"写序号:", 	0 ,4, 	WHITE, BLUE, &font48, 6*8, 6},		//22
	{"交易号:", 	0 ,6, 	WHITE, BLUE, &font48, 6*8, 6},		//23
	{"流水号:", 	0 ,0, 	WHITE, BLUE, &font48, 6*8, 6},		//24

	{"硬币器-" , 0, 2, 	WHITE, BLUE, &font48, 6*8, 2},		//25
	{"状态:" , 	0, 4, 	WHITE, BLUE, &font48, 6*8, 4},		//26
	{"层级:", 	0 ,6, 	WHITE, BLUE, &font48, 6*8, 6},		//27
	{"小数:" ,  0, 0, 	WHITE, BLUE, &font48, 6*8, 2},		//28
	{"国家代码:",0 ,2, 	WHITE, BLUE, &font48, 6*8, 6},	//29
	{"通道:" , 0, 4, 	WHITE, BLUE, &font48, 6*8, 2},		//30
	{"面值:" , 	0, 6, 	WHITE, BLUE, &font48, 6*8, 4},		//31
	{"总金额:", 	0 ,0, 	WHITE, BLUE, &font48, 6*8, 6},		//32

	{"纸币器-:", 0 ,2, 	WHITE, BLUE, &font48, 6*8, 6},		//33
	{"状态:" , 	0, 4, 	WHITE, BLUE, &font48, 6*8, 4},		//34
	{"层级:", 	0 ,6, 	WHITE, BLUE, &font48, 6*8, 6},		//35
	{"小数:" , 	0, 0, 	WHITE, BLUE, &font48, 6*8, 2},		//36
	{"国家代码:",0 ,2, 	WHITE, BLUE, &font48, 6*8, 6},		//37
	{"通道:" , 	0, 4, 	WHITE, BLUE, &font48, 6*8, 2},		//38
	{"面值:" , 	0, 6, 	WHITE, BLUE, &font48, 6*8, 4},		//39
	{"总金额:", 	0 ,0, 	WHITE, BLUE, &font48, 6*8, 6},		//40
	{"暂存箱通道:" , 0, 2, 	WHITE, BLUE, &font48, 6*8, 4},	//41
	{"找零箱面值:" , 0, 4, 	WHITE, BLUE, &font48, 6*8, 2},	//42
	{"找零箱金额",	0 ,6, 	WHITE, BLUE, &font48, 6*8, 6},	//43

	{"CPU",	0 ,0, 	WHITE, BLUE, &font48, 6*8, 6},		//44

};

/*
驱动板设置界面
*/
const WORD chs_drvset[] = {
	{"适配类型(0N1J)" , 	0, 0, 	WHITE, BLUE, &font48, 8*15, 0},		//0
	{"温度类型(0-2)" , 	0, 2, 	WHITE, BLUE, &font48, 8*15, 2},		//1
	{"启动电流(MA)" , 	0, 4, 	WHITE, BLUE, &font48, 8*12, 4},		//2
	{"短路电流(MA)" , 	0, 0, 	WHITE, BLUE, &font48, 8*12, 0},		//3
	{"过载电流(MA)" , 	0, 2, 	WHITE, BLUE, &font48, 8*12, 2},		//4
	{"过载时间(MS)" , 	0, 4, 	WHITE, BLUE, &font48, 8*12, 4},		//5
	{"超时未启动(S)" , 	0, 0, 	WHITE, BLUE, &font48, 8*14, 0},	//6
	{"超时未停止(S)" , 	0, 2, 	WHITE, BLUE, &font48, 8*14, 2},	//7
	{"停止延时(MS)" ,	0, 4, 	WHITE, BLUE, &font48, 8*12, 4},	//8
	{"信号检测次数" , 	0, 0, 	WHITE, BLUE, &font48, 8*14, 0},	//9
	{"检测间隔(US)" ,	0, 2, 	WHITE, BLUE, &font48, 8*12, 2},	//10
	{"恢复默认(1确认)" ,	0, 4, 	WHITE, BLUE, &font48},			//11
	{"驱动板设置" , 	3*8, 0, 	WHITE, BLUE, &font48},			//12
	{"请输入密码" , 	3*8, 2, 	WHITE, BLUE, &font48, 3*8, 4},	//13
};

#elif SCREEN == V_SCREENRA8875
/*============================================================================================
==============================================================================================
彩色屏幕 竖屏（目前只支持RA8875）
==============================================================================================
============================================================================================*/

/*
警告提示信息
*/
const WORD chs_warring[] = {
{"设置成功", 	140, 750, WHITE, BLUE, &font48},	//0
{"设置失败", 	140, 750, WHITE, BLUE, &font48},	//1
{"设置超限", 	140, 750, WHITE, BLUE, &font48},	//2
{"设置位数不足", 140, 750, WHITE, BLUE, &font48},	//3
{"正在测试中", 	140, 750, WHITE, BLUE, &font48},	//4
{"正在恢复中...",140, 750, WHITE, BLUE, &font48},	//5
{"恢复成功     ",140, 750, WHITE, BLUE, &font48},	//6
{"显示时间", 			0, 0, WHITE, BLUE, &font48},//7
{"HardFault_Handler", 	0, 0, WHITE, BLUE, &font48},//8

//图标
{"號", 		10,  100, WHITE, BLUE, &font48},	//信号图标			//9	
{"門", 		70, 100, WHITE, BLUE, &font48},	//门图标					//10
{"機", 		130,100, WHITE, BLUE, &font48},	//机柜图标				//11

//RTC提示
{"EEPROM恢复成功", 	30, 200, WHITE, BLUE, &font48},	//12
{"RTC初始化", 		30, 0, WHITE, BLUE, &font48},	//13
{"RTC配置成功", 		30, 50, WHITE, BLUE, &font48},	//14
{"CRYSTAL ERR", 	30, 100, WHITE, BLUE, &font48},	//15

//图标
{"凍", 	190, 100, WHITE, BLUE, &font48},//制冷图标					//16
{"燈", 	250, 100, WHITE, BLUE, &font48},//照明图标					//17
{"殺", 	310, 100, WHITE, BLUE, &font48},//杀毒图标 					//18
{"攝", 10, 400, WHITE, BLUE, &font48, 110, 400},//未装温度探头图标 	//19 后面坐标表示快捷键查询中正常温度值的位置
{"熱", 190, 100, WHITE, BLUE, &font48},//制热图标 					//20
{"撥", 370, 100, WHITE, BLUE, &font48},//玻璃加热图标 				//21
{"數", 230, 400, WHITE, BLUE, &font48, 280, 400},//待发送数据图标 	//22
{"溫", 60, 400, WHITE, BLUE, &font48, 160, 400},//摄氏度图标 		//23 后面2个坐标表示快捷键查询温度图标的另一个位置
{"讀", 0, 6, WHITE, BLUE, &font48, 6*8, 6},//刷卡器图标				//24

};


/*
待机
*/
const WORD chs_standby[] = {
{"欢迎光临",	50, 300, WHITE, BLUE, &font96},			//0
{"欢迎光临",	50, 300, WHITE, BLUE, &font96},			//1
{"欢迎光临",	50, 300, WHITE, BLUE, &font96},			//2
{"欢迎光临",	50, 300, WHITE, BLUE, &font96},			//3
//#键查询在线卡信息界面
{"卡号:", 		30, 30, WHITE, BLUE, &font48, 200, 30},		//4
{"余额:", 		30, 80, WHITE, BLUE, &font48, 200, 80},		//5
{"限购:", 		30, 130, WHITE, BLUE, &font48,200, 130},		//6
{"请刷卡查询", 	30, 700, WHITE, BLUE, &font48, 600, 400},	//7
{"网络未连接", 	30, 700, WHITE, BLUE, &font48, 600, 400},	//8
{"不限", 		200, 130, WHITE, BLUE, &font48},			//9
{"次", 			250, 130, WHITE, BLUE, &font48},			//10
{"卡不存在  ", 	30, 700, WHITE, BLUE, &font48, 14*8,6},		//11
{"无效卡    ", 	30, 700, WHITE, BLUE, &font48, 14*8,6},		//12
{"非本校卡  ", 	30, 700, WHITE, BLUE, &font48, 14*8,6},		//13
{"测试卡    ", 	30, 700, WHITE, BLUE, &font48, 14*8,6},		//14
{"非本机卡  ", 	30, 700, WHITE, BLUE, &font48, 14*8,6},		//15
{"处理异常  ", 	30, 700, WHITE, BLUE, &font48, 14*8,6},		//16

//欢迎光临辅助提示
{"网络故障无法消费", 50, 600, WHITE, BLUE, &font48},		//17
{"刷卡故障无法消费", 50, 600, WHITE, BLUE, &font48},		//18
{"机器故障无法消费", 50, 600, WHITE, BLUE, &font48},		//19
{"   请选择商品   ", 50, 600, WHITE, BLUE, &font48},		//20

};

/*
开机界面
*/
const WORD chs_start[] = {
	{"正在检测设备...",  0, 0, WHITE, BLUE, &font48},		//0
	{"纸币器正常",		0, 150, WHITE, BLUE, &font48},		//1
	{"硬币器正常", 		0, 100, WHITE, BLUE, &font48},		//2
	{"纸币器未连接", 	0, 150, WHITE, BLUE, &font48},		//3
	{"硬币器未连接", 	0, 100, WHITE, BLUE, &font48},		//4
	{"驱动板正常", 		0, 250, WHITE, BLUE, &font48},		//5
	{"驱动板未连接", 	0, 250, WHITE, BLUE, &font48},		//6
	{"读卡器正常", 		0, 200, WHITE, BLUE, &font48},		//7
	{"读卡器未连接", 	0, 200, WHITE, BLUE, &font48},		//8
};

/*
选货界面
*/	
const WORD chs_choose[] = {
{"商品:",  30, 30, WHITE, BLUE, &font48, 180, 30},		//0
{"价格:" , 30, 80, WHITE, BLUE, &font48, 180, 80},		//1
{"库存:" , 30, 130, WHITE, BLUE, &font48, 180, 130},	//2
{"无此货道", 	30, 700, WHITE, BLUE, &font48}, 		//3
{"货道已禁用", 	30, 700, WHITE, BLUE, &font48}, 		//4
{"商品已售空", 	30, 700, WHITE, BLUE, &font48}, 		//5
{"货道停止服务", 30, 700, WHITE, BLUE, &font48}, 	//6
{"网络未连接", 	30, 700, WHITE, BLUE, &font48},		//7
{"请刷卡购物", 	30, 700, WHITE, BLUE, &font48, 600, 400}, 		//8
{"请刷卡或扫描二维码", 30, 700, WHITE, BLUE, &font48},	//9
{"已读卡后台正验证  ", 30, 700, WHITE, BLUE, &font48},	//10
//在线卡出货
{"卡号:", 30, 180, WHITE, BLUE, &font48, 180, 180},			//11
{"机柜暂停服务", 30, 700, WHITE, BLUE, &font48}, 	//12
{"余额:", 		30, 230, WHITE, BLUE, &font48, 180, 230},	//13
{"卡不存在不能消费", 30, 700, WHITE, BLUE, &font48},	//14
{" 无效卡不能消费 ", 30, 700, WHITE, BLUE, &font48},	//15
{"非本校卡不能消费", 30, 700, WHITE, BLUE, &font48},	//16
{"次数超限不能消费", 30, 700, WHITE, BLUE, &font48},	//17
{"非本机卡不能消费", 30, 700, WHITE, BLUE, &font48},	//18
{"余额不足不能消费", 30, 700, WHITE, BLUE, &font48},	//19
{"第 层商品停售   ", 30, 700, WHITE, BLUE, &font48, 78, 400},	//20
{"扣款失败        ", 30, 700, WHITE, BLUE, &font48},	//21
{"处理异常不能消费", 30, 700, WHITE, BLUE, &font48},	//22
{"通知管理员      ", 30, 700, WHITE, BLUE, &font48},	//23
{"暂停服务        ", 30, 700, WHITE, BLUE, &font48}, //24
{"请扫码支付      ", 30, 700, WHITE, BLUE, &font48}, //25

{"价格错误请重选货",  30, 700, WHITE, BLUE, &font48},	//26	
{"库存错误请重选货",  30, 700, WHITE, BLUE, &font48},	//27	

};

/*
菜单界面
*/
const WORD chs_menu[] = {
{"商品设置" , 0, 50, WHITE, BLUE, &font48},//0
{"货道测试" , 0, 100, WHITE, BLUE, &font48},//1
{"货道设置" , 0, 150, WHITE, BLUE, &font48},//2
{"温控设置" , 0, 200, WHITE, BLUE, &font48},//3
{"控制设置" , 0, 250, WHITE, BLUE, &font48},//4
{"支付设置" , 0, 300, WHITE, BLUE, &font48},//5
{"机器设置" , 0, 350, WHITE, BLUE, &font48},//6
{"机器信息" , 1000, 0, WHITE, BLUE, &font48},//7		//这个不显示
{"通讯设置" , 300, 200, WHITE, BLUE, &font48},//8
{"管理设置" , 0, 0, WHITE, BLUE, &font48, 300, 0}, //9
{"售货机信息" , 1000, 0, WHITE, BLUE, &font48}, //10			//这个不显示
//侧边栏显示
{"机器ID:", 0, 400 , WHITE, BLUE, &font48, 180, 400},//11
{"版本号:", 0, 450,  WHITE, BLUE, &font48, 180, 450},//12
{"驱动板:", 0, 500,  WHITE, BLUE, &font48, 180, 500},//13
{"正常", 	0, 150, WHITE, BLUE, &font48},//14
{"未连接", 	0, 150, WHITE, BLUE, &font48},//15
{"读卡器:", 0, 550, WHITE, BLUE, &font48, 180, 550},//16
{"纸币器:", 0, 600, WHITE, BLUE, &font48, 180, 600},//17
{"硬币器:", 0, 650, WHITE, BLUE, &font48, 180, 650},//18
{"网  络:", 0, 700, WHITE, BLUE, &font48, 180, 700},//19
{"连网", 	0, 350, WHITE, BLUE, &font48},//20
{"未连网", 	0, 350, WHITE, BLUE, &font48},//21
{"温  度:", 0, 750, WHITE, BLUE, &font48, 180, 750},//22
{"故障", 	0, 400, WHITE, BLUE, &font48},//23
{"通讯密码:",0, 400, WHITE, BLUE, &font48},//24
};
/*
出货界面
*/
const WORD chs_salling[] = {

{"余额", 30, 230, WHITE, BLUE, &font48},							//0
{"正在出货请稍候     ", 30, 700, WHITE, BLUE, &font48},				//1
{"出货成功           ", 30, 700, WHITE, BLUE, &font48},				//2
{"出货失败           ", 30, 700, WHITE, BLUE, &font48, 400, 700},	//3
{"请取走商品         ", 30, 700, WHITE, BLUE, &font48},				//4
};

/*
设置界面提示语
*/
const WORD chs_set[] = {

{"设置成功", 	140, 750, WHITE, BLUE, &font48},	//0
{"设置失败", 	140, 750, WHITE, BLUE, &font48},	//1
{"设置超限", 	140, 750, WHITE, BLUE, &font48}, 	//2
{"设置位数不足", 140, 750, WHITE, BLUE, &font48},	//3
{"正在测试", 148, 750, WHITE, BLUE, &font48},	//4
{"正常电机数", 500, 400, WHITE, BLUE, &font48},	//5
{"不在位电机", 500, 400, WHITE, BLUE, &font48},	//6
{"正在恢复出厂..." , 500, 400, WHITE, BLUE, &font48},	//7
{"恢复成功", 500, 400, WHITE, BLUE, &font48},			//8
{"条码",	500, 400, WHITE, BLUE, &font48},			//9

//货道测试
{"机柜", 		0 , 	400, WHITE, BLUE, &font48, 96, 	400},		//10
{"托盘", 		144 , 	400, WHITE, BLUE, &font48, 240, 400},		//11
{"0123456789", 	0 , 	450, WHITE, BLUE, &font48, 500,	550},		//12	表示货道,valx为货道使能纵坐标，valy为货道状态纵坐标
{"输入", 		288 , 	400, WHITE, BLUE, &font48, 290, 450},		//13	

//商品设置
{"商品:" , 0, 400, WHITE, BLUE, &font48, 150, 400},	//14
{"库存:" , 0, 450, WHITE, BLUE, &font48, 150, 450},	//15
{"价格:" , 0, 500, WHITE, BLUE, &font48, 150, 500},	//16
{"条码:" , 0, 550, WHITE, BLUE, &font48, 150, 550},	//17
//机器设置
{"机器ID:" ,   			0, 400, WHITE, BLUE, &font48, 200, 400},		//18
{"连网密码:" , 			0, 450, WHITE, BLUE, &font48, 250, 450},		//19
{"开启连网(0关1开):" , 	0, 500, WHITE, BLUE, &font48, 430, 500},		//20
{"系统语言(0中1英):" , 	0, 550, WHITE, BLUE, &font48, 430, 550},		//21
{"机柜数量(1-9):" , 	0, 600, WHITE, BLUE, &font48, 430, 600},		//22
{"小数位数(0-8):" ,	 	0, 650, WHITE, BLUE, &font48, 430, 650},		//23
{"价格位数(1-9):" , 	0, 700, WHITE, BLUE, &font48, 430, 700},		//24
{"输入位数(1-9):" , 	0, 400, WHITE, BLUE, &font48, 430, 400},		//25
{"货道位数(1-3):" , 	0, 450, WHITE, BLUE, &font48, 430, 450},		//26
{"恢复出厂(1 确认) ",	0, 700, WHITE, BLUE, &font48, 400, 700},		//27
//支付设置
{"扫码支付(0关1开):",	0, 400, WHITE, BLUE, &font48, 430, 400},	//28
{"刷卡接口:" , 	0, 450, WHITE, BLUE, &font48, 300, 450},			//29
{"取货接口:" , 	0, 500, WHITE, BLUE, &font48, 300, 500},			//30
//温度设置
{"目标温度:" , 			0, 400, WHITE, BLUE, &font48, 250, 400},	//31
{"开启时间:" , 			0, 450, WHITE, BLUE, &font48, 250, 450},	//32
{"关闭时间:" , 			0, 500, WHITE, BLUE, &font48, 250, 500},	//33
{"温控方式(0冷1热):" , 	0, 550, WHITE, BLUE, &font48, 400, 550},	//34
//控制回路设置
{"风机:" , 		0, 400, 	WHITE, BLUE, &font48, 300, 400},	//35
{"开启时间:" , 	0, 450, 	WHITE, BLUE, &font48, 300, 450},	//36
{"关闭时间:" , 	0, 500, 	WHITE, BLUE, &font48, 300, 500},	//37 
{"照明:" , 		0, 550, 	WHITE, BLUE, &font48, 300, 550},	//38
{"开启时间:" , 	0, 600, 	WHITE, BLUE, &font48, 300, 600},	//39
{"关闭时间:" , 	0, 650, 	WHITE, BLUE, &font48, 300, 650},	//40 
{"消毒:" , 		0, 700, 	WHITE, BLUE, &font48, 300, 700},	//41
{"开启时间:" , 	0, 400, 	WHITE, BLUE, &font48, 300, 400},	//42
{"关闭时间:" , 	0, 450, 	WHITE, BLUE, &font48, 300, 450},	//43 
{"玻璃加热:" , 	0, 500, 	WHITE, BLUE, &font48, 300, 500},	//44
{"开启时间:" , 	0, 550, 	WHITE, BLUE, &font48, 300, 550},	//45
{"关闭时间:" , 	0, 600, 	WHITE, BLUE, &font48, 300, 600},	//46 
{"备用2:" , 	0, 650, 	WHITE, BLUE, &font48, 300, 650},	//47
{"开启时间:" , 	0, 700, 	WHITE, BLUE, &font48, 300, 700},	//48
{"关闭时间:" , 	0, 400, 	WHITE, BLUE, &font48, 300, 400},	//49 
//货道设置
{"货道:" , 				0, 400, 	WHITE, BLUE, &font48, 300, 400},	//50
{"弹簧圈数:" , 			0, 450, 	WHITE, BLUE, &font48, 300, 450},	//51
{"状态(0禁能1使能):" , 	0, 500, 	WHITE, BLUE, &font48, 30, 550},		//52 	//这个货道使能禁能显示到下一行
//货道测试
{"货道测试:" , 	0, 400, 	WHITE, BLUE, &font48, 300, 400},	//53
{"托盘测试:" , 	0, 450, 	WHITE, BLUE, &font48, 300, 450},	//54
{"整机测试:" , 	0, 500, 	WHITE, BLUE, &font48, 50, 750},	//55  //这个后面的参数是显示货道状态时，货道状态前面的货道号的位置
{"   无此机柜", 50 , 750, WHITE, BLUE, &font48},		//56

//机器设置
{"托盘数量:" , 			0, 500, WHITE, BLUE, &font48, 430, 500},	//57
{"清空数据(1 确认)" , 	0, 650, WHITE, BLUE, &font48, 0, 650},	//58

//货道测试结果
{"执行", 288, 500, WHITE, BLUE, &font48, 290, 550},	//59

//日期时间设置
{"日期:", 0, 550, WHITE, BLUE, &font48, 150, 550},	//60
{"时间:", 0, 600, WHITE, BLUE, &font48, 150, 600},	//61

};	


/*
信息界面
*/
const WORD chs_info[] = {
	
	{"主板-->" ,10, 400, 	WHITE, BLUE, &font48, 10, 450},	//0	后面两个坐标表示快捷键查看信息的版本号位置
	{"版本:" , 	10, 450, 	WHITE, BLUE, &font48, 6*24, 450},	//1
	{"FLASH:", 	10 ,500, 	WHITE, BLUE, &font48, 8*24, 500},	//2
	{"ID:" , 	10, 550, 	WHITE, BLUE, &font48, 6*24, 550},	//3
	
	{"驱动板-->" , 10, 600, 	WHITE, BLUE, &font48, 10, 500},		//4	后面两个坐标表示快捷键查看信息的版本号位置
	{"版本:" , 	10, 650, 	WHITE, BLUE, &font48, 6*24, 650},		//5
	{"温度1:", 	10 ,700, 	WHITE, BLUE, &font48, 8*24, 700},		//6
	{"温度2:", 	10 ,750, 	WHITE, BLUE, &font48, 8*24, 750},		//7
	{"压缩机:", 10 ,400, 	WHITE, BLUE, &font48, 6*24, 400},		//8
	{"风机:", 	10 ,450, 	WHITE, BLUE, &font48, 6*24, 450},		//9
	{"照明:", 	10 ,500, 	WHITE, BLUE, &font48, 6*24, 500},		//10
	{"消毒:", 	10 ,550, 	WHITE, BLUE, &font48, 6*24, 550},		//11
	{"备用1:", 	10 ,600, 	WHITE, BLUE, &font48, 6*24, 600},		//12
	{"备用2:", 	10 ,650, 	WHITE, BLUE, &font48, 6*24, 650},		//13

	{"读卡器-->" , 10, 700, 	WHITE, BLUE, &font48},		//14
	{"状态:" , 	10, 750, 	WHITE, BLUE, &font48, 8*24, 750},		//15


	{"网络-->" ,10, 400, 	WHITE, BLUE, &font48,},		//16
	{"状态:" , 	10, 450, 	WHITE, BLUE, &font48, 8*24, 450},		//17
	{"待发:", 	10 ,500, 	WHITE, BLUE, &font48, 8*24, 500},		//18
	{"存储:", 	10 ,550, 	WHITE, BLUE, &font48, 8*24, 550},		//19
	{"已用:" , 	10, 600, 	WHITE, BLUE, &font48, 8*24, 600},		//20
	{"读序号:",	10, 650, 	WHITE, BLUE, &font48, 8*24, 650},		//21
	{"写序号:", 10 ,700, 	WHITE, BLUE, &font48, 8*24, 700},		//22
	{"交易号:", 10 ,750, 	WHITE, BLUE, &font48, 8*24, 750},		//23
	{"流水号:", 10 ,400, 	WHITE, BLUE, &font48, 8*24, 400},		//24

	{"硬币器-->" , 10, 450, 	WHITE, BLUE, &font48, 6*24, 450},		//25
	{"状态:" , 	10, 500, 	WHITE, BLUE, &font48, 6*24, 500},		//26
	{"层级:", 	10 ,550, 	WHITE, BLUE, &font48, 6*24, 550},		//27
	{"小数:" ,  10, 600, 	WHITE, BLUE, &font48, 6*24, 600},		//28
	{"国家代码:",10 ,650, 	WHITE, BLUE, &font48, 6*24, 650},		//29
	{"通道:" , 10, 700, 	WHITE, BLUE, &font48, 6*24, 700},		//30
	{"面值:" , 10, 750, 	WHITE, BLUE, &font48, 6*24, 750},		//31
	{"金额:",  10 ,400, 	WHITE, BLUE, &font48, 6*24, 400},		//32

	{"纸币器-->", 10 ,450, 	WHITE, BLUE, &font48, 6*24, 450},		//33
	{"状态:" , 	10, 500, 	WHITE, BLUE, &font48, 6*24, 500},		//34
	{"层级:", 	10 ,550, 	WHITE, BLUE, &font48, 6*24, 550},		//35
	{"小数:" , 	10, 600, 	WHITE, BLUE, &font48, 6*24, 600},		//36
	{"国家代码:",10 ,650, 	WHITE, BLUE, &font48, 6*24, 650},		//37
	{"通道:" , 	10, 700, 	WHITE, BLUE, &font48, 6*24, 700},		//38
	{"面值:" , 	10, 750, 	WHITE, BLUE, &font48, 6*24, 750},		//39
	{"金额:", 	10 ,400, 	WHITE, BLUE, &font48, 6*24, 400},		//40
	{"暂存箱通道:" ,10, 450, 	WHITE, BLUE, &font48, 6*24, 450},	//41
	{"找零箱面值:" , 10, 500, 	WHITE, BLUE, &font48, 6*24, 500},	//42
	{"找零箱金额:",	10 ,550, 	WHITE, BLUE, &font48, 6*24, 550},	//43

	{"CPU->",	10 ,600, 	WHITE, BLUE, &font48, 6*8, 0},		//44

};

/*
驱动板设置界面
*/
const WORD chs_drvset[] = {
	{"适配类型(0N1J):" , 	10, 400, 	WHITE, BLUE, &font48, 24*16, 400},		//0
	{"温度类型(0-2):" , 	10, 450, 	WHITE, BLUE, &font48, 24*16, 450},	//1
	{"启动电流(MA):" , 	10, 500, 	WHITE, BLUE, &font48, 24*14, 500},	//2
	{"短路电流(MA):" , 	10, 550, 	WHITE, BLUE, &font48, 24*14, 550},	//3
	{"过载电流(MA):" , 	10, 600, 	WHITE, BLUE, &font48, 24*14, 600},	//4
	{"过载时间(MS):" , 	10, 650, 	WHITE, BLUE, &font48, 24*14, 650},	//5
	{"超时未启动(S):" , 	10, 700, 	WHITE, BLUE, &font48, 24*14, 700},	//6
	{"超时未停止(S):" , 	10, 400, 	WHITE, BLUE, &font48, 24*14, 400},	//7
	{"停止延时(MS):" ,	10, 450, 	WHITE, BLUE, &font48, 24*14, 450},	//8
	{"出货信号次数:" , 	10, 500, 	WHITE, BLUE, &font48, 24*14, 500},	//9
	{"信号间隔(US):" ,	10, 550, 	WHITE, BLUE, &font48, 24*14, 550},	//10
	{"查询信号次数:" , 	10, 600, 	WHITE, BLUE, &font48, 24*14, 600},	//11
	{"信号间隔(US):" ,	10, 650, 	WHITE, BLUE, &font48, 24*14, 650},	//12
	{"恢复默认(1确认):" ,10, 700, 	WHITE, BLUE, &font48},				//13
	
	{"驱动板设置:" , 	3*24, 450, 	WHITE, BLUE, &font48},				//14
	{"请输入密码:" , 	3*24, 500, 	WHITE, BLUE, &font48, 3*24, 550},	//15
	{"驱动板未连接:",	2*24, 500, WHITE, BLUE, &font48},				//16
};


#endif






CHARACTER pt_word; 


/*
版本号初始化
版本号第二位表示屏幕类型，对应screen值
*/
static void version_init(void)
{
	version[1] = SCREEN + '0';
	version[2] = YICHU_DOOR + '0';
	version[3] = ZJ_VS_XY + '0';
}

/*
字符界面初始化
*/
void word_init(void)
{
	version_init();
	if(g_vm.language == 1)//英文
	{
		
		
	}
	else
	{
		pt_word.standby = chs_standby;
		pt_word.choose = chs_choose;
		pt_word.menu = chs_menu;
		pt_word.salling = chs_salling;
		pt_word.set = chs_set;
		pt_word.start = chs_start; 
		pt_word.warring = chs_warring;
		pt_word.info = chs_info;
		pt_word.drvset = chs_drvset;
	}
	
}


