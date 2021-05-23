/* 
SST26VF032B 
驱动
2017-8-5
*/

#include "stm32_lib.h"
#include "ucos_ii.h"
#include "SST25VF016BDrv.h"

#define __F016B_WP     		0x20						//WP
#define __F016B_CS          0x40                		//P0.6口为25VF016B的片选 CS
#define __F032B_CE_Low()    	GPIOA->BRR = GPIO_Pin_4 	//CS为0	GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define __F032B_CE_High()   	GPIOA->BSRR = GPIO_Pin_4  	//CS为1  GPIO_SetBits(GPIOA,GPIO_Pin_4)


#define __NOTHING	0X00	//无操作
#define __WRSR		0X01	//写状态寄存器
#define __RDSR		0X05	//读状态寄存器
#define __RDCR		0X35	//读配置寄存器

#define __READ		0x03	//40M 读
#define __HREAD		0X0B	//104M 高速读
#define __WREN		0X06	//写使能
#define __WRDI		0X04	//写禁能
#define __PP		0x02	//页编程

#define __ULBPR	0X98		//全局保护解除

#define __SE	0X20		//擦除4k，一块
#define __BE	0xD8		//擦除64/32/8 k
#define __CE	0XC7		//擦除整片


static OS_EVENT  *SSP_SEM;	//锁住执行过程信号量



/*
发送 1 Byte SST26VF032B
*/
static uint8_t __f032bSendByte (uint8_t data)
{   
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)//等待发送空
	{
	}
	SPI_I2S_SendData(SPI1, data);                                  //发送

	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);    //等待接收完成
	return SPI_I2S_ReceiveData(SPI1);                              //返回接收的数据
}

/*
收 1 Byte
*/
static uint8_t __f032bGetByte (uint8_t *pdata)
{
	*pdata = __f032bSendByte(0xFF);//SPI器件返回需要先发送一个无意义数据，然后接收
    return F032BOK;
}


/*
等待操作完成
*/
static uint8_t __WaitWrEnd (void)
{   
    uint8_t StatRgVal = 0;
    
    int32_t n = 0x3ffff;
    
    
    do {
        __F032B_CE_Low();      
        __f032bSendByte(__RDSR);      //读状态寄存器             
        __f032bGetByte(&StatRgVal);                                     /*  保存读得的状态寄存器值       */
        __F032B_CE_High();
        n--;
        if (n == 0) {
            break;
        }                
    }
    while (StatRgVal & 0x1);                                           /*  一直等待，直到芯片空闲       */
    
    if (n > 0)
	{
        return F032BOK;
    } 
	else 
	{
        return F032BERROR;
    }
}

/*
等待允许写操作
*/
//static uint8_t __WaitWrOk (void)
//{   
//    uint8_t StatRgVal = 0;
//    
//    int32_t n = 0x3ffff;
//    
//    
//    do {
//        __F032B_CE_Low();      
//        __f032bSendByte(__RDSR);      //读状态寄存器             
//        __f032bGetByte(&StatRgVal);                                     /*  保存读得的状态寄存器值       */
//        __F032B_CE_High();
//        n--;
//        if (n == 0) {
//            break;
//        }                
//    }
//    while (StatRgVal != 0x02);                                           /*  一直等待，直到芯片空闲       */
//    
//    if (n > 0)
//	{
//        return F032BOK;
//    } 
//	else 
//	{
//        return F032BERROR;
//    }
//}

/*
读函数
参数：DST:目标地址；NByte：字节数；RcvBufPt：数据缓冲
注意：每次的读指令不能跨扇区,每扇区4k
*/
uint8_t sst26vf032b_read(uint32_t Dst, uint32_t NByte, volatile uint8_t *RcvBufPt)
{
	uint32_t i = 0 ,j = 0;
	uint8_t err;
	uint32_t addr = Dst;
    
    if (((Dst + NByte -1) > __F032B_MAX_ADDR) || (NByte == 0)) {        /*  检查入口参数                 */
        return (F032BERROR);
    }
	
  	OSSemPend(SSP_SEM,0,&err);
	
	while(NByte != 0)
	{
		__WaitWrEnd();
		
		__F032B_CE_Low();
		__f032bSendByte(__HREAD);    //104M 高速读
		__f032bSendByte((uint8_t)((Dst & 0xFFFFFF) >> 16));                   /*  发送地址信息:3个字节组成     */
		__f032bSendByte((uint8_t)((Dst & 0xFFFF) >> 8));
		__f032bSendByte((uint8_t)(Dst & 0xFF));
		__f032bSendByte(0xFF);                                              /*  发送一个哑字节以读取数据     */
		
		for (i = 0; i < NByte; i++, addr++, j++)
		{
			if (F032BERROR == __f032bGetByte((uint8_t*)(RcvBufPt + j))) 
			{
				OSSemPost(SSP_SEM);     
				__F032B_CE_High(); 
			  return F032BERROR;
			} 
			
			if(addr % 256 == 0 && i != 0)//一次读 256 Byte
			{
				Dst = addr;
				break;
			}
		}
		__F032B_CE_High();//停止读
		NByte -= i;


//		for (j = 0; (NByte != 0 && j < 256); j++, NByte--)
//		{
//			if (F032BERROR == __f032bGetByte((uint8_t*)(RcvBufPt ++))) 
//			{
//				OSSemPost(SSP_SEM);     
//				__F032B_CE_High(); 
//				return F032BERROR;
//			}
//		}
//		__F032B_CE_High();//停止读
//		Dst += 256;
	}
    OSSemPost(SSP_SEM);     
    return (F032BOK);
}

/*
读设备ID
*/
uint8_t sst26vf032b_Jedec_ID_Read (uint32_t *RcvbufPt)
{
    uint8_t temp[4] = {0};
    uint8_t err;
	
    OSSemPend(SSP_SEM,0,&err);
	
	__WaitWrEnd();
	__F032B_CE_Low();
	__f032bSendByte(0x9F);      //发送读JEDEC ID命令(9Fh)
	__f032bGetByte(temp + 2);	//制造商编号	0xBF
	__f032bGetByte(temp + 1);	//设备类型	0x26
	__f032bGetByte(temp);       //设备ID    0x42    
	__F032B_CE_High();      
	*RcvbufPt = (uint32_t)(temp[0] | (temp[1] << 8) | (temp[2] << 16));
	
	OSSemPost(SSP_SEM);
	return (F032BOK);
}

/**********************************************************************************************************
** Function name:       sstf032b_IsExist 
** Descriptions:        检测SPINorFlash是否存在 
** input parameters:    None
** output parameters:   None
** Returned value:      F032BERROR: 不存在; F032BOK: 存在
** Created by:          zhangzheng
** Created Date:        2008.02.18
**---------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
**---------------------------------------------------------------------------------------------------------
**********************************************************************************************************/
uint8_t sstf032b_IsExist (void)
{
    uint32_t FId = 0;
	
	__WaitWrEnd();
    if (F032BOK == sst26vf032b_Jedec_ID_Read(&FId)) 
	{
        if (0xBF2600 == (FId & 0x00ffff00)) 
		{
            return F032BOK;
        } 
		else 
		{
            return F032BERROR;
        }
    } 
	else 
	{
        return F032BERROR;
    }
}

/*
写函数
参数：Dst：目标地址；SndbufPt：待写入数据指针；Nbyte：写入数据字节数
注意：使用页编程指令，每页256Byte，一次最多写一页，每次不能跨页写
页写指令的写入地址必须被256整除
*/
uint8_t sst26vf032b_write (uint32_t Dst, volatile uint8_t *SndbufPt, uint32_t NByte)
{
    uint32_t i = 0;
    uint8_t err, status = FALSE;
    uint32_t addr = Dst;
	
    
    if (((Dst + NByte - 1) > __F032B_MAX_ADDR) || (NByte == 0)) {       /*  检查入口参数                 */
        return (F032BERROR);                                            
    }
    OSSemPend(SSP_SEM, 0, &err);

//	__WaitWrEnd();
//	__F032B_CE_Low();     
//	__f032bSendByte(__ULBPR);//全局块保护解锁
//	__F032B_CE_High();
	
	while(NByte != 0)//注意，一次写入最多256字节，地址的低8位 A[7:0]需要为0
	{
		__WaitWrEnd();
		
		__F032B_CE_Low();     
		__f032bSendByte(__WREN);      //首先发送写使能
		__F032B_CE_High();
		
//		__WaitWrOk();
		
		__F032B_CE_Low();     
		__f032bSendByte(__PP);	//页编程
		__f032bSendByte((uint8_t)(((Dst & 0xFFFFFF) >> 16)));             /*  发送3个字节的地址信息        */
		__f032bSendByte((uint8_t)(((Dst & 0xFFFF) >> 8)));
		__f032bSendByte((uint8_t)(Dst & 0xFF));
		
		for( i = 0; ( NByte != 0 && i < 256 ); (i++, NByte--,addr++) ) //一次最多写256字节
		{
			if((addr % 256 == 0) && (i != 0) )
			{
				Dst = addr; 
				status = TRUE;
				break;
			}
			__f032bSendByte((uint8_t)(*SndbufPt++));                      /*  发送被烧写的数据             */
		}
		__F032B_CE_High(); 
		__WaitWrEnd();	//等待写完成
		
		if(NByte != 0)
		{
			if(status == TRUE)
			{
				status = FALSE;
			}
			else
			{
				Dst += 256;//如果写完256字节后，地址需要加上，再写256字节
			}
		}


//		for( i = 0; ( NByte != 0 && i < 256 ); (i++, NByte--)) //一次最多写256字节
//		{
//			__f032bSendByte( (uint8_t)(*(SndbufPt++)) );  //缓冲区数据
//		}
//		__F032B_CE_High();
//		__WaitWrEnd();	//等待写完成
//		addr += 256;
	} 
		
	__F032B_CE_Low();
	__f032bSendByte(__WRDI);      //写禁能
	__F032B_CE_High();    
	OSSemPost(SSP_SEM);
    return (F032BOK);   
}

/**********************************************************************************************************
根据指定的扇区号选取最高效的算法擦除 
参数：
sec1: 起始扇区号,范围(0~511)
sec2: 终止扇区号,范围(0~511)
返回值：
F032BERROR: 错误; F032BOK: 成功
**********************************************************************************************************/
uint8_t sst26vf032b_erase (uint32_t sec1, uint32_t sec2)
{
    uint8_t  err;
    uint32_t temp        = 0;
    uint32_t SecnHdAddr  = 0;          
    uint32_t no_SecsToEr = 0;                                             /*  要擦除的扇区数目             */
    uint32_t CurSecToEr  = 0;                                             /*  当前要擦除的扇区号           */
    
    if ((sec1 > __F032B_SEC_MAX) || (sec2 > __F032B_SEC_MAX)) {
        return (F032BERROR);  
    }
	
    OSSemPend(SSP_SEM, 0, &err);
	
    if (sec1 > sec2) {                                                  /*  如果用户输入的起始扇区号大于 */
                                                                        /*  终止扇区号，则在内部作出调整 */
        temp = sec1;
        sec1 = sec2;
        sec2 = temp;
    } 
 
    if (sec1 == sec2) {                                                 /*  起止扇区号相等则擦除单个扇区 */
        SecnHdAddr = __F032B_SEC_SIZE * sec1;                           /*  计算扇区的起始地址           */
		__WaitWrEnd();
		
		__F032B_CE_Low();     
		__f032bSendByte(__WREN);        //写使能
		__F032B_CE_High();      
		
//		__WaitWrOk();

		__F032B_CE_Low();     
        __f032bSendByte(__SE);        //扇区擦除指令，擦除4k指令
        __f032bSendByte((uint8_t)(((SecnHdAddr & 0xFFFFFF) >> 16)));   //发送擦除起始地址
        __f032bSendByte((uint8_t)(((SecnHdAddr & 0xFFFF) >> 8)));
        __f032bSendByte((uint8_t)(SecnHdAddr & 0xFF));
        __F032B_CE_High();      
        __WaitWrEnd();
        goto EraseEnd;     
    }
  

	//擦除整片
    if ((sec2 - sec1) == __F032B_SEC_MAX) {                             /*  根据起始扇区和终止扇区间距调 */
                                                                        /*  用最快速的擦除功能           */ 
		__WaitWrEnd();
		
//		__F032B_CE_Low();     
//		__f032bSendByte(__ULBPR);//全局块保护解锁
//		__F032B_CE_High();
		
		__F032B_CE_Low();
		__f032bSendByte(__WREN);        //写使能
		__F032B_CE_High();
		
//		__WaitWrOk();
		
		__F032B_CE_Low();
        __f032bSendByte(0xC7);  //擦除整片 0xc7
        __F032B_CE_High();
        __WaitWrEnd();
        goto EraseEnd;
    }
  
    no_SecsToEr = sec2 - sec1 + 1;                                      /*  获取要擦除的扇区数目         */
    CurSecToEr  = sec1;                                                 /*  从起始扇区开始擦除           */
  
    while (no_SecsToEr >= 1) {                                          /*  用扇区擦除算法擦除剩余扇区   */
        SecnHdAddr = __F032B_SEC_SIZE * CurSecToEr;                     /*  计算扇区的起始地址           */

		__WaitWrEnd();
		
		__F032B_CE_Low();     
		__f032bSendByte(__WREN);        //写使能
		__F032B_CE_High();
		
//		__WaitWrOk();
		
		__F032B_CE_Low(); 
        __f032bSendByte(__SE);     //发送扇区擦除指令  0x20
        __f032bSendByte((uint8_t)(((SecnHdAddr & 0xFFFFFF) >> 16)));      /*  发送3个字节的地址信息        */
        __f032bSendByte((uint8_t)(((SecnHdAddr & 0xFFFF) >> 8)));
        __f032bSendByte((uint8_t)(SecnHdAddr & 0xFF));
        __F032B_CE_High();      
        __WaitWrEnd();		//等待完成
        CurSecToEr  += 1;
        no_SecsToEr -= 1;
    }
    
EraseEnd:
    __F032B_CE_Low();  
    __f032bSendByte(__WRDI);   //发送写禁能 0x04
    __F032B_CE_High();      

    OSSemPost(SSP_SEM);
    return (F032BOK);
}







/*
测试代码
整扇区读写
*/
void sst26vf32b_test1(void)
{
	uint32_t i,j ;
	uint8_t recv[4096], str[4096];
	for(j = 0; j < 0x400; j++)
	{
		for(i = 0; i<4096; i++)
		{
			str[i] = i; 
			recv[i] = 3; 
			
		}
		sst26vf032b_erase(0, j);
		sst26vf032b_read(j  ,4096 , recv);
		for(i = 0; i<4096; i++)
		{
			if(recv[i] != 0xff)
			 i = i;
		}
		sst26vf032b_write(j , str ,4096);
		sst26vf032b_read(j  ,4096 , recv);
		for(i = 0; i<4096; i++)
		{
			if(recv[i] != str[i])
			 i = i;
		}
		sst26vf032b_read(j  ,4096 , recv);
		for(i = 0; i<4096; i++)
		{
			if(recv[i] != str[i])
			 i = i;
		}
	}
	OSTimeDly(1000);
}
/*
测试代码
非整扇区读写
*/
	uint32_t i, j;

void sst26vf32b_test2(void)
{
	uint8_t recv[4096], str[4096];
	while(1)
	{
		for(j = 0; j < 0x3ff; j++)
		{
			for(i = 0; i<4096; i++)
			{
				str[i] = i; 
				recv[i] = 0; 
				
			}
			sst26vf032b_erase(j, j+1);
			sst26vf032b_write(j*4096 + 13 , str, 4096);
			sst26vf032b_read(j*4096 + 13  ,4096 , recv);
			for(i = 0; i < 63; i++)
			{
				if(str[i] != recv[i])
				{
					i = i;
				}
			}

			if(j > 1)
			sst26vf032b_erase(j*4093/4096 - 1, j+1);
			else
			sst26vf032b_erase(0, j+1);
			
			sst26vf032b_read(j*4093  ,4096 , recv);
			for(i = 0; i<4096; i++)
			{
				if(recv[i] != 0xff)
				 i = i;
			}
			sst26vf032b_write(j*4093 , str ,4096);
			sst26vf032b_read(j*4093  ,4096 , recv);
			for(i = 0; i<4096; i++)
			{
				if(recv[i] != str[i])
				 i = i;
			}
			sst26vf032b_read(j*4093  ,4096 , recv);
			for(i = 0; i<4096; i++)
			{
				if(recv[i] != str[i])
				 i = i;
			}
		}
	i=i; 
	OSTimeDly(1000); 
	}

}


/*
sst26vf032b 芯片初始化
*/
static void sst26vf032b_init()  //SPI1 configuration
{

	
	
	//复位flash
	//复位使能
//	__F032B_CE_Low();     
//	__f032bSendByte(0x66);
//	__F032B_CE_High();
//	__F032B_CE_Low();     
//	__f032bSendByte(0x99);
//	__F032B_CE_High();
	
	//写状态寄存器
//	__F032B_CE_Low();     
//	__f032bSendByte(0X01);
//	__f032bSendByte(0X08);//第1 字节是没有作用的
//	__f032bSendByte(0X08);//第2 字节写状态寄存器
//	__F032B_CE_High();
	
	//解除falsh块保护
	__WaitWrEnd();
	
	__F032B_CE_Low();     
	__f032bSendByte(__WREN);        //写使能
	__F032B_CE_High();
	
//	__WaitWrOk();
	
	__F032B_CE_Low();     
	__f032bSendByte(__ULBPR);//全局块保护解锁
	__F032B_CE_High();
	__WaitWrEnd();
}


/*
SPI初始化
*/
void  Spi_Flash_Configuration (void)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOB, ENABLE);//打开GPIO时钟与SPI1时钟
	
	/*
	引脚初始化
	*/
	//PA5--SPI1_SCK    PA6--SPI1--MISO    PA7--SPI1--MOSI
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//PA4--SPI1_NSS
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;  						//#CE 引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);                          
	
	/*
	SPI寄存器初始化
	*/
	SPI_I2S_DeInit(SPI1); //SPI1寄存器复位
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   //SPI双线双向双全工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                        //主模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                    //SPI 8bit 位宽
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;                          //时钟悬空高	(模式3)
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;                         //第 2 个时钟沿捕获数据
//	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                         //第 1 个时钟沿捕获数据
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                            //软控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;   //4分频时钟(最高 18M，可提高到36M，但是不一定稳定)
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                   //先传送高位
	SPI_InitStructure.SPI_CRCPolynomial = 2;                             //CRC校验多项式7.
	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_Cmd(SPI1, ENABLE);                                               //使能 SPI1
	
    SSP_SEM = OSSemCreate(1);//创建互斥型信号量
	sst26vf032b_init();
}

