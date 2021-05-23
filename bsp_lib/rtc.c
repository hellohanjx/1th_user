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

#define RTC_USER_FLAG	0xAAAA	//RTC ��־λ

uint32_t THH = 0, TMM = 0, TSS = 0;

//�ж��Ƿ������꺯��
//�·�   1  2  3  4  5  6  7  8  9  10 11 12
//����   31 29 31 30 31 30 31 31 30 31 30 31
//������ 31 28 31 30 31 30 31 31 30 31 30 31
//year:���
//����ֵ:������ǲ�������.1,��.0,����
uint8_t Is_Leap_Year(uint16_t year)
{			  
	if(year%4==0) //�����ܱ�4����
	{ 
		if(year%100==0) 
		{ 
			if(year%400==0)return 1;//�����00��β,��Ҫ�ܱ�400���� 	   
			else return 0;   
		}else return 1;   
	}else return 0;	
}	 			   

//����ʱ�� 
//�������ʱ��ת��Ϊ���� 
//��1970��1��1��Ϊ��׼ 
//1970~2099��Ϊ�Ϸ���� 
//����ֵ:0,�ɹ�;����:�������. 
//�·����ݱ�                         
uint8_t const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //���������ݱ�      
//ƽ����·����ڱ� 
const uint8_t mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31}; 

static bool RTC_Set(uint16_t syear,uint8_t smon,uint8_t sday,uint8_t hour,uint8_t min,uint8_t sec) 
{ 
	uint16_t t; 
	uint32_t seccount=0; 
	if(syear<1970 || syear>2099)
		return FALSE;    
	
	for(t=1970;t<syear;t++)             //��������ݵ�������� 
	{      
		if(Is_Leap_Year(t))
			seccount+=31622400;			//����������� 
		else 
			seccount+=31536000;         //ƽ��������� 
	} 
	smon-=1; 
	for(t=0;t<smon;t++)                	//��ǰ���·ݵ���������� 
	{    
		seccount+=(uint32_t)mon_table[t]*86400;    	//�·���������� 
		if(Is_Leap_Year(syear)&&t==1)
			seccount+=86400;					//���� 2 �·�����һ���������
	} 
	seccount+=(uint32_t)(sday-1)*86400;                //��ǰ�����ڵ����������   
	seccount+=(uint32_t)hour*3600;                     //Сʱ������ 
	seccount+=(uint32_t)min*60;                        //���������� 
	seccount+=sec;                                //�������Ӽ���ȥ 
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);      //ʹ��PWR ��BKP ����ʱ��    
	PWR_BackupAccessCmd(ENABLE);      				//ʹ�� RTC�ͺ󱸼Ĵ�������   
	RTC_SetCounter(seccount);                       //����RTC��������ֵ
	RTC_WaitForLastTask();                        	//�ȴ����һ�ζ�RTC�Ĵ�����д�������    
	return TRUE;        
} 


//������������ڼ�
//��������:���빫�����ڵõ�����(ֻ����1901-2099��)
//year,month,day������������ 
//����ֵ�����ں�																						 
static uint16_t RTC_Get_Week(uint16_t year,uint8_t month,uint8_t day)
{	
	uint16_t temp2;
	uint8_t yearH,yearL;
	
	yearH=year/100;	yearL=year%100; 
	// ���Ϊ21����,�������100  
	if (yearH>19)yearL+=100;
	// ����������ֻ��1900��֮���  
	temp2=yearL+yearL/4;
	temp2=temp2%7; 
	temp2=temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp2--;
	return(temp2%7);
}			  


//�õ���ǰ��ʱ�䣬���������calendar�ṹ������
//����ֵ:0,�ɹ�;����:�������.
static uint32_t RTC_Get(void)
{
	static uint16_t daycnt=0;
	uint32_t timecount=0; 
	uint32_t temp=0;
	uint16_t temp1=0;
	
	timecount = RTC_GetCounter();//��ʵ���ǵõ����������ܹ������룬Ȼ��ʼ����

 	temp=timecount/86400;   //�õ�����(��������Ӧ��)
	if(daycnt!=temp)//����һ����
	{	  
		daycnt=temp;
		temp1 = 1970;	//��1970�꿪ʼ
		while(temp>=365)
		{				 
			if(Is_Leap_Year(temp1))//������
			{
				if(temp>=366)temp-=366;//�����������
				else break;  
			}
			else 
				temp-=365;	  //ƽ�� 
			temp1++;  
		}   
		calendar.w_year=temp1;//�õ����
		temp1=0;
		while(temp>=28)//������һ����
		{
			if(Is_Leap_Year(calendar.w_year)&&temp1==1)//�����ǲ�������/2�·�
			{
				if(temp>=29)temp-=29;//�����������
				else break; 
			}
			else 
			{
				if(temp>=mon_table[temp1])temp-=mon_table[temp1];//ƽ��
				else break;
			}
			temp1++;  
		}
		calendar.w_month=temp1+1;	//�õ��·�
		calendar.w_date=temp+1;  	//�õ����� 
	}
	temp=timecount%86400;     		//�õ�������   	   
	calendar.hour=temp/3600;     	//Сʱ
	calendar.min=(temp%3600)/60; 	//����	
	calendar.sec=(temp%3600)%60; 	//����
	calendar.week = RTC_Get_Week(calendar.w_year,calendar.w_month,calendar.w_date);//��ȡ����   
	return timecount;
}	 




/*
RTC��BKP����
*/
static void RTC_Configuration(void) 
{	
	uint16_t	i = 0;
	
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE );	//ʹ�ܵ�Դ�뱸����ʱ��
	PWR_BackupAccessCmd(ENABLE);//�������BKP������
	BKP_TamperPinCmd(DISABLE);	//��ֹ������
	BKP_DeInit();				//��λ������
	RCC_LSEConfig(RCC_LSE_ON);	//����LSE
	
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET && i++ < 10000)//�ȴ�LSE����
	{
		OSTimeDly(1);
	}
	if(i >= 10000)
	{
		lcd_show_character(&pt_word.warring[15]);//CRYSTAL ERR �������
	}
	
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//ѡ��LSEΪRTCʱ��Դ
	RCC_RTCCLKCmd(ENABLE);			//RTCʱ��ʹ��
	RTC_WaitForLastTask();			//�ȴ�����RTC�Ĵ�����д�������
	RTC_WaitForSynchro();			//�ȴ�RTC�Ĵ���ͬ��
	RTC_WaitForLastTask();			//�ȴ�����RTC�Ĵ�����д�������
	RTC_EnterConfigMode();			//���� RTC ����ģʽ
	RTC_SetPrescaler(32767); 		//����RTCʱ�ӷ�Ƶֵ32767�������Ƶ��= (32.768 KHz)/(32767+1)=1Hz(1s)
	RTC_WaitForLastTask();	  		//�ȴ�����RTC�Ĵ�����д�������
	RTC_Set(2017,5,1,0,0,0);    	//����ʱ��   
	RTC_ExitConfigMode();         	//�˳�RTC����ģʽ
	RTC_ITConfig(RTC_IT_SEC, ENABLE);//RTC ���ж�ʹ��
}


//==========================
//RCT �жϷ������� 
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
���ܣ�1.RTC��ʼ����2.flash���ݲ�����ָ���3.e2prom���ݲ������ȡȫ�ֱ���
*/
void Rtc_Init(void)
{
	
	if(BKP_ReadBackupRegister(BKP_DR1) != RTC_USER_FLAG)//��鱸������־λ
	{
		lcd_show_character(&pt_word.warring[13]);		//RTC��δ����
		
		RTC_Configuration();                            //RTC��ʼ������
		BKP_WriteBackupRegister(BKP_DR1, RTC_USER_FLAG);	 	//�ѱ�������־λд��BKP_DR1�Ĵ���
		
		if(flash_poweron_init() == TRUE)//flash���ݲ�û�ж�ʧ
		{
			flash_recovery();
		}
		lcd_show_character(&pt_word.warring[14]);		//RTC��λ�ɹ�
		OSTimeDly(OS_TICKS_PER_SEC*2);
	}
	else//�����ù� RTC
	{
		if(RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)       	//���POWER�Ƿ�����λ��־
		{
			//LCD_ShowString("��Դ��λ",displayLeftOffset , ROW_1, WHITE, FONT_48);	//RTC��δ����
		}
		else 
		if(RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)	 //���PIN��λ��־����
		{
			//LCD_ShowString("���¸�λ��", displayLeftOffset , ROW_1, WHITE, FONT_48);
		}

		RCC_APB1PeriphClockCmd( RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE );	//ʹ�ܵ�Դ�뱸����ʱ��
		PWR_BackupAccessCmd(ENABLE);//�������BKP������
		BKP_TamperPinCmd(DISABLE);	//��ֹ������
		
		RTC_WaitForSynchro();	                    //�ȴ�RTC�Ĵ���ͬ�����ȴ����һ�ζ�RTC�Ĵ�����д������� ��
		RTC_ITConfig(RTC_IT_SEC, ENABLE);	        //RTC ���ж�ʹ��
		RTC_WaitForLastTask();	                    //�ȴ�����RTC�Ĵ�����д�������
		
		flash_poweron_init();
	}
  	RTC_NVIC_Config();//RCT �ж�����
	RTC_Get();        //����ʱ��
	
	e2p_poweron_init();			//e2prom�ϵ��ʼ��
	e2p_get_global();			//��ȡȫ������
	
	return ;                                       
}


/*
�ṩ��ҵ��㸴λrtc��flash
*/
void bkp_flash_reset(void)
{
	uint8_t dat[10];
	uint32_t time;
	
	time = RTC_Get();    	//��ȡʱ�� 
	BKP_WriteBackupRegister(BKP_DR1, 0);	//�����־λ
	RTC_Configuration();
	BKP_WriteBackupRegister(BKP_DR1, RTC_USER_FLAG);	//�ָ����ݺ���д��������־λ��������Ҫ�뿪������־λ��ͬ��������������һ������
	
	//�ָ�rtcʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);      //ʹ��PWR ��BKP ����ʱ��    
	PWR_BackupAccessCmd(ENABLE);      				//ʹ�� RTC�ͺ󱸼Ĵ�������   
	RTC_SetCounter(time);                       //����RTC��������ֵ
	RTC_WaitForLastTask();                        	//�ȴ����һ�ζ�RTC�Ĵ�����д�������   
	
	do{
		sst26vf032b_erase(0, 0x3ff - 2);//����flash��Ϣ
		sst26vf032b_read(0, 10 ,dat);	//������
	}while(!(dat[0] == 0xff && dat[3] == 0xff && dat[5] == 0xff && dat[7] == 0xff && dat[9] == 0xff));
}	
	


//======================================
//rtc ʵʱʱ���ж�
//======================================
void RTC_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_SEC) != RESET)//���ж�
	{
		RTC_ClearITPendingBit(RTC_IT_SEC);    		//����жϱ�־λ
		RTC_Get();									//����ʱ��
	}
//	if(RTC_GetITStatus(RTC_IT_ALR)!= RESET)         //�����ж� 
//	{ 
//	  RTC_ClearITPendingBit(RTC_IT_ALR);      		//�������ж�         
//	}                                
	RTC_ClearITPendingBit(RTC_IT_SEC | RTC_IT_OW); 	//�����ж��������жϱ�־λ
	RTC_WaitForLastTask();				        	//�ȴ� RTC д�������
  
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
//	tkkkk.tm_isdst = 0;//��ʵ������ʱ
////	tkkkk.tm_yday = 0;
//	
//	tblock = &tkkkk;
// 
//	time2 = mktime(tblock);//ʱ���ʽת��Ϊ�룬��1900�꿪ʼ
//	
//	time3 = time2;

////	timer = time(NULL);//????????time(&timer);   //���ϵͳʱ�䣬���Ҫ�Լ�д
//	ff = localtime(&time3); //�õ�����ʱ�䣻������
//	ttp = asctime(ff);		//ת��ʱ��Ϊ�ַ���
////	ttp = ctime(&time3);	//ת��ʱ��Ϊ�ַ���
//	show_string (ttp,0,300);//����
//	tt = gmtime(&time2);	//�õ�����ʱ�䣨�ò��ˣ�
//	kk = 0;
//	
////	printf("%d year %02d month %02d day",(1900+p->tm_year),(1+p->tm_mon),p->tm_mday);

}

/*
����ϵͳʱ��
������type��0��ʱ��/ʱ�䣻1������+ʱ�䣨�����ʱ�����������ֻ�к� 2 λ��
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
	
	if(type == 0)//���� ����/ʱ��
	{
		return RTC_Set(year, month, day, hour, min, sec);
	}
	else
	if(type == 1)//���� ����+ʱ��
	{
		return RTC_Set(year + 2000, month, day, hour, min, sec);
	}
	return FALSE;
}

