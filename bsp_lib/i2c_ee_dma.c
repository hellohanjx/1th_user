/*
i2c1 驱动
采用中断方式，并且使用了DMA
*/
#include "header.h" 
#include "stm32_lib.h"


#define ADDRESS_MAX (1024*AT24Cxx/8)	//最大地址+1

#if (AT24Cxx<4)	//AT24C02
 	#define I2C_PageSize      8
#elif  (AT24Cxx<=16)//AT24C04, AT24C08, AT24C16
	#define I2C_PageSize      16	
#elif  (AT24Cxx == 256) //AT24C512
	#define I2C_PageSize      64	//页大小
	#define EE_ADDRESS_NUM	  2		//(从)设备地址 2Byte 0xffff
#elif  (AT24Cxx == 512) //AT24C512 512K bit 即是64K Byte 资料上说1 Million的擦写次数
	#define I2C_PageSize      128	//页大小
	#define EE_ADDRESS_NUM	  2		//(从)设备地址 2Byte 0xffff
#else
#error "unsupport eeprom"
#endif
	  
#define I2C1_DR_Address        0x40005410	//i2c1 数据寄存器地址
#define I2C2_DR_Address        0x40005810	//i2c2 数据寄存器地址
#define Transmitter            0x00
#define Receiver               0x01

/* Local variables -------------------------------------------------------*/
static volatile uint8_t MasterDirection = Transmitter;	//设备方向
static volatile uint16_t SlaveADDR;						//从机地址
static volatile uint16_t DeviceOffset=0x0;				//数据地址
static bool check_begin = FALSE;
static bool OffsetDone = FALSE;

volatile uint8_t MasterReceptionComplete = 0;	//接受完成标志 0：初始化 1：完成
volatile uint8_t MasterTransitionComplete = 0;	//接受完成标志 0：初始化 1：完成

uint32_t I2C_NumByteToWrite = 0;
uint16_t I2C_WriteAddr = 0;
uint8_t I2C_NumByteWritingNow = 0;
uint8_t EE_Addr_Count = 0;	//地址字节数
uint8_t *I2C_pBuffer = 0; 

//static volatile uint8_t PV_flag_1;			//这其实就是个信号量
static volatile I2C_STATE i2c_comm_state;		//I2C 总线状态
static OS_EVENT* sem_i2cbus_lock;				//总线锁



static volatile uint32_t t_evt,t_err , t_dma6 , t_dma7 ;



/*******************************************************************************
* Function Name  : I2C_EE_Init
* Description    : Initializes peripherals used by the I2C EEPROM driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void  I2C_EE_Init(void)
{
	/******* GPIO configuration and clock enable *********/
	GPIO_InitTypeDef  GPIO_InitStructure; 
	I2C_InitTypeDef  I2C_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	//引脚设置
	GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE); //管脚重映射
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	//i2c 设置
	I2C_DeInit(I2C1);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C; 			//设置为I2C模式
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;  //占空比
	I2C_InitStructure.I2C_OwnAddress1 = I2C1_ADDRESS7;  //stm32作为从机的地址
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable; 		//使能应答 ack
	#ifdef SLAVE_10BIT_ADDRESS  
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_10bit;  // user define
	#else
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;	//应答地址（7bit）
	#endif
	I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED; //时钟频率 <= 400KHz
	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_Cmd(I2C1, ENABLE);
	sem_i2cbus_lock=OSSemCreate(1);
	
  
/************** I2C NVIC configuration *************************/  
    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
    NVIC_InitStructure.NVIC_IRQChannel = I2C1_ER_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//抢占式优先级(这是最高优先级的中断)
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}

/*******************************************************************************
* Function Name  : I2C_EE_BufferRead
* Description    : Reads a block of data from the EEPROM.
* Input          : - pBuffer : pointer to the buffer that receives the data read 
*                    from the EEPROM.
*                  - ReadAddr : EEPROM's internal address to read from.
*                  - NumByteToRead : number of bytes to read from the EEPROM.
* Output         : None
* Return         : None
*******************************************************************************/
static void i2c_ee_readbuffer(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead)
{
	uint8_t err;
	DMA_InitTypeDef  DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* PV operation */
//	if (PV_flag_1 !=0)
//	  return;
	if(NumByteToRead==0)
		return;
	OSSemPend(sem_i2cbus_lock, 0, &err);
//	PV_flag_1 = 1;
	
	/* DMA initialization */
	DMA_DeInit(DMA1_Channel7);	//DMA 复位
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&I2C1->DR;			//DMA外设基地址 指向 i2c1 数据寄存器地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pBuffer; 				//DMA内存基地址
	DMA_InitStructure.DMA_BufferSize = NumByteToRead; 						//DMA缓存大小
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; 						//外设作为数据传输来源
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 		//外设基地址寄存器不递增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 				//内存基地址递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //外设数据宽度 
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 		//内存数据宽度
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 							//工作在正常（还有循环方式）缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;  				//DMA 通道优先级（4种）
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; 							//DMA 通道没有设置为内存到内存传输
	
	DMA_Init(DMA1_Channel7, &DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);							//使能传输完成中断
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;				//开DMA7中断
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/*initialize static parameter*/
	MasterDirection = Receiver;			//表示主机工作方式：收数据
	MasterReceptionComplete = 0;		//接收完成
	
	/*initialize static parameter according to input parameter*/ 
	SlaveADDR = EEPROM_ADDRESS;			//从设备地址
	DeviceOffset = ReadAddr;			//数据地址
	OffsetDone = FALSE;					//数据地址完成标志
	
	/* global state variable i2c_comm_state */
	i2c_comm_state = COMM_PRE;			//i2c 总线状态
	    
	I2C_AcknowledgeConfig(I2C1, ENABLE);//使能i2c应答功能
	I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_ERR , ENABLE);//使能I2C事件与异常中断
	
	/* Send START condition */
	//if(I2C1->CR1 & 0x200)		//判断是否有停止条件 stop产生
	//I2C1->CR1 &= 0xFDFF;		//清除 stop
	
	if(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF))//读stop标志位 并清除(根据库函数写的流程)
		I2C_Cmd(I2C1, ENABLE);
	
	I2C_GenerateSTART(I2C1, ENABLE);	//产生 Start 条件
	while(i2c_comm_state != COMM_DONE && i2c_comm_state != COMM_EXIT);	//一直等到完成或者发生错误
	
	OSSemPost(sem_i2cbus_lock);
}

void I2C_EE_ReadBuffer(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead)
{
	if(NumByteToRead == 1)//读 1 BYTE
	{
		uint8_t data[2];
		i2c_ee_readbuffer(data,ReadAddr,2);
		pBuffer[0]=data[0];
	}
	else//读多字节
	{
		i2c_ee_readbuffer(pBuffer,ReadAddr,NumByteToRead);
	}
}

/*******************************************************************************
* Function Name  : I2C_EE_PageWrite
* Description    : Writes more than one byte to the EEPROM with a single WRITE
*                  cycle. The number of byte can't exceed the EEPROM page size.
* Input          : - pBuffer : pointer to the buffer containing the data to be 
*                    written to the EEPROM.
*                  - WriteAddr : EEPROM's internal address to write to (1-16).
*                  - NumByteToWrite : number of bytes to write to the EEPROM.
* Output         : None
* Return         : None
*******************************************************************************/
static void I2C_EE_PageWrite(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite)
{
	DMA_InitTypeDef  DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	DMA_DeInit(DMA1_Channel6);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&I2C1->DR);//指向i2c1数据寄存器
	
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pBuffer; 	//内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; 			//外设作为数据传输的目的地
	DMA_InitStructure.DMA_BufferSize = NumByteToWrite; 			//DMA 缓存大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 	//外设地址不增加
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 			//内存地址增加
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte; //外设数据宽度
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 	//内存数据宽度
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 						//正常模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;  			//非常高 的优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; 						//非内存到内存的传输
	
	DMA_Init(DMA1_Channel6, &DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE);						//开传输完成中断
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/*initialize static parameter*/
	MasterDirection = Transmitter;		//i2c 主设备方向
	MasterTransitionComplete = 0;		//发送完成标志初始化
	
	/*initialize static parameter according to input parameter*/ 
	SlaveADDR = EEPROM_ADDRESS; 	//从设备地址
	DeviceOffset = WriteAddr; 		//数据地址
	OffsetDone = FALSE;				//数据地址传输完成标志
	    
	I2C_AcknowledgeConfig(I2C1, ENABLE);	//使能ack应答
	I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR, ENABLE);	//开启i2c 事件/缓冲/错误中断
	
	/* Send START condition */
//	if(I2C1->CR1 & 0x200)
//	I2C1->CR1 &= 0xFDFF;

	if(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF))//读stop标志位 并清除(根据库函数缩写流程)
	{
		I2C_Cmd(I2C1, ENABLE);
	}
	
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));   //如果总线忙，则等待
	
	I2C_GenerateSTART(I2C1, ENABLE);	//起始条件

}

//=========================================
//页写函数
//注意：不能跨页写
//=========================================
static void I2C_EE_WriteOnePage(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite)
{
	uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;

	Addr = WriteAddr % I2C_PageSize;				//是否整页对齐
	count = I2C_PageSize - Addr;					//非整页对齐时此页可写入的字节
	NumOfPage =  NumByteToWrite / I2C_PageSize;		//要写的整页数量
	NumOfSingle = NumByteToWrite % I2C_PageSize;	//要写的不足整页的字节长度
	I2C_NumByteWritingNow=0;						//已写的字节

	if(Addr == 0) //整页对齐
	{
		if(NumOfPage == 0) //不足一页
		{
			I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
			I2C_NumByteWritingNow=NumOfSingle;		//已写的数量等于不足整页的字节长度
		}
		else  //超过一页
		{
			I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize); 
			I2C_NumByteWritingNow=I2C_PageSize;		//已写入的数量等于页长度
		}
	}

	else	//非整页对齐 
	{
		if(NumOfPage == 0) 
		{
			I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
			I2C_NumByteWritingNow = NumOfSingle;
		}
		else
		{
			if(count != 0)
			{  
				I2C_EE_PageWrite(pBuffer, WriteAddr, count);//一次只能写入 count字节
				I2C_NumByteWritingNow = count;
			}
			else 
			{
				lcd_show_string("address error", 0, 350, RED, WHITE, font48);
			} 
		}
	}  
}

//============================
//这个函数貌似没啥用
//============================
void I2C_EE_WriteOnePageCompleted(void)
{
	I2C_pBuffer += I2C_NumByteWritingNow;
	I2C_WriteAddr += I2C_NumByteWritingNow;
	I2C_NumByteToWrite -= I2C_NumByteWritingNow;
}
/*******************************************************************************
* Function Name  : I2C_EE_BufferWrite
* Description    : Writes buffer of data to the I2C EEPROM.
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the EEPROM.
*                  - WriteAddr : EEPROM's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the EEPROM.
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_WriteBuffer(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite)
{
	uint8_t err;
	/* PV operation */ 
//	if (PV_flag_1 !=0)
//	  return;
	if(NumByteToWrite == 0)//写入0字节
		return;
	
	OSSemPend(sem_i2cbus_lock, 0, &err);
//	PV_flag_1 = 1;

	i2c_comm_state = COMM_PRE;	//i2c 总线状态

	I2C_pBuffer = pBuffer;
	I2C_WriteAddr = WriteAddr;
	I2C_NumByteToWrite = NumByteToWrite;

	I2C_EE_WriteOnePage(I2C_pBuffer,I2C_WriteAddr,I2C_NumByteToWrite);
	
	while(i2c_comm_state != COMM_DONE && i2c_comm_state != COMM_EXIT);	//一直等到完成或者发生错误
	
	OSSemPost(sem_i2cbus_lock);

}

/*******************************************************************************
* Function Name  : I2C_EE_WaitOperationIsCompleted
* Description    : wait operation is completed
* Input          : None 
* Output         : None
* Return         : None
*******************************************************************************/
static void I2C_EE_WaitOperationIsCompleted(void)
{
	while(i2c_comm_state != COMM_DONE);
}	

/*
功能：DMA1_6 通道中断
说明：作为 I2C1 发送通道
*/
void DMA1_Channel6_IRQHandler(void)
{
	if (DMA_GetFlagStatus(DMA1_FLAG_TC6))//检查通道传输完成标志
	{
		t_dma6 ++;
		//if (I2C1->SR2 & 0x01) //主模式
		if(I2C_GetFlagStatus(I2C1, I2C_FLAG_MSL))//主模式
		{
			//DMA1-6 (I2C1 Tx DMA)transfer complete ISR
			I2C_DMACmd(I2C1, DISABLE);			//禁能 i2c DMA请求
			DMA_Cmd(DMA1_Channel6, DISABLE);	//禁能 DMA 通道6
			
			//wait until BTF
			//while (!(I2C1->SR1 & 0x04));		//字节发送是否结束 1结束 0未结束   btf == 1
			while(!(I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF)));//@@ 这里需要加个超时处理
			I2C_GenerateSTOP(I2C1, ENABLE);		//产生i2c 结束条件
			OSTimeDlyHMSM(0,0,0,10);
			// wait until BUSY clear
			//while (I2C1->SR2 & 0x02);			//检查总线是否有通讯 0无 1有		busy == 0
			while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));//@@ 总线忙；这里也需要超时处理
			MasterTransitionComplete = 1;		//发送完成
			i2c_comm_state = COMM_IN_PROCESS;	//进入处理过程
			I2C_ITConfig(I2C1, I2C_IT_EVT, ENABLE); //使能 i2c 事件中断
			
			check_begin = TRUE;
			
			//if(I2C1->CR1 & 0x200)				//检查是否产生停止条件
			//I2C1->CR1 &= 0xFDFF;				//清除停止条件
			if(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF))//读stop标志位 并清除(根据库函数缩写流程)
				I2C_Cmd(I2C1, ENABLE);

			I2C_GenerateSTART(I2C1, ENABLE);	//产生开始条件
		}
		else //从模式
		{
			lcd_show_string("dma6 i2c slave mode ", 0, 350, RED, WHITE, font48); //进入这种模式肯定是不对的
		}
		DMA_ClearFlag(DMA1_FLAG_TC6);	//清除DMA传输完成标志
	}
	
	if (DMA_GetFlagStatus(DMA1_FLAG_GL6))	//检查通道6 全局中断标志
	{
		DMA_ClearFlag( DMA1_FLAG_GL6);
	}
	if (DMA_GetFlagStatus(DMA1_FLAG_HT6))	//检查通道6 传输过半标志
	{
		DMA_ClearFlag( DMA1_FLAG_HT6);
	}
}

/*
功能：DMA1_7 通道中断
说明：作为 I2C1 接收通道
*/
void DMA1_Channel7_IRQHandler(void)
{
  //LCD_ShowString("7", 400 , 300, RED, FONT_48);
	if (DMA_GetFlagStatus(DMA1_FLAG_TC7))//通道7传输完成标志
	{
		if (I2C1->SR2 & 0x01)	//i2c 主模式
		{
			I2C_DMACmd(I2C1, DISABLE);
			I2C_GenerateSTOP(I2C1, ENABLE);
			i2c_comm_state = COMM_DONE;
			MasterReceptionComplete = 1;
//			PV_flag_1 =0;
			
		}
		else //i2c 从模式
		{
			lcd_show_string("dma7 i2c slave mode ", 0, 350, RED, WHITE, font48);//进入这种模式肯定是不对的
		}
		DMA_ClearFlag(DMA1_FLAG_TC7);//清除发送完成标志位
	}
  
	if (DMA_GetFlagStatus(DMA1_FLAG_GL7))//通道7全局标志
	{
		DMA_ClearFlag( DMA1_FLAG_GL7);
	}
	
	if (DMA_GetFlagStatus(DMA1_FLAG_HT7))//通道7传输过半标志
	{
		DMA_ClearFlag( DMA1_FLAG_HT7);
	}
}

/*
功能：i2c1 总线事件中断
注意：除ack外，其他数据都是主机发送的，因为只有主机可控SDA
*/
void I2C1_EV_IRQHandler(void)
{
	//注意，返回的事件都是各种标志位的组合，可能需要具体标志位分析
	switch (I2C_GetLastEvent(I2C1))//获得最近一次事件
	{
		//起始条件已发送
		case I2C_EVENT_MASTER_MODE_SELECT:	//EV5
		{
			if(!check_begin)
			{
				i2c_comm_state = COMM_IN_PROCESS;//通讯开始
			}
            
			if (MasterDirection == Receiver) //i2c 接收
			{
				if (!OffsetDone)//数据地址未发完
				{
					I2C_Send7bitAddress(I2C1,SlaveADDR,I2C_Direction_Transmitter);	//发从机地址，写操作
				}
				else	
				{
					I2C_Send7bitAddress(I2C1, SlaveADDR, I2C_Direction_Receiver); 	//发从机地址，读操作
					OffsetDone = FALSE;
				}
            }
			else	//i2c 方向发送
			{
				I2C_Send7bitAddress(I2C1, SlaveADDR, I2C_Direction_Transmitter);	//发从机地址，写操作
			}
			I2C_ITConfig(I2C1, I2C_IT_BUF , ENABLE);//使能缓存中断
		}
		break;
              
		//主模式 接收事件
		case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED:  //EV6，注意EV6_1没有这种事件
		{
			//LCD_ShowString("EV6R", 0 , 50, RED, FONT_48);
			I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF, DISABLE);	//禁能事件与缓存中断
			//without it, no NAK signal on bus after last Data
			//I2C data line would be hold low 		//sda被从机拉低，就是返回了ack
			I2C_DMALastTransferCmd(I2C1, ENABLE);	//使得下次 DMA 传输为最后一次传输
			I2C_DMACmd(I2C1, ENABLE);				//使能i2c DMA请求
			DMA_Cmd(DMA1_Channel7, ENABLE);			//打开DMA通道
		}			
		break;
      
		//主模式字节接收事件
		case I2C_EVENT_MASTER_BYTE_RECEIVED:    //EV7
		{
			lcd_show_string("EV7", 0 , 100, RED, WHITE, font48);
		}
		break;
      
		//主模式，发送事件
		case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:	//EV6 与 EV8_1（i2c发送状态的ack返回，不仅仅是这个）
		{
			//LCD_ShowString("EV6S", 0 , 200, RED, FONT_48);
			if (check_begin)
			{
				check_begin = FALSE;
				I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF |I2C_IT_ERR, DISABLE);	//禁能i2c所有中断
				I2C_GenerateSTOP(I2C1, ENABLE);										//产生停止条件
				//!! write over
				I2C_EE_WriteOnePageCompleted();		//写入数据
				
				if(I2C_NumByteToWrite > 0)//数据未未写完
				{
					I2C_EE_WriteOnePage(I2C_pBuffer,I2C_WriteAddr,I2C_NumByteToWrite);
				}
				else	//数据写完
				{
					i2c_comm_state = COMM_DONE;
//					PV_flag_1 = 0;
				}
				break;//这里直接退出
			}

			EE_Addr_Count++;//数据字节计数器
			if (EE_Addr_Count < EE_ADDRESS_NUM)
			{
				I2C_SendData(I2C1, DeviceOffset>>8);//发送数据地址高字节
			}
			else
			{
				I2C_SendData(I2C1, DeviceOffset);//发送数据地址低字节
			}
		}
		break;
              
		case I2C_EVENT_MASTER_BYTE_TRANSMITTING:	//EV8
		{
			//LCD_ShowString("EV8", 0 , 250, RED, FONT_48);

			if(!OffsetDone)//数据地址没发完，继续发
			{
				if (EE_Addr_Count < EE_ADDRESS_NUM)
				{
//					while(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF));//读stop标志位 并清除(根据库函数缩写流程)
//					I2C_GenerateSTART(I2C1, ENABLE);
					I2C_SendData(I2C1, DeviceOffset);//发送数据地址低字节
					EE_Addr_Count++;	
				}
				else
				{
					EE_Addr_Count = 0;
					OffsetDone = TRUE;	//数据地址发送完
				}
				break;
			}
			
			//上面已发送完成数据地址，下面开读数据或者写数据
			if (MasterDirection == Receiver)	//读 e2prom
			{
				
				I2C_ITConfig(I2C1, I2C_IT_BUF , DISABLE);		//关缓冲中断
				//while ((I2C1->CR1 & 0x200) == 0x200); 		//已产生停止条件（这里应该是别的地方先发送停止条件，这里才能检查到）
				while(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF));	//检测到停止条件，等待硬件清除
				I2C_GenerateSTART(I2C1, ENABLE);				//发起始条件，重新开始一个流程
				//break;
			}
			else	//写 e2prom
			{
				t_evt ++;

				I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF, DISABLE);	//关事件中断与缓冲中断
				I2C_DMACmd(I2C1, ENABLE);								//使能i2c的DMA请求
				DMA_Cmd(DMA1_Channel6, ENABLE);							//开DMA通道
				//break;//直接退出
			}
		}
		break;
        
		case I2C_EVENT_MASTER_BYTE_TRANSMITTED:       // EV8-2  要求设置停止位（这里1个从机的话不设置也行）
		{
			//LCD_ShowString("EV8-2", 0 , 300, RED, FONT_48);			
		}
		break;
	}
}

/*
功能：i2c1 总线错误中断
*/
void I2C1_ER_IRQHandler(void)
{
	if (I2C_GetFlagStatus(I2C1, I2C_FLAG_AF))//应答错误，响应失败
	{
		t_err ++;
		//LCD_ShowString("err1", 0 , 400, RED, FONT_48);
		if (check_begin)
			I2C_GenerateSTART(I2C1, ENABLE);
		else
		if (I2C1->SR2 &0x01)	//主模式
		{	
			//!! receive over
			I2C_GenerateSTOP(I2C1, ENABLE);
			i2c_comm_state = COMM_EXIT;
//			PV_flag_1 = 0;
		}
		I2C_ClearFlag(I2C1, I2C_FLAG_AF);
	}

	if (I2C_GetFlagStatus(I2C1, I2C_FLAG_BERR))//总线错误
	{
		lcd_show_string("err2", 0 , 400, RED, WHITE, font48);
		if (I2C1->SR2 &0x01)
		{
			I2C_GenerateSTOP(I2C1, ENABLE);
			i2c_comm_state = COMM_EXIT;
//			PV_flag_1 = 0;
		}
		I2C_ClearFlag(I2C1, I2C_FLAG_BERR);
	}
}





/*
//查询方式读写I2c
void I2C_EEPROM_ReadNByte(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead)
{
	uint16_t curNum;
	uint8_t err;
	OSSemPend(sem_i2cbus_lock,0,&err);
	do
	{
		curNum=ReadAddr%I2C_PageSize;
		if(curNum)
		{
			curNum=(curNum<NumByteToRead)  ? curNum : NumByteToRead;
		}
		else
		{
			curNum=(NumByteToRead>I2C_PageSize)? I2C_PageSize : NumByteToRead;
		}
		I2C_EEPROM_ReadNByte(pBuffer,ReadAddr,curNum);
		pBuffer+=curNum;
		ReadAddr+=curNum;
		NumByteToRead-=curNum;
	}while(NumByteToRead>0);
	OSSemPost(sem_i2cbus_lock);
}

void I2C_EEPROM_WriteNByte(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead)
{
	uint16_t curNum;
	uint8_t err;
	OSSemPend(sem_i2cbus_lock,0,&err);
	do
	{
		curNum=ReadAddr%I2C_PageSize;
		if(curNum)
		{
			curNum=(curNum<NumByteToRead) ? curNum : NumByteToRead;
		}
		else
		{
			curNum=(NumByteToRead>I2C_PageSize) ? I2C_PageSize : NumByteToRead;
		}
		I2C_EEPROM_WriteNByte(pBuffer,ReadAddr,curNum);
		pBuffer+=curNum;
		ReadAddr+=curNum;
		NumByteToRead-=curNum;
	}while(NumByteToRead>0);
	OSSemPost(sem_i2cbus_lock);
}*/















