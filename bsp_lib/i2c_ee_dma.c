/*
i2c1 ����
�����жϷ�ʽ������ʹ����DMA
*/
#include "header.h" 
#include "stm32_lib.h"


#define ADDRESS_MAX (1024*AT24Cxx/8)	//����ַ+1

#if (AT24Cxx<4)	//AT24C02
 	#define I2C_PageSize      8
#elif  (AT24Cxx<=16)//AT24C04, AT24C08, AT24C16
	#define I2C_PageSize      16	
#elif  (AT24Cxx == 256) //AT24C512
	#define I2C_PageSize      64	//ҳ��С
	#define EE_ADDRESS_NUM	  2		//(��)�豸��ַ 2Byte 0xffff
#elif  (AT24Cxx == 512) //AT24C512 512K bit ����64K Byte ������˵1 Million�Ĳ�д����
	#define I2C_PageSize      128	//ҳ��С
	#define EE_ADDRESS_NUM	  2		//(��)�豸��ַ 2Byte 0xffff
#else
#error "unsupport eeprom"
#endif
	  
#define I2C1_DR_Address        0x40005410	//i2c1 ���ݼĴ�����ַ
#define I2C2_DR_Address        0x40005810	//i2c2 ���ݼĴ�����ַ
#define Transmitter            0x00
#define Receiver               0x01

/* Local variables -------------------------------------------------------*/
static volatile uint8_t MasterDirection = Transmitter;	//�豸����
static volatile uint16_t SlaveADDR;						//�ӻ���ַ
static volatile uint16_t DeviceOffset=0x0;				//���ݵ�ַ
static bool check_begin = FALSE;
static bool OffsetDone = FALSE;

volatile uint8_t MasterReceptionComplete = 0;	//������ɱ�־ 0����ʼ�� 1�����
volatile uint8_t MasterTransitionComplete = 0;	//������ɱ�־ 0����ʼ�� 1�����

uint32_t I2C_NumByteToWrite = 0;
uint16_t I2C_WriteAddr = 0;
uint8_t I2C_NumByteWritingNow = 0;
uint8_t EE_Addr_Count = 0;	//��ַ�ֽ���
uint8_t *I2C_pBuffer = 0; 

//static volatile uint8_t PV_flag_1;			//����ʵ���Ǹ��ź���
static volatile I2C_STATE i2c_comm_state;		//I2C ����״̬
static OS_EVENT* sem_i2cbus_lock;				//������



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

	//��������
	GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE); //�ܽ���ӳ��
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	//i2c ����
	I2C_DeInit(I2C1);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C; 			//����ΪI2Cģʽ
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;  //ռ�ձ�
	I2C_InitStructure.I2C_OwnAddress1 = I2C1_ADDRESS7;  //stm32��Ϊ�ӻ��ĵ�ַ
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable; 		//ʹ��Ӧ�� ack
	#ifdef SLAVE_10BIT_ADDRESS  
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_10bit;  // user define
	#else
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;	//Ӧ���ַ��7bit��
	#endif
	I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED; //ʱ��Ƶ�� <= 400KHz
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
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//��ռʽ���ȼ�(����������ȼ����ж�)
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
	DMA_DeInit(DMA1_Channel7);	//DMA ��λ
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&I2C1->DR;			//DMA�������ַ ָ�� i2c1 ���ݼĴ�����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pBuffer; 				//DMA�ڴ����ַ
	DMA_InitStructure.DMA_BufferSize = NumByteToRead; 						//DMA�����С
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; 						//������Ϊ���ݴ�����Դ
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 		//�������ַ�Ĵ���������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 				//�ڴ����ַ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //�������ݿ�� 
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 		//�ڴ����ݿ��
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 							//����������������ѭ����ʽ������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;  				//DMA ͨ�����ȼ���4�֣�
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; 							//DMA ͨ��û������Ϊ�ڴ浽�ڴ洫��
	
	DMA_Init(DMA1_Channel7, &DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);							//ʹ�ܴ�������ж�
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;				//��DMA7�ж�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/*initialize static parameter*/
	MasterDirection = Receiver;			//��ʾ����������ʽ��������
	MasterReceptionComplete = 0;		//�������
	
	/*initialize static parameter according to input parameter*/ 
	SlaveADDR = EEPROM_ADDRESS;			//���豸��ַ
	DeviceOffset = ReadAddr;			//���ݵ�ַ
	OffsetDone = FALSE;					//���ݵ�ַ��ɱ�־
	
	/* global state variable i2c_comm_state */
	i2c_comm_state = COMM_PRE;			//i2c ����״̬
	    
	I2C_AcknowledgeConfig(I2C1, ENABLE);//ʹ��i2cӦ����
	I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_ERR , ENABLE);//ʹ��I2C�¼����쳣�ж�
	
	/* Send START condition */
	//if(I2C1->CR1 & 0x200)		//�ж��Ƿ���ֹͣ���� stop����
	//I2C1->CR1 &= 0xFDFF;		//��� stop
	
	if(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF))//��stop��־λ �����(���ݿ⺯��д������)
		I2C_Cmd(I2C1, ENABLE);
	
	I2C_GenerateSTART(I2C1, ENABLE);	//���� Start ����
	while(i2c_comm_state != COMM_DONE && i2c_comm_state != COMM_EXIT);	//һֱ�ȵ���ɻ��߷�������
	
	OSSemPost(sem_i2cbus_lock);
}

void I2C_EE_ReadBuffer(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead)
{
	if(NumByteToRead == 1)//�� 1 BYTE
	{
		uint8_t data[2];
		i2c_ee_readbuffer(data,ReadAddr,2);
		pBuffer[0]=data[0];
	}
	else//�����ֽ�
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
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&I2C1->DR);//ָ��i2c1���ݼĴ���
	
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pBuffer; 	//�ڴ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; 			//������Ϊ���ݴ����Ŀ�ĵ�
	DMA_InitStructure.DMA_BufferSize = NumByteToWrite; 			//DMA �����С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 	//�����ַ������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 			//�ڴ��ַ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte; //�������ݿ��
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 	//�ڴ����ݿ��
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 						//����ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;  			//�ǳ��� �����ȼ�
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; 						//���ڴ浽�ڴ�Ĵ���
	
	DMA_Init(DMA1_Channel6, &DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE);						//����������ж�
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/*initialize static parameter*/
	MasterDirection = Transmitter;		//i2c ���豸����
	MasterTransitionComplete = 0;		//������ɱ�־��ʼ��
	
	/*initialize static parameter according to input parameter*/ 
	SlaveADDR = EEPROM_ADDRESS; 	//���豸��ַ
	DeviceOffset = WriteAddr; 		//���ݵ�ַ
	OffsetDone = FALSE;				//���ݵ�ַ������ɱ�־
	    
	I2C_AcknowledgeConfig(I2C1, ENABLE);	//ʹ��ackӦ��
	I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR, ENABLE);	//����i2c �¼�/����/�����ж�
	
	/* Send START condition */
//	if(I2C1->CR1 & 0x200)
//	I2C1->CR1 &= 0xFDFF;

	if(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF))//��stop��־λ �����(���ݿ⺯����д����)
	{
		I2C_Cmd(I2C1, ENABLE);
	}
	
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));   //�������æ����ȴ�
	
	I2C_GenerateSTART(I2C1, ENABLE);	//��ʼ����

}

//=========================================
//ҳд����
//ע�⣺���ܿ�ҳд
//=========================================
static void I2C_EE_WriteOnePage(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite)
{
	uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;

	Addr = WriteAddr % I2C_PageSize;				//�Ƿ���ҳ����
	count = I2C_PageSize - Addr;					//����ҳ����ʱ��ҳ��д����ֽ�
	NumOfPage =  NumByteToWrite / I2C_PageSize;		//Ҫд����ҳ����
	NumOfSingle = NumByteToWrite % I2C_PageSize;	//Ҫд�Ĳ�����ҳ���ֽڳ���
	I2C_NumByteWritingNow=0;						//��д���ֽ�

	if(Addr == 0) //��ҳ����
	{
		if(NumOfPage == 0) //����һҳ
		{
			I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
			I2C_NumByteWritingNow=NumOfSingle;		//��д���������ڲ�����ҳ���ֽڳ���
		}
		else  //����һҳ
		{
			I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize); 
			I2C_NumByteWritingNow=I2C_PageSize;		//��д�����������ҳ����
		}
	}

	else	//����ҳ���� 
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
				I2C_EE_PageWrite(pBuffer, WriteAddr, count);//һ��ֻ��д�� count�ֽ�
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
//�������ò��ûɶ��
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
	if(NumByteToWrite == 0)//д��0�ֽ�
		return;
	
	OSSemPend(sem_i2cbus_lock, 0, &err);
//	PV_flag_1 = 1;

	i2c_comm_state = COMM_PRE;	//i2c ����״̬

	I2C_pBuffer = pBuffer;
	I2C_WriteAddr = WriteAddr;
	I2C_NumByteToWrite = NumByteToWrite;

	I2C_EE_WriteOnePage(I2C_pBuffer,I2C_WriteAddr,I2C_NumByteToWrite);
	
	while(i2c_comm_state != COMM_DONE && i2c_comm_state != COMM_EXIT);	//һֱ�ȵ���ɻ��߷�������
	
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
���ܣ�DMA1_6 ͨ���ж�
˵������Ϊ I2C1 ����ͨ��
*/
void DMA1_Channel6_IRQHandler(void)
{
	if (DMA_GetFlagStatus(DMA1_FLAG_TC6))//���ͨ��������ɱ�־
	{
		t_dma6 ++;
		//if (I2C1->SR2 & 0x01) //��ģʽ
		if(I2C_GetFlagStatus(I2C1, I2C_FLAG_MSL))//��ģʽ
		{
			//DMA1-6 (I2C1 Tx DMA)transfer complete ISR
			I2C_DMACmd(I2C1, DISABLE);			//���� i2c DMA����
			DMA_Cmd(DMA1_Channel6, DISABLE);	//���� DMA ͨ��6
			
			//wait until BTF
			//while (!(I2C1->SR1 & 0x04));		//�ֽڷ����Ƿ���� 1���� 0δ����   btf == 1
			while(!(I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF)));//@@ ������Ҫ�Ӹ���ʱ����
			I2C_GenerateSTOP(I2C1, ENABLE);		//����i2c ��������
			OSTimeDlyHMSM(0,0,0,10);
			// wait until BUSY clear
			//while (I2C1->SR2 & 0x02);			//��������Ƿ���ͨѶ 0�� 1��		busy == 0
			while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));//@@ ����æ������Ҳ��Ҫ��ʱ����
			MasterTransitionComplete = 1;		//�������
			i2c_comm_state = COMM_IN_PROCESS;	//���봦�����
			I2C_ITConfig(I2C1, I2C_IT_EVT, ENABLE); //ʹ�� i2c �¼��ж�
			
			check_begin = TRUE;
			
			//if(I2C1->CR1 & 0x200)				//����Ƿ����ֹͣ����
			//I2C1->CR1 &= 0xFDFF;				//���ֹͣ����
			if(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF))//��stop��־λ �����(���ݿ⺯����д����)
				I2C_Cmd(I2C1, ENABLE);

			I2C_GenerateSTART(I2C1, ENABLE);	//������ʼ����
		}
		else //��ģʽ
		{
			lcd_show_string("dma6 i2c slave mode ", 0, 350, RED, WHITE, font48); //��������ģʽ�϶��ǲ��Ե�
		}
		DMA_ClearFlag(DMA1_FLAG_TC6);	//���DMA������ɱ�־
	}
	
	if (DMA_GetFlagStatus(DMA1_FLAG_GL6))	//���ͨ��6 ȫ���жϱ�־
	{
		DMA_ClearFlag( DMA1_FLAG_GL6);
	}
	if (DMA_GetFlagStatus(DMA1_FLAG_HT6))	//���ͨ��6 ��������־
	{
		DMA_ClearFlag( DMA1_FLAG_HT6);
	}
}

/*
���ܣ�DMA1_7 ͨ���ж�
˵������Ϊ I2C1 ����ͨ��
*/
void DMA1_Channel7_IRQHandler(void)
{
  //LCD_ShowString("7", 400 , 300, RED, FONT_48);
	if (DMA_GetFlagStatus(DMA1_FLAG_TC7))//ͨ��7������ɱ�־
	{
		if (I2C1->SR2 & 0x01)	//i2c ��ģʽ
		{
			I2C_DMACmd(I2C1, DISABLE);
			I2C_GenerateSTOP(I2C1, ENABLE);
			i2c_comm_state = COMM_DONE;
			MasterReceptionComplete = 1;
//			PV_flag_1 =0;
			
		}
		else //i2c ��ģʽ
		{
			lcd_show_string("dma7 i2c slave mode ", 0, 350, RED, WHITE, font48);//��������ģʽ�϶��ǲ��Ե�
		}
		DMA_ClearFlag(DMA1_FLAG_TC7);//���������ɱ�־λ
	}
  
	if (DMA_GetFlagStatus(DMA1_FLAG_GL7))//ͨ��7ȫ�ֱ�־
	{
		DMA_ClearFlag( DMA1_FLAG_GL7);
	}
	
	if (DMA_GetFlagStatus(DMA1_FLAG_HT7))//ͨ��7��������־
	{
		DMA_ClearFlag( DMA1_FLAG_HT7);
	}
}

/*
���ܣ�i2c1 �����¼��ж�
ע�⣺��ack�⣬�������ݶ����������͵ģ���Ϊֻ�������ɿ�SDA
*/
void I2C1_EV_IRQHandler(void)
{
	//ע�⣬���ص��¼����Ǹ��ֱ�־λ����ϣ�������Ҫ�����־λ����
	switch (I2C_GetLastEvent(I2C1))//������һ���¼�
	{
		//��ʼ�����ѷ���
		case I2C_EVENT_MASTER_MODE_SELECT:	//EV5
		{
			if(!check_begin)
			{
				i2c_comm_state = COMM_IN_PROCESS;//ͨѶ��ʼ
			}
            
			if (MasterDirection == Receiver) //i2c ����
			{
				if (!OffsetDone)//���ݵ�ַδ����
				{
					I2C_Send7bitAddress(I2C1,SlaveADDR,I2C_Direction_Transmitter);	//���ӻ���ַ��д����
				}
				else	
				{
					I2C_Send7bitAddress(I2C1, SlaveADDR, I2C_Direction_Receiver); 	//���ӻ���ַ��������
					OffsetDone = FALSE;
				}
            }
			else	//i2c ������
			{
				I2C_Send7bitAddress(I2C1, SlaveADDR, I2C_Direction_Transmitter);	//���ӻ���ַ��д����
			}
			I2C_ITConfig(I2C1, I2C_IT_BUF , ENABLE);//ʹ�ܻ����ж�
		}
		break;
              
		//��ģʽ �����¼�
		case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED:  //EV6��ע��EV6_1û�������¼�
		{
			//LCD_ShowString("EV6R", 0 , 50, RED, FONT_48);
			I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF, DISABLE);	//�����¼��뻺���ж�
			//without it, no NAK signal on bus after last Data
			//I2C data line would be hold low 		//sda���ӻ����ͣ����Ƿ�����ack
			I2C_DMALastTransferCmd(I2C1, ENABLE);	//ʹ���´� DMA ����Ϊ���һ�δ���
			I2C_DMACmd(I2C1, ENABLE);				//ʹ��i2c DMA����
			DMA_Cmd(DMA1_Channel7, ENABLE);			//��DMAͨ��
		}			
		break;
      
		//��ģʽ�ֽڽ����¼�
		case I2C_EVENT_MASTER_BYTE_RECEIVED:    //EV7
		{
			lcd_show_string("EV7", 0 , 100, RED, WHITE, font48);
		}
		break;
      
		//��ģʽ�������¼�
		case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:	//EV6 �� EV8_1��i2c����״̬��ack���أ��������������
		{
			//LCD_ShowString("EV6S", 0 , 200, RED, FONT_48);
			if (check_begin)
			{
				check_begin = FALSE;
				I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF |I2C_IT_ERR, DISABLE);	//����i2c�����ж�
				I2C_GenerateSTOP(I2C1, ENABLE);										//����ֹͣ����
				//!! write over
				I2C_EE_WriteOnePageCompleted();		//д������
				
				if(I2C_NumByteToWrite > 0)//����δδд��
				{
					I2C_EE_WriteOnePage(I2C_pBuffer,I2C_WriteAddr,I2C_NumByteToWrite);
				}
				else	//����д��
				{
					i2c_comm_state = COMM_DONE;
//					PV_flag_1 = 0;
				}
				break;//����ֱ���˳�
			}

			EE_Addr_Count++;//�����ֽڼ�����
			if (EE_Addr_Count < EE_ADDRESS_NUM)
			{
				I2C_SendData(I2C1, DeviceOffset>>8);//�������ݵ�ַ���ֽ�
			}
			else
			{
				I2C_SendData(I2C1, DeviceOffset);//�������ݵ�ַ���ֽ�
			}
		}
		break;
              
		case I2C_EVENT_MASTER_BYTE_TRANSMITTING:	//EV8
		{
			//LCD_ShowString("EV8", 0 , 250, RED, FONT_48);

			if(!OffsetDone)//���ݵ�ַû���꣬������
			{
				if (EE_Addr_Count < EE_ADDRESS_NUM)
				{
//					while(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF));//��stop��־λ �����(���ݿ⺯����д����)
//					I2C_GenerateSTART(I2C1, ENABLE);
					I2C_SendData(I2C1, DeviceOffset);//�������ݵ�ַ���ֽ�
					EE_Addr_Count++;	
				}
				else
				{
					EE_Addr_Count = 0;
					OffsetDone = TRUE;	//���ݵ�ַ������
				}
				break;
			}
			
			//�����ѷ���������ݵ�ַ�����濪�����ݻ���д����
			if (MasterDirection == Receiver)	//�� e2prom
			{
				
				I2C_ITConfig(I2C1, I2C_IT_BUF , DISABLE);		//�ػ����ж�
				//while ((I2C1->CR1 & 0x200) == 0x200); 		//�Ѳ���ֹͣ����������Ӧ���Ǳ�ĵط��ȷ���ֹͣ������������ܼ�鵽��
				while(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF));	//��⵽ֹͣ�������ȴ�Ӳ�����
				I2C_GenerateSTART(I2C1, ENABLE);				//����ʼ���������¿�ʼһ������
				//break;
			}
			else	//д e2prom
			{
				t_evt ++;

				I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF, DISABLE);	//���¼��ж��뻺���ж�
				I2C_DMACmd(I2C1, ENABLE);								//ʹ��i2c��DMA����
				DMA_Cmd(DMA1_Channel6, ENABLE);							//��DMAͨ��
				//break;//ֱ���˳�
			}
		}
		break;
        
		case I2C_EVENT_MASTER_BYTE_TRANSMITTED:       // EV8-2  Ҫ������ֹͣλ������1���ӻ��Ļ�������Ҳ�У�
		{
			//LCD_ShowString("EV8-2", 0 , 300, RED, FONT_48);			
		}
		break;
	}
}

/*
���ܣ�i2c1 ���ߴ����ж�
*/
void I2C1_ER_IRQHandler(void)
{
	if (I2C_GetFlagStatus(I2C1, I2C_FLAG_AF))//Ӧ�������Ӧʧ��
	{
		t_err ++;
		//LCD_ShowString("err1", 0 , 400, RED, FONT_48);
		if (check_begin)
			I2C_GenerateSTART(I2C1, ENABLE);
		else
		if (I2C1->SR2 &0x01)	//��ģʽ
		{	
			//!! receive over
			I2C_GenerateSTOP(I2C1, ENABLE);
			i2c_comm_state = COMM_EXIT;
//			PV_flag_1 = 0;
		}
		I2C_ClearFlag(I2C1, I2C_FLAG_AF);
	}

	if (I2C_GetFlagStatus(I2C1, I2C_FLAG_BERR))//���ߴ���
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
//��ѯ��ʽ��дI2c
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















