/**
  * @file    RTC.c 
  * @author  WB R&D Team - openmcu666
  * @version V1.0
  * @date    2016.05.04
  * @brief   Real-Time Clock Driver
  */
	
#include "header.h"
#include "stm32_lib.h"
#include "time.h"
volatile _calendar_obj calendar;

#define RTC_USER_FLAG	0xAAAA	//RTC 标志位

uint32_t THH = 0, TMM = 0, TSS = 0;

//判断是否是闰年函数
//月份   1  2  3  4  5  6  7  8  9  10 11 12
//闰年   31 29 31 30 31 30 31 31 30 31 30 31
//非闰年 31 28 31 30 31 30 31 31 30 31 30 31
//year:年份
//返回值:该年份是不是闰年.1,是.0,不是
uint8_t Is_Leap_Year(uint16_t year)
{			  
	if(year%4==0) //必须能被4整除
	{ 
		if(year%100==0) 
		{ 
			if(year%400==0)return 1;//如果以00结尾,还要能被400整除 	   
			else return 0;   
		}else return 1;   
	}else return 0;	
}	 			   

//设置时钟 
//把输入的时钟转换为秒钟 
//以1970年1月1日为基准 
//1970~2099年为合法年份 
//返回值:0,成功;其他:错误代码. 
//月份数据表                         
uint8_t const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表      
//平年的月份日期表 
const uint8_t mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31}; 

static bool RTC_Set(uint16_t syear,uint8_t smon,uint8_t sday,uint8_t hour,uint8_t min,uint8_t sec) 
{ 
	uint16_t t; 
	uint32_t seccount=0; 
	if(syear<1970 || syear>2099)
		return FALSE;    
	
	for(t=1970;t<syear;t++)             //把所有年份的秒钟相加 
	{      
		if(Is_Leap_Year(t))
			seccount+=31622400;			//闰年的秒钟数 
		else 
			seccount+=31536000;         //平年的秒钟数 
	} 
	smon-=1; 
	for(t=0;t<smon;t++)                	//把前面月份的秒钟数相加 
	{    
		seccount+=(uint32_t)mon_table[t]*86400;    	//月份秒钟数相加 
		if(Is_Leap_Year(syear)&&t==1)
			seccount+=86400;					//闰年 2 月份增加一天的秒钟数
	} 
	seccount+=(uint32_t)(sday-1)*86400;                //把前面日期的秒钟数相加   
	seccount+=(uint32_t)hour*3600;                     //小时秒钟数 
	seccount+=(uint32_t)min*60;                        //分钟秒钟数 
	seccount+=sec;                                //最后的秒钟加上去 
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);      //使能PWR 和BKP 外设时钟    
	PWR_BackupAccessCmd(ENABLE);      				//使能 RTC和后备寄存器访问   
	RTC_SetCounter(seccount);                       //设置RTC计数器的值
	RTC_WaitForLastTask();                        	//等待最近一次对RTC寄存器的写操作完成    
	return TRUE;        
} 


//获得现在是星期几
//功能描述:输入公历日期得到星期(只允许1901-2099年)
//year,month,day：公历年月日 
//返回值：星期号																						 
static uint16_t RTC_Get_Week(uint16_t year,uint8_t month,uint8_t day)
{	
	uint16_t temp2;
	uint8_t yearH,yearL;
	
	yearH=year/100;	yearL=year%100; 
	// 如果为21世纪,年份数加100  
	if (yearH>19)yearL+=100;
	// 所过闰年数只算1900年之后的  
	temp2=yearL+yearL/4;
	temp2=temp2%7; 
	temp2=temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp2--;
	return(temp2%7);
}			  


//得到当前的时间，结果保存在calendar结构体里面
//返回值:0,成功;其他:错误代码.
static uint32_t RTC_Get(void)
{
	static uint16_t daycnt=0;
	uint32_t timecount=0; 
	uint32_t temp=0;
	uint16_t temp1=0;
	
	timecount = RTC_GetCounter();//其实就是得到计数器中总共多少秒，然后开始计算

 	temp=timecount/86400;   //得到天数(秒钟数对应的)
	if(daycnt!=temp)//超过一天了
	{	  
		daycnt=temp;
		temp1 = 1970;	//从1970年开始
		while(temp>=365)
		{				 
			if(Is_Leap_Year(temp1))//是闰年
			{
				if(temp>=366)temp-=366;//闰年的秒钟数
				else break;  
			}
			else 
				temp-=365;	  //平年 
			temp1++;  
		}   
		calendar.w_year=temp1;//得到年份
		temp1=0;
		while(temp>=28)//超过了一个月
		{
			if(Is_Leap_Year(calendar.w_year)&&temp1==1)//当年是不是闰年/2月份
			{
				if(temp>=29)temp-=29;//闰年的秒钟数
				else break; 
			}
			else 
			{
				if(temp>=mon_table[temp1])temp-=mon_table[temp1];//平年
				else break;
			}
			temp1++;  
		}
		calendar.w_month=temp1+1;	//得到月份
		calendar.w_date=temp+1;  	//得到日期 
	}
	temp=timecount%86400;     		//得到秒钟数   	   
	calendar.hour=temp/3600;     	//小时
	calendar.min=(temp%3600)/60; 	//分钟	
	calendar.sec=(temp%3600)%60; 	//秒钟
	calendar.week = RTC_Get_Week(calendar.w_year,calendar.w_month,calendar.w_date);//获取星期   
	return timecount;
}	 




/*
RTC与BKP配置
*/
static void RTC_Configuration(void) 
{	
	uint16_t	i = 0;
	
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE );	//使能电源与备份区时钟
	PWR_BackupAccessCmd(ENABLE);//允许访问BKP备份域
	BKP_TamperPinCmd(DISABLE);	//禁止侵入检测
	BKP_DeInit();				//复位备份域
	RCC_LSEConfig(RCC_LSE_ON);	//开启LSE
	
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET && i++ < 10000)//等待LSE起振
	{
		OSTimeDly(1);
	}
	if(i >= 10000)
	{
		lcd_show_character(&pt_word.warring[15]);//CRYSTAL ERR 晶振故障
	}
	
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//选择LSE为RTC时钟源
	RCC_RTCCLKCmd(ENABLE);			//RTC时钟使能
	RTC_WaitForLastTask();			//等待最后对RTC寄存器的写操作完成
	RTC_WaitForSynchro();			//等待RTC寄存器同步
	RTC_WaitForLastTask();			//等待最后对RTC寄存器的写操作完成
	RTC_EnterConfigMode();			//进入 RTC 配置模式
	RTC_SetPrescaler(32767); 		//设置RTC时钟分频值32767，则计数频率= (32.768 KHz)/(32767+1)=1Hz(1s)
	RTC_WaitForLastTask();	  		//等待最后对RTC寄存器的写操作完成
	RTC_Set(2017,5,1,0,0,0);    	//设置时间   
	RTC_ExitConfigMode();         	//退出RTC配置模式
	RTC_ITConfig(RTC_IT_SEC, ENABLE);//RTC 秒中断使能
}


//==========================
//RCT 中断分组设置 
//==========================
static void RTC_NVIC_Config(void)
{       
	NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}


/*
功能：1.RTC初始化；2.flash数据擦除或恢复；3.e2prom数据擦除或读取全局变量
*/
void Rtc_Init(void)
{
	
	if(BKP_ReadBackupRegister(BKP_DR1) != RTC_USER_FLAG)//检查备份区标志位
	{
		lcd_show_character(&pt_word.warring[13]);		//RTC尚未配置
		
		RTC_Configuration();                            //RTC初始化配置
		BKP_WriteBackupRegister(BKP_DR1, RTC_USER_FLAG);	 	//把备份区标志位写入BKP_DR1寄存器
		
		if(flash_poweron_init() == TRUE)//flash数据并没有丢失
		{
			flash_recovery();
		}
		lcd_show_character(&pt_word.warring[14]);		//RTC复位成功
		OSTimeDly(OS_TICKS_PER_SEC*2);
	}
	else//已配置过 RTC
	{
		if(RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)       	//检查POWER是否发生复位标志
		{
			//LCD_ShowString("电源复位",displayLeftOffset , ROW_1, WHITE, FONT_48);	//RTC尚未配置
		}
		else 
		if(RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)	 //检查PIN复位标志设置
		{
			//LCD_ShowString("按下复位键", displayLeftOffset , ROW_1, WHITE, FONT_48);
		}

		RCC_APB1PeriphClockCmd( RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE );	//使能电源与备份区时钟
		PWR_BackupAccessCmd(ENABLE);//允许访问BKP备份域
		BKP_TamperPinCmd(DISABLE);	//禁止侵入检测
		
		RTC_WaitForSynchro();	                    //等待RTC寄存器同步（等待最近一次对RTC寄存器的写操作完成 ）
		RTC_ITConfig(RTC_IT_SEC, ENABLE);	        //RTC 秒中断使能
		RTC_WaitForLastTask();	                    //等待最后对RTC寄存器的写操作完成
		
		flash_poweron_init();
	}
  	RTC_NVIC_Config();//RCT 中断设置
	RTC_Get();        //更新时间
	
	e2p_poweron_init();			//e2prom上电初始化
	e2p_get_global();			//读取全局设置
	
	return ;                                       
}


/*
提供给业务层复位rtc与flash
*/
void bkp_flash_reset(void)
{
	uint8_t dat[10];
	uint32_t time;
	
	time = RTC_Get();    	//读取时间 
	BKP_WriteBackupRegister(BKP_DR1, 0);	//清除标志位
	RTC_Configuration();
	BKP_WriteBackupRegister(BKP_DR1, RTC_USER_FLAG);	//恢复数据后重写备份区标志位，这里需要与开机检测标志位相同，否则会重新清除一次数据
	
	//恢复rtc时间
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);      //使能PWR 和BKP 外设时钟    
	PWR_BackupAccessCmd(ENABLE);      				//使能 RTC和后备寄存器访问   
	RTC_SetCounter(time);                       //设置RTC计数器的值
	RTC_WaitForLastTask();                        	//等待最近一次对RTC寄存器的写操作完成   
	
	do{
		sst26vf032b_erase(0, 0x3ff - 2);//擦除flash信息
		sst26vf032b_read(0, 10 ,dat);	//读测试
	}while(!(dat[0] == 0xff && dat[3] == 0xff && dat[5] == 0xff && dat[7] == 0xff && dat[9] == 0xff));
}	
	


//======================================
//rtc 实时时钟中断
//======================================
void RTC_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_SEC) != RESET)//秒中断
	{
		RTC_ClearITPendingBit(RTC_IT_SEC);    		//清除中断标志位
		RTC_Get();									//更新时间
	}
//	if(RTC_GetITStatus(RTC_IT_ALR)!= RESET)         //闹钟中断 
//	{ 
//	  RTC_ClearITPendingBit(RTC_IT_ALR);      		//清闹钟中断         
//	}                                
	RTC_ClearITPendingBit(RTC_IT_SEC | RTC_IT_OW); 	//清秒中断与闹钟中断标志位
	RTC_WaitForLastTask();				        	//等待 RTC 写操作完成
  
}


void time_check(void)
{
//	 time_t timer = 3600,time2,time3;
//	uint8_t kk;
//	char tmp[50];
//	char *ttp;
//	//time_t??long int ??   
//	struct tm *tblock, *ff,*tt;
//	struct tm tkkkk,fuck;
//	ff = &fuck;
//	
//	tkkkk.tm_year = 117;
//	tkkkk.tm_mon = 6;
//	tkkkk.tm_mday = 17;
//	tkkkk.tm_hour = 20;
//	tkkkk.tm_min = 41;
//	tkkkk.tm_sec = 10;
////	tkkkk.tm_wday = 0;
//	
//	tkkkk.tm_isdst = 0;//不实行夏令时
////	tkkkk.tm_yday = 0;
//	
//	tblock = &tkkkk;
// 
//	time2 = mktime(tblock);//时间格式转换为秒，从1900年开始
//	
//	time3 = time2;

////	timer = time(NULL);//????????time(&timer);   //获得系统时间，这个要自己写
//	ff = localtime(&time3); //得到本地时间；可以用
//	ttp = asctime(ff);		//转换时间为字符串
////	ttp = ctime(&time3);	//转换时间为字符串
//	show_string (ttp,0,300);//可用
//	tt = gmtime(&time2);	//得到世界时间（用不了）
//	kk = 0;
//	
////	printf("%d year %02d month %02d day",(1900+p->tm_year),(1+p->tm_mon),p->tm_mday);

}

/*
设置系统时间
参数：type：0：时间/时间；1：日期+时间（网络对时，发来的年的只有后 2 位）
*/
bool set_rtc_time(uint8_t type, uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
	if(year > 100 && type == 1)
		return FALSE;
	if(month < 1 || month > 12)
		return FALSE;
	if(day < 1 || day > 31)
		return FALSE;
	if(hour > 23)
		return FALSE;
	if(min > 59)
		return FALSE;
	if(sec > 59)
		return FALSE;
	
	if(type == 0)//设置 日期/时间
	{
		return RTC_Set(year, month, day, hour, min, sec);
	}
	else
	if(type == 1)//设置 日期+时间
	{
		return RTC_Set(year + 2000, month, day, hour, min, sec);
	}
	return FALSE;
}

