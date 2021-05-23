#include <i2c_dma.h>


typedef enum {FALSE = 0, TRUE = !FALSE} bool;


#define Transmitter             0x00
#define Receiver                0x01

static __IO uint8_t MasterDirection = Transmitter;
static uint16_t         SlaveDevAddr;                                                /*        ???????                                */
static uint8_t         SlaveDevRegAddr;                                /*        ??????????        */
static bool check_begin = FALSE;
static bool OffsetDone = FALSE;
static __IO   I2C_STATE i2c_state;



/*******************************************************************************************************
** ????: I2C_MasterCommunication
** ????: I2C????,??DMA??,?? I2C2,???
                                                        direction: ????
                                                                                                I2C_Direction_Transmitter ????
                                                                                                I2C_Direction_Receiver?
???
                                                        slave_addr: ????
                                                        offset:????
                                                        buffer:????
                                                        length:?????,????1
** ?    ?: ?
********************************************************************************************************/
void I2C_MasterCommunication(uint8_t direction,uint16_t slave_addr,uint8_t offset,uint8_t* buffer,uint32_t length)
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&I2C1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buffer;
	DMA_InitStructure.DMA_BufferSize = length;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	if(direction == I2C_Direction_Transmitter)
	{ 
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  
		DMA_DeInit(DMA1_Channel4);
		DMA_Init(DMA1_Channel4, &DMA_InitStructure);
		DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
		NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
		MasterDirection = Transmitter;
	}  
	else 
	if(direction == I2C_Direction_Receiver)
	{
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  
		DMA_DeInit(DMA1_Channel5); 
		DMA_Init(DMA1_Channel5, &DMA_InitStructure);
		DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);
		NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
		MasterDirection = Receiver;
	}
	
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	SlaveDevAddr = slave_addr;
	SlaveDevRegAddr = offset;
	OffsetDone = FALSE;
	i2c_state = 1;    
	I2C_AcknowledgeConfig(I2C2,ENABLE);
	I2C_ITConfig(I2C2, I2C_IT_EVT | I2C_IT_ERR ,ENABLE);
	if(I2C2->CR1 & 0x200)I2C2->CR1 &= 0xFDFF;
	I2C_GenerateSTART(I2C2,ENABLE);
	while(i2c_state != FinishFlag);        
}

/*******************************************************************************************************
** ????: I2C2_EV_IRQHandler
** ????: I2C????????
** ?    ?: ?
********************************************************************************************************/
void I2C2_EV_IRQHandler(void)
{          
  switch (I2C_GetLastEvent(I2C2))
  {
                case I2C_EVENT_MASTER_MODE_SELECT:        /* EV5 */
                        if(OffsetDone){ 
                                I2C_Send7bitAddress(I2C2,SlaveDevAddr,I2C_Direction_Receiver); 
                                OffsetDone = FALSE;
                        }                                        
                        else{
                                I2C_Send7bitAddress(I2C2,SlaveDevAddr,I2C_Direction_Transmitter);
                        }
                        I2C_ITConfig(I2C2,I2C_IT_BUF ,ENABLE);
                        break;              

                case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED:  /* EV6 */
                        I2C_ITConfig(I2C2, I2C_IT_EVT | I2C_IT_BUF,DISABLE);
                        I2C_DMALastTransferCmd(I2C2,ENABLE);
                        I2C_DMACmd(I2C2,ENABLE);
                        DMA_Cmd(DMA1_Channel5,ENABLE);            
                        break;   

                case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED: /* EV6 */
                        I2C_SendData(I2C2, SlaveDevRegAddr);
                        OffsetDone = TRUE;   
                        break;                                                
                case I2C_EVENT_MASTER_BYTE_TRANSMITTING:       /* EV8 */
                        if (MasterDirection == Receiver)
                        {
                                I2C_ITConfig(I2C2, I2C_IT_BUF ,DISABLE);
                                while ((I2C2->CR1 & 0x200) == 0x200); 
                                I2C_GenerateSTART(I2C2, ENABLE);
                        }
                        else
                        {
                                I2C_ITConfig(I2C2, I2C_IT_EVT | I2C_IT_BUF,DISABLE);
                                I2C_DMACmd(I2C2,ENABLE);
                                DMA_Cmd(DMA1_Channel4,ENABLE);        
                        }
                        break;
  }
}

/*******************************************************************************************************
** ????: DMA1_Channel4_IRQHandler
** ????: DMA1 ??4??????
** ?    ?: ?
********************************************************************************************************/
void DMA1_Channel4_IRQHandler(void)
{
  if (DMA_GetFlagStatus(DMA1_FLAG_TC4))
  {
                I2C_DMACmd(I2C2,DISABLE);
                DMA_Cmd(DMA1_Channel4,DISABLE);
                I2C_DMALastTransferCmd(I2C2,DISABLE);
                I2C_GenerateSTOP(I2C2,ENABLE);
                i2c_state = FinishFlag;
                DMA_ClearFlag(DMA1_FLAG_TC4);
  }
}
/*******************************************************************************************************
** ????: DMA1_Channel5_IRQHandler
** ????: DMA1 ??5??????
** ?    ?: ?
********************************************************************************************************/
void DMA1_Channel5_IRQHandler(void)
{        
  if(DMA_GetFlagStatus(DMA1_FLAG_TC5))
  {
                I2C_DMACmd(I2C2,DISABLE);
                I2C_DMALastTransferCmd(I2C2,DISABLE);
                I2C_GenerateSTOP(I2C2,ENABLE);
                i2c_state = FinishFlag;
    DMA_ClearFlag(DMA1_FLAG_TC5);
  }
}

/*******************************************************************************************************
** ????: I2C2_ER_IRQHandler
** ????: I2C??????
** ?    ?: ?
********************************************************************************************************/
void I2C2_ER_IRQHandler(void)
{        
	if(I2C_GetFlagStatus(I2C2,I2C_FLAG_AF))
	{
			if(check_begin)I2C_GenerateSTART(I2C2,ENABLE);
			else if(I2C2->SR2 &0x01)
			{
					I2C_GenerateSTOP(I2C2, ENABLE);
					i2c_state = ExitFlag;
			}
			I2C_ClearFlag(I2C2, I2C_FLAG_AF);
	}

	if(I2C_GetFlagStatus(I2C2,I2C_FLAG_BERR))
	{
			if(I2C2->SR2 &0x01)
			{
					I2C_GenerateSTOP(I2C2,ENABLE);
					i2c_state = ExitFlag;
			}
			I2C_ClearFlag(I2C2, I2C_FLAG_BERR);
	}
}