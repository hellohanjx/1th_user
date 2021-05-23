/*
状态机文件

*/
#ifndef LC256_H
#define LC256_H

#define CAT24WC256  0xA0            	// 定义器件地址

//放置在第1页
#define ID10               0x0000              //售货机基本信息
#define ID10_LENTH             38              //ID1的长度
#define ID11               0                   //售货机编号偏移地址
#define ID11_LENTH             5
#define ID12               7                  //生产厂家编号偏移地址
#define ID12_LENTH             7
#define ID13               14                  //售货机类型偏移地址
#define ID13_LENTH             2    
#define ID14               16                  //出厂日期
#define ID14_LENTH             4
#define ID15               20                  //部署大类
#define ID15_LENTH             4
#define ID16               24				   //部署子类
#define ID16_LENTH             4 
#define ID17               28				   //部署时间
#define ID17_LENTH             4
#define ID18               32                  //售货机汇报金额小数点位置
#define ID18_LENTH             1  
#define IDFLAG             33                  //设置标志
#define IDFLAG_LENTH           1
#define ID19               34
#define ID19_LENTH             4

//放置在第2页
 //通讯系统
#define CO10               0x0040             
#define CO10_LENTH             52
#define CO11               0                   //通讯使能
#define CO11_LENTH             1
#define CO12               1                   //本机识别号
#define CO12_LENTH             11
#define CO13               12                  //通讯密码
#define CO13_LENTH             7
#define CO14               19                  //数据中心地址
#define CO14_LENTH             30
#define CO15               49                  //通讯端口
#define CO15_LENTH             2
#define CO16               51                  //汇报频率
#define CO16_LENTH             1
#define CO17               52                  //黑名单下载使能
#define CO17_LENTH             1



//放置在第3页
#define TE10               0x0080			//温控系统信息
#if (EQU_TYPE==3 || EQU_TYPE==4)
	#define TE10_LENTH             47
#else
	#define TE10_LENTH             11
#endif
#define TE11               0                  //目标温度
#define TE11_LENTH             1
#define TE12               1                  //报警阀值
#define TE12_LENTH             1
#define TE13               2                  //温控方式
#define TE13_LENTH             1
#define TE14               3                  //温控开始时刻1
#define TE14_LENTH             2
#define TE15               5                  //温控结束时刻1
#define TE15_LENTH             2
#define TE16               7                  //温控开始时刻2
#define TE16_LENTH             2
#define TE17               9                  //温控结束时刻2
#define TE17_LENTH             2

#define WIND10				0x008F		//风机控制信息
#define WIND10_LENTH			13
#define WIND11				11			//开启方式 0定时开启，1强制开启，2强制关闭
#define WIND11_LENTH			1
#define WIND12				12			//风机开启时刻1
#define WIND12_LENTH			2		
#define WIND13				14			//风机关闭时刻1
#define WIND13_LENTH			2
#define WIND14				16			//风机开启时刻2
#define WIND14_LENTH			2
#define WIND15				18			//风机关闭时刻2
#define WIND15_LENTH			2

#define ULT10               0x009C      //紫外灯系统信息
#define ULT10_LENTH             13
#define ULT11               20          //开启方式 0定时开启，1强制开启，2强制关闭
#define ULT11_LENTH             1
#define ULT12               21          //紫外灯开启时刻1
#define ULT12_LENTH             2
#define ULT13               23          //紫外灯关闭时刻1
#define ULT13_LENTH             2
#define ULT14               25          //紫外灯开启时刻2
#define ULT14_LENTH             2
#define ULT15               27          //紫外灯关闭时刻2
#define ULT15_LENTH             2

#ifdef FOUR_DRV
#define LI10               0x00A9             //光照系统信息
#define LI10_LENTH             13
#define LI11               0                  //开启方式 0定时开启，1强制开启，2强制关闭
#define LI11_LENTH             1
#define LI12               1                  //光照开启时刻1
#define LI12_LENTH             2
#define LI13               3                  //光照关闭时刻1
#define LI13_LENTH             2
#define LI14               5                  //光照开启时刻2
#define LI14_LENTH             2
#define LI15               7                  //光照关闭时刻2
#define LI15_LENTH             2
#else
#define LI10               0x00A9       //光照系统1信息
#define LI10_LENTH              13
#define LI11                29           //开启方式 0定时开启，1强制开启，2强制关闭
#define LI11_LENTH              1
#define LI12                30           //光照开启时刻1
#define LI12_LENTH              2
#define LI13                32           //光照关闭时刻1
#define LI13_LENTH              2
#define LI14                34           //光照开启时刻2
#define LI14_LENTH              2
#define LI15                36           //光照关闭时刻2
#define LI15_LENTH              2
#endif

#ifdef FOUR_DRV
#define ST10               0x00B6             //消毒系统信息
#define ST10_LENTH             13
#define ST11               0                  //开启方式，0定时开启，1强制开启，2强制关闭
#define ST11_LENTH             1
#define ST12               1                  //消毒开启时刻1
#define ST12_LENTH             2
#define ST13               3                  //消毒开启时刻1
#define ST13_LENTH             2
#define ST14               5                  //消毒开启时刻2
#define ST14_LENTH             2
#define ST15               7                  //消毒开启时刻2
#define ST15_LENTH             2
#else
#define ST10               0x00B6       //光照系统2信息
#define ST10_LENTH             13
#define ST11               38           //开启方式，0定时开启，1强制开启，2强制关闭
#define ST11_LENTH             1
#define ST12               39           //光照开启时刻1
#define ST12_LENTH             2
#define ST13               41           //光照开启时刻1
#define ST13_LENTH             2
#define ST14               43           //光照开启时刻2
#define ST14_LENTH             2
#define ST15               45           //光照开启时刻2
#define ST15_LENTH             2
#endif


//放置在第5页              
#define CA10               0x0100             //硬币器信息
#define CA10_LENTH             3   
#define CA11               0                  //硬币器使能
#define CA11_LENTH             1
#define CA12               1                  //硬币器货币代码
#define CA12_LENTH             2

#define CC00               0x0104            
#define CC00_LENTH              3              
#define CC01               0                  //面值
#define CC01_LENTH              2
#define CC02               2                  //接收使能
#define CC02_LENTH              1
#define CC0(X)             (CC00+CC00_LENTH*X) //硬币X的信息

#define CB10               0x0120            //溢币盒信息
#define CB10_LENTH             3
#define CB11               0                 //溢币盒使能
#define CB11_LENTH             1
#define CB12               1                 //溢币盒容量
#define CB12_LENTH             2

//放置在第6页              
#define BA10               0x0140            //纸币器信息
#define BA10_LENTH             3             
#define BA11               0                 //纸币器使能
#define BA11_LENTH             1
#define BA12               1                 //货币代码
#define BA12_LENTH             2

#define BC00               0x0144            
#define BC00_LENTH              3              
#define BC01               0                  //面值
#define BC01_LENTH              2
#define BC02               2                  //接收使能
#define BC02_LENTH              1
#define BC0(X)             (BC00+BC00_LENTH*X) //硬币X的信息

#define BB10               0x0160            //钞箱信息
#define BB10_LENTH             3
#define BB11               0                 //钞箱使能
#define BB11_LENTH             1
#define BB12               1                 //钞箱容量
#define BB12_LENTH             2


//放置在第7页         
#define CR10               0x0180            //读卡器信息
#define CR10_LENTH             3
#define CR11               0                 //读卡器使能
#define CR11_LENTH             1
#define CR12               1                 //最大响应时间
#define CR12_LENTH             2

#define PH10               0x01A0            //手机读卡器信息
#define PH10_LENTH             3 
#define PH11               0                 //手机读卡器使能
#define PH11_LENTH             1
#define PH12               1                 //最大响应时间
#define PH12_LENTH             2


//放置在第8页
#define PA10               0x01C0            //货道信息
#define PA10_LENTH             16
#define PA11               0                 //货道设置，0禁能 ，1开启，2联动
#define PA11_LENTH             1
#define PA12               1                 //货道的编号
#define PA12_LENTH             3
#define PA13               4                 //货道的容量
#define PA13_LENTH             1
#define PA14               5                 //商品编号
#define PA14_LENTH             4
#define PA15               9                 //商品价格
#define PA15_LENTH             3
#define PA17               12                //备份价格
#define PA17_LENTH             3
#define PA16               15                //0普通货道，1冷藏箱货道
#define PA16_LENTH             1
#define P(y,x)             (PA10+(100*y+x)*16)


//放置在83页
#define LYX0               0x1480//货道的销售数据
#define LYX0_LENTH             17
#define L01                0       //货道的库存
#define L01_LENTH              1
#define L02                1       //货道的销售金额
#define L02_LENTH              4
#define L03                5       //货道的销售次数
#define L03_LENTH              4 
#define L04				   9
#define L04_LENTH              4
#define L05			   	   13
#define L05_LENTH              4
#define L(y,x)             (LYX0+(100*y+x)*32)

//放置在233页              
#define initflag           0x3A00    
#define initflag_lenth         1       
#define password           0x3A01            
#define password_lenth         6
#define salemode           0x3A07
#define salemode_lenth         1
#define freesale           0x3A08
#define freesale_lenth         1
#define language           0x3A09
#define language_lenth         1
#define blacklistversion   0x3A0A
#define blacklistv_lenth       4
#define suppassword        0x3A0E
#define suppassword_lenth      6
#define peifu              0x3A14
#define peifu_lenth            1
#define dtst               0x3A15
#define dtst_lenth            1
#define dropmode		   0x3A16
#define dropmode_lenth	   	   1
#define chuhuomode			0x3A17	//出货方式 其他:指定货道 2循环 3分组
#define chuhuomode_lenth	   	1
#define paymentpoint		0x3A18	//支付设备小数点位置
#define paymentpoint_lenth		2	//正反存两份
#define storemode			0x3A19	//库存是否启用
#define storemode_lenth			1
#define coinmaxlevel		0x3A20	//硬币器最高层级设置
#define coinmaxlevel_lenth		1
#define tempCtrlMode		0x3A21	//温控方式
#define tempCtrlMode_lenth		1
#define heatTarget		0x3A22	//制热目标温度
#define heatTarget_lenth		1
#define lanuage_time_mode       0x3A23  //语音控制模式
#define lanuage_time_mode_len       1
#define lanuage_time_start     	0x3A24	//语音开始时刻
#define lanuage_time_start_len      2
#define lanuage_time_stop       0x3A26  //语音结束时刻
#define lanuage_time_stop_len       2

//升降机参数
#define LFT10		0x3A40
#define LFT10_LEN		5	//简易升降机有5个参数,盒饭机有7个参数
#define LFT11		0		//小车纵向移动格数
#define LFT11_LEN		2
#define LFT12		2		//盒饭:(出货延时时长);简易(货道电机提前启动的升降格数)
#define LFT12_LEN		1
#define LFT13		3		//盒饭:(小车到滑道高度);简易(翻板电机提前启动的升降格数)
#define LFT13_LEN		1
#define LFT14		4		//盒饭:(小车左右移动格数);简易升降机(弹簧出完货后的等待时间)
#define LFT14_LEN		1
#define LFT15		5		//托盘间隔
#define LFT15_LEN		1
#define LFT16		6		//货道间隔
#define LFT16_LEN		1
#define LIFT(y,x)		(LFT10+(y*100+x)*8)//这里8必须被64整除 //y货箱,x货道;每货道预留2个字节

//升降机参数左右格数(货道相关)
#define LIFT_BASKET1		0x4000	//盒饭机用
#define LIFT_BASKET1_LEN		1
#define LIFT_BASKET2		0x4001
#define LIFT_BASKET2_LEN		1
#define LIFT_BASKET3		0x4002
#define LIFT_BASKET3_LEN		1
#define LIFT_BASKET(y,x)	(LIFT_BASKET1+y*5+x)//y货箱,x货道(这里预留了2个参数)

//指示灯参数，盒饭机的LED灯
#define LED10		0x4000	
#define LED10_LEN		4
#define LED11		0		
#define LED11_LEN		1
#define LED12		1	
#define LED12_LEN		1
#define LED13		2	
#define LED13_LEN		1
#define LED14		3
#define LED14_LEN		1
#define LED(y,x)	(LED10+y*4+x)//y货箱,x货道(这里预留了2个参数)

//以下为弹簧与库存
#define SA10 			0X4B00
#define SA10_LEN         	2
#define SA11  			0		//库存
#define SA11_LEN			1	
#define SA12			1		//弹簧圈数
#define SA12_LEN			1
#define SA(y,x)         (SA10+(100*y+x)*2)

//存储云台出货时的配置参数(这个大小计算不对，只算了单货箱)
#define SALE10			0x4CA0
#define SALE10_LEN			2
#define SALE11			0
#define SALE11_LEN			1
#define SALE12			1
#define SALE12_LEN			1
#define SALE(y)         (SALE10+2*y)

//以下为黑名单
#define blacklistflag         0X4D00

//放置在第235页
#define batteryramdata     0X5200	//这个用来做备份数据的(目前可以不用了)






extern uint8 data_buf[64];
extern uint16 addr;

//存储锁机信息(其实为了安全可以把这些数据存储到其他数据中间)
//511页
#define LOCK_STATUS 		0x7FC0		//锁机状态标志位1Byte（存储两份，正反存）
#define LOCK_STATUS_LEN			2
#define LOCK_UNIT			0x7FC2		//倒计时时间单位1Byte（存储两份，正反存）
#define LOCK_UNIT_LEN			2
#define LOCK_LEFTTIME		0x7FC4		//倒计时4Byte（存储两份）
#define LOCK_LEFTTIME_LEN		8
#define LOCK_CNTCC			0x7FCC		//倒计时（一个备用区）4Byte
#define LOCK_CNTCC_LEN			8
#define LOCK_SN				0x7FD4		//机器序列号16Byte（存储两份）
#define LOCK_SN_LEN				32


void LC256Init(uint8);
void channel_sale_reset(void);//货道销售数据复位。
void savebatteryramdata(void);
void loadbatteryramdata(void);
#endif