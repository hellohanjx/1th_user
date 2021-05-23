/*
带字库蓝屏
屏幕写满第一行，接着后面的写第3行
这个破屏幕，搞了整天才搞明白，比彩屏还麻烦
注意：用字库的话只能显示4行，并且位置不能变化
所以采用点阵来显示汉字
注意GDRAM的地址对应屏幕的相对关系

注意：用这个小屏幕一定要关掉fsmc，否则读数据时会出现问题（当读取得数据是0xfx与0xex时，读出的数据不对）

原理：这个屏幕一个地址需要写入2Byte 数据，所以写一个Byte时必须读出另一个Byte的值，否则会把不需要写的Byte对应位置刷掉
*/


#include "header.h" 
#include "stm32_lib.h"
#include "lcd_header.h" 


#if SCREEN == TM12864Z


/*
DDRAM地址
*/
const uint8_t AC_TABLE[] = {
0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,
0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,
0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
};

/*
引脚映射
PD14 -> RS	(D0)
PD0 -> R/W	(D2)
PE7 -> E	(D4)

PE9 -> DB0		(D6)
PE11 -> DB1		(D8)
PE13 -> DB2		(D10)
PE15 -> DB3		(D12)
PD9 -> DB4		(D14)
PG12 -> DB5		(FSMC_NE4)
PD5 -> DB6		(FSMC_NWE)
PG15 -> DB7		(TOUCH_RES#)
	
PG14 -> PSB		(I2C2_SCL)
PG13 -> RST		(I2C2_SDA)

电源与背光都接3v3
VO空，NC空，VOUT空
*/
#define CMD_TYPE(X)		(X & 0x1) ? Bit_SET : Bit_RESET

#define RS(X)		X ? GPIO_SetBits(GPIOD, GPIO_Pin_14) : GPIO_ResetBits(GPIOD, GPIO_Pin_14)
#define RW(X)		X ? GPIO_SetBits(GPIOD, GPIO_Pin_0) :  GPIO_ResetBits(GPIOD, GPIO_Pin_0)      
#define EN(X)   	X ? GPIO_SetBits(GPIOE, GPIO_Pin_7) :  GPIO_ResetBits(GPIOE, GPIO_Pin_7) 

#define DB0_W(X)		GPIO_WriteBit(GPIOE, GPIO_Pin_9,  X);
#define DB1_W(X)		GPIO_WriteBit(GPIOE, GPIO_Pin_11, X);
#define DB2_W(X)		GPIO_WriteBit(GPIOE, GPIO_Pin_13, X);
#define DB3_W(X)		GPIO_WriteBit(GPIOE, GPIO_Pin_15, X);
#define DB4_W(X)		GPIO_WriteBit(GPIOD, GPIO_Pin_9,  X);
#define DB5_W(X)		GPIO_WriteBit(GPIOG, GPIO_Pin_12, X);
#define DB6_W(X)		GPIO_WriteBit(GPIOD, GPIO_Pin_5,  X);
#define DB7_W(X)		GPIO_WriteBit(GPIOG, GPIO_Pin_15, X);

#define DB0_R		 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_9)
#define DB1_R		 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_11)
#define DB2_R		 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_13)
#define DB3_R		 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_15)
#define DB4_R		 GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_9)
#define DB5_R		 GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_12)
#define DB6_R		 GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_5)
#define DB7_R		 GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_15)

/*
设置数据引脚为浮空输入
*/
static void dat_pin_float (void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_5 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_15 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
}

/*
设置数据引脚为推挽输出
*/
static void dat_pin_out(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_5 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_15 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
}

//static volatile uint8_t pin_status = 0;//引脚状态：0：输入，1输出
 
/*
等待12768的忙状态结束
*/
static void wait_busy(void)
{
	uint32_t i = 0;
//	if(pin_status != 0)
//	{
//		dat_pin_float();	//数据引脚定义为浮空输入
//		pin_status = 0;
//	}
	dat_pin_float();
    RS(0);     			//RS = 0.
	RW(1);        		//RW = 1.
    EN(1);        		//EN = 1.
    while(GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_15) && i++ < 100000);//DB7 的值为1则说明液晶屏在忙
    EN(0);      		//EN = 0;
    dat_pin_out();      //数据引脚设为推挽输出
}


/*
写命令
写指令寄存器
*/
void tm12864_WriteCmd(uint8_t cmd)
{
	wait_busy();            //等待液晶屏忙完
//	if(pin_status != 1)
//	{
//		dat_pin_out();      //数据引脚设为推挽输出
//		pin_status = 1;
//		for(i = 0; i < 2300; i++){};//这里需要等待  
//	}


	RS(0);      //RS = 0.
    RW(0);      //RW = 0. 
    EN(1);		//EN = 1.
	DB0_W(CMD_TYPE(cmd >> 0));
	DB1_W(CMD_TYPE(cmd >> 1));
	DB2_W(CMD_TYPE(cmd >> 2));
	DB3_W(CMD_TYPE(cmd >> 3));
	DB4_W(CMD_TYPE(cmd >> 4));
	DB5_W(CMD_TYPE(cmd >> 5));
	DB6_W(CMD_TYPE(cmd >> 6));
	DB7_W(CMD_TYPE(cmd >> 7));
    EN(0);	//EN = 0
}


/*
写数据函数
*/
static void tm12864_WriteData(uint8_t  cmd)  
{
	wait_busy(); 
	
//	if(pin_status != 1)
//	{
//		dat_pin_out();      //数据引脚设为推挽输出
//		pin_status = 1;
//	}
	
    RS(1);	//RS = 1. 
    RW(0);	//RW = 0. 
    EN(1);	//EN = 1.
	
	DB0_W(CMD_TYPE(cmd >> 0));
	DB1_W(CMD_TYPE(cmd >> 1));
	DB2_W(CMD_TYPE(cmd >> 2));
	DB3_W(CMD_TYPE(cmd >> 3));
	DB4_W(CMD_TYPE(cmd >> 4));
	DB5_W(CMD_TYPE(cmd >> 5));
	DB6_W(CMD_TYPE(cmd >> 6));
	DB7_W(CMD_TYPE(cmd >> 7));

	EN(0);	//EN = 0;
}

/*
读数据函数
*/
static uint8_t tm12864_ReadData(void)  
{
	uint8_t tmp;
	uint32_t i;
	wait_busy();
//	if(pin_status != 0)
//	{
//		dat_pin_float();	//设置为输入引脚
//		pin_status = 0;
//	//	OSTimeDly(1);//
////		for(i = 0; i < 2000; i++){};//这里需要等待
//	}
	dat_pin_float();	//设置为输入引脚
//	for(i = 0; i < 1300; i++){};//这里需要等待
    RS(1);	//RS = 1. 
    RW(1);	//RW = 1. 
    EN(1);	//EN = 1.
	for(i = 0; i < 700; i++){};//这个等待要根据花屏情况来计算
	tmp = (DB0_R) | (DB1_R << 1) | (DB2_R << 2) | (DB3_R << 3) | (DB4_R << 4) | (DB5_R << 5) | (DB6_R << 6) | (DB7_R << 7); 
	EN(0);	//EN = 0;
	return tmp;
}


 /*
 引脚配置
 */
static void LCD_GPIO_Config(void)
{
    
	GPIO_InitTypeDef GPIO_InitStructure;
	
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOG , DISABLE);
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, DISABLE); //关闭fsmc时钟 
//	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM2, DISABLE); 
//	FSMC_NORSRAMDeInit(FSMC_Bank1_NORSRAM2);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOG , ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_0 | GPIO_Pin_9 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//初始化推挽输出
	GPIO_Init(GPIOD, &GPIO_InitStructure);
		
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7 | GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_15 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//初始化推挽输出
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12 | GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//初始化推挽输出
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOG, GPIO_Pin_14);     //PSB 设置为高电平。并口模式
	GPIO_ResetBits(GPIOG, GPIO_Pin_13);     //rst 设置为高电平
	OSTimeDly(100); 
	GPIO_SetBits(GPIOG, GPIO_Pin_13);     //rst 设置为高电平
//    GPIO_SetBits(GPIOE, GPIO_Pin_7);                    //EN = 1.
}


/*
初始化 12864
*/
static void tm12864_init(void)
{			
	
	OSTimeDly(2); 
	tm12864_WriteCmd(0x30); //8位并行，基础指令
    OSTimeDly(2); 
	tm12864_WriteCmd(0x03);	//AC归0（ DDRAM address）,不改变DDRAM内容；光标归零
    OSTimeDly(2); 
	tm12864_WriteCmd(0x0C); //显示ON，游标OFF，游标闪烁OFF
    OSTimeDly(2); 
    tm12864_WriteCmd(0x01); //清屏,AC = 0
    OSTimeDly(2); 
    tm12864_WriteCmd(0x06); //写入时,游标右移动，AC + 1
    OSTimeDly(2); 
    tm12864_WriteCmd(0x80); //DDRAM 地址计数器 AC = 0
    OSTimeDly(2); 

}

/*
写字符串
*/
void DisStr(char *s, uint8_t x, uint8_t y)
{
	tm12864_WriteCmd(AC_TABLE[8*x + y]);	//起始位置
	while(*s != '\0')
    {
        tm12864_WriteData(*s);
        s++;
        OSTimeDly(2);                                   
    }
}


/*
屏幕方向设置
*/
void bsp_lcd_set_directrion(uint8_t type, uint16_t *x, uint16_t *y)
{
	
}

void lcd_memory_init(void);


/*
lcd 硬件初始化
*/
void bsp_lcd_hard_congfigure(void)
{
	LCD_GPIO_Config();
	tm12864_init();
//	lcd_memory_init();//显存初始化
	
//	DisStr("商品已出货",  0 , 0 );
}

/*
驱动封装层获得物理参数
*/
uint16_t bsp_get_xsize(void)
{
	return 128;//横向范围
}
uint16_t bsp_get_ysize(void)
{
	return 64;//纵向范围
}


/*
显示游标
*/
void cusor_show(void)
{
	tm12864_WriteCmd(0x30);	//基础指令集
	tm12864_WriteCmd(0x0c); //显示游标,游标位反白
}




/*************************************************************************************
使用自定义显存方案
*************************************************************************************/
/*
为了提高刷屏速度，开辟1k = 128*64/8 控件作为显存
每次数据写完后，直接刷入屏幕
内存分布，按对应到屏幕为按行排列，0~15 为第一行，16~31为第二行，以此类推，共64行

- - - - - - - - - - - - - - - - 	0~15
- - - - - - - - - - - - - - - - 	16~31
- - - - - - - - - - - - - - - - 	32~47
- - - - - - - - - - - - - - - - 	48~63
- - - - - - - - - - - - - - - - 	64~79
- - - - - - - - - - - - - - - - 	80~85
- - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - 
*/


//uint8_t lcd_memory[1024];
//uint8_t lcd_memory_back[1024];

///*
//初始化内存
//*/
//void lcd_memory_init(void)
//{
//	uint16_t i;
//	for(i =0; i < 1024; i++)
//	{
//		lcd_memory[i] = 0;
//		lcd_memory_back[i] = 0;
//	}
//}

///*
//参数：	x，横坐标，取值范围0~15；
//		y，纵坐标，取值范围0~7
//*/
//static uint8_t show_16x16(const uint8_t *table_16, uint8_t y, uint8_t x, uint8_t type)
//{
//	uint8_t i, j, t, n;
//		
//	if(x > 6 || y > 14)//超过显示范围 (这里显示16*16，所以如果是半个字符直接退出不显示出来)x:0~15;y:0~31
//		return FALSE;
//	
//	y *= 8;	//每个字的纵坐标起始为8的倍数（因为允许在诸如1,3,5的行显示）
//	//更新显存
//	for(i = 0, j = 0; i < 16; i++)//按行更新数据
//	{
//		if(type == 1)
//		{
//			lcd_memory[(y + i)*16 + x] = ~table_16[j++];
//			lcd_memory[(y + i)*16 + x + 1] = ~table_16[j++];
//		}
//		else
//		{
//			lcd_memory[(y + i)*16 + x] = table_16[j++];
//			lcd_memory[(y + i)*16 + x + 1] = table_16[j++];
//		}
//	}

//	for(i = y;i < y + 16; i++)//刷16行
//	{
//		tm12864_WriteCmd(0x80 | i);	//水平坐标值
//		tm12864_WriteCmd(0x80);		//垂直坐标值
//		for(j = x/8*2; j < x/8*2+16; j++)//0~15上半屏，16~31下半屏~
//		{
//			t = j/16;
////			if(lcd_memory[ (i + t*32)*16 + j%16] != lcd_memory_back[(i + t*32)*16 + j%16])
//			tm12864_WriteData(lcd_memory[ (i + t*32)*16 + j%16]);
//		}
//	}
//	//这样写不对，要考虑那种半个字的情况，否则会写空
////	for(i = 0, j = 0; i < 16; i++)//按行更新数据
////	{
////		if(type == 1)
////		{
////			lcd_memory_back[(y + i)*16 + x] = ~table_16[j++];
////			lcd_memory_back[(y + i)*16 + x + 1] = ~table_16[j++];
////		}
////		else
////		{
////			lcd_memory_back[(y + i)*16 + x] = table_16[j++];
////			lcd_memory_back[(y + i)*16 + x + 1] = table_16[j++];
////		}
////	}
//	
//	
//	return TRUE;
//}


//static uint8_t show_16x8(const uint8_t *table_8, uint8_t y, uint8_t x, bool type)
//{
//	uint8_t i, j, t;
//	y *= 8;
//	//更新显存
//	for(i = 0, j =0; i < 16; i++)
//	{
//		if(type == 1)
//		lcd_memory[(y + i)*16 + x] = ~table_8[j++];
//		else
//		lcd_memory[(y + i)*16 + x] = table_8[j++];	
//	}
//	
//	
////	tm12864_WriteCmd(0x34);//关闭图形显示
//	//清屏
//	for(i = 0;i < 32; i++)//上下半屏每行同时刷
//	{
//		tm12864_WriteCmd(0x80 | i);	//水平坐标值
//		tm12864_WriteCmd(0x80);		//垂直坐标值
//		for(j = 0; j < 32; j++)//0~15上半屏，16~31下半屏~
//		{
//			t = j/16;
//			tm12864_WriteData(lcd_memory[ (i + t*32)*16 + j%16]);
//		}
//	}
////	tm12864_WriteCmd(0x36);//开图形显示
//	return TRUE;
//}




/********************************************************
按照屏幕内存分布刷屏
速度有点慢
********************************************************/

/*
显示16*8点阵
x：行（显示在第几行）
y：列（显示在第几列）
*/
static uint8_t show_16x8(const uint8_t *table_8, uint8_t x, uint8_t y, bool type)
{
	uint8_t i, n, k = 0;
	uint8_t dataH, dataL;
		
	if(x > 7 || y > 15)//超过显示范围 x:0~15;y:0~31
		return FALSE; 
	
//	tm12864_WriteCmd(0x34);//扩展指令集，关闭图形显示
	
	for(i = 0, n =0; i < 16; i++)//写16行，逐行写(16*16点阵)
	{
		if((x*8 + i) < 32)//上半屏：0~7
		{
			tm12864_WriteCmd(0x80 | (i + x*8));	//垂直地址(y,第0~31行)
			tm12864_WriteCmd(0x80 + y/2);		//水平地址(0~7上半屏；8~15下半屏)
//			for(j = 0; j < 2; j++)//一次要向GDRAM一个地址写入两个字节数据(每个地址对应两个数据字节)
			{
				//这里应该读取前半个字节的数据，然后连后半个字节一起写回
				//先写左半个字节
				tm12864_ReadData();
				dataH = tm12864_ReadData();
				dataL = tm12864_ReadData();
				//因为地址自动增加，所以这里要写回原地址
				tm12864_WriteCmd(0x80 | (i + x*8));	//垂直地址(y,第0~31行)
				tm12864_WriteCmd(0x80 + y/2);		//水平地址(0~7上半屏；8~15下半屏)
				if(y%2 == 0)
				{
					if(type == 1)
					tm12864_WriteData(~table_8[k++]);
					else
					tm12864_WriteData(table_8[k++]);
					tm12864_WriteData(dataL);
				}
				else
				{
					tm12864_WriteData(dataH);
					if(type == 1)
					tm12864_WriteData(~table_8[k++]);
					else
					tm12864_WriteData(table_8[k++]);

				}
			}
		}
		else
		if((x*8 + i) >= 32 )//下半屏 8~15
		{
			if(i == 0 && n == 0)
			{
				n = x*8 - 32;
			}
			
			tm12864_WriteCmd(0x80 | n);	//垂直地址(y,第0~31行)
			tm12864_WriteCmd(0x80 + y/2 + 8);	//水平地址(0~7上半屏；8~15下半屏)
//			for(j = 0; j < 2; j++)//一次要向GDRAM一个地址写入两个字节数据(每个地址对应两个数据字节)
			{
				//这里应该读取前半个字节的数据，然后连后半个字节一起写回
				//先写左半个字节
				tm12864_ReadData();
				dataH = tm12864_ReadData();
				dataL = tm12864_ReadData();
				//因为地址自动增加，所以这里要写回原地址
				tm12864_WriteCmd(0x80 | n);	//垂直地址(y,第0~31行)
				tm12864_WriteCmd(0x80 + y/2+ 8);	//水平地址(0~7上半屏；8~15下半屏)
				if(y%2 == 0)
				{
					if(type == 1)
					tm12864_WriteData(~table_8[k++]);
					else
					tm12864_WriteData(table_8[k++]);
					tm12864_WriteData(dataL);
				}
				else
				{
					tm12864_WriteData(dataH);
					if(type == 1)
					tm12864_WriteData(~table_8[k++]);
					else
					tm12864_WriteData(table_8[k++]);
				}
			}
			n++;
		}
			
	}
//	tm12864_WriteCmd(0x36);//打开图形显示
	return TRUE;
}



/*
显示16*16点阵
参数：table_16：16*16点阵;x：字符所在行；y：字符所在列；type :0 普通显示，1 反白显示

坐标约定
行(x)：0~7  间隔8个像素
列(y): 0~15 间隔8个像素

*/
static uint8_t show_16x16(const uint8_t *table_16, uint8_t x, uint8_t y, uint8_t type)
{
	uint8_t i, j ,n, k = 0;
	uint8_t dataH, dataL;
		
	if(x > 6 || y > 14)//超过显示范围 (这里显示16*16，所以半个字符不要显示了)x:0~15;y:0~31
		return FALSE;
	
	//	tm12864_WriteCmd(0x34);//扩展指令集，关闭图形显示(这个不要居然也可以)
	
	for(i = 0, n =0; i < 16; i++)//写16行，逐行写(16*16点阵)
	{
		if((x*8 + i) < 32)//上半屏：0~7
		{
			tm12864_WriteCmd(0x80 | (i + x*8));	//垂直地址(y,第0~31行)
			tm12864_WriteCmd(0x80 + y/2);		//水平地址(0~7上半屏；8~15下半屏)
			for(j = 0; j < 2; j++)//一次要向GDRAM一个地址写入两个字节数据(每个地址对应两个数据字节)
			{
				if(y%2 != 0)//2个数据字节不在一个地址中
				{
					//这里应该读取前半个字节的数据，然后连后半个字节一起写回
					//先写左半个字节
					if(j == 0)
					{
						tm12864_ReadData();
						dataH = tm12864_ReadData();
						dataL = tm12864_ReadData();
						//因为地址自动增加，所以这里要写回原地址
						tm12864_WriteCmd(0x80 | (i + x*8));	//垂直地址(y,第0~31行)
						tm12864_WriteCmd(0x80 + y/2);		//水平地址(0~7上半屏；8~15下半屏)
						tm12864_WriteData(dataH);//地址右半边字符为高地址字符
						if(type == 1)
							tm12864_WriteData(~table_16[k++]);
						else
							tm12864_WriteData(table_16[k++]);
					}
					else
					{
						//后写右半个字节
						tm12864_WriteCmd(0x80 | (i + x*8));	//垂直地址(y,第0~31行)
						tm12864_WriteCmd(0x80 + y/2 + 1 );		//水平地址(0~7上半屏；8~15下半屏)
						tm12864_ReadData();
						dataH = tm12864_ReadData();
						dataL = tm12864_ReadData();
						//因为地址自动增加，所以这里要写回原地址
						tm12864_WriteCmd(0x80 | (i + x*8));	//垂直地址(y,第0~31行)
						tm12864_WriteCmd(0x80 + y/2 + 1 );	//水平地址(0~7上半屏；8~15下半屏)
						if(type == 1)
							tm12864_WriteData(~table_16[k++]);
						else
							tm12864_WriteData(table_16[k++]);
						tm12864_WriteData(dataL);//地址左半边字符为低地址字符
					}
				}
				else//2个数据字节在同一个地址中
				{
					if(type == 1)
						tm12864_WriteData(~table_16[k++]);
					else
						tm12864_WriteData(table_16[k++]);

				}
			}
		}
		else
		if((x*8 + i) >= 32 )//下半屏 8~15
		{
			if(i == 0 && n == 0)
			{
				n = x*8 - 32;
			}
			
			tm12864_WriteCmd(0x80 | n);	//垂直地址(y,第0~31行)
			tm12864_WriteCmd(0x80 + y/2 + 8);	//水平地址(0~7上半屏；8~15下半屏)
			for(j = 0; j < 2; j++)//一次要向GDRAM一个地址写入两个字节数据(每个地址对应两个数据字节)
			{
				if(y%2 != 0)//2个数据字节不在一个地址中
				{
					//这里应该读取前半个字节的数据，然后连后半个字节一起写回
					//先写左半个字节
					if(j == 0)
					{
						tm12864_ReadData();
						dataH = tm12864_ReadData();
						dataL = tm12864_ReadData();
						//因为地址自动增加，所以这里要写回原地址
						tm12864_WriteCmd(0x80 | n);	//垂直地址(y,第0~31行)
						tm12864_WriteCmd(0x80 + y/2+ 8);	//水平地址(0~7上半屏；8~15下半屏)
						tm12864_WriteData(dataH);
						if(type == 1)
							tm12864_WriteData(~table_16[k++]);
						else
							tm12864_WriteData(table_16[k++]);
					}
					else
					{
						//后写右半个字节
						tm12864_WriteCmd(0x80 | n);	//垂直地址(y,第0~31行)
						tm12864_WriteCmd(0x80 + y/2 + 9);		//水平地址(0~7上半屏；8~15下半屏)
						tm12864_ReadData();
						dataH = tm12864_ReadData();
						dataL = tm12864_ReadData();
						//因为地址自动增加，所以这里要写回原地址
						tm12864_WriteCmd(0x80 | n);	//垂直地址(y,第0~31行)
						tm12864_WriteCmd(0x80 + y/2 + 9);	//水平地址(0~7上半屏；8~15下半屏)
						if(type == 1)
							tm12864_WriteData(~table_16[k++]);
						else
							tm12864_WriteData(table_16[k++]);
						tm12864_WriteData(dataL);
					}
				}
				else//2个数据字节在同一个地址中
				{
					if(type == 1)
						tm12864_WriteData(~table_16[k++]);
					else
						tm12864_WriteData(table_16[k++]);
				}
			}
			n++;
		}
			
	}
//	tm12864_WriteCmd(0x36);//打开图形显示
	return TRUE;
}












///////////////////////


/*
清屏
*/
void lcd_fill_color(uint16_t xStart,uint16_t yStart,uint16_t xLong,uint16_t yLong,uint16_t Color)
{
	uint32_t i,j;
	
//	for(i = 0; i < 1024; i++)
//		lcd_memory[i] = 0;
	
	tm12864_WriteCmd(0x34);//关闭图形显示
	//清屏
	for(i = 0;i < 32; i++)//上下半屏每行同时刷
	{
		tm12864_WriteCmd(0x80 | i);	//水平坐标值
		tm12864_WriteCmd(0x80);		//垂直坐标值
		for(j=0; j < 32; j++)
		{
			tm12864_WriteData(0);//(lcd_memory[i*j]);	 		
		}
	}
	tm12864_WriteCmd(0x36);//开图形显示
	
	//DDRAM清屏，用于显示字库字符的清屏
//	tm12864_WriteCmd(0x30);//基础指令集
//	tm12864_WriteCmd(0x1);//清屏
}

/*
显示二维码
参数：x ,y 起始坐标
xlong 显示宽度 因为是方形，长宽相同
scale 放大系数
dat 点阵，一个字节表示一个点
*/
void show_ercode(uint16_t x, uint16_t y, uint16_t xylong, uint8_t scale , uint8_t dat[][41]) 
{
	
	uint32_t i, j;
	uint8_t tmp_y[5], k;

	tm12864_WriteCmd(0x34);//关闭图形显示
	
	//上半屏显示
	for(i = 0;i < 32; i++)//上半屏的0~31行
	{
		tm12864_WriteCmd(0x80 | i);	//i在这里表示半屏的0~31行(纵坐标)
		tm12864_WriteCmd(0x80 | 5);	//这里0~7表示上半屏，8~15表示下半屏；这个是自增的(横坐标)
		
		for(j = 0; j < 5; j++)
		{
			for(k = 0, tmp_y[j] = 0; k < 8; k++)//8个点(8个字节)组成一个字节数据
			{
				tmp_y[j] |= dat[i][j*8 +k] << (7 - k);
			}
			tm12864_WriteData(~tmp_y[j]);	
		}
		//画最右边的竖线（因为一次写入8bit，1Byte，所以41点阵有一个点没写；
		//注意这里写1个字节，右边一个字节内不能有显示内容，否则会覆盖）
		tm12864_WriteData(~dat[i][40] << 7);
	}
	
	//下半屏显示
	for(i = 0; i < 9; i++)//下半屏的0~9行
	{
		tm12864_WriteCmd(0x80 | i);	//i在这里表示半屏的0~31行(纵坐标)
		tm12864_WriteCmd(0x80 | 13);//这里0~7表示上半屏，8~15表示下半屏；这个是自增的(横坐标)
		for(j = 0; j < 5; j++)
		{
			for(k = 0, tmp_y[j] = 0; k < 8; k++)//8个点(8个字节)组成一个字节数据
			{
				tmp_y[j] |= dat[i+32][j*8 +k] << (7 - k);
			}
			tm12864_WriteData(~tmp_y[j]);
		}
		//画最右边的竖线（因为一次写入8bit，1Byte，所以41点阵有一个点没写；
		//注意这里写1个字节，右边一个字节内不能有显示内容，否则会覆盖）
		tm12864_WriteData(~dat[i+32][40] << 7);
	}

	tm12864_WriteCmd(0x36);//开图形显示
}

/*
清除屏幕显示二维码
*/
void clear_ercode(uint16_t x, uint16_t y, uint16_t xylong, uint8_t scale) 
{
	
	uint32_t i, j;

	tm12864_WriteCmd(0x34);//关闭图形显示
	
	//上半屏显示
	for(i = 0;i < 32; i++)//上半屏的0~31行
	{
		tm12864_WriteCmd(0x80 | i);	//i在这里表示半屏的0~31行(纵坐标)
		tm12864_WriteCmd(0x80 | 5);	//这里0~7表示上半屏，8~15表示下半屏；这个是自增的(横坐标)
		
		for(j = 0; j < 5; j++)
		{
			tm12864_WriteData(0);	
		}
		//画最右边的竖线（因为一次写入8bit，1Byte，所以41点阵有一个点没写；
		//注意这里写1个字节，右边一个字节内不能有显示内容，否则会覆盖）
		tm12864_WriteData(0);
	}
	
	//下半屏显示
	for(i = 0; i < 9; i++)//下半屏的0~9行
	{
		tm12864_WriteCmd(0x80 | i);	//i在这里表示半屏的0~31行(纵坐标)
		tm12864_WriteCmd(0x80 | 13);//这里0~7表示上半屏，8~15表示下半屏；这个是自增的(横坐标)
		for(j = 0; j < 5; j++)
		{
			tm12864_WriteData(0);
		}
		//画最右边的竖线（因为一次写入8bit，1Byte，所以41点阵有一个点没写；
		//注意这里写1个字节，右边一个字节内不能有显示内容，否则会覆盖）
		tm12864_WriteData(0);
	}

	tm12864_WriteCmd(0x36);//开图形显示
}
/*
字模点阵打印
参数：	lattice 点阵指针；xSize 字符宽度；Ysize 字符高度（无用）；x 横坐标；y 纵坐标；
		color 字体颜色（高亮与否）；background 字体背景色（无用）
*/
void lcd_print_lattic(const uint8_t *lattice,uint32_t xSize,uint32_t ySize,uint16_t x,uint16_t y,uint16_t forcground,uint16_t background)
{
	bool type = FALSE;
	if(forcground == BLACK)//高亮显示
		type = TRUE;
	
	x /= 8;//计算相对值
	if(xSize == 8)
	{
		show_16x8(lattice, y, x, type);
	}
	else
	if(xSize == 16)
	{
		show_16x16(lattice, y, x, type);
	}

}

/*
字模点阵打印
参数：	lattice 点阵指针；xSize 字符宽度；Ysize 字符高度（无用）；x 横坐标；y 纵坐标；
		color 字体颜色（高亮与否）；background 字体背景色（无用）
*/
void print_lattice(const uint8_t *lattice,uint32_t xSize,uint32_t ySize,uint16_t x,uint16_t y,uint16_t color,uint16_t background)
{ 
	bool type = FALSE;
	if(color == BLACK)
		type = TRUE;
	
	x /= 8;//计算相对值
	if(xSize == 8)
	{
		show_16x8(lattice, y, x, type);
	}
	else
	if(xSize == 16)
	{
		show_16x16(lattice, y, x, type);
	}
}



#endif

